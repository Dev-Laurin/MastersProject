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
#include "binData.hpp"
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

/** 
* Arguments for height map 
* Accepted arguments: 
* -binSize=<centimeters> The sizes of the squares for the map in meters. 
For example: We want an accuracy of 4cm. So -binSize=4
* -angle=<degrees> The angle of the Kinect (X axis) in degrees from the flat floor plane. 
* -height=<centimeters> The height of the Kinect from the ground. 
* -clearance=<centimeters> The height where the robot can clear if points are found at this height or above. 
*/ 
int main(int argc, char *argv[])
/// [main]
{
 /////////////////////Height Map Code////////////////////////
 //Can change these as needed (see args first)
  //Variables needed for coordinate change to world coords 
  double cameraHeight = 0.0; //meters 
  double xAxisAngleRotation = 0; //degrees 
  double heightClearance = 5.0; //meters 
  double drivableCM = 1.0; //at what centimeter value can the robot drive over?

  //Variables needed for bin creation/height map 
  //The Kinect's width and depth that's viewable by sensor in Meters
 double kinectMinX = -3.2; 
 double kinectMinZ = 0.4; 
 double binSize = 0.04; //meters 
 double kinectMaxXWindow = 6.4; //Meters 
 

  //////////////////////////////////////////////////////////////

  ///
  std::string program_path(argv[0]);
  std::cerr << "Version: " << LIBFREENECT2_VERSION << std::endl;
  std::cerr << "Environment variables: LOGFILE=<protonect.log>" << std::endl;
  std::cerr << "Usage: " << program_path << " [-binSize=<sizeInCentiMeters> -angle=<degrees> -height=<heightOfKinectFromGroundInCentiMeters> -gpu=<id>] [gl | cl | clkde | cuda | cudakde | cpu] [<device serial>]" << std::endl;
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
    else if(arg.substr(0, 9) == "-binSize="){
      //Get our argument number 
      std::string binStr = arg.substr(9, arg.size()-1);  

      //convert str to number 
      binSize = atoi(binStr.c_str()); 

      //Convert from centimeters to meters 
      binSize /= 100.0; 

      if(binSize == 0){
        std::cerr << "Bin Size cannot be 0." << endl;
        return 0; 
      }
    }
    else if(arg.substr(0, 7) == "-angle="){
      std::string angle = arg.substr(7, arg.size()-1); 
      xAxisAngleRotation = atoi(angle.c_str()); 
      //Convert to radians 
      const double PI = 3.1415926535897; 
      //mult by -1 because we are rotating down (right)
      xAxisAngleRotation = ((xAxisAngleRotation * PI) / 180.0) * -1; 
    }
    else if(arg.substr(0, 8) == "-height="){
      std::string height = arg.substr(8, arg.size()-1); 
      cameraHeight = atoi(height.c_str()); 
      //Convert from centimeters to meters 
      cameraHeight /= 100.0; 
    }
    else if(arg.substr(0, 11) == "-clearance="){
      std::string height = arg.substr(11, arg.size()-1); 
      heightClearance = atoi(height.c_str()); 
      //Convert from centimeters to meters 
      heightClearance /= 100.0; 
    }
    else if(arg.substr(0, 16) == "-maxCmDriveable="){
      std::string str = arg.substr(16, arg.size()-1); 
      drivableCM = atoi(str.c_str()); 
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
      return -1; 
    }
  }

  //Do not change these
 int width = kinectMaxXWindow/binSize; //meters 
 vector<vector<binData>> bins(width,
      vector<binData>(width , binData())); 
  //keep track of which point is the biggest in the Y 
  vector<vector<Point>> maximums(width, 
        vector<Point>(width, Point(0,0,0,0)));

  //initialize grid size for JS visualization 
  ofstream jsVariablesFile("variables.js"); 
  jsVariablesFile << "//Size of grid based on arguments" << endl; 
  jsVariablesFile << "var gridSize = " << width << ";" << endl; 
  jsVariablesFile << "var binSize = " << binSize << ";" << endl; 
  jsVariablesFile << "var cmCanDrive = " << drivableCM << ";" << endl;
  jsVariablesFile << "var clearance = " << heightClearance << ";" << endl; 
  jsVariablesFile.close(); 
  //////////////////////////////////

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
///////////////////////////////Height Map Code//////////////////////////////
      //(RGB frame, Depth Frame, Undistored Depth frame, Color image for depth image, 
        //enable filter)
      registration->apply(rgb, depth, &undistorted, &registered, true);

      //write all our raw points to file for graphing / analyzing 
       ofstream rawFile("rawPoints.csv"); 
       rawFile << "X,Y,Z,W" << endl; 

      //MARK: Getting Point Data
      vector<Point> framePoints(undistorted.width*undistorted.height); 
      int index = 0; 
      for(unsigned int i=0; i<undistorted.height; ++i){
        for(unsigned int j=0; j<undistorted.width; ++j){
          float x,y,z; 

          //IF color is not needed can use getPointXYZ() for faster
            //computation 
          registration->getPointXYZ(&undistorted, i, j, x, y, z);  
          Point p(x,y,z); 
          framePoints[index] = p; 
          if(std::isnan(p.x) || std::isnan(p.y) || 
      std::isnan(p.z)){
            
          }
          else{
            rawFile <<  p.x << "," << p.y << "," << p.z << "," << endl;
          }
          ++index; 
        }
      }
      rawFile.close(); 
      

      
      //Transform Points to World Space 
      transformPoints(framePoints, xAxisAngleRotation, cameraHeight); 

      //Filter bad points 
      filterPoints(framePoints, heightClearance); 

      // ofstream fp("fp.csv"); 
      // fp << "X,Y,Z," << endl; 
      // for(int i=0; i<framePoints.size(); i++){
      //   fp << framePoints[i].x << "," << framePoints[i].y << ",";
      //   fp << framePoints[i].z << "," << endl; 
      // }
      // fp.close(); 

      //MARK: Segment Into Bins 
      //place points into bins based on their x & z value
      segmentIntoObjects(framePoints, binSize, 
        maximums, bins, kinectMinX, kinectMinZ, xAxisAngleRotation,
        cameraHeight, heightClearance);

     //  //Save all points gathered in bins to file for statistical analysis
 
      //MARK: Save maximums to JS file for viewing later 
     ofstream jsFile("data.js"); 
     initJSFile(jsFile, "grid");
     writeToJS(maximums, jsFile); 

     endWritingPoints(jsFile); 
     writeVariable(jsFile, width, width);

     jsFile.close(); 

      

/////////////////////////////////////////////////////////////////////

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

  delete registration;

  return 0;
}
