//writeToJSFile.hpp
//Laurin Fisher
//9-9-18
//Functions to use to write values obtained from the C++ program to a JS file for easy 3D viewing. 
#include <iostream> 
#include <fstream> 
using std::ofstream; 

//initialize the JS file to store the variables correctly.
void initJSFile(ofstream & file, string & variableName){
	file << "var " + variableName + " = ["; 
}

//write to the JS file 
void writeToJS(vector<Point>points, ofstream & file){
	file << "{x: " << points[0] << ", y: " << plane[1]; 
	file << ", z: " << plane[2] << " }, " << endl;  
}

void endFile(ofstream & file){
	file << " ] " << endl; 
	file.close(); 
}