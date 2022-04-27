#include "mbed.h"
#include "PinDetect.h"
#include <vector>
// must import Cookbook PinDetct library into project
// URL: http://mbed.org/users/AjK/libraries/PinDetect/lkyxpw

DigitalOut myled(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
DigitalOut BD_trig(p12);
DigitalOut SD_trig(p13);
DigitalOut CH_trig(p14);
DigitalOut CP_trig(p15);
//DigitalIn BD_pb(p6);
//DigitalIn SD_pb(p7);
//DigitalIn CH_pb(p8);
//DigitalIn CP_pb(p9);
uint8_t bitMask = 1;
int seqLength = 8;
int numDrums = 4;
bool volatile recordMode = false;
bool volatile playMode = true; 
int volatile stepCount = 0;
Timer timer;
Timer timer2;
int previousStepCountBD, previousStepCountSD, previousStepCountCH, previousStepCountCP;




//uint8_t bitmask = 1;

//for (int i = 0; i < numberOfSteps; i++) {
//for(int j = 0; j < numberofDrums; j++) {
//if (sequence[i] & bitmask == true) drum[j].hit();
//bitmask << (j+1)
//}
//}

class Drum {
public:
    Drum(PinName p) : x(p) {}
    
    void off() { x = 0; }
    void hit() { 
        x = 1;
        t.attach(this, &Drum::off, 0.01);
        //printf("drum hit");
    }
        
    DigitalOut x;
    Timeout t;
};
 
 
//PinDetect recordButton(p25); 

float timeIncrement=1;
BusIn step(p6, p7, p8 , p9);//button input for drums
BusOut myleds(LED1, LED2, LED3); 
int sequence[8]={0,0,0,0,0,0,0,0} ;
PinDetect BD_pb(p6);
PinDetect SD_pb(p7);
PinDetect CH_pb(p8);
PinDetect CP_pb(p9);
PinDetect recordButton(p30);
PinDetect playButton(p29);

Drum drums[4] = {p12, p13, p14, p15};//output for drums 

// SPST Pushbutton debounced count demo using interrupts and callback
// no external PullUp resistor needed
// Pushbutton from P8 to GND.
// A pb hit generates an interrupt and activates the callback function
// after the switch is debounced

// Global count variable
int volatile count=0;


void playback(){
    playMode= !playMode;
}

void record(void){
    recordMode = !recordMode;
    
}
/*
        
        while( j <timeIncrement){
            if (not step[0]){
                drums[0].hit();
                sequence[i] = sequence[i] + 1;
                i++;
                continue;
            
            }
            if (not step[1]){
                drums[1].hit();
                sequence[i] = sequence[i] + 1;
            continue;
            }
            if (not step[2]){
            drums[2].hit();
            i++;
            continue;
            }
            if (not step[3]){
            drums[3].hit();
            i++;
            continue;
            
            }
        recordMode = false;
            
    }
     
    while( i<8){
        //printf("recording \n");
        //sequence[i] =step;
        //printf("step = %i", sequence[i], "\n");
        if (not step[0]){
            drums[0].hit();
            i++;
            continue;
            
        }
        if (not step[1]){
            drums[1].hit();
            i++;
            continue;
        }
        if (not step[2]){
            drums[2].hit();
            i++;
            continue;
        }
        if (not step[3]){
            drums[3].hit();
            i++;
            continue;
        //continue;
        }
        //wait(timeI

*/
// Callback routine is interrupt activated by a debounced pb hit
void BD_pb_hit_callback (void) {
    printf("drum 0");
    drums[0].hit();
//    int currentStepCount = stepCount;
//    if (recordMode && currentStepCount != previousStepCountBD){
//        sequence[stepCount] += 1;
//    }
//    previousStepCountBD = stepCount; 
    if(recordMode){
        if ( (sequence[stepCount] & 1) == 0) {
            sequence[stepCount] += 1;
        }
    }
    //playback();
    //myled2 = !myled2 ;
    //count++;
    //myled4 = count & 0x01;
    //myled3 = (count & 0x02)>>1;
    //myled2 = (count & 0x04)>>2;
}

void SD_pb_hit_callback (void) {;
    drums[1].hit();
 //   int currentStepCount = stepCount;
//    if (recordMode && currentStepCount != previousStepCountSD){
//        sequence[stepCount] += 2;
//    }
//    previousStepCountSD = stepCount; 
    if(recordMode){
        if ( (sequence[stepCount] & 2) == 0) {
            sequence[stepCount] += 2;
        }
        //drums[1].hit();
    }
    //count++;
    //myled4 = count & 0x01;
    //myled3 = (count & 0x02)>>1;
    //myled2 = (count & 0x04)>>2;
}

void CH_pb_hit_callback (void) {
    drums[2].hit();
//    int currentStepCount = stepCount;
//    if (recordMode && currentStepCount != previousStepCountCH){
//        sequence[stepCount] += 4;
//    }
//    previousStepCountCH = stepCount; 
//    
    if(recordMode){
        if ( (sequence[stepCount] & 4) == 0) {
            sequence[stepCount] += 4;
        }
        //drums[2].hit();
    }
    //count++;
    //myled4 = count & 0x01;
    //myled3 = (count & 0x02)>>1;
    //myled2 = (count & 0x04)>>2;
}

void CP_pb_hit_callback (void) {
    drums[3].hit();
//    int currentStepCount = stepCount;
//    if (recordMode && currentStepCount != previousStepCountCP){
//        sequence[stepCount] += 8;
//    }
//    previousStepCountCP = stepCount; 
    if(recordMode){
        if ( (sequence[stepCount] & 8) == 0) {
            sequence[stepCount] += 8;
        }
        //drums[3].hit();
        
    }
    //count++;
    //myled4 = count & 0x01;
    //myled3 = (count & 0x02)>>1;
    //myled2 = (count & 0x04)>>2;
}
int main() {

    // Use internal pullup for pushbutton
    recordButton.mode(PullUp);
    playButton.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(2);
    printf("hello \n");
    // Setup Interrupt callback function for a pb hit
    recordButton.attach_deasserted(&record);
    playButton.attach_deasserted(&playback);
    BD_pb.attach_deasserted(&BD_pb_hit_callback);
    SD_pb.attach_deasserted(&SD_pb_hit_callback);
    CH_pb.attach_deasserted(&CH_pb_hit_callback);
    CP_pb.attach_deasserted(&CP_pb_hit_callback);
    
    // Start sampling pb input using interrupts
    recordButton.setSampleFrequency();
    playButton.setSampleFrequency();
    BD_pb.setSampleFrequency();
    SD_pb.setSampleFrequency();
    CH_pb.setSampleFrequency();
    CP_pb.setSampleFrequency();

    //Blink myled in main routine forever while responding to pb changes
    // via interrupts that activate the callback counter function
    while (1) {
        if (recordMode) {
            stepCount = 0;
            myled3=1;
            timer.start();
            while(1){
                if (timer > timeIncrement) {
                    if (stepCount == (seqLength-1) ) {
                        stepCount = 0;
                        } else { 
                        stepCount++;
                        }
                    timer.reset();
                    printf("sequence step value is: %d \n", sequence[stepCount]);
                }
                if (!recordMode) break;
            }
            myled3=0;
        //////PLAYMODE/////////
        if (playMode){
            stepCount = 0;
            myled4=1;
            timer2.start();
            while(1){
                if (timer2 > timeIncrement) {
                    if (stepCount == (seqLength-1) ) {
                        stepCount = 0;
                        } else { 
                        stepCount++;
                        for (int j = 0; j < numDrums ; j++) {
                            if ((sequence[stepCount] & bitMask) == 1) {
//                          printf("play drum %d \n", j);
                            drums[j].hit();
                            }
                        bitMask = bitMask << (j+1);  
                        }
                    }    
                    timer.reset();
                    printf("playmode sequence step value is: %d \n", sequence[stepCount]);
                }
            if (!playMode) break;
            }
            myled3=0;
        }   
    } 
}
}  
        //printf("%s \n", step);
        /*
        if (not step[0]){
            drums[0].hit();
            printf("%c \n", step);
        }
        if (not step[1]){
            drums[1].hit();
        }
        if (not step[2]){
            drums[2].hit();
        }
        if (not step[3]){
            drums[3].hit();
        }
        */
        //wait(timeIncrement);
        /*
        for(int i=0; i<4; i++){
            if(step[i]){
                drums[i].hit();
            }
            //drums[i%4].hit();
            wait(timeIncrement);
        
        }
        */
        //step.read();
        
        //switch(step){
            //case 0x0: myled3 = !myled3; break;
            //case 0x1: drums[0].hit(); break;
            //case 0x2: drums[1].hit(); break;
            //case 0x8: drums[2].hit(); break;
        
        //}
        
        //sequence.append(step);
        //wait(timeIncrement);
        
        //myled = !myled;
        //wait(timeIncrement);
        //wait(timeIncrement);
        /*
        for(int i=0; i<4; i++){
            drums[i].hit();
            wait(timeIncrement);
        }*/



