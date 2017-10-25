#include <iostream>
using std::cout;
using std::endl;
#include <unistd.h> //sleep()
using std::flush; 
#include <string>
using std::string;
using std::cin; 
#include <libfreenect2/libfreenect2.hpp>

int main(){

	//Hi Animation
	cout << "hi" << flush;
	for(;;){
		sleep(1);
		cout << "\b\bih" << flush;
		sleep(1);
		cout << "\b\bhi" << flush;
	}

	//Find Kinect Devices


	return 0; 
}
