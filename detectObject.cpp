//detectObject.cpp
//Laurin Fisher
//
#include <iostream>
// using std::cout; 
// using std::endl; 
#include <random>
using std::mt19937; 
#include <vector>
using std::vector; 
#include "point.hpp"
//#include <ctime>

int main(){

	/*
		| 3 2 2 | 
		| 4 5 7 | 
		| 3 2 1 |

		(row, column) 
	*/ 


	vector<vector<Point> > threeD; //vector of 3D points
	vector<Point>v; 

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


	for(int i=0; i<v.size(); i++){
		vector<Point> newV;
		newV.push_back(v[i]); 
		v[i].draw();  
		threeD.push_back(newV); 
		threeD[i][0].draw(); 
	}

	mt19937 gen(time(0)); 
	std::uniform_int_distribution<int>xDistro(0, threeD.size()-1);
	std::uniform_int_distribution<int>yDistro(0, threeD[0].size()-1);
	

	vector<float>plane(4); 
	vector<float>normal(3); 
	Point onPlane; 
	findFloorPlane(threeD, plane, gen, xDistro, yDistro, normal,
		onPlane); 
	cout << "Plane found: " << endl; 
	for(int i=0; i<plane.size(); ++i){
		cout << plane[i] << endl; 
	}

	
}