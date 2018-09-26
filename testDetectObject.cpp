//testDetectObject.cpp
//Laurin Fisher
//
#include <iostream>
using std::cout; 
using std::endl; 
#include <random>
using std::mt19937; 
#include <vector>
using std::vector; 
//include "point.hpp"
//#include "detectObject.hpp"
#include "writeToJSFile.hpp"
//#include <ctime>

//Tells catch to provide a main (one file only)
#define CATCH_CONFIG_MAIN
#include "Catch2.hpp" //C++ Testing Framework

/*
TEST_CASE("Testing findFloorPlane: normal vector != 0", 
	"Using basic block & floor setup."){

	//Basic block & floor
	vector<Point>v; //vector of 3D points 

	//box points
	v.push_back(Point(1.0,1.0,1.0,0.0)); 
	v.push_back(Point(2.0,1.0,1.0,0.0)); 
	v.push_back(Point(1.0,0.5,1.0,0.0)); 
	v.push_back(Point(2.0,0.5,1.0,0.0)); 

	//floor points
	v.push_back(Point(0.0,2.0,2.0,0.0)); 
	v.push_back(Point(3.0,2.0,2.0,0.0)); 
	v.push_back(Point(3.0,1.0,2.0,0.0));
	v.push_back(Point(3.0,0.0,2.0,0.0)); 
	v.push_back(Point(0.0,0.0,2.0,0.0)); 
 	v.push_back(Point(0.0,1.0,2.0,0.0)); 

 	//Make the random generator & distribution
	mt19937 gen(time(0)); 
	std::uniform_int_distribution<int>xDistro(0, v.size()-1);

	//Make the variables 
	vector<float>plane(4); 
	vector<float>normal(3); 
	Point onPlane; 

	//Call the function
	findFloorPlane(v, plane, gen, xDistro, normal,
		onPlane);
	
	//Test if normal vector is 0 
	if(normal[0]==0 and normal[1]==0 and normal[2]==0)
		REQUIRE(1!=1);
	else
		REQUIRE(0==0); //the normal vec is not zero, true
}

TEST_CASE("Testing findFloorPlane: plane actually contains 3 points", 
	"Using basic block & floor setup."){

	//Basic block & floor
	vector<Point>v; //vector of 3D points 

	//box points
	v.push_back(Point(1.0,1.0,1.0,0.0)); 
	v.push_back(Point(2.0,1.0,1.0,0.0)); 
	v.push_back(Point(1.0,0.5,1.0,0.0)); 
	v.push_back(Point(2.0,0.5,1.0,0.0)); 

	//floor points
	v.push_back(Point(0.0,2.0,2.0,0.0)); 
	v.push_back(Point(3.0,2.0,2.0,0.0)); 
	v.push_back(Point(3.0,1.0,2.0,0.0));
	v.push_back(Point(3.0,0.0,2.0,0.0)); 
	v.push_back(Point(0.0,0.0,2.0,0.0)); 
 	v.push_back(Point(0.0,1.0,2.0,0.0)); 

 	//Make the random generator & distribution
	mt19937 gen(time(0)); 
	std::uniform_int_distribution<int>xDistro(0, v.size()-1);

	//Make the variables 
	vector<float>plane(4); 
	vector<float>normal(3); 
	Point onPlane; 

	//Call the function
	findFloorPlane(v, plane, gen, xDistro, normal,
		onPlane);
	
	//test if the plane contains at least 3 points from the dataset
	int pointsInPlane = 0; 
	for(int i=0; i<v.size(); ++i){
		//using the plane equation, calculate if point is in
		if((plane[0]*v[i].x + plane[1]*v[i].y + plane[2]*v[i].z - plane[3])==0){
			//is in plane add to count
			++pointsInPlane; 
		}
	}

	REQUIRE(pointsInPlane>=3); 
}

TEST_CASE("Testing findFloorPlane: test if correct floor plane is eventually found", 
	"Using basic block & floor setup."){

	//Basic block & floor
	vector<Point>v; //vector of 3D points 

	//floor points
	v.push_back(Point(0.0,2.0,2.0,0.0)); 
	v.push_back(Point(3.0,2.0,2.0,0.0)); 
	v.push_back(Point(3.0,1.0,2.0,0.0));
	v.push_back(Point(3.0,0.0,2.0,0.0)); 
	v.push_back(Point(0.0,0.0,2.0,0.0)); 
 	v.push_back(Point(0.0,1.0,2.0,0.0)); 

 	//Make the random generator & distribution
	mt19937 gen(time(0)); 
	std::uniform_int_distribution<int>xDistro(0, v.size()-1);

	//Make the variables 
	vector<float>plane(4); 
	vector<float>normal(3); 
	Point onPlane; 

	//Call the function 
	int pointsInPlane = 0;
	findFloorPlane(v, plane, gen, xDistro, normal,
			onPlane);		

	//test if all 6 floor points are in plane
	pointsInPlane = 0; 
	for(int i=0; i<v.size(); ++i){	
		//using the plane equation, calculate if point is in 
		auto val = (plane[0]*v[i].x + plane[1]*v[i].y + plane[2]*v[i].z - plane[3]); 
		if(val > -1e-5 and val < 1.e-5){
			//is in plane add to count
			++pointsInPlane; 
		}
	}

	REQUIRE(pointsInPlane==6); 
}
*/
TEST_CASE("Testing writing to JS file."){
	ofstream jsFile("test.js"); 
	initJSFile(jsFile); 

	//create a fake plane 
	vector<float> plane = {2, 3, 5}; 

	writePlaneToJS(plane, jsFile); 

	endFile(jsFile); 
}