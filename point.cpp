
#ifndef POINT_H_INCLUDED
#define POINT_H_INCLUDED

class Point{
public: 
	Point(float x2, float y2, float z2, float rgb2) : x(x2), y(y2), z(z2), rgb(rgb2) {}
	float x; 
	float y; 
	float z; 
	float rgb; 

	void draw(){
		cout << "(" << x << ", " << y << ", " << z << ")" << endl; 
	}
};

#endif /* POINT_H_INCLUDED */ 