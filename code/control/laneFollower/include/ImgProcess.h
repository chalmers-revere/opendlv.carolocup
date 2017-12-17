

#ifndef CAROLOCUP_SOFTWARE_IMGPROCESS_H
#define CAROLOCUP_SOFTWARE_IMGPROCESS_H

#include "../../defines/defines.h"

#include <stdint.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <math.h>
#include <unistd.h>
#include <algorithm>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opendavinci/odcore/data/Container.h>
#include <opendavinci/odcore/data/TimeStamp.h>

#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>
#include <opendavinci/odcore/base/KeyValueConfiguration.h>
#include <opendavinci/odcore/base/Lock.h>

namespace carolocup
{
	namespace control
	{
		using namespace std;
		using namespace cv;
		using namespace odcore::data::image;

		class ImgProcess
		{
		private:
			ImgProcess(const ImgProcess & /*obj*/) = delete;

			ImgProcess &operator=(const ImgProcess & /*obj*/) = delete;

		public:
			/**
			 * Constructor
			 */
			ImgProcess();

			/**
			 * Destructor.
			 */
			virtual ~ImgProcess();

			/**
			 * This method is called to process an incoming container.
			 *
			 * @param c Container to process.
			 * @return true if c was successfully processed.
			 */
			bool readSharedImage(odcore::data::Container &c);

			void processImage();

			double errorCalculation();

			void showDebugImage();

			void setUpWindowDebug();

			void destroyWindowDebug();

			void setState(int st);

			int getState();

			void setPrev_State(int st);

			int getPrev_State();

			bool *Sim, *m_debug;

			double *p_gain;
			double *i_gain;
			double *d_gain;

			int *_isOvertaker;

			bool stop;
			double *stopCounter;

			bool *inRightLane;   //Flip this value to indicate lane change
			int currentDistance;

			double *_speed, *_steer;

		private:
			shared_ptr <odcore::wrapper::SharedMemory> m_sharedImageMemory;
			bool m_hasAttachedToSharedImageMemory;

			cv::Mat m_image;

			/**
			 *Canny algoithm
			 * http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html
			 *
			 *If a pixel gradient is higher than the upper threshold, the pixel is accepted as an edge
			 *If a pixel gradient value is below the lower threshold, then it is rejected.
			 *If the pixel gradient is between the two thresholds, then it will be accepted only if it is connected to a pixel that is above the upper threshold.
			 *
			 **/
			int32_t m_threshold1;
			int32_t m_threshold2;
			int32_t m_control_scanline;
			int32_t m_stop_scanline;
			int32_t m_distance;

			// Class variables
			Mat m_image_mat, m_image_new, m_image_dst, m_hough;

			double counter;
			int state, prevState;

			double Median(cv::Mat mat);
		};
	}
}

#endif //CAROLOCUP_SOFTWARE_IMGPROCESS_H
