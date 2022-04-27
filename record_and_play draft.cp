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
DigitalIn BD_pb(p6);
DigitalIn SD_pb(p7);
DigitalIn CH_pb(p8);
DigitalIn CP_pb(p9);
uint8_t bitMask = 1;
int seqLength = 8;
int numDrums = 4;



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
    }
        
    DigitalOut x;
    Timeout t;
};
 
 
//PinDetect recordButton(p25); 

float timeIncrement=1;
BusIn step(p6, p7, p8 , p9);//button input for drums
BusOut myleds(LED1, LED2, LED3); 
int sequence[8]; 
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
    myled4=1;
    int tempStep;
    for (int i=0; i< seqLength; i++) {
        for(int j=0; j<numDrums; j++) {
            if (sequence[i] & bitMask == 1) {
                drums[j].hit();
            }
        bitMask = bitMask << (j+1);    
        }  
    }
    /*
    for(int i=0; i < 8; i++){
        sequence[i] = tempStep;
        switch(tempStep) {
            case 0x3: printf("Hello!\n"); break; // p5 and p6 are 1
            case 0x8: printf("World!\n"); break; // p11 is 1
        if (not tempStep[1]){
            drums[1].hit();
        }
        if (not tempStep[2]){
            drums[2].hit();
        }
        if (not tempStep[3]){
            drums[3].hit();
        }
        wait(timeIncrement);
        
            
        
        
    }
    */
    myled4=0;
    //sequence.clear();
    
}


void record(){
    myled3=1;
    //int tempStep;
    for(int i=0; i<8; i++){
        
        sequence[i] =step;
        if (not step[0]){
            drums[0].hit();
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
        wait(timeIncrement);
    }
    myled3=0;
    //sequence.push_back(step);
    //for(int i =0; i< 99; i++){
        //step=i;
        
    //} 
    
}


// Callback routine is interrupt activated by a debounced pb hit
void pb_hit_callback (void) {
    //playback();
    myled2 = !myled2 ;
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
    printf("hello");
    // Setup Interrupt callback function for a pb hit
    recordButton.attach_deasserted(&record);
    playButton.attach_deasserted(&playback);
    // Start sampling pb input using interrupts
    recordButton.setSampleFrequency();
    playButton.setSampleFrequency();

    //Blink myled in main routine forever while responding to pb changes
    // via interrupts that activate the callback counter function
    while (1) {
        if (not step[0]){
            drums[0].hit();
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
        myled = !myled;
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
    }

}

