all: detectObject.cpp
	g++ -std=c++17 detectObject.cpp -o detect.o 
	./detect.o