#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define MAX(a,b)       (a < b) ? (b) : (a)
#define MIN(a,b)       (a < b) ? (a) : (b)

#define MAXSTATIONS 32 // the maximum number of contenders
#define CWmin 32
#define MAX_BACKOFF_STAGE 5
#define MAX_SLOTS 100000 //simulation length in slots
#define STICKINESS 1
#define WSS 100 //Window size stickiness (how long until halving the CW)
#define KENDUFFY 1 //It is true if a station transmits a number of packets equal to two to the power of its backoff stage.

#define DEBUG

int main (int argc, char *argv[])
{
  int number_of_sta; //number of nodes
  int backoff_counter_sta[MAXSTATIONS];
  int backoff_stage_sta[MAXSTATIONS];
  int stickiness_counter_sta[MAXSTATIONS];
  int wss_counter_sta[MAXSTATIONS];
  long int successful_tx_sta[MAXSTATIONS];
  long int collisions_sta[MAXSTATIONS];
  long int slot;
  int number_of_transmissions_in_slot;
  int sta; //variable for loops
  long int time; //accounts for the simulated time (in ms). Needed to compute throughput.
  long int number_of_successful_tx; //total number of successful tx. Needed to compute throughput.
  long int number_of_collisions;
  int largest_bos;

  number_of_sta=atoi(argv[1]);
  printf("number of sta %d\n", number_of_sta);
  srand(getpid());
  time=0;
  number_of_successful_tx=0;
  number_of_collisions = 0;
  for (sta=0;sta<number_of_sta;sta++)
    {
    successful_tx_sta[sta]=0;
    collisions_sta[sta]=0;
    backoff_counter_sta[sta]=(rand()%CWmin);
    backoff_stage_sta[sta]=0;
    stickiness_counter_sta[sta]=0;
    wss_counter_sta[sta]=0;
    }
  for (slot=0;slot<MAX_SLOTS;slot++)
    {
    //count the number of transmissions in this slot
    number_of_transmissions_in_slot=0;
    for (sta=0;sta<number_of_sta;sta++)
      {
      if (backoff_counter_sta[sta]==0)
        {
        number_of_transmissions_in_slot++;
        }
      }

    // empty, success or collision
    switch (number_of_transmissions_in_slot)
      {
      //empty
      case 0: 
        time+=20;
        break;
      //success
      case 1:
        for (sta=0;sta<number_of_sta;sta++)
          {
          if (backoff_counter_sta[sta]==0)
            {
            printf("success sta %d\n", sta);
            if (KENDUFFY)
              {
              successful_tx_sta[sta]+=1<<backoff_stage_sta[sta];
              time+=50+10+192+20+(1528*8/11.0)*(1<<backoff_stage_sta[sta])+56;
              }else
              {
              successful_tx_sta[sta]++;
              time+=50+10+192+20+(1528*8/11.0)+56;
              }
            stickiness_counter_sta[sta]=STICKINESS;
            if (stickiness_counter_sta[sta]>1)
              {
                backoff_counter_sta[sta]=((CWmin/2)<<backoff_stage_sta[sta]);
              }else{
                backoff_counter_sta[sta]=rand()%(CWmin<<backoff_stage_sta[sta])+1;
              }
            //We reset the contention window when the stickiness counter for the window reaches zero.
            if ((wss_counter_sta[sta]--==0)&&(backoff_stage_sta[sta]>0))
              {
              backoff_stage_sta[sta]=0;
              wss_counter_sta[sta]=WSS;
              }
            }
          }
        break;
      //collision
      default:
        largest_bos=0;
        for (sta=0;sta<number_of_sta;sta++)
          {
          if (backoff_counter_sta[sta]==0)
            {
            collisions_sta[sta]++;
            largest_bos=MAX(largest_bos, backoff_stage_sta[sta]);
            if (stickiness_counter_sta[sta] > 1)
              {
                stickiness_counter_sta[sta]--;
                backoff_counter_sta[sta]=(CWmin/2)<<backoff_stage_sta[sta];
              }else{
                backoff_stage_sta[sta]=MIN(backoff_stage_sta[sta]+1, MAX_BACKOFF_STAGE);
                backoff_counter_sta[sta]=rand()%(CWmin<<backoff_stage_sta[sta])+1;
                wss_counter_sta[sta]=WSS;
              }
            }
          }
        if (KENDUFFY)
          {
          time+=50+10+192+20+(1528*8/11.0)*(1<<largest_bos)+56;
          }else
          {
          time+=50+10+192+20+(1528*8/11.0)+56;
          }
      }

      // decrement the backoff counters
      for (sta=0;sta<number_of_sta;sta++)
        {
        backoff_counter_sta[sta]--;
        }

#ifdef DEBUG
     printf("%ld\t%d\n", slot, number_of_transmissions_in_slot);
     for (sta=0;sta<number_of_sta;sta++)
       {
       printf("%d\t%d\t%d\t%d\t%ld\t%ld\n",sta, backoff_counter_sta[sta], backoff_stage_sta[sta], stickiness_counter_sta[sta],successful_tx_sta[sta], collisions_sta[sta]);
       }
#endif
    }
   for (sta=0;sta<number_of_sta;sta++)
     {
     number_of_successful_tx+=(successful_tx_sta[sta]);
     number_of_collisions+=(collisions_sta[sta]);
     }
   printf("number of successful tx: %ld\n",number_of_successful_tx);
   printf("number of collisions: %ld\n", number_of_collisions);
   printf("time: %ld\n", time);
   printf("throughput: %f\n", number_of_successful_tx*8*1500/(1.0*time));
}
