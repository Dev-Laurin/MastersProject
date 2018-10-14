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


//find the floor plane
	//RANSAC
//robotSize: height in meters to Kinect from Floor
void findFloorPlane(const vector<Point>& threeD, 
	vector<float>&plane, mt19937 gen, 
	std::uniform_int_distribution <int> xDistro, 
	vector<float>&normal, Point&pointOnPlane, 
	const vector<float>&gravityNormal, const float&robotSize){


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

	//DOT PRODUCT INFO: 
	/*	Vectors orthogonal (90 deg) = 0
		Vectors opposite = -1
		Range -0.99 to -1 
	*/

	//If we find the normal vector, or we find that
		//the normal is not in the up direction, try again
	//1. angle bet gravity normal & this normal 
	float dotProduct = gravityNormal[0]*normal[0] +
	gravityNormal[1]*normal[1] + gravityNormal[2]*normal[2];

	//3. Threshold distance (not shorter than layla)
	//cosTheta should be < 5 deg or 0.08726646 radians difference 

		//The plane is relatively flat (drivable) and
			//is not closer to the Kinect than the 
			//robot's wheels 

		// = Most likely to be floor plane 


	//2. # points
		//This is a bad threshold value due to the possibility
			//of a cluttered floor -> not implementing 

	if((normal[0]==0 and normal[1]==0 and normal[2]==0)
		|| (dotProduct < -0.45) || (dotProduct > 0.45)  || //check angle from gravity vector & 
		plane[1]>=robotSize){ //y axis (height) 
		
		//check if it's less than the robot size
		//Not floor plane, next 3 points.
	
	/*	const vector<Point>& threeD, 
	vector<float>&plane, mt19937 gen, 
	std::uniform_int_distribution <int> xDistro, 
	vector<float>&normal, Point&pointOnPlane, 
	const vector<float>&gravityNormal, const float&robotSize
	*/
		cout << "DID not find floor plane." << endl; 
		cout << "(" << normal[0] << ", " << normal[1] << ", " << 
		normal[2] << ")" << endl; 
		cout << "Dot Product: " << dotProduct << " > " << -0.99 << endl; 
		cout << plane[1] << " >= " << robotSize << endl; 
		return findFloorPlane(threeD, plane, gen, 
			xDistro, normal, pointOnPlane, gravityNormal, robotSize); 
	}

	plane[0] = normal[0];
	plane[1] = normal[1];
	plane[2] = normal[2]; 
	plane[3] = d; 

	if(std::isnan(normal[0])){
		cout << " IS NAN " << endl;
		cout << "AB: < ";  
		cout << randomPoints[1].x - randomPoints[0].x; 
		cout << ", " << randomPoints[1].y - randomPoints[0].y;
		cout << ", " << randomPoints[1].z - randomPoints[0].z;
		cout << " >" << endl; 

		cout << "AC: < "; 
		cout << randomPoints[2].x - randomPoints[0].x; 
		cout << ", " << randomPoints[2].y - randomPoints[0].y;
		cout << ", " << randomPoints[2].z - randomPoints[0].z; 
		cout << " >" << endl; 
	}

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

/*
//Draws the 3D plane onto the 2D color frame 
void drawFloorPlane(libfreenect2::Frame * rgb_frame, 
	const vector<float>&plane, const vector<Point>& framePoints, 
	vector<float>&normal){

	//Transform plane into 2D pixels
		//Get all points that match that plane & turn them
			//into a different color
	vector<int>indices; 
	for(size_t i=0; i<framePoints.size(); ++i){
		float val = framePoints[i].x*normal[0] +
		 framePoints[i].y*normal[1] + 
		 framePoints[i].z*normal[2] - plane[3]; 
	 	if(val > -1e-5 and val < 1.e-5){
	 		//Point is on plane
	 		indices.push_back(i); 

	 	}
	}
	//Change the color of the rgb frame of those pixels
		//to draw our plane 
	//.width, .height, .data (unsigned * char)

	//Test
	unsigned int black = 0; 
	// for(int i=0; i<100000; ++i){
	// 	rgb_frame->data[i] = black; 
	// }
	cout << "Plane found: " << endl;
	cout << plane[0] << "x + " << plane[1] << "y + "; 
	cout << plane[2] << "z - " << plane[3] << " = 0"; 
	cout << endl; 

	cout << "Normal: " << endl; 
	cout << "< " << normal[0] << ", " << normal[1];
	cout << ", " << normal[2] << " >" << endl;

	cout << "Indices size: " << indices.size() << endl;
	for(size_t i=0; i<indices.size(); ++i){
		rgb_frame->data[indices[i]] = black; 
		////What is this? the data? 
	}

	//Draw Plane in seperate GL window for representation

} */ 

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
	//multiply by the rotation matrix

	if(xAxisRotationAngle != 0){
		//save for math correctness 
		Point kinect = point; 
		//Y 
		point.y = (kinect.y * cos(xAxisRotationAngle)) - (sin(xAxisRotationAngle) * kinect.z);  
		//Z
		point.z = kinect.y * (sin(xAxisRotationAngle) + kinect.z * cos(xAxisRotationAngle)); 
		
	}
	
	//add translation transform to point
	point.y = point.y - cameraHeightFromGround;
}

void transformPoints(vector<Point>&filteredPoints, float xAxisRotationAngle, float cameraHeightFromGround){

	vector<double>normal(3); 
	
	for(unsigned int i=0; i<filteredPoints.size(); i++){
		transformPoint(filteredPoints[i], xAxisRotationAngle, cameraHeightFromGround); 
	}
}


int roundToNextInt(double & x){
	int roundedX = 0; 
	if(x > 0){
		roundedX = (int)(x + 1.0);  
	}
	else {
		//round up if negative
		roundedX = (int)(x - 1.0); 
	}
	return roundedX; 
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

//Found the floor plane, now segment into objects
	//using depth data & Euclidean Distance
void segmentIntoObjects(vector<Point>& threeD, 
	float&robotBase, vector<vector<Point>>&maximums, 
	vector<vector<Point>>&bins){
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
		int roundedX = roundToNextInt(x); 

		//get the index in the bin 
		//shift values to the right for array index
		int index = bins.size()/2 + roundedX;
		if(index < 0){
			//error!! 
			 cout << "Error, negative index: "; 
			 cout << index << " For X: "; 
			 cout << roundedX << endl;
			
		} 
		else{

			//place point into bins 
			int z = ((int)threeD[i].z);  
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

}



#endif /* DETECTOBJECT_HPP */ 