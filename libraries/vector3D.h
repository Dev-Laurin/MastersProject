//vector3D.h 
//Laurin Fisher
//11-3-18
//A single vector imitating a 3D vector for performance
#include <vector>

template <typename T> 
class vector3D {
public: 	
	vector3D(int width, int width2, int width3){
		//make the vector size 
		vec3.resize(width * width2 * width3); 
		w1 = width; 
		w2 = width2; 
		w3 = width3; 
	}

	//Add another element to 3rd vector
	void push_back(int i, int j, T obj){

		vec3.insert(vec3.begin() + , obj); 

		//resize
		vec3.resize(width * width2 * width3) 
		vec3[w1*w2*w3].push_back(obj); 

	}

	T & operator()(int i, int j, int k){
		return vec3[i*w2*w3 + j*w3 + k]; 
	}

private: 
	int w1, w2, w3; 
	std::vector<T> vec3; 
}; 