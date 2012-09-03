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

#define MIN(a,b)       (a < b) ? (a) : (b)

#define MAX_SLOTS 100000
#define MAX_STA 32
#define CWmin 16
#define MAX_STA_BACKOFF_STAGE 5
#define MAX_STICKYNESS 2

using namespace std;

int main(int argc, const char * argv[])
{

    int number_sta; //number of stations
    int sticky;	//stickness as Ken Duffy's proposal
    int PRINT;
    
    number_sta = 5;
    sticky = 1;    
        
    if(argc >=2) number_sta = atoi(argv[1]);
    if(argc >=3) sticky = atoi(argv[2]);
    if(argc >=3) PRINT = atoi(argv[3]);
    srand(getpid());
    
    
    if(PRINT == 1)cout << "Number of stations: " << number_sta << endl;
    
    //--------------------------------------------------//
    
    long int sta_backoff_counter[number_sta];
    long int slot_tx[MAX_SLOTS];
    int sta_collisions[number_sta];	//collisions per station
    int sta_tx[number_sta];		//transmitted packets per station
    int sta_stickyness[number_sta];	//it is 1 if the station is allowed to repeat its backoff timer, 0 otherwise
    int sta_backoff_stage[number_sta];	//when increases, decreases the chances of collision
    
    //Filling the arrays of the size of number of stations
    
    for(int sta = 0; sta < number_sta; sta++){
    	    sta_backoff_counter[sta] = 0;
    	    sta_collisions[sta] = 0;
    	    sta_tx[sta] = 0;
    	    sta_stickyness[sta] = 0;
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
                        sta_tx[sta]++;
                        if(sticky == 1){ //determines if the station should pick a deterministic backoff timer	
                        	sta_backoff_counter[sta] = (CWmin/2) << sta_backoff_stage[sta];
                        	if(sta_stickyness[sta] <= MAX_STICKYNESS){
                        		//commenting below, will look like Jaume's proposal
                        		sta_stickyness[sta]++; //gains the ability to pick the same backoff on the next slot
                        	}
                        	else{
                        		sta_stickyness[sta] = MAX_STICKYNESS;
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
                		if(sta_stickyness[sta] > 0){
                			sta_backoff_counter[sta] = (CWmin/2) << sta_backoff_stage[sta];
                			sta_stickyness[sta]--; //losses one opportunity to chose a deterministic backoff in the next round if it collides
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
    
    
    for(int sta = 0; sta < number_sta; sta++){
    	    tx_packets += sta_tx[sta];
    	    overallCollisions += sta_collisions[sta];
    }
    
    cout << "The overall number of packets sent are: " << tx_packets << endl;
    cout << "The overall collisions are: " << overallCollisions << endl;
    
    display(number_sta, tx_packets, overallCollisions);
    
    return 0;
}

