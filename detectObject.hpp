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
#include <math.h> /* isnan */ 
#include <fstream> 
using std::ofstream; 
//#include <libfreenect2/frame_listener_impl.h>


void filterPoints(vector<Point>&points, vector<Point>&
	filteredPoints){

	int index = 0; 
	for(size_t i=0; i<points.size(); ++i){
		if(!(std::isnan(points[i].x)) && !(std::isnan(points[i].y)) && 
			!(std::isnan(points[i].z))){
			//Put into filtered
			filteredPoints[index] = points[i]; 
			++index; 
		}
	}
	//The real size of the vector
	filteredPoints.resize(++index); 
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

void transformPoint(Point &point, float &xAxisRotationAngle, float &cameraHeightFromGround){
	
	//X axis reversed 
	point.x = -point.x;

	const double PI = 3.1415926535897; 

	//multiply by the rotation matrix
	if(xAxisRotationAngle != 0){
		//save for math correctness 
		Point kinect = point; 
		//Y 
		point.y = (kinect.y * cos((xAxisRotationAngle * PI) / 180.0)) - (sin((xAxisRotationAngle * PI) / 180.0) * kinect.z);  
		//Z
		point.z = kinect.y * sin((xAxisRotationAngle * PI) / 180.0) + kinect.z * cos((xAxisRotationAngle * PI) / 180.0); 
		
		cout << "Point.y after rotation: " << endl; 
		cout << point.y << endl; 
	}
	
	//add translation transform to point
	point.y = point.y - cameraHeightFromGround;
}

void transformPoints(vector<Point>&filteredPoints, float xAxisRotationAngle, float cameraHeightFromGround){

	for(unsigned int i=0; i<filteredPoints.size(); i++){
		transformPoint(filteredPoints[i], xAxisRotationAngle, cameraHeightFromGround); 
	}
}


void getFloorPlane(vector<float>&plane, float xAxisRotationAngle, float cameraHeightFromGround){
	//3 points we know are on the plane 
	Point one(0,0,0,0);
	Point two(0,0,1,0); 
	Point three(0,0,2,0);

	transformPoint(one, xAxisRotationAngle, cameraHeightFromGround); 
	transformPoint(two, xAxisRotationAngle, cameraHeightFromGround); 
	transformPoint(three, xAxisRotationAngle, cameraHeightFromGround); 
	
	//turn 3 points in new coordinate system into plane 
		//make 2 vectors from the 3 points
	vector<float> AB{two.x - one.x, 
		two.y - one.y, 
		two.z - one.z};
	vector<float> AC{three.x - one.x, 
		three.y - one.y, 
		three.z - one.z}; 

	//Get the normal/orthogonal vector using cross product
	vector<float>normal(3); 
	normal[0] = AB[1]*AC[2] - AB[2]*AC[1]; //i 
	normal[1] = -(AB[0]*AC[2] - AB[2]*AC[0]); //j 
	normal[2] =	AB[0]*AC[1] - AB[1]*AC[0]; //k

	//Make the plane out of the normal and one of the points
	float a = two.x; 
	float b = two.y; 
	float c = two.z; 
	float d = a*normal[0] + b*normal[1] + c*normal[2]; 

	plane[0] = a; 
	plane[1] = b; 
	plane[2] = c; 
	plane[3] = d; 
}


int roundToNextInt(double & value, float & binSize, float & min){
	
	return (value - min)/binSize; 
}

//Found the floor plane, now segment into objects
	//using depth data & Euclidean Distance
void segmentIntoObjects(vector<Point>& threeD, 
	float&binSize, vector<vector<Point>>&maximums, 
	vector<vector<Point>>&bins, float & kinectMinX, 
	float & kinectMinZ){
	//get all the points that are not on or under the plane
	/* IS THIS NEEDED? IF points are under than it estimates a hole
	vector<Point> filteredPoints(threeD.size(), Point(-1,-1,-1,-1)); 
	filterIntoObjectPointsOnly(threeD, plane, filteredPoints,
		normal, onPlane);
	*/  

	//Segment the 3D points into objects 
	//robot base is how big the squares will be 
	//loop through the points and do a transform based on calculated plane 
	for(unsigned int i=0; i<threeD.size(); i++){

		if(std::isnan(threeD[i].x) || std::isnan(threeD[i].y) || 
			std::isnan(threeD[i].z)){
			continue; //next loop iteration 
		}

		double x = threeD[i].x; 

		//Round the value up to next int 
		int index = roundToNextInt(x, binSize, kinectMinX); 

		//place point into bins 
		int z = roundToNextInt(threeD[i].z, binSize, kinectMinZ);  
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