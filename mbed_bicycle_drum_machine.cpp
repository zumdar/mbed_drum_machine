#include "PinDetect.h"
#include "mbed.h"
#include <vector>
// must import Cookbook PinDetct library into project
// URL: http://mbed.org/users/AjK/libraries/PinDetect/lkyxpw
// https://github.com/ozerik/EasyEi8ht used as reference

DigitalOut myled(LED1);
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
DigitalOut BD_trig(p12);
DigitalOut SD_trig(p13);
DigitalOut CH_trig(p14);
DigitalOut CP_trig(p15);
// DigitalIn BD_pb(p6);
// DigitalIn SD_pb(p7);
// DigitalIn CH_pb(p8);
// DigitalIn CP_pb(p9);
int volatile bitMask = 1;
int seqLength = 8;
int numDrums = 4;
bool volatile recordMode = false;
bool volatile playMode = false;

int volatile stepCount = 0;
Timer timer;
Timer timer2;
int previousStepCountBD, previousStepCountSD, previousStepCountCH,
    previousStepCountCP;

class Drum {
public:
  Drum(PinName p) : x(p) {}

  void off() { x = 0; }
  void hit() {
    x = 1;
    t.attach(this, &Drum::off, 0.005);
    // printf("drum hit");
  }

  DigitalOut x;
  Timeout t;
};

float timeIncrement = 0.3;
BusIn step(p6, p7, p8, p9); // button input for drums
BusOut myleds(LED1, LED2, LED3);
int sequence[8] = {0, 0, 0, 0, 0, 0, 0, 0};
PinDetect pbTap(p5);
PinDetect BD_pb(p6);
PinDetect SD_pb(p7);
PinDetect CH_pb(p8);
PinDetect CP_pb(p9);
PinDetect recordButton(p30);
PinDetect playButton(p29);
Drum drums[4] = {p12, p13, p14, p15}; // output for drums
// Global count variable
int volatile count = 0;
unsigned long quarterNote = 500; // sets the default internal tempo, quarter notes, in milliseconds. 500 = 120BPM
uint16_t sixteenthNote;
unsigned long tapDebounce = 0;
int tapTracker = -5;
unsigned long sixteenth;
unsigned long newTap;
unsigned long oldTap;
unsigned long tapAvg;
unsigned long taps[21];
uint32_t milli; 
uint32_t micro;
bool startRhythm = false;



void playback() { playMode = !playMode; }
void record(void) { recordMode = !recordMode; }
void tapMode(uint32_t millis, uint32_t micros);



void BD_pb_hit_callback(void) {
  drums[0].hit();
  if (recordMode) {
    if ((sequence[stepCount] & 1) == 0) {
      sequence[stepCount] += 1;
    }
  }
      
      
}

void SD_pb_hit_callback(void) {
  drums[1].hit();
  if (recordMode) {
    if ((sequence[stepCount] & 2) == 0) {
      sequence[stepCount] += 2;
    }
    
   
  }
 
}

void CH_pb_hit_callback(void) {
  drums[2].hit();
  if (recordMode) {
    if ((sequence[stepCount] & 4) == 0) {
      sequence[stepCount] += 4;
    }
  }

}

void CP_pb_hit_callback(void) {
  drums[3].hit();
  if (recordMode ) {
    if ((sequence[stepCount] & 8) == 0) {
      sequence[stepCount] += 8;
    }
  }
 
}

void pb_tap_hit_callback (void)
{
    tapMode(milli, micro);
     //printf("%d  ", quarterNote);
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
  
  clock_t x_startTime,x_countTime;
    
    
    pbTap.mode(PullUp);
    wait(.01);
    pbTap.attach_deasserted(&pb_tap_hit_callback);
    pbTap.setSampleFrequency();
    
    x_startTime=clock();
    
    
  while (1) {
    
   x_countTime=clock(); // update timer difference
   milli = (x_countTime-x_startTime)* 10;
   micro = milli * 1000;
   //pc.printf("%d  ", micro);
   /*if (startRhythm) //&& (x_countTime%quarterNote == 0))
   {
      printf("hello");
      for (int j = 0; j < numDrums; j++) {
            int tempbits = sequence[stepCount];
            if (((tempbits >> j) & 0x01) == (1)) {
              drums[j].hit();
              printf("hit the drum : %d \n", j);
            }
        }
      wait(quarterNote * .000001);
    }*/
    
    
    if (recordMode) {
      stepCount = 0;
      myled3 = 1;
      timer.start();
      while (1) {
     x_countTime=clock(); // update timer difference
      milli = (x_countTime-x_startTime)* 10;
      micro = milli * 1000;
       if (startRhythm)
        {
            timeIncrement = quarterNote*0.000001;
        }
        if (timer > timeIncrement) {
          for (int j = 0; j < numDrums; j++) {
            int tempbits = sequence[stepCount];
            if (((tempbits >> j) & 0x01) == (1)) {
              drums[j].hit();
              printf("hit the drum : %d \n", j);
            }
          }
          if (stepCount == (seqLength - 1)) {
            stepCount = 0;
          } else {
            stepCount++;
          }
          timer.reset();
          printf("sequence step value is: %d \n", sequence[stepCount]);
        }
        if (!recordMode)
          break;
      }
      myled3 = 0;
    }
    //////PLAYMODE/////////
    if (playMode) {
      
      stepCount = 0;
      myled4 = 1;
      timer2.start();
      while (1) {
      x_countTime=clock(); // update timer difference
      milli = (x_countTime-x_startTime)* 10;
      micro = milli * 1000;
       if (startRhythm)
        {
            timeIncrement = quarterNote*0.000001;
        }
        bitMask = 1;
        if (timer2 > timeIncrement) {
          for (int j = 0; j < numDrums; j++) {
            int tempbits = sequence[stepCount];
            if (((tempbits >> j) & 0x01) == (1)) {
              drums[j].hit();
              printf("hit the drum : %d \n", j);
            }
          }
          if (stepCount == (seqLength - 1)) {
            stepCount = 0;
          } else {
            stepCount++;
          }
          timer2.reset();
          
          printf("playmode sequence step value is: %d \n", sequence[stepCount]);
        }
        if (!playMode)
          break;
      }
      myled4 = 0;
    }
  }
}


void tapMode(uint32_t millis, uint32_t micros) 
{
  if (millis - tapDebounce > 1500) 
  {  // first tap after 1.5 seconds
    tapTracker = -5;
    oldTap = micros;
    tapAvg = 0;
     //pc.printf("%d  ", tapTracker);
    //myled1 = tapTracker;
  
  } 
  else if (tapTracker < 0) 
  {          // some subsequent taps
    newTap = micros;
    unsigned long TA = newTap - oldTap;
    if ((TA > 200000) && (TA < 1200000)) 
    {  // limits BPM inputs to between 300bpm and 50 bpm
      tapAvg += newTap - oldTap;
      //myled1 = tapTracker;
     // pc.printf("tapTracker: %d" ,tapTracker);
    } 
    else 
    {
        tapTracker--;  
    }
    oldTap = newTap;
    if (tapTracker == -1) 
    {         // fills the array of numbers to be averaged with the calculated average so far
      startRhythm = true; 
      tapAvg = tapAvg * 0.25;
      for (int i = 0; i < 20; i++) //changed byte to int
      {
        taps[i] = tapAvg;
      }
      //myled2 = tapAvg;
    // pc.printf("tap Avg: %d", tapAvg);
      quarterNote = tapAvg;
      tapAvg = 0;
     }
   } 
  else 
  {              // regular taps, Each tap increases the accuracy of the average, up to 20 taps
    newTap = micros;
    taps[tapTracker] = newTap - oldTap;
    oldTap = newTap;
    for (int i = 0; i < 20; i++)  // changed byte to int
    {
      if (taps[i] > ((sixteenthNote * 2) + 70000) && (taps[i] < (quarterNote * 2) - 70000)) 
      {
        tapAvg += taps[i];
      } 
      else 
      {
        tapAvg += taps[i + 1];
      }
    }
    quarterNote = (tapAvg + 240) * 0.05;
    
    //myled2 = tapAvg;
   // pc.printf("tap Avg: %d", tapAvg);
    tapAvg = 0;
   }
   
  tapDebounce = millis;
  tapTracker++;
  
  if (tapTracker >= 20)
  {
       tapTracker = 0;
  }
}



