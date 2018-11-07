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

void transformPoint(Point &point, double &xAxisRotationAngle, 
	double &cameraHeightFromGround){
	
	//X axis reversed 
	point.x = -point.x;

	point.y = -point.y; 

	//multiply by the rotation matrix
	if(xAxisRotationAngle != 0){
		//save for math correctness 
		Point kinect = point; 
		//Y 
		point.y = (kinect.y * cos(xAxisRotationAngle)) - (kinect.z * sin(xAxisRotationAngle));  
		//Z
		point.z = kinect.y * sin(xAxisRotationAngle) + kinect.z * cos(xAxisRotationAngle); 
	}
	
	//add translation transform to point
	point.y = point.y + cameraHeightFromGround;
}

void transformPoints(vector<Point>&points, double& xAxisRotationAngle, double& cameraHeightFromGround){

	for(unsigned int i=0; i<points.size(); i++){
		//Filter out nan values 
		if(std::isnan(points[i].x) || std::isnan(points[i].y) || 
			std::isnan(points[i].z)){
			
			//An incorrect z = point is invalid (kinect cant see behind it)
			points[i] = Point(0.0,0.0, -2.0); 
		}

		transformPoint(points[i], xAxisRotationAngle, cameraHeightFromGround); 
	}
}

//How to place points into bins 
int roundToNextInt(double & value, double & binSize, double & min){
	return (value - min)/binSize; 
}

inline double distanceSquared(const Point &a,const Point &b) {
	double x=a.x-b.x;
	double y=a.y-b.y;
	double z=a.z-b.z;
	return x*x+y*y+z*z;
}

//Filter noise and bad data out 
void filterPoints(vector<Point>&points, double & heightClearance){

	for(size_t i=0; i<points.size(); i++){

		//if the point is higher than the robot 
		if (points[i].y> heightClearance){
			points[i] = Point(0.0, 0.0, -2.0); 
			continue; // robot won't hit this
		} 

	    //Check if this depth is plausible--agrees with neighbors
	    double noise=0.1; // 

	    //Left neighbors 
	    for (int nbor=-2;nbor<0;nbor++) 
	    	if (distanceSquared(points[i],points[i-nbor])>noise*noise)
	    		points[i] = Point(0.0, 0.0, -2.0);

	    //Right neighbors  
	    for(int nbor=1; nbor<3;nbor++)
	    	if (distanceSquared(points[i],points[i+nbor])>noise*noise)
	    		points[i] = Point(0.0, 0.0, -2.0); 
	}
}


//Filter, transform, and segment the 3D points into a 2D vector 
void segmentIntoObjects(vector<Point>& threeD, 
	double &binSize, vector<vector<Point>>&maximums, 
	vector<vector<vector<Point>>>&bins, double & kinectMinX, 
	double & kinectMinZ, double & xAxisRotationAngle, 
	double & cameraHeightFromGround, double &heightClearance){

	ofstream binsByIndexFile("binsByIndex.txt"); 
	for(unsigned int i=2; i<threeD.size(); i++){

		//If point has a negative Z it is invalid. The kinect 
		//cannot see behind itself. 
		if(threeD[i].z < 0){
			continue; //next loop iteration 
		}

		//Place into 2D vector bins 
		double x = threeD[i].x; 

		//Round the value up to next int 
		int index = (x - kinectMinX)/binSize; 

		//place point into bins 
		int z = (threeD[i].z - kinectMinZ)/binSize; 

		//add point to bin  
		threeD[i].draw(cout); 
		bins[index][z].push_back(threeD[i]); 

		binsByIndexFile << "x: " << index << "z: " << z << " : " << endl; 
		threeD[i].draw(binsByIndexFile); 

	//	threeD[i].draw(cout); 
	//	bins[index][z][bins[index][z].size() -1 ].draw(binFile); 

		//Check if this point's Y is a new max 
		/*
		if(maximums[index][z].y < threeD[i].y){
			maximums[index][z] = threeD[i]; 
		}
		else if(maximums[index][z].y == 0){
			maximums[index][z] = threeD[i];
		}*/
		maximums[index][z] = threeD[i];
	}

	binsByIndexFile.close(); 

	// ofstream binFile("binnedPoints.txt"); 
 //      binFile << "Size: " << bins.size() << endl; 
 //      binFile << "J: " << bins[0].size() << endl; 
 //      binFile << "K: " << bins[0][0].size() << endl; 
 //      for(int i=0; i<bins.size(); i++){
 //        binFile << "[ " << endl; 
 //        for(int j=0; j<bins[i].size(); j++){
 //          binFile << "[ " << endl; 
 //          for(int k=0; k<bins[i][j].size(); k++){
 //            bins[i][j][k].draw(binFile);
 //          }
 //          binFile << "]" << endl; 
 //        }
 //        binFile << "]" << endl; 
 //      }
 //    binFile.close(); 
 
}



#endif /* DETECTOBJECT_HPP */ 