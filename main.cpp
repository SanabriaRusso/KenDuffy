//
//  main.cpp
//  Duffy
//
//  Created by Luis Sanabria Russo on 8/24/12.
//  Copyright (c) 2012 Luis Sanabria Russo. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "defs/display.h"
#include "defs/slotTime11nParameters.h"

#define MIN(a,b)       (a < b) ? (a) : (b)

#define MAX_SLOTS 100000
#define MAX_STA 32
#define CWmin 16
#define MAX_STA_BACKOFF_STAGE 5

using namespace std;

int main(int argc, const char * argv[])
{

    int number_sta; //number of stations
    int PRINT;
    int MAX_STICKINESS = 0;
    
    //Aggregation and performance are parameters to determine the
    //successfull transmission delay's parameters.
    //Values range [0-1]. Zero being LESS and one MORE of that parameter.
    //For insights see def/slotTime11nParameters.h
    
    int aggregation = 0;
    int performance = 0;
    
    number_sta = 5;
        
    if(argc >=2) number_sta = atoi(argv[1]);
    if(argc >=3) MAX_STICKINESS = atoi(argv[2]);
    if(argc >=4) aggregation = atoi(argv[3]);
    if(argc >=5) performance = atoi(argv[4]);
    if(argc >=6) PRINT = atoi(argv[5]);
    
    srand(getpid());
    
    
    if(PRINT == 1)cout << "Number of stations: " << number_sta << endl;
    
    //--------------------------------------------------//
    
    long int sta_backoff_counter[number_sta];
    long int slot_tx[MAX_SLOTS];
    long int sta_collisions[number_sta];	//collisions per station
    long int sta_tx[number_sta];		//transmitted packets per station
    int sta_stickiness[number_sta];	//it is 1 if the station is allowed to repeat its backoff timer, 0 otherwise
    int sta_backoff_stage[number_sta];	//when increases, decreases the chances of collision
    double TxD;	//sucessful transmission delay per packet
    
    //Filling the arrays of the size of number of stations
    
    for(int sta = 0; sta < number_sta; sta++){
    	    sta_backoff_counter[sta] = 0;
    	    sta_collisions[sta] = 0;
    	    sta_tx[sta] = 0;
    	    sta_stickiness[sta] = 0;
    	    sta_backoff_stage[sta] = 0;
    }
    
    
    //Transmissions
    for(int sta = 0; sta < number_sta; sta++){
        sta_backoff_counter[sta] = rand()%CWmin;
        if(PRINT == 1)cout << "Station: " << sta << " setted backoff timer to: " << sta_backoff_counter[sta] << endl;
    }
    
    for(long int slots = 0; slots < MAX_SLOTS; slots++){
    	slot_tx[slots] = 0;
        //Checking if there are transmissions in this slot
            //Transmitting if the medium is free
        for(int sta = 0; sta < number_sta; sta++){
            if(sta_backoff_counter[sta] == 0){
                slot_tx[slots]++;
                if(PRINT == 1)cout << "**Station: " << sta << " sent a packet at slot: " << slots << endl;
            }
        }
        switch (slot_tx[slots]) {
            case 0: //free slot
                break;
            case 1: //there has been 1 successful transmission
                for(int sta = 0; sta < number_sta; sta++){
                    if(sta_backoff_counter[sta] == 0) {
                    	    sta_tx[sta]+= (1) << sta_backoff_stage[sta];
                        if(MAX_STICKINESS > 0){ //determines if the station should pick a deterministic backoff timer	
                        	sta_backoff_counter[sta] = (CWmin/2) << sta_backoff_stage[sta];
                        	if(sta_stickiness[sta] < MAX_STICKINESS){
                        		
                        		//sta_stickiness[sta]++; //gains the ability to pick the same backoff on the next slot
                        		
                        		sta_stickiness[sta] = MAX_STICKINESS; //this is Jaume's approach
                        	}
                        }
                        else{
                        	sta_backoff_counter[sta] = rand()%(CWmin << sta_backoff_stage[sta]);
                        }
                    }
                }
                break;
                
            default: //collisions
                if(PRINT == 1)cout << "##There are: " << slot_tx[slots] << " packets colliding at slot: " << slots << endl;
                for(int sta = 0; sta < number_sta; sta++){
                	if(sta_backoff_counter[sta] == 0){
                		sta_collisions[sta]++;
                		if(sta_stickiness[sta] > 0){
                			sta_backoff_counter[sta] = (CWmin/2) << sta_backoff_stage[sta];
                			sta_stickiness[sta]--; //losses one opportunity to chose a deterministic backoff in the next round if it collides
                		}
                		else{
                			sta_backoff_stage[sta] = MIN(sta_backoff_stage[sta] + 1, MAX_STA_BACKOFF_STAGE);
                			sta_backoff_counter[sta] = rand()%(CWmin << sta_backoff_stage[sta]);
                		}
                	}
                }
                break;
        }
        
        //Decrememting the  backoff counters
        for(int sta = 0; sta < number_sta ; sta++){
        	if(sta_backoff_counter[sta] == 0) continue;
        	sta_backoff_counter[sta]--;
        	if(PRINT == 1) cout << "Station: " << sta << " new backoff timer is: " << sta_backoff_counter[sta] << endl;
        }
    }
    
    //Gathering statistics
    int tx_packets = 0;
    int overallCollisions = 0;
    double *throughput;
    double averageThroughput = 0.0;
    
    throughput = new double[number_sta];
    
    
    for(int sta = 0; sta < number_sta; sta++){
    	tx_packets += sta_tx[sta];
    	overallCollisions += sta_collisions[sta]; 
    }
    
    //Determining the succesfull transmission delays
    
    TxD = slotTime11nParameters(aggregation, performance);
    
    for(int sta = 0; sta < number_sta; sta++){
    	    throughput[sta] = (sta_tx[sta]) / ((MAX_SLOTS - sta_tx[sta])*9E-6 + (sta_tx[sta] * TxD));
    	if(PRINT == 1){
    		cout << "Station " << sta << "'s approx. throughput is: " << throughput[sta]*12000 << " bps" << endl;	
    	}
    	averageThroughput += throughput[sta];
    }
    
    cout << "The approximate delay per successful transmission is: " << TxD << endl;
    cout << "The average throughput is: " << (averageThroughput/number_sta)*12000 << endl; 
    cout << "The overall number of packets sent are: " << tx_packets << endl;
    cout << "The overall collisions are: " << overallCollisions << endl;
    
    
    display(number_sta, tx_packets, overallCollisions);
    
    return 0;
}

