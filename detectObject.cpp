//detectObject.cpp
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
#include <ctime>


//find the floor plane
	//RANSAC
void findFloorPlane(vector<vector<Point > >&  threeD, 
	vector<float>&plane, mt19937 gen, 
	std::uniform_int_distribution <int> xDistro,
	std::uniform_int_distribution<int> yDistro
	){

	cout << "Plane size: " << plane.size() << endl; 

	//1. Randomly choose a point

	//X
	int xrandIndex = xDistro(gen); 

	cout << xrandIndex << endl; 

	//Y
	int yrandIndex = yDistro(gen); 

	cout << "Point chosen: " << endl; 
	threeD[xrandIndex][yrandIndex].draw(); 
	//get the pixel grid neighborhood of the point
	vector<Point> neighbors;
	neighbors.push_back(threeD[xrandIndex][yrandIndex]); 

	clock_t start; 
	double duration; 
	start = clock(); 

	//2nd Random Point 
	xrandIndex = xDistro(gen);
	yrandIndex = yDistro(gen);  
	neighbors.push_back(threeD[xrandIndex][yrandIndex]); 

	//3rd Random Point 
	xrandIndex = xDistro(gen); 
	yrandIndex = yDistro(gen); 
	neighbors.push_back(threeD[xrandIndex][yrandIndex]); 

	//if statements w/neighbors = 4,000ns 
	//Choosing random points 2,000ns
	
	duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	cout << "Time Difference: " << duration << endl; 

	cout << "Chosen points: " << endl; 
	for(int i=0; i<neighbors.size(); i++){
		neighbors[i].draw(); 
	}

 
	//make 2 vectors from the 3 points in neighbors
	vector<float> AB{neighbors[1].x - neighbors[0].x, 
		neighbors[1].y - neighbors[0].y, 
		neighbors[1].z - neighbors[0].z};
	vector<float> AC{neighbors[2].x - neighbors[0].x, 
		neighbors[2].y - neighbors[0].y, 
		neighbors[2].z - neighbors[0].z}; 

	//Get the normal/orthogonal vector using cross product
	vector<float>normal{AB[1]*AC[2] - AB[2]*AC[1], //i 
		-(AB[0]*AC[2] - AB[2]*AC[0]), //j 
		AB[0]*AC[1] - AB[1]*AC[0]}; //k

	//Make the plane out of the normal and one of the points
	float a = neighbors[0].x; 
	float b = neighbors[0].y; 
	float c = neighbors[0].z; 
	float d = a*normal[0] + b*normal[1] + c*normal[2]; 

	if(normal[0]==0 and normal[1]==0 and normal[2]==0){
		cout << "The zero vector, next 3 points." << endl;
		return findFloorPlane(threeD, plane, gen, 
			xDistro, yDistro); 
	}

	cout << "Plane equation: " << normal[0] << "x + "; 
	cout << normal[1] << "y + " << normal[2] << "z = ";
	cout <<  d << endl;
	plane[0] = normal[0];
	plane[1] = normal[1];
	plane[2] = normal[2]; 
	plane[3] = d; 

	int pointCount = 0; 
	//check to see how many points agree with this plane
	for(int i=0; i<threeD.size(); ++i){
		for(int j=0; j<threeD[i].size(); ++j){
			float val = threeD[i][j].x*normal[0] + threeD[i][j].y*normal[1] + threeD[i][j].z*normal[2] - d; 
			if(val > -1e-5 and val < 1.e-5){
				//point agrees
				++pointCount; 
			}
			else{
				cout << "Point: (" << threeD[i][j].x << ", " << threeD[i][j].y; 
				cout << ", " << threeD[i][j].z << ") Doesn't agree "; 
				cout << val << endl; 

			}

		}

	}
	cout << "Point count: " << pointCount << "/" << threeD[0].size()*threeD.size();
	cout << endl; 
}


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
	findFloorPlane(threeD, plane, gen, xDistro, yDistro); 
	cout << "Plane found: " << endl; 
	for(int i=0; i<plane.size(); ++i){
		cout << plane[i] << endl; 
	}

	
}