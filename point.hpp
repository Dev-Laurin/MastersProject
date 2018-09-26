
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
/*
	void changeRGB(uint8_t R, uint8_t G, uint8_t B){

		// How to unpack data: 
		const uint8_t *p = reinterpret_cast<uint8_t*>(&rgb); 
		/*
		uint8_t b = p[0]; 
		uint8_t g = p[1]; 
		uint8_t r = p[2]; 
		

		p[0] = B; 
		p[1] = G; 
		p[2] = R; 

		//cast it 
		rgb = reinterpret_cast<float*>(&p);  
	} */ 
};

#endif /* POINT_H_INCLUDED */ 