#include <iostream>
#include <bitset>
#include <limits>
#include <string>
#include "TxServiceTime.h"

using namespace std;


//The values of the parameters range from 0 to 1.
//Being 0 LESS and 1 MORE of the specified parameter

double slotTime11nParameters(int aggregation, int performance){
	
	int timeParameters;
	double TxD; //successful transmission delay
	
	std::bitset<2> picker;
	picker.set(1, aggregation);
	picker.set(0, performance);
	
	timeParameters = (int)picker.to_ulong();
	
	/*s: Number of Spatial Streams (1,2,3,4)
	L: Packet Length
	nAg: Number of packets aggregated together
	coding: Channel Coding (1/2, 2/3, ... )
	mod: Bits per symbol (BPSK = 1, QPSK = 2, etc.)
	subcarriers: 20 MHz = 48, 40 MHz = 108
	M: Number of antennas >= s*/
	
	int s;
	long int L;
	int nAg;
	float coding;
	int mod;
	double subcarriers;
	int M;
	
	switch(timeParameters){
	
	case 0:
		//Minimum Performance (No Aggregation)
		M = 1;
		subcarriers = 48;
		mod = 1;
		coding = 1./2;
		L = 12000;
		nAg = 1;
		s = M;
		
		TxD = TxServiceTime(s, L, nAg, coding, mod, subcarriers, M);
		
		break;
	
	case 1: 
		//Maximum Performance (No Aggregation)
		M = 4;
		subcarriers = 108;
		mod = 6;
		coding = 5./6;
		L= 12000;
		nAg = 1;
		s = M;
		
		TxD = TxServiceTime(s,L,nAg,coding,mod,subcarriers,M);
		
		break;
		
	case 2: 
		//Minimum Performance (Aggregation)
		M = 1;
		subcarriers = 48;
		mod = 1;
		coding = 1./2;
		L = 12000;
		nAg = 64;
		s = M;

		TxD = TxServiceTime(s,L,nAg,coding,mod,subcarriers,M);
		
		break;
		
	case 3: 
		//Maximum Performance (Aggregation)
		M = 4;
		subcarriers = 108;
		mod = 6;
		coding = 5./6;
		L = 12000;
		nAg = 64;
		s = M;

		TxD = TxServiceTime(s,L,nAg,coding,mod,subcarriers,M);
		
		break;
		
	default:
		cout << "There is some problem with the parameters. Please try again with the" << endl;
		cout << "parameters on the range from [0-1]" << endl;
		
		break;
	}
	
	//cout << "Successful transmission delay (TxD): " << TxD << endl;
	
	return((double)TxD);
	
}
