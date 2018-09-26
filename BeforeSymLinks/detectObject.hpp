//detectObject.hpp
//Laurin Fisher
//2-16-18
//The functions used to detect objects in the Kinect's frame.
//Uses RANSAC to find the floor plane, then objects detected 
	//above it are obstacles. 
#ifndef DETECTOBJECT_HPP
#define DETECTOBJECT_HPP

#include <random>
using std::mt19937; 
#include <vector>
using std::vector; 
#include "point.hpp"
#include <cmath> 
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h> 
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

//find the floor plane
	//RANSAC
void findFloorPlane(vector<Point>& threeD, 
	vector<float>&plane, mt19937 gen, 
	std::uniform_int_distribution <int> xDistro, 
	vector<float>&normal, Point&pointOnPlane){

	//1. Randomly choose a point

	//X
	int xrandIndex = xDistro(gen); 

	//get the pixel grid neighborhood of the point
	vector<Point> randomPoints(3);
	randomPoints[0]=(threeD[xrandIndex]); 

	//2nd Random Point 
	xrandIndex = xDistro(gen);
	randomPoints[1]=(threeD[xrandIndex]); 

	//3rd Random Point 
	xrandIndex = xDistro(gen); 
	randomPoints[2]=(threeD[xrandIndex]); 

	//save one of the points on the plane
	pointOnPlane = threeD[xrandIndex]; 
 
	//make 2 vectors from the 3 points in neighbors
	vector<float> AB{randomPoints[1].x - randomPoints[0].x, 
		randomPoints[1].y - randomPoints[0].y, 
		randomPoints[1].z - randomPoints[0].z};
	vector<float> AC{randomPoints[2].x - randomPoints[0].x, 
		randomPoints[2].y - randomPoints[0].y, 
		randomPoints[2].z - randomPoints[0].z}; 

	//Get the normal/orthogonal vector using cross product
	normal[0] = AB[1]*AC[2] - AB[2]*AC[1]; //i 
	normal[1] = -(AB[0]*AC[2] - AB[2]*AC[0]); //j 
	normal[2] =	AB[0]*AC[1] - AB[1]*AC[0]; //k

	//Make the plane out of the normal and one of the points
	float a = randomPoints[0].x; 
	float b = randomPoints[0].y; 
	float c = randomPoints[0].z; 
	float d = a*normal[0] + b*normal[1] + c*normal[2]; 

	if(normal[0]==0 and normal[1]==0 and normal[2]==0){
		//cout << "The zero vector, next 3 points." << endl;
		return findFloorPlane(threeD, plane, gen, 
			xDistro, normal, pointOnPlane); 
	}

	plane[0] = normal[0];
	plane[1] = normal[1];
	plane[2] = normal[2]; 
	plane[3] = d; 

	// int pointCount = 0; 
	// //check to see how many points agree with this plane
	// for(size_t i=0; i<threeD.size(); ++i){
	// 	float val = threeD[i].x*normal[0] + threeD[i].y*normal[1] + threeD[i].z*normal[2] - d; 
	// 	if(val > -1e-5 and val < 1.e-5){
	// 		//point agrees
	// 		++pointCount; 
	// 	}
	// }
}


//Draws the 3D plane onto the 2D color frame 
void drawFloorPlane(libfreenect2::Frame * rgb_frame, 
	const vector<float>&plane){

	//Transform plane into 2D pixels

	//Change the color of the rgb frame of those pixels
		//to draw our plane 
	//.width, .height, .data (unsigned * char)

	//Test
	rgb_frame->data[0] = 5.32613 * pow(10,-39); 
}

//Filters out object points that are above floor plane
void filterIntoObjectPointsOnly(vector<Point>& threeD,
	vector<float>&plane, vector<Point>&filteredPoints,
	vector<float>&normal, Point&onPlane){

	int filterIndex = 0; 

	for(size_t i=0; i<threeD.size(); ++i){
		//Check if point is invalid
		if(threeD[i].x != -1){
			//Check if point is above the plane -> Could increase speed by checking 
						//if point is not on plane, but if floor plane
						// isn't perfect then points could be under 
						//the floor
			//Get point that lies within plane 
			//Get vector from point in plane to this point
			vector<float>AB{onPlane.x - threeD[i].x, 
						onPlane.y - threeD[i].y, 
						onPlane.z - threeD[i].z }; 
			//Measure angle with computed vector and normal of plane
				//According to: https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors#16544330
			double dotProduct = onPlane.x*threeD[i].x + onPlane.y*threeD[i].y + 
				onPlane.z*threeD[i].z; 
			double magnitude0 = onPlane.x*onPlane.x + onPlane.y*onPlane.y +
				onPlane.z*onPlane.z; 
			double magnitude1 = threeD[i].x*threeD[i].x + threeD[i].y*threeD[i].y +
				threeD[i].z*threeD[i].z; 
			double angle = acos(dotProduct/sqrt(magnitude0 * magnitude1));
			//Convert to degrees from radians
			angle = angle * 180 / 3.14159265; 
			//If the angle is 
				//0,90,180,270 = on plane 
				//positive bet 0-90, 90-180
				//negative - shouldn't happen 
			if((angle<90 and angle>0) or(angle<180 and angle>90)){
				//above plane 
				filteredPoints[filterIndex] = threeD[i];
				++filterIndex;  
			}

			//Check if my logic is correct? 
			if(angle<0){
				cout << "Error! angle is 0 in filterIntoObjectPointsOnly" << endl;
			}
		}

	}
}

//Found the floor plane, now segment into objects
	//using depth data & Euclidean Distance
void segmentIntoObjects(vector<vector<int>> & objects, 
	vector<float>&plane, vector<Point>& threeD, 
	vector<float>&normal, Point&onPlane){
	//get all the points that are not on or under the plane
	vector<Point> filteredPoints(threeD.size(), Point(-1,-1,-1,-1)); 
	filterIntoObjectPointsOnly(threeD, plane, filteredPoints,
		normal, onPlane); 

	//Segment the 3D points into objects 

}

#endif /* DETECTOBJECT_HPP */ 