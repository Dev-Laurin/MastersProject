#ifndef BINDATA_HPP_INCLUDED 
#define BINDATA_HPP_INCLUDED

#include "point.hpp"

class binData{
public: 

	binData(){
		sum = 0.0; 
		maximum = 0.0; 
		minimum = 0.0; 
		totalPoints = 0.0; 
		squaredSum = 0.0; 
		variance = 0.0; 
		trimmedMean = 0.0; 
	}
	double sum; 
	double maximum; 
	double minimum; 
	double totalPoints; 
	double squaredSum; 
	double variance; 
	double trimmedMean; 

	vector<Point> data; 

}; 

#endif /* BINDATA_HPP_INCLUDED */ 