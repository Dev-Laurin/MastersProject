/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2011 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

/** @file Protonect.cpp Main application file. */

#include <iostream>
using std::cout; 
using std::endl; 
#include <cstdlib>
#include <signal.h>
#include <vector>
using std::vector; 
#include "point.hpp"
#include "detectObject.hpp"
#include <fstream> 
#include "writeToJSFile.hpp"
using std::ofstream; 

/// [headers]
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
/// [headers]
#ifdef EXAMPLES_WITH_OPENGL_SUPPORT
#include "viewer.h"
#endif


bool protonect_shutdown = false; ///< Whether the running application should shut down.

void sigint_handler(int s)
{
  protonect_shutdown = true;
}

bool protonect_paused = false;
libfreenect2::Freenect2Device *devtopause;

//Doing non-trivial things in signal handler is bad. If you want to pause,
//do it in another thread.
//Though libusb operations are generally thread safe, I cannot guarantee
//everything above is thread safe when calling start()/stop() while
//waitForNewFrame().
void sigusr1_handler(int s)
{
  if (devtopause == 0)
    return;
/// [pause]
  if (protonect_paused)
    devtopause->start();
  else
    devtopause->stop();
  protonect_paused = !protonect_paused;
/// [pause]
}

//The following demostrates how to create a custom logger
/// [logger]
#include <fstream>
#include <cstdlib>
class MyFileLogger: public libfreenect2::Logger
{
private:
  std::ofstream logfile_;
public:
  MyFileLogger(const char *filename)
  {
    if (filename)
      logfile_.open(filename);
    level_ = Debug;
  }
  bool good()
  {
    return logfile_.is_open() && logfile_.good();
  }
  virtual void log(Level level, const std::string &message)
  {
    logfile_ << "[" << libfreenect2::Logger::level2str(level) << "] " << message << std::endl;
  }
};
/// [logger]

/// [main]
/**
 * Main application entry point.
 *
 * Accepted argumemnts:
 * - cpu Perform depth processing with the CPU.
 * - gl  Perform depth processing with OpenGL.
 * - cl  Perform depth processing with OpenCL.
 * - <number> Serial number of the device to open.
 * - -noviewer Disable viewer window.
 */
int main(int argc, char *argv[])
/// [main]
{
  //Create JS File for writing 
 
  
 int width = 10; //meters 
 vector<vector<Point>> bins(width,
      vector<Point>(width , Point(0, 0, 0, 0))); 
  //keep track of which point is the biggest in the Y 
  vector<vector<Point>> maximums(width, 
        vector<Point>(width, Point(0,0,0,0)));

  ///
  std::string program_path(argv[0]);
  std::cerr << "Version: " << LIBFREENECT2_VERSION << std::endl;
  std::cerr << "Environment variables: LOGFILE=<protonect.log>" << std::endl;
  std::cerr << "Usage: " << program_path << " [-gpu=<id>] [gl | cl | clkde | cuda | cudakde | cpu] [<device serial>]" << std::endl;
  std::cerr << "        [-noviewer] [-norgb | -nodepth] [-help] [-version]" << std::endl;
  std::cerr << "        [-frames <number of frames to process>]" << std::endl;
  std::cerr << "To pause and unpause: pkill -USR1 Protonect" << std::endl;
  size_t executable_name_idx = program_path.rfind("Protonect");

  std::string binpath = "/";

  if(executable_name_idx != std::string::npos)
  {
    binpath = program_path.substr(0, executable_name_idx);
  }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  // avoid flooing the very slow Windows console with debug messages
  libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Info));
#else
  // create a console logger with debug level (default is console logger with info level)
/// [logging]
  libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Debug));
/// [logging]
#endif
/// [file logging]
  MyFileLogger *filelogger = new MyFileLogger(getenv("LOGFILE"));
  if (filelogger->good())
    libfreenect2::setGlobalLogger(filelogger);
  else
    delete filelogger;
/// [file logging]

/// [context]
  libfreenect2::Freenect2 freenect2;
  libfreenect2::Freenect2Device *dev = 0;
  libfreenect2::PacketPipeline *pipeline = 0;
/// [context]

  std::string serial = "";

  bool viewer_enabled = true;
  bool enable_rgb = true;
  bool enable_depth = true;
  int deviceId = -1;
  size_t framemax = -1;

  for(int argI = 1; argI < argc; ++argI)
  {
    const std::string arg(argv[argI]);

    if(arg == "-help" || arg == "--help" || arg == "-h" || arg == "-v" || arg == "--version" || arg == "-version")
    {
      // Just let the initial lines display at the beginning of main
      return 0;
    }
    else if(arg.find("-gpu=") == 0)
    {
      if (pipeline)
      {
        std::cerr << "-gpu must be specified before pipeline argument" << std::endl;
        return -1;
      }
      deviceId = atoi(argv[argI] + 5);
    }
    else if(arg == "cpu")
    {
      if(!pipeline)
/// [pipeline]
        pipeline = new libfreenect2::CpuPacketPipeline();
/// [pipeline]
    }
    else if(arg == "gl")
    {
#ifdef LIBFREENECT2_WITH_OPENGL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenGLPacketPipeline();
#else
      std::cout << "OpenGL pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "cl")
    {
#ifdef LIBFREENECT2_WITH_OPENCL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenCLPacketPipeline(deviceId);
#else
      std::cout << "OpenCL pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "clkde")
    {
#ifdef LIBFREENECT2_WITH_OPENCL_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::OpenCLKdePacketPipeline(deviceId);
#else
      std::cout << "OpenCL pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "cuda")
    {
#ifdef LIBFREENECT2_WITH_CUDA_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::CudaPacketPipeline(deviceId);
#else
      std::cout << "CUDA pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg == "cudakde")
    {
#ifdef LIBFREENECT2_WITH_CUDA_SUPPORT
      if(!pipeline)
        pipeline = new libfreenect2::CudaKdePacketPipeline(deviceId);
#else
      std::cout << "CUDA pipeline is not supported!" << std::endl;
#endif
    }
    else if(arg.find_first_not_of("0123456789") == std::string::npos) //check if parameter could be a serial number
    {
      serial = arg;
    }
    else if(arg == "-noviewer" || arg == "--noviewer")
    {
      viewer_enabled = false;
    }
    else if(arg == "-norgb" || arg == "--norgb")
    {
      enable_rgb = false;
    }
    else if(arg == "-nodepth" || arg == "--nodepth")
    {
      enable_depth = false;
    }
    else if(arg == "-frames")
    {
      ++argI;
      framemax = strtol(argv[argI], NULL, 0);
      if (framemax == 0) {
        std::cerr << "invalid frame count '" << argv[argI] << "'" << std::endl;
        return -1;
      }
    }
    else
    {
      std::cout << "Unknown argument: " << arg << std::endl;
    }
  }

  if (!enable_rgb && !enable_depth)
  {
    std::cerr << "Disabling both streams is not allowed!" << std::endl;
    return -1;
  }

/// [discovery]
  if(freenect2.enumerateDevices() == 0)
  {
    std::cout << "no device connected!" << std::endl;
    return -1;
  }

  if (serial == "")
  {
    serial = freenect2.getDefaultDeviceSerialNumber();
  }
/// [discovery]

  if(pipeline)
  {
/// [open]
    dev = freenect2.openDevice(serial, pipeline);
/// [open]
  }
  else
  {
    dev = freenect2.openDevice(serial);
  }

  if(dev == 0)
  {
    std::cout << "failure opening device!" << std::endl;
    return -1;
  }

  devtopause = dev;

  signal(SIGINT,sigint_handler);
#ifdef SIGUSR1
  signal(SIGUSR1, sigusr1_handler);
#endif
  protonect_shutdown = false;

/// [listeners]
  int types = 0;
  if (enable_rgb)
    types |= libfreenect2::Frame::Color;
  if (enable_depth)
    types |= libfreenect2::Frame::Ir | libfreenect2::Frame::Depth;
  libfreenect2::SyncMultiFrameListener listener(types);
  libfreenect2::FrameMap frames;

  dev->setColorFrameListener(&listener);
  dev->setIrAndDepthFrameListener(&listener);
/// [listeners]

/// [start]
  if (enable_rgb && enable_depth)
  {
    if (!dev->start())
      return -1;
  }
  else
  {
    if (!dev->startStreams(enable_rgb, enable_depth))
      return -1;
  }

  std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
  std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
/// [start]

/// [registration setup]
  libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
  libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4);
/// [registration setup]

  size_t framecount = 0;
#ifdef EXAMPLES_WITH_OPENGL_SUPPORT
  Viewer viewer;
  if (viewer_enabled)
    viewer.initialize();
#else
  viewer_enabled = false;
#endif

/// [loop start]
  while(!protonect_shutdown && (framemax == (size_t)-1 || framecount < framemax))
  {
    if (!listener.waitForNewFrame(frames, 10*1000)) // 10 sconds
    {
      std::cout << "timeout!" << std::endl;
      return -1;
    }
    libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
    libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
    libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

/// [loop start]

    if (enable_rgb && enable_depth)
    {
/// [registration]
      //(RGB frame, Depth Frame, Undistored Depth frame, Color image for depth image, 
        //enable filter)
      registration->apply(rgb, depth, &undistorted, &registered, true);

      //MARK: Getting Point Data
      //GET RBGD Points from combined Depth & Color image
      vector<Point> framePoints(registered.width*registered.height); 

      int index = 0; 
      for(size_t i=0; i<registered.height; ++i){
        for(size_t j=0; j<registered.width; ++j){
          float x,y,z,rgbData; 

          //IF color is not needed can use getPointXYZ() for faster
            //computation 
          registration->getPointXYZRGB(&undistorted, &registered, i, j, x, y, z, rgbData);  
          Point p(x,y,z,rgbData); 
          framePoints[index] = p; 
          ++index; 
        }
      }

      //MARK: Filter 
      //Filter out bad data 
    //  vector<Point>filteredPoints(registered.width*registered.height);  
   //   filterPoints(framePoints,filteredPoints); 

      ofstream debugFile("debug.txt"); 

/*
      for(int i=0; i<framePoints.size(); i++){
        framePoints[i].draw(debugFile); 
        debugFile << endl; 
      } */ 

      //MARK: Find Floor Algorithm 
      //Find floor 
      vector<float> plane(4);
      vector<float> normal(3); 
      // std::mt19937 gen(time(0)); 
      // Point onPlane; 
      vector<float> gravityNormalVector({0, -1, 0}); 
       //3 m? TODO: Change
      float cameraHeight = 0.0; // 3.0;
      float xAxisAngleRotation = 0; // 20; //degrees 

     // std::uniform_int_distribution<int>dis(0, filteredPoints.size()-1);
     // findFloorPlane(filteredPoints, plane, gen, dis, normal, 
     //   onPlane, gravityNormalVector, robotSize); 
  
      //MARK: Draw Floor 
      //Draw the Floor plane on the RGB plane
      //drawFloorPlane(rgb, plane, filteredPoints, normal); 

      //Calculate the floor plane from our knowledge of the kinect's position
      //https://stackoverflow.com/questions/22234248/how-to-convert-points-between-two-coordinate-systems-with-different-rotations
      //https://gamedev.stackexchange.com/questions/26084/how-to-get-the-rotation-matrix-to-transform-between-two-3d-cartesian-coordinate
      //Multiplication Help: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/#an-introduction-to-matrices
      //https://math.stackexchange.com/questions/72014/given-an-angle-in-radians-how-could-i-calculate-a-4x4-rotation-matrix-about-the
      getFloorPlane(plane, xAxisAngleRotation, cameraHeight); 
      
      //write the plane coordinates to JS file for viewing later
    //  writePlaneToJS(plane); 

      //Transform points into our 3D space (rotation transform) where
      //  our camera is level for easy algorithm manipulation of data 
  /*    transformPoints(framePoints, xAxisAngleRotation, cameraHeight); 
debugFile << "After Transform: " << endl; 
for(int i=0; i<framePoints.size(); i++){
        framePoints[i].draw(debugFile); 
        debugFile << endl; 
      } */ 


      //MARK: Segment Into Objects 
      //place points into bins based on their x & z value
      
      //Segment out the objects based on 3D point depth & Euclidean Distance
        //to each other 
      float robotBase = 1.0; //2meters? 
      segmentIntoObjects(framePoints, robotBase, 
        maximums, bins);
  
      //Save maximums to JS file for viewing later 
      ofstream jsFile("data.js"); 
      initJSFile(jsFile, "grid");
      writeToJS(maximums, jsFile); 

      endWritingPoints(jsFile); 
      writeVariable(jsFile, width, width);

      jsFile.close(); 
      debugFile.close(); 
      /*
        //raw data from kinect 
      ofstream rawKinectDataFile("raw.js"); 

      for(unsigned int i=0; i<framePoints.size(); i++){
        rawKinectDataFile << "(" << framePoints[i].x << ", "; 
        rawKinectDataFile << framePoints[i].y << ", "; 
        rawKinectDataFile << framePoints[i].z << ")" << endl; 
      }
      */ 

       //cout << framePoints[100][100].rgb << endl; //Printing out x-coordinate of first pixel
     
/// [registration]
    }

    framecount++;
    if (!viewer_enabled)
    {
      if (framecount % 100 == 0)
        std::cout << "The viewer is turned off. Received " << framecount << " frames. Ctrl-C to stop." << std::endl;
      listener.release(frames);
      continue;
    }

#ifdef EXAMPLES_WITH_OPENGL_SUPPORT
    if (enable_rgb)
    {
      viewer.addFrame("RGB", rgb);
    }
    if (enable_depth)
    {
      viewer.addFrame("ir", ir);
      viewer.addFrame("depth", depth);
    }
    if (enable_rgb && enable_depth)
    {
      viewer.addFrame("registered", &registered);
    }

    protonect_shutdown = protonect_shutdown || viewer.render();
#endif

/// [loop end]
    listener.release(frames);
    /** libfreenect2::this_thread::sleep_for(libfreenect2::chrono::milliseconds(100)); */
  }
/// [loop end]

  // TODO: restarting ir stream doesn't work!
  // TODO: bad things will happen, if frame listeners are freed before dev->stop() :(
/// [stop]
  dev->stop();
  dev->close();
/// [stop]
   
  

  

  //rawKinectDataFile.close(); 

  delete registration;

  return 0;
}
