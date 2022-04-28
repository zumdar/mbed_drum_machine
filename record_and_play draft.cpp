#include "PinDetect.h"
#include "mbed.h"
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
PinDetect BD_pb(p6);
PinDetect SD_pb(p7);
PinDetect CH_pb(p8);
PinDetect CP_pb(p9);
PinDetect recordButton(p30);
PinDetect playButton(p29);
Drum drums[4] = {p12, p13, p14, p15}; // output for drums
// Global count variable
int volatile count = 0;

void playback() { playMode = !playMode; }

void record(void) { recordMode = !recordMode; }

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
  if (recordMode) {
    if ((sequence[stepCount] & 8) == 0) {
      sequence[stepCount] += 8;
    }
  }
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
  while (1) {
    if (recordMode) {
      stepCount = 0;
      myled3 = 1;
      timer.start();
      while (1) {
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
