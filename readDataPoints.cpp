#include <iostream>
#include <fstream> 
#include <string>
using std::string; 
using std::fstream;
using std::cout; 
using std::endl;  
#include <sstream> 
using std::stringstream; 

//argv[1] = filename 
//argv[2] = points on x axis that are errors. Example: >2 
//argv[3] = points on z axis that are errors. 

int main(int argc, char* argv[]){
	string filename = "../../libfreenect2/build/bin/"; 
	string arg = argv[1]; 
	filename += arg; 
	fstream file(filename.c_str());

	cout << "Filename: " << filename << endl; 
	if(!file){
		cout << "Error file could not be opened." << endl; 
		return 1; 
	}

	string xLimits;   
	string zLimits;  
	double xValue = 0; 
	double zValue = 0;
	char zSymbol; 

	if(argc >= 3){
		xLimits = argv[2];  
		string xVal = ""; 
		for(int i=1; i<xLimits.size(); i++){
			xVal += xLimits[i]; 
		}
		stringstream argX(xVal); 
		argX >> xValue; 

		zLimits = argv[3]; 
		string zVal = ""; 
		zSymbol = zLimits[0]; 
		for(int i=1; i<zLimits.size(); i++){
			zVal += zLimits[i]; 
		}
		stringstream argZ(zVal); 
		argZ >> zValue; 
		
	}

	cout << "xValue: " << xValue << endl; 
	cout << "zValue: " << zValue << endl; 
	

	cout << "Filename: " << argv[1] << endl; 
	cout << "Xlimit: " << xLimits << endl; 
	cout << "Zlimit: " << argv[3] << endl; 


	std::ofstream errorFile("errorPoints.txt"); 


	int totalPoints = 0; 
	double sum = 0; 
	int totalErrorPoints = 0;  
	while(!file.eof()){

		//Get line from file
		string line; 
		getline(file, line); 

		//cout << line << endl;

		if(file.eof())
			break; 

		totalPoints++; 

		int i = 0; 
		string X = ""; 
		while(line[i] != ','){
			X += line[i]; 
			i++; 
		}
		stringstream num(X); 
		double x = 0; 
		num >> x; 
		
		string Y = ""; 
		i++; 
		while(line[i] != ','){
			Y += line[i]; 
			i++; 
		}
		stringstream num2(Y); 
		double y = 0;
		num2 >> y; 
		
		i++; 
		string Z = ""; 
		while(line[i] != ','){
			Z += line[i]; 
			i++;
		}
		stringstream num3(Z); 
		double z = 0; 
		num3 >> z;  

		//Check if this point is an error point
		bool xIsError = false; 

	//	cout << xLimits << endl; 
		if(xLimits[0] == 'g'){
			if(x > xValue){
				//is an error point
				totalErrorPoints++; 
				xIsError = true; 

				errorFile << line << endl; 
			}
		}
		else{
			if(x < xValue){
				//is an error point
				totalErrorPoints++; 
				xIsError = true; 
				errorFile << line << endl; 
			}
		}
		cout << y << endl; 

			if(y > 0.05 && !xIsError){
				//is an error point
				totalErrorPoints++; 
				errorFile << line << endl; 
			}

		// if(zSymbol == 'g'){
		// 	if(z > zValue && !xIsError){
		// 		//is an error point
		// 		totalErrorPoints++; 
		// 		errorFile << line << endl; 
		// 	}
		// }
		// else{
		// 	if(z < zValue && !xIsError){
		// 		//is an error point 
		// 		totalErrorPoints++; 
		// 		errorFile << line << endl; 
		// 	}
		// }	
	}

	errorFile.close(); 

	cout << "Total errors: " << totalErrorPoints << endl; 
	cout << "Total points: " << totalPoints << endl; 

	//Calculate the error percentage
	cout << "Error percentage: " << double(totalErrorPoints) / double(totalPoints) << endl; 

	
}