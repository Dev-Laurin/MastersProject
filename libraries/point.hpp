
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
	Point(double x2, double y2, double z2, double rgb2) : x(x2), y(y2), z(z2), rgb(rgb2) {}
	Point(double x2, double y2, double z2) : x(x2), y(y2), z(z2) {}

	void draw(std::ostream & os){
	 	os << "(" << x << ", " << y << ", " << z << ")" << endl; 
	}

	double x; 
	double y; 
	double z; 
	double rgb; 
};

#endif /* POINT_H_INCLUDED */ 