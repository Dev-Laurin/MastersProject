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
	vector<vector<vector<double>>>&bins, double & kinectMinX, 
	double & kinectMinZ, double & xAxisRotationAngle, 
	double & cameraHeightFromGround, double &heightClearance){

//	ofstream binsByIndexFile("binsByIndex.txt"); 
	// ofstream filterPoints("filterPoints.csv");
	// filterPoints << "X,Y,Z,W" << endl;  
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

	//	binsByIndexFile << "x: " << index << "z: " << z << " : " << endl; 
	//	threeD[i].draw(binsByIndexFile); 

		// filterPoints << threeD[i].x << "," << threeD[i].y << ",";
		// filterPoints << threeD[i].z << "," << endl; 

	//	threeD[i].draw(cout); 
	//	bins[index][z][bins[index][z].size() -1 ].draw(binFile); 

		//Bins Mean Trimmed Variables 
		//SUM 
		bins[index][z][0] += threeD[i].y; 
		//Maximum 
		if( bins[index][z][1] < threeD[i].y) 
			bins[index][z][1] = threeD[i].y; 
		//Minimum 
		if( bins[index][z][2] > threeD[i].y )
			bins[index][z][2] = threeD[i].y; 
		//Total points 
		bins[index][z][3]++; 
		//Squared Sum 
		bins[index][z][4] += threeD[i].y * threeD[i].y; 
		//Variance so far 
		/* 
	    //Get Variance without storing all data until end and looping through
	    First, I added up all of the numbers:1 + 2 + 3 + 4 + 5 = 15
	    I squared the total, and then divided the number of items in the data set 15 x 15 = 225
	    225 / 5 = 45
	    I took my set of original numbers from step 1, squared them individually this time, and added them all up:(1 x 1) + (2 x 2) + (3 x 3) + (4 x 4) + (5 x 5) = 55
	    I subtracted the amount in step 2 from the amount in step 3:55 - 45 = 10
	    I subtracted 1 from the number of items in my data set:5 - 1 = 4
	    I divided the number in step 4 by the number in step 5:10 / 4 = 2.5
	    This is my Variance!
	    Finally, I took the square root of the number from step 6 (the Variance),
	    √(2.5) = 1.5811388300841898
	    This is my Standard Deviation!
	    ///From: https://www.statisticshowto.datasciencecentral.com/calculators/variance-and-standard-deviation-calculator/
	    */ 
		double sumSquared = bins[index][z][0] * bins[index][z][0]; 
		bins[index][z][5] = (bins[index][z][4] - (sumSquared / bins[index][z][3])) / (bins[index][z][3] - 1); 
		double trimmedMean = (bins[index][z][0] - bins[index][z][1] - bins[index][z][2]) / (bins[index][z][3] - 2); 
		//Check if this point's Y is a new max 
		/*
		if(maximums[index][z].y < threeD[i].y){
			maximums[index][z] = threeD[i]; 
		}
		else if(maximums[index][z].y == 0){
			maximums[index][z] = threeD[i];
		}*/

		threeD[i].y = trimmedMean; 
		if(std::isinf(trimmedMean) || std::isnan(trimmedMean))
			threeD[i].y = bins[index][z][1]; 
		maximums[index][z] = threeD[i];
	}
//filterPoints.close(); 
//	binsByIndexFile.close(); 

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