/*
 * James Hubbard
 * CameraInterface.hpp
 * Header file defining class CameraInterface
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/optflow.hpp>
#include <opencv2/video.hpp>
#include <OpenNI.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

class CameraInterface {
	public:
		// Stream properties copied from OniProperties.h
		typedef enum
		{
		        STREAM_PROPERTY_CROPPING                        = 0, // OniCropping*
		        STREAM_PROPERTY_HORIZONTAL_FOV                  = 1, // float: radians
		        STREAM_PROPERTY_VERTICAL_FOV                    = 2, // float: radians
		        STREAM_PROPERTY_VIDEO_MODE                      = 3, // OniVideoMode*

		        STREAM_PROPERTY_MAX_VALUE                       = 4, // int
		        STREAM_PROPERTY_MIN_VALUE                       = 5, // int

		        STREAM_PROPERTY_STRIDE                          = 6, // int
		        STREAM_PROPERTY_MIRRORING                       = 7, // OniBool

		        STREAM_PROPERTY_NUMBER_OF_FRAMES                = 8, // int

		        // Camera
		        STREAM_PROPERTY_AUTO_WHITE_BALANCE              = 100, // OniBool
		        STREAM_PROPERTY_AUTO_EXPOSURE                   = 101, // OniBool
		        STREAM_PROPERTY_EXPOSURE                                = 102, // int
		        STREAM_PROPERTY_GAIN                                    = 103, // int

		} StreamProperty;

		CameraInterface();
		~CameraInterface();

		cv::Mat readDepth16U();
		cv::Mat readColor();

		void getProperty(StreamProperty p, void *data, bool color = true);
		void setProperty(StreamProperty p, void *data, bool color = true);


	private:

		openni::VideoFrameRef		m_depthFrame;
		openni::VideoFrameRef		m_colorFrame;
		openni::VideoMode		m_depthVideoMode;
		openni::VideoMode		m_colorVideoMode;
		
		openni::Device*			m_device;
		openni::VideoStream*		m_depthStream;
		openni::VideoStream*		m_colorStream;

};


