//
//  testings.cpp
//  Duffy
//
//  Created by Luis Sanabria Russo on 8/30/12.
//  Copyright (c) 2012 Luis Sanabria Russo. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include "defs/lengthOfFile.h"

using namespace std;

struct stats{
  long int TX;
  long int collisions;
};

int main(int argc, const char * argv[])
{
	int repetitions = 100;
	int stations = 10;
	int numberOfLines = 0;
	stringstream command (stringstream::in | stringstream::out);
	struct stats *statistics;
	long int accumTX = 0;
	long int accumCol = 0;
	
	if(argc >= 2) repetitions = atoi(argv[1]);
	if(argc >= 3) stations = atoi(argv[2]);
	
	command << "./simulator " << stations << " 1 0";
	
	//cout << command.str().c_str() << endl;
	
	for(int i = 0; i <= repetitions; i++){
		system(command.str().c_str()); 
	}
	
	//order the resulting file
	const char *routeToFile = "results.txt";
	
	ifstream fin(routeToFile);
	numberOfLines = lengthOfFile(&fin);
	
	statistics = new struct stats[numberOfLines]; //setting the size now we know the length of the file
	
	for(int i = 0; i < numberOfLines; i++){
		statistics[i].TX = 0;
		statistics[i].collisions = 0;
	}
	
	ifstream inputFile(routeToFile);
	string input;
    
    //Getting the values separatetly and putting them in variables.
    
    int iterator = 0;
    while(getline(inputFile,input)){
    	istringstream tokenizer(input);
        string token;
        
        getline(tokenizer, token, ' ');
        istringstream float_iss(token);
        float stations;
        float_iss >> stations;
        //cout << stations << endl;
        
        getline(tokenizer, token, ' ');
        istringstream float_iss2(token);
        float successful_TX;
        float_iss2 >> successful_TX;
        //cout << successful_TX << endl;
        statistics[iterator].TX = successful_TX;
        
        getline(tokenizer, token, ' ');
        istringstream float_iss3(token);
        float overall_collisions;
        float_iss3 >> overall_collisions;
        //cout << overall_collisions << endl;
        statistics[iterator].collisions = overall_collisions;
        
        iterator++;
    }
    
    for(int i = 0; i <= iterator; i++){
    	accumTX += statistics[i].TX;
    	accumCol += statistics[i].collisions;
    }
    
    //cout << "Average number of succesful TX: " << accumTX/iterator << endl;
    //cout << "Average number of collisions: " << accumCol/iterator << endl;
    
    ofstream output;
	output.open("results_averaged.txt");
	output << accumTX/iterator << " " << accumCol/iterator << endl;
	output.close();
    
	return 0;
}