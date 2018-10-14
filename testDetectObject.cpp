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
#include "point.hpp"
#include "detectObject.hpp"
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
	/*
	ofstream jsFile("test.js"); 
	initJSFile(jsFile, "plane"); 

	//create a fake plane 
	vector<float> plane = {2, 3, 5}; 

	writePlaneToJS(plane, jsFile); 

	endFile(jsFile); 
	*/ 
}

TEST_CASE("PlacePointsIntoBins"){
	vector<Point> points(1000, Point(1,1,1,1)); 
	float roboBase = 1; //meter 
	int width = 10; 
	vector<vector<Point>> max(width, 
        vector<Point>(width, Point(0,0,0,0)));
	vector<vector<Point>> bins(width,
      vector<Point>(width , Point(0, 0, 0, 0))); 
	segmentIntoObjects(points, roboBase, max, bins); 

	//max height should be 1 
	//cout << max[7][1].y << endl; 
	REQUIRE(max[7][1].y == 1); 

	REQUIRE(max.size() == width); 
	//cout << "size of max: " << max.size() << endl; 


	ofstream file("test.js"); 
	initJSFile(file, "grid");

	writeToJS(max, file); 

	endWritingPoints(file);
	writeVariable(file, width, width);  

}


TEST_CASE("PlacePointsIntoBins Different Points"){
	vector<Point> points(1000, Point(1,1,1,1)); 
	//Change half the points
	for(int i=points.size()/2; i<points.size(); i++){
		points[i] = Point(-2, 3, 3, 1); 
	}

	float roboBase = 1; //meter 
	int width = 10; 
	vector<vector<Point>> max(width, 
        vector<Point>(width, Point(0,0,0,0)));
	vector<vector<Point>> bins(width,
      vector<Point>(width , Point(0, 0, 0, 0))); 
	segmentIntoObjects(points, roboBase, max, bins); 

	//max height should be 1 
	REQUIRE(max[7][1].y == 1); //Note: If this fails, rest of code won't run
	//max height should be 3 
	REQUIRE(max[2][3].y == 3); 
	cout << endl; 
	cout << "max size, part 2 : " << endl; 
	cout << max.size() << endl; 

	ofstream o("output.txt");


	for(int i=0; i<max.size(); i++){
		for(int j=0; j<max[i].size(); j++){
			o << "max[" << i << "][" << j << "]: "; 
			o << max[i][j].x << ", " << max[i][j].y; 
			o << ", " << max[i][j].z << ")" << endl; 

		}
	}

	o.close(); 

	//write data to file 
	ofstream file("test.js"); 
	initJSFile(file, "grid");

	writeToJS(max, file); 

	endWritingPoints(file);
	writeVariable(file, width, width);  

	file.close(); 

}

TEST_CASE("Test segmentIntoObjects with real data (static)"){
	vector<Point> points = {Point( 1.01761, -2.58591, 1.98454, 0 ), 
		Point( 2.25616, -3.17233, 4.32981, 0), Point(0.469131, -2.78297, 0.999927, 0), 
		Point(1.09255, -2.55152, 2.09672, 0 )
	}; 
	//Change half the points
	for(int i=points.size()/2; i<points.size(); i++){
		points[i] = Point(-2, 3, 3, 1); 
	}

	float roboBase = 1; //meter 
	int width = 10; 
	vector<vector<Point>> max(width, 
        vector<Point>(width, Point(0,0,0,0)));
	vector<vector<Point>> bins(width,
      vector<Point>(width , Point(0, 0, 0, 0))); 
	segmentIntoObjects(points, roboBase, max, bins); 

	 
	REQUIRE(max[5][2].y > -2.58); //Note: If this fails, rest of code won't run
	
	cout << endl; 
	cout << "max size, part 2 : " << endl; 
	cout << max.size() << endl; 

	ofstream o("output.txt");


	for(int i=0; i<max.size(); i++){
		for(int j=0; j<max[i].size(); j++){
			o << "max[" << i << "][" << j << "]: "; 
			o << max[i][j].x << ", " << max[i][j].y; 
			o << ", " << max[i][j].z << ")" << endl; 

		}
	}

	o.close(); 

	//write data to file 
	ofstream file("test.js"); 
	initJSFile(file, "grid");

	writeToJS(max, file); 

	endWritingPoints(file);
	writeVariable(file, width, width);  

	file.close(); 
}

TEST_CASE("Test segmentIntoObjects from recorded real data. (bins)"){
	vector<Point> points = {Point( 1.01761, -2.58591, 1.98454, 0 ), 
		Point( 2.25616, -3.17233, 4.32981, 0), Point(0.469131, -2.78297, 0.999927, 0), 
		Point(1.09255, -2.55152, 2.09672, 0 )
	}; 

	float roboBase = 1; //meter 
	int width = 10; 
	vector<vector<Point>> max(width, 
        vector<Point>(width, Point(0,0,0,0)));
	vector<vector<Point>> bins(width,
      vector<Point>(width , Point(0, 0, 0, 0))); 
	segmentIntoObjects(points, roboBase, max, bins); 

	//check that points went into the correct bins 
	REQUIRE(max[7][1].y > -2.58);  
	REQUIRE(max[1][0].y < -2.7); 
	REQUIRE(max[3][4].y < -3.1); 

}

//check for nan in y 
//make sure transforms are correct
TEST_CASE("Test the transforming of points"){
	//No transform needed 
	vector<Point> points = {Point( 1.01761, -2.58591, 1.98454, 0 ), 
		Point( 2.25616, -3.17233, 4.32981, 0), Point(0.469131, -2.78297, 0.999927, 0), 
		Point(1.09255, -2.55152, 2.09672, 0 )
	}; 

	float angle = 0.0; 
	float cameraHeight = 0.0; 
	transformPoints(points, angle, cameraHeight); 

	REQUIRE(points[0].x == 1.01761); 
	REQUIRE(points[0].y == -2.58591); 
	REQUIRE(points[0].z == 1.98454); 

	//

}