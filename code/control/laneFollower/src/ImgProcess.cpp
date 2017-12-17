//
// Created by raphael on 12/2/17.
//

#include "ImgProcess.h"

namespace carolocup
{
	namespace control
	{
		using namespace std;
		using namespace cv;
		using namespace odcore::base;
		using namespace odcore::data;
		using namespace odcore::data::image;
		using namespace odcore::wrapper;
		using namespace odcore::data::dmcp;

		ImgProcess::ImgProcess() :
				Sim(),
				m_debug(),
				p_gain(),
				i_gain(),
				d_gain(),
				_isOvertaker(),
				stop(false),
				stopCounter(),
				inRightLane(),
				currentDistance(0),
				_speed(),
				_steer(),
				m_sharedImageMemory(),
				m_hasAttachedToSharedImageMemory(false),
				m_image(),
				m_threshold1(50),  // Both thresholds are dynamically adjusted at image processing
				m_threshold2(200),
				m_control_scanline((M_CONTROL_SCAN_LINE * 2 / 4) + 10),// Lane markings are searched for at this pixel line
				m_stop_scanline(M_STOP_SCAN_LINE),// Stop line lane marking searched for at this pixel line
				m_distance(M_DISTANCE),  // Distance from the lane marking at which the car attempts to drive
				m_image_mat(),
				m_image_new(),
				m_image_dst(),
				m_hough(),
				counter(0),
				state(_MOVING),
				prevState(_MOVING)
		{}

		ImgProcess::~ImgProcess()
		{}

// This method returns a boolean true if it gets an image from the shared image memory, copying it into a iplimage
		bool ImgProcess::readSharedImage(Container &c)
		{
			bool retVal = false;

			if (c.getDataType() == SharedImage::ID())
			{
				SharedImage si = c.getData<SharedImage>();

				// Check if we have already attached to the shared memory.
				if (!m_hasAttachedToSharedImageMemory)
				{
					m_sharedImageMemory = odcore::wrapper::SharedMemoryFactory::attachToSharedMemory(
							si.getName());
					m_hasAttachedToSharedImageMemory = true;
				}

				// Check if we could successfully attach to the shared memory.
				if (m_sharedImageMemory->isValid())
				{
					// Lock the memory region to gain exclusive access using a scoped lock.
					m_sharedImageMemory->lock();

					if (m_image.empty())
					{ // If image is empty, create a new image to hold the shared image data
						m_image.create(si.getHeight(), si.getWidth(),
									   CV_8UC1); //From opencv documentation: CV_8UC3 = unsigned integer 8bit matrix/image wih 3 channels (typically RGB or BRG in opencv case)
					}
					else
					{ // Copying the image data
						memcpy(m_image.data, m_sharedImageMemory->getSharedMemory(),
							   si.getWidth() * si.getHeight() * si.getBytesPerPixel());
					}


					m_sharedImageMemory->unlock(); // Release the memory region lock
					// If in Sim mode, flip the image
					if (*Sim)
					{
						flip(m_image, m_image, -1);
					}
					retVal = true;
				}
			}
			return retVal;
		}

// Process image logic
		void ImgProcess::processImage()
		{
//			// New mat image
//			m_image_new = Mat(m_image.rows, m_image.cols, CV_8UC1);
//			// Copy the original image to the new image as greyscale
//			cvtColor(m_image, m_image_new, COLOR_BGR2GRAY);
			cerr << "size rows -> " <<  m_image.rows << " size cols -> " << m_image.cols << endl;
			cerr << "size width -> " <<  m_image.size().width << " height -> " << m_image.size().height << endl;
			//Region of interest
			Mat image_roi = m_image(Rect(0, m_image.size().height/4, m_image.size().width, m_image.size().height*2/4));

			// Apply a gaussian blur to the image, to smooth it out
			GaussianBlur(image_roi, m_image_new, Size(5, 5), 0, 0);

			// Calculate median of pixel color in order to dynamically calculate Canny thresholds
			double median;
			median = Median(m_image_new);

			// Thresholds calculation
			m_threshold1 = max(static_cast<double>(0), ((1.0 - 0.33) * median));
			m_threshold2 = min(static_cast<double>(255), (1.0 + 0.33) * median);

			// See header for algorithm and threshold explanation
			Canny(m_image_new, m_image_new, m_threshold1, m_threshold2, 3);

			// Test for dilation operation see: https://docs.opencv.org/2.4/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html
			Mat element = getStructuringElement(MORPH_RECT,
												Size(2 + 1, 2 + 1),
												Point(2, 2));

			dilate(m_image_new, m_image_dst, element);

			// Apply hough line transformation algorithm, for more info see:  https://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html
			cvtColor(m_image_dst, m_hough, CV_GRAY2BGR);

            vector<Vec4i> lines;

			//threshold: The minimum number of intersections to “detect” a line
			//minLinLength: The minimum number of points that can form a line. Lines with less than this number of points are disregarded
			//maxLineGap: The maximum gap between two points to be considered in the same line.
            HoughLinesP(m_image_dst, lines, 1, CV_PI/180, 60, 25, 150);

            for( size_t i = 0; i < lines.size(); i++ )
            {
				Vec4i l = lines[i];
				TimeStamp now;
				//cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " LINES " << abs(l[2] - l[0]) << endl;
				if (l[1] != l[3]) {
					if ((abs(l[2] - l[0]) / (abs(l[3] - l[1]))) < 5) {
						line( m_image_new, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 2, CV_AA);
					}
//					else
//					{
//						line( m_image_new, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 0), 2, CV_AA);
//					}

				}
//				if ( abs(l[2] - l[0]) < 300)
//				{
//					line( m_image_new, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 0, 0), 2, CV_AA);
//				}

			}

//			for( size_t i = 0; i < lines.size(); i++ )
//			{
//				double rho = lines[i][0], theta = lines[i][1];
//				Point pt1, pt2;
//				double a = cos(theta), b = sin(theta);
//				double x0 = a*rho, y0 = b*rho;
//				pt1.x = cvRound(x0 + 1000*(-b));
//				pt1.y = cvRound(y0 + 1000*(a));
//				pt2.x = cvRound(x0 - 1000*(-b));
//				pt2.y = cvRound(y0 - 1000*(a));
//				line( m_image_new, pt1, pt2, Scalar(255,0,0), 3, CV_AA);
//			}
		}

// Pixel median value calculation
		double ImgProcess::Median(Mat mat)
		{
			double m = (mat.rows * mat.cols) / 2;
			int bin = 0;
			double med = -1.0;

			int histSize = 256;
			float range[] = {0, 256};
			const float *histRange = {range};
			bool uniform = true;
			bool accumulate = false;
			Mat hist;
			calcHist(&mat, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

			for (int i = 0; i < histSize && med < 0.0; ++i)
			{
				bin += cvRound(hist.at<float>(i));
				if (bin > m && med < 0.0)
					med = i;
			}

			return med;
		}

// Calculate deviation from goal
		double ImgProcess::errorCalculation()
		{
			TimeStamp now;

			// Values adjusted for simulation environment, if sim flag is set
			if (*Sim)
			{
				m_stop_scanline = M_CONTROL_SCAN_LINE_SIM;
				m_control_scanline = M_STOP_SCAN_LINE_SIM; // calibrated length to right: 280px
				m_distance = M_DISTANCE_SIM;  // distance from right lane marking
			}

			double e = 0;

			int32_t y = m_control_scanline;

			uchar pixelLeft, pixelRight, pixelLeft2, pixelRight2;
			Point left, right, left2, right2;  // P(x, y) = P (column, row)

			left.y = y;
			left2.y = y - 200;
			left.x = -1;
			left2.x = -1;

			// Search from middle to the left
			for (int x = m_image_new.cols / 2; x > 0; x--)
			{//from the middle to the left
				pixelLeft = m_image_new.at<uchar>(Point(x, y));
				if (pixelLeft >= 150)
				{
					left.x = x;
					break;
				}
			}

			right.y = y;
			right2.y = y - 200;
			right.x = -1;
			right2.x = -1;
			// Search from middle to the right
			for (int x = m_image_new.cols / 2;
				 x < m_image_new.cols; x++)
			{
				pixelRight = m_image_new.at<uchar>(Point(x, y));
				if (pixelRight >= 150)
				{
					right.x = x;
					break;
				}
			}

			if (!*Sim)
			{
				if (right.x == -1 && left.x == -1)
				{  //setting state if the car does not see any line
					// The following 2 for loops search for lane markings at a row close to the top of the image if no lane markings are found at the control_scanline
					for (int x = m_image_new.cols / 2; x > 0; x--)
					{
						pixelLeft2 = m_image_new.at<uchar>(Point(x, 100));
						if (pixelLeft2 >= 150)
						{
							left2.x = x;
							break;
						}
					}
					// Search from middle to the right
					for (int x = m_image_new.cols / 2;
						 x < m_image_new.cols - 30; x++)
					{
						pixelRight2 = m_image_new.at<uchar>(Point(x, 100));
						if (pixelRight2 >= 150)
						{
							right2.x = x;
							break;
						}
					}
					// If the second check also returns -1 for both sides, then no followable lane markings are found
					if (right2.x == -1 && left2.x == -1)
					{
						state = _DANGER;
					}

				}
				else
				{
					if (state != _STOP && state != _RESUME)
					{
						state = _MOVING;
					}
				}
			}

				// Moving the pixel perception to the right, as to better keep track of right lane marking
				if (right.x > 0) right.x += 10;

				if (left.x > 0) left.x += 10;


				if (y == m_control_scanline)
				{

					if (*inRightLane)
					{ //Adapt to following the right lane

						if (right.x > 0)
						{

							e = ((right.x - m_image_new.cols / 2.0) - m_distance) / m_distance;
						}
						else if (left.x > 0)
						{

							e = (m_distance - (m_image_new.cols / 2.0 - left.x)) / m_distance;
						}
					}
					else
					{  //Adapt to following the left lane

						if (left.x > 0)
						{

							e = (m_distance - (m_image_new.cols / 2.0 - left.x)) / m_distance;
						}
						else if (right.x > 0)
						{

							e = ((right.x - m_image_new.cols / 2.0) - m_distance) / m_distance;
						}
					}
				}

			// Stopline logic
			uchar front_left, front_right;
			Point stop_left, stop_right;

			int left_dist = 0;
			// Set the column/ row at which to search
			stop_left.x = (m_image_new.cols / 2) - 25;
			stop_left.y = m_control_scanline;

			// Find first grey pixel in the front of the car left side
			for (int i = m_control_scanline; i > m_stop_scanline; i--)
			{
				front_left = m_image_new.at<uchar>(Point(stop_left.x, i));
				if (front_left > 150)
				{
					stop_left.y = i;
					left_dist = m_control_scanline - stop_left.y;
					break;
				}
			}

			int right_dist = 0;
			// Set the column/ row at which to search
			stop_right.x = (m_image_new.cols / 2) + 15;
			stop_right.y = m_control_scanline;

			// Find first grey pixel in front of the car right side
			for (int i = m_control_scanline; i > m_stop_scanline; i--)
			{
				front_right = m_image_new.at<uchar>(Point(stop_right.x, i));
				if (front_right > 150)
				{
					stop_right.y = i;
					right_dist = m_control_scanline - stop_right.y;
					break;
				}
			}

			// Draw lines if debug true
			if (*m_debug)
			{
				if (stop_left.y < m_control_scanline)
				{
					line(m_image_new, Point(stop_left.x, m_control_scanline), stop_left, Scalar(255, 0, 0));
				}

				if (stop_right.y < m_control_scanline)
				{
					line(m_image_new, Point(stop_right.x, m_control_scanline), stop_right, Scalar(255, 0, 0));
				}
			}

			// Prints several pieces of information onto the image for debugging purposes if debug flag is set to true
			if (*m_debug)
			{
				cerr << now.getYYYYMMDD_HHMMSS_noBlank() << " IMG KP " << *p_gain << endl;

				std::string speed = std::to_string(*_speed);
				putText(m_image_new, "Speed is " + speed, Point(m_image_new.cols - 150, 20), FONT_HERSHEY_PLAIN, 1,
						CV_RGB(255, 255, 255));

				std::string steer = std::to_string(90 + (*_steer * (180 / 3.14)));
				putText(m_image_new, "Steering: " + steer, Point(m_image_new.cols - 150, 40), FONT_HERSHEY_PLAIN, 1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "State: " + std::to_string(state), Point(m_image_new.cols - 150, 138), FONT_HERSHEY_PLAIN, 1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "Old state: " + std::to_string(prevState), Point(m_image_new.cols - 150, 153), FONT_HERSHEY_PLAIN,
						1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "Stop counter :" + std::to_string(*stopCounter), Point(m_image_new.cols - 150, 168),
						FONT_HERSHEY_PLAIN, 1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "P_gain: " + std::to_string(*p_gain), Point(m_image_new.cols - 150, 60),
						FONT_HERSHEY_PLAIN,
						1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "D_gain: " + std::to_string(*d_gain), Point(m_image_new.cols - 150, 100),
						FONT_HERSHEY_PLAIN,
						1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "I_gain: " + std::to_string(*i_gain), Point(m_image_new.cols - 150, 80),
						FONT_HERSHEY_PLAIN, 1,
						CV_RGB(255, 255, 255));

				putText(m_image_new, "Distance " + std::to_string(currentDistance), Point(m_image_new.cols - 150, 120),
						FONT_HERSHEY_PLAIN, 1,
						CV_RGB(255, 255, 255));

				if (left.x > 0)
				{
					line(m_image_new, Point(m_image.cols / 2, y), left, Scalar(255, 0, 0), 1, 8);
					std::string left_reading = std::to_string((m_image_new.cols / 2 - left.x));


					putText(m_image_new, left_reading, Point(m_image_new.cols / 2 - 100, y - 2), FONT_HERSHEY_PLAIN, 1,
							CV_RGB(255, 255, 255));
				}
				if (right.x > 0)
				{
					line(m_image_new, cvPoint(m_image.cols / 2, y), right, Scalar(255, 0, 0), 1, 8);
					std::string right_reading = std::to_string((right.x - m_image_new.cols / 2));
					currentDistance = (right.x - m_image_new.cols / 2);

					putText(m_image_new, right_reading, Point(m_image_new.cols / 2 + 100, y - 2), FONT_HERSHEY_PLAIN, 1,
							CV_RGB(255, 255, 255));
				}
				if (left2.x > 0)
				{
					line(m_image_new, Point(m_image.cols / 2, 50), left2, Scalar(255, 0, 0), 1, 8);
					std::string left_reading = std::to_string((m_image_new.cols / 2 - left.x));


					putText(m_image_new, left_reading, Point(m_image_new.cols / 2 - 100, 50 - 2), FONT_HERSHEY_PLAIN, 1,
							CV_RGB(255, 255, 255));
				}
				if (right2.x > 0)
				{
					line(m_image_new, cvPoint(m_image.cols / 2, 50), right2, Scalar(255, 0, 0), 1, 8);
					std::string right_reading = std::to_string((right.x - m_image_new.cols / 2));

					putText(m_image_new, right_reading, Point(m_image_new.cols / 2 + 100, 50 - 2), FONT_HERSHEY_PLAIN,
							1,
							CV_RGB(255, 255, 255));
				}
			}

			// Checks whether the detected stopline is at a similar distance on both sides
			if (counter < 3 && (left_dist - right_dist) > -10 && (left_dist - right_dist) < 10 && left_dist != 0 &&
				right_dist != 0)
			{
				if (!*Sim)
				{
					if (left_dist < 35 || right_dist < 35)
					{
						counter++;
					}
				}
				else
				{
					counter++;
				}

			}
			else
			{
				counter = 0;
			}
			if (counter > 2)
			{
				stop = true;
			}
			else
			{
				stop = false;
			}
			return e;
		}

		void ImgProcess::showDebugImage()
		{
			// Show resulting image from image processing if debuf flag set
			if (*m_debug)
			{
				if (m_image.data != NULL)
				{
					imshow("Debug Image",
						   m_image_new);
					waitKey(10);
				}
			}

//			//display the resulting image from hough algorithm implmentation
//            if (*m_debug) {
//                if (!m_hough.empty()) {
//                    imshow("Hough algorithm applied",
//                           m_hough);
//                    waitKey(10);
//                }
//            }
//
//			//display the resulting image from dilation algorithm implmentation
//			if (*m_debug) {
//				if (!m_image_dst.empty()) {
//					imshow("Dilation algorithm applied",
//						   m_image_dst);
//					waitKey(10);
//				}
//			}
		}

		void ImgProcess::setUpWindowDebug()
		{
			//cvNamedWindow("Debug Image", CV_WINDOW_AUTOSIZE);
			//cvMoveWindow("Debug Image", 300, 100);
		}

		void ImgProcess::destroyWindowDebug()
		{
			if (!m_image.empty())
			{
				m_image.deallocate();
			}
			if (*m_debug)
			{
				cvDestroyWindow("WindowShowImage");
			}
		}

		void ImgProcess::setState(int st)
		{
			state = st;
		}

		int ImgProcess::getState()
		{
			return state;
		}

		void ImgProcess::setPrev_State(int st)
		{
			prevState = st;
		}

		int ImgProcess::getPrev_State()
		{
			return prevState;
		}

	} //control
}//carolocup

