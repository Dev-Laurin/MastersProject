//detectObject.hpp
//Laurin Fisher
//10-23-18
//The functions are used to segment 3D points into bins for a height map. 

#ifndef DETECTOBJECT_HPP
#define DETECTOBJECT_HPP

#include <random>
using std::mt19937; 
#include <vector>
using std::vector; 
#include "point.hpp"
#include <cmath> 
#include <math.h> /* isnan */ 
#include <fstream> 
using std::ofstream; 

void transformPoint(Point &point, float &xAxisRotationAngle, 
	float &cameraHeightFromGround){
	
	//X axis reversed 
	point.x = -point.x;

	//multiply by the rotation matrix
	if(xAxisRotationAngle != 0){
		//save for math correctness 
		Point kinect = point; 
		//Y 
		point.y = (kinect.y * cos(xAxisRotationAngle)) - (kinect.z * sin(xAxisRotationAngle));  
		//Z
		point.z = kinect.y * sin(xAxisRotationAngle) + kinect.z * cos(xAxisRotationAngle); 
		
		cout << "Point.y after rotation: " << endl; 
		cout << point.y << endl; 
	}
	
	//add translation transform to point
	point.y = point.y + cameraHeightFromGround;
}


int roundToNextInt(double & value, float & binSize, float & min){
	
	return (value - min)/binSize; 
}

//Filter, transform, and segment the 3D points into a 2D vector 
void segmentIntoObjects(vector<Point>& threeD, 
	float&binSize, vector<vector<Point>>&maximums, 
	vector<vector<Point>>&bins, float & kinectMinX, 
	float & kinectMinZ, float & xAxisRotationAngle, 
	float & cameraHeightFromGround){

	//Segment the 3D points into objects 
	//robot base is how big the squares will be 
	//loop through the points and do a transform based on calculated plane 
	for(unsigned int i=0; i<threeD.size(); i++){

		//Filter out nan values 
		if(std::isnan(threeD[i].x) || std::isnan(threeD[i].y) || 
			std::isnan(threeD[i].z)){
			continue; //next loop iteration 
		}

		//Transform the point to world coords 
		transformPoint(threeD[i], xAxisRotationAngle, 
			cameraHeightFromGround); 


		//Place into 2D vector bins 
		double x = threeD[i].x; 

		//Round the value up to next int 
		int index = (x - kinectMinX)/binSize; 

		//place point into bins 
		int z = (threeD[i].z - kinectMinZ)/binSize;  
		bins[index][z] = threeD[i]; 

		//Check if this point's Y is a new max 
		if(maximums[index][z].y < threeD[i].y){
			maximums[index][z] = threeD[i]; 
		}
		else if(maximums[index][z].y == 0){
			maximums[index][z] = threeD[i];
		}
	}

}



#endif /* DETECTOBJECT_HPP */ 