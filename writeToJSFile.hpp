//writeToJSFile.hpp
//Laurin Fisher
//9-9-18
//Functions to use to write values obtained from the C++ program to a JS file for easy 3D viewing. 
#include <iostream> 
#include <fstream> 
#include <string> 
using std::string; 
using std::ofstream; 

//initialize the JS file to store the variables correctly.
void initJSFile(ofstream & file, string variableName){
	file << "//Author: Laurin Fisher" << endl; 
	file << "//Date Revised: " << endl; 
	file << "//Purpose: To transfer data from C++ to "; 
	file << "Javascript for my Master's Project in "; 
	file << "Kinect Object Detection." << endl; 

	file << endl; 
	file << "//Our point data in bins. X bins, then "; 
	file << "inside those are Z bins." << endl; 
	file << "var " + variableName + " = ["; 
}

//write to the JS file 
void writeToJS(vector<vector<Point>>points, ofstream & file){

	for(unsigned int i=0; i<points.size(); i++){
		file << " [ "; 
		for(unsigned int j=0; j<points[i].size(); j++){
			file << "{x: " << points[i][j].x << ", y: ";
			file << points[i][j].y; 
			file << ", z: " << points[i][j].z << " }, " << endl; 
		}
		file << " ], " << endl; 
	} 
}

void endWritingPoints(ofstream & file){
	file << " ]; " << endl; 
	file << endl; 
}

void writeVariable(ofstream & file, int viewWidth, 
	int viewDepth){
	file << "//The width of our kinect view (how"; 
	file << " many squares in X direction)." << endl; 
	file << "var viewWidth = " << viewWidth << " ;" << endl; 

	file << "//The depth of our kinect view (how"; 
	file << " many squares in Z direction)." << endl; 
	file << "var viewDepth = " << viewDepth << " ;" << endl; 
}