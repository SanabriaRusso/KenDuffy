//
//  display.h
//  Duffy
//
//  Created by Luis Sanabria Russo on 8/30/12.
//  Copyright (c) 2012 Luis Sanabria Russo. All rights reserved.
//
#include <iostream>
#include <fstream>
using namespace std;

void display(int sta, long int tx,  long int collisions){
	ofstream output;
	output.open("results.txt", ios::app);
	output << sta << " " << tx << " " << collisions << endl;
	output.close();
}