#include "mbed.h"
#include "PinDetect.h"
#include <time.h>

void tapMode(uint32_t millis, uint32_t micros);

unsigned long quarterNote = 500; // sets the default internal tempo, quarter notes, in milliseconds. 500 = 120BPM
uint16_t sixteenthNote;
unsigned long tapDebounce = 0;
int tapTracker = -5;

// unsigned long tempo;
// unsigned long flash;
// unsigned long debounce;
// unsigned long debounce2;

unsigned long sixteenth;
unsigned long newTap;
unsigned long oldTap;
unsigned long tapAvg;
unsigned long taps[21];
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
PinDetect pbTap(p5);
Serial pc(USBTX, USBRX);

Timer t;
float f;
uint32_t milli;
uint32_t micro;

void pb_tap_hit_callback(void)
{
  tapMode(milli, micro);
  pc.printf("%d  ", quarterNote);
}

int main()
{
  clock_t x_startTime, x_countTime;

  pbTap.mode(PullUp);
  wait(.01);
  pbTap.attach_deasserted(&pb_tap_hit_callback);
  pbTap.setSampleFrequency();

  x_startTime = clock();

  while (1)
  {
    x_countTime = clock(); // update timer difference
    milli = (x_countTime - x_startTime) * 10;
    micro = milli * 1000;
    // pc.printf("%d  ", milli);
  }
}

void tapMode(uint32_t millis, uint32_t micros)
{
  if (millis - tapDebounce > 1500)
  { // first tap after 1.5 seconds
    tapTracker = -5;
    oldTap = micros;
    tapAvg = 0;
    // pc.printf("%d  ", tapTracker);
    // myled1 = tapTracker;
  }
  else if (tapTracker < 0)
  { // some subsequent taps
    newTap = micros;
    unsigned long TA = newTap - oldTap;
    if ((TA > 200000) && (TA < 1200000))
    { // limits BPM inputs to between 300bpm and 50 bpm
      tapAvg += newTap - oldTap;
      // myled1 = tapTracker;
      // pc.printf("tapTracker: %d" ,tapTracker);
    }
    else
    {
      tapTracker--;
    }
    oldTap = newTap;
    if (tapTracker == -1)
    { // fills the array of numbers to be averaged with the calculated average so far
      tapAvg = tapAvg * 0.25;
      for (int i = 0; i < 20; i++) // changed byte to int
      {
        taps[i] = tapAvg;
      }
      // myled2 = tapAvg;
      // pc.printf("tap Avg: %d", tapAvg);
      quarterNote = tapAvg;
      tapAvg = 0;
    }
  }
  else
  { // regular taps, Each tap increases the accuracy of the average, up to 20 taps
    newTap = micros;
    taps[tapTracker] = newTap - oldTap;
    oldTap = newTap;
    for (int i = 0; i < 20; i++) // changed byte to int
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
    // myled2 = tapAvg;
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
