#include <iostream>
using std::cout;
using std::endl;
#include <unistd.h> //sleep()
using std::flush; 
#include <string>
using std::string;
using std::cin; 
#include <libfreenect2/libfreenect2.hpp>
using libfreenect2::libfreenect2; 

int main(){

	//Hi Animation
	cout << "hi" << flush;
	for(;;){
		sleep(1);
		cout << "\b\bih" << flush;
		sleep(1);
		cout << "\b\bhi" << flush;
	}

	//Initialize one Kinect Device
	libfreenect2::Freenect2 freenect2; 
	libfreenect2::Freenect2Device *dev = 0;
	libfreenect2::PacketPipeline *pipeline = 0; 
	string serial = ""; 

	//Enumerate all Kinect2 devices
		//if no devices connected
	if(freenect2.enumerateDevices()==0){
		cout << "No devices connected." << endl; 
		return -1; 
	}

	//Get Default Device
	if(serial==""){
		serial = freenect2.getDefaultDeviceSerialNumber();
	}

	//Open Device
	dev = freenect2.openDevice(serial); 

	if(dev==0){
		cout << "Error opening device." << endl; 
		return -1; 
	}

	//Setup Listeners for Frames
	bool enable_rgb = true;
	bool enable_depth = true;
	int types = 0; 
	//Get frame from each type first
	if(enable_rgb){
		types |= libfreenect2::Frame::Color;
	}
	if(enable_depth){
		types |= libfreenect2::Frame:Ir | libfreenect2::Frame::Depth;
	}

	libfreenect2::SyncMultiFrameListener listener(types);
	libfreenect2::FrameMap frames;

	dev->setColorFrameListener(&listener);
	dev->setIrAndDepthFrameListener(&listener);

	//Start the Device
	if(!dev->start())
		return -1; 
	else{
		if(!dev->startStreams(enable_rgb, enable_depth))
			return -1; 
	}

	cout << "Device Serial: " << dev->getSerialNumber() << endl; 
	cout << "Device Firmware: " << dev->getFirmwareVersion() << endl; 

	//Registration = depth camera calibration & enables frame data to be fed into other programs
	libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), 
		dev->getColorParams());
	libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4); 


	size_t framemax = -1; 
	size_t framecount = 0; 

	//Get Image Frames
	while(!protonect_shutdown && (framemax == (size_t)-1 || framecount < framemax)){
		if(!listener.waitForNewFrame(frames, 10*1000)){ //10 seconds
			cout << "Timeout." << endl; 
			return -1;
		} 
		libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color]; 
		libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
		libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth]; 

		//Do stuff with frames

		//Finished with Frame 
		listener.release(frames); 
	}

	dev->stop();
	dev->close();

	delete registration; 


	return 0; 
}
