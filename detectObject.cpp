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
#include "eigen-git-mirror/Eigen/Eigenvalues"
#include "point.cpp"

using Eigen::MatrixXd;
using Eigen::EigenSolver;  
using Eigen::VectorXcd; 

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

	//find the floor plane
		//RANSAC

	//1. Randomly choose a point

	//X
	mt19937::result_type seed = time(0); 
	//auto xrandIndex = std::bind(std::uniform_int_distribution<int>(0,threeD.size()-1),
	//	mt19937(seed));

//	cout << xrandIndex << endl; 

	//Y
//	auto yrandIndex = std::bind(std::uniform_int_distribution<int>(0,threeD[0].size()-1),
//		mt19937(seed)); 

	int xrandIndex = 1; 
	int yrandIndex = 0; 
	
	cout << "Point chosen: " << endl; 
	threeD[xrandIndex][yrandIndex].draw(); 
	//get the pixel grid neighborhood of the point
	vector<Point> neighbors;
	neighbors.push_back(threeD[xrandIndex][yrandIndex]); 

	//top
	if(yrandIndex>0){
		neighbors.push_back(threeD[xrandIndex][yrandIndex-1]);
	}

	//bottom
	if(yrandIndex+1<threeD[0].size()){
		neighbors.push_back(threeD[xrandIndex][yrandIndex+1]);
	}

	//right
	if(xrandIndex+1<threeD.size()){
		neighbors.push_back(threeD[xrandIndex+1][yrandIndex]);
	}

	//left
	if(xrandIndex>0){
		neighbors.push_back(threeD[xrandIndex-1][yrandIndex]);
	}

	//diagonal upper right
	if(xrandIndex+1<threeD.size() && yrandIndex>0){
		neighbors.push_back(threeD[xrandIndex+1][yrandIndex-1]);
	}

	//diagonal upper left
	if(xrandIndex>0 && yrandIndex>0){
		neighbors.push_back(threeD[xrandIndex-1][yrandIndex-1]);
	}

	//diagonal bottom right
	if(xrandIndex+1<threeD.size() && yrandIndex+1<threeD[0].size()){
		neighbors.push_back(threeD[xrandIndex+1][yrandIndex+1]);
	}

	//diagonal bottom left
	if(xrandIndex>0 && yrandIndex+1<threeD[0].size()){
		neighbors.push_back(threeD[xrandIndex-1][yrandIndex+1]);
	}

	cout << "Chosen points: " << endl; 
	for(int i=0; i<neighbors.size(); i++){
		neighbors[i].draw(); 
	}
 
	//Get the eigen values
	MatrixXd m(3,3); 
	m(0,0) = neighbors[0].x; 
	m(0,1) = neighbors[0].y; 
	m(0,2) = neighbors[0].z; 
	m(1,0) = neighbors[1].x; 
	m(1,1) = neighbors[1].y; 
	m(1,2) = neighbors[1].z; 
	m(2,0) = neighbors[2].x; 
	m(2,1) = neighbors[2].y; 
	m(2,2) = neighbors[2].z; 
	cout << m << endl; 
	


	EigenSolver<MatrixXd> e(m); 
	VectorXcd eivals = e.eigenvalues(); 
	cout << "Eigen Values in vector: " << eivals << endl; 
	//Find the maximum eigenvalue
	
	auto maxValue = eivals[0]; 
	//double Eigen::DenseCoeffsBase<Eigen::Matrix<std::complex<double, -1, 1> maxValue = eivals[0]; 
	int index = 0; 
	// for(int i=0; i<eivals.size(); ++i){
	// 	if(maxValue < eivals[i]){
	// 		maxValue = eivals[i];
	// 		index = i;  
	// 	}
	// }

	cout << "Max eigen value: " << maxValue << endl; 
	cout << "Corresponding eigen vector: " << endl; 
	auto eigenVectors = e.eigenvectors(); 
	cout << "Eigen vectors: " << endl; 
	cout << eigenVectors << endl; 
	cout << "e3: " << endl; 
	cout << eigenVectors.col(2) << endl; 
	//ax + by + cz = d 
	//a = e31
	auto a = eigenVectors.col(2).row(0); 
	cout << "a: " <<  a << endl; 
	//b = e32
	auto b = eigenVectors.col(2).row(1); 
	//c = e33 
	auto c = eigenVectors.col(2).row(2); 

	//Mean vector
	vector<int> meanVector; 
	for(int i=0; i<3; i++){
		int sum = 0; 
		for(int j=0; j<3; j++){
			sum+=m(i,j); 
		} 
		meanVector.push_back(sum/3); 
	}

	//d = e31m1 + e32m2 + e33m3 
	auto d = a*meanVector[0]  + b*meanVector[1] + c*meanVector[2]; 
	cout << "d: " << d << endl; 

	cout << "Plane equation: " << a << "x" << " + ";  
	cout << b << "y" << " + " << c << "z" << " = "; 
	cout << d << endl; 

	//determine if this plane fits most points
	int numOfPointsInPlane = 0; 
	for(int i=0; i<threeD.size(); ++i){
		for(int j=0; j<threeD[i].size(); ++j){
			//calculate dot product to see if point is in plane
			auto cal = a*threeD[i][j].x + b*threeD[i][j].y + c*threeD[i][j].z - d; 
			double threshold = 1.0e-5; 
			//if((cal) > threshold){
			//	++numOfPointsInPlane; 
			
			//}
			cout << "Calculate: " << cal << endl; 
		}
	}

	cout << "Number of points that agree." << numOfPointsInPlane;
	cout << endl; 
}