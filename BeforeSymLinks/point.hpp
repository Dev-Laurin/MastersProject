
#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED
#include <iostream>
using std::endl; 
#include <ostream> 

class Point{
public: 
	Point(){
		x = 0.0; 
		y = 0.0; 
		z = 0.0; 
		rgb = 0.0; 
	}
	Point(float x2, float y2, float z2, float rgb2) : x(x2), y(y2), z(z2), rgb(rgb2) {}
	float x; 
	float y; 
	float z; 
	float rgb; 

	void draw(std::ostream & os){
	 	os << "(" << x << ", " << y << ", " << z << ")" << endl; 
	}
};

#endif /* POINT_H_INCLUDED */ 