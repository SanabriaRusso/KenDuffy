#include <iostream>
#include <string>
#include <math.h>

#define SIFS 16E-6
#define DIFS 34E-6
#define SLOT 9E-6
#define TS 4E-6 //800 ns guard interval

using namespace std;

double TxServiceTime(int s, long int L, int nAg, float coding, int mod, int subcarriers, int M){
	
	double dataRate;
	//double PHY_h_MM; //PHY header in Mided Mode
	double PHY_h_GF; //Phy header in Green Field Mode
	
	double PHY_h; //PHY header
	
	int MAC_h;
	int FCS; //frame check sequence
	
	int MPDU_Del;
	int BACK;
	int MPDU;
	int serviceField;
	int Nes;
	
	int tailBits;
	int PSDU;
	
	double TxPSDU;
	int PSDU_BAK;
	double TxBAK;
	
	double TxDelay; //successful transmission delay
	
	dataRate = (s * subcarriers * mod * coding) / TS;
	
	//Mided Mode
	//PHY_h_MM = 8E-6 + 8E-6 +4E-6 + 8E-6 + 4E-6 + M*4E-6;
	
	//Green Field Mode
	PHY_h_GF = 8E-6 + 8E-6 + 8E-6 + (M*4E-6);
	
	PHY_h = PHY_h_GF;
	
	//MAC header
	MAC_h = 36*8;
	FCS = 4*8;
	
	//Aggregation parameters (A-MPDU)
	MPDU_Del = 4*8;
	BACK = 32*8;
	MPDU = MAC_h + L + FCS;
	serviceField = 16;
	Nes = 1;
	
	//Convolutional encoding branches
	tailBits = 6*Nes;
	PSDU = -1;
	
	if(nAg == 1){ 
		PSDU = serviceField + MPDU + tailBits;
	}
	else{
		PSDU = serviceField + (nAg * (MPDU_Del + MPDU)) + tailBits;
	}
	
	TxPSDU = PHY_h + ceil(PSDU / (subcarriers * mod * coding)) * TS;
	PSDU_BAK = serviceField + BACK + tailBits;
	TxBAK = PHY_h + ceil(PSDU_BAK / (s * subcarriers * mod* coding)) * TS;
	
	TxDelay = TxPSDU + SIFS + TxBAK + DIFS + SLOT;
	
	/*cout << "Slot duration [seconds]: " << TxDelay << endl;
	cout << "Throughput (continuous Tx) [Mbps]: " << nAg*(L/TxDelay)/1E6 << endl;
	cout << "Data rate [Mbps]: " << dataRate/1E6 << endl;
	cout << "Efficiency (%): " << (nAg*(L/TxDelay)/1E6) / (dataRate/1E6)*100 << endl;*/
	
	
	return ((double)TxDelay);
	
}
