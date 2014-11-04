/*
 simtronyx_RGB_LED.h - Library RGB LEDs & Strips.
 Based on a Library by Ronny Simon, 2013.
 (c) Ronny Simon  http://blog.simtronyx.de
 extended by sr (c): added hold time, removed delay calls. 
 */

#ifndef SIMTRONYX_RGB_LED_h
#define SIMTRONYX_RGB_LED_h

// Add the possibility to use other Serial-Ports (for instance MEGA or Leonardo)
#ifndef SERIALPORT
#define SERIALPORT Serial
#endif

#include "Arduino.h"

#define ANIMATETYPE_RGB 1
#define ANIMATETYPE_HSV 2

#define MAX_ANI_COLORS 25
#define ARRAY 5
#define COL1_OFFSET 0
#define COL2_OFFSET 1
#define COL3_OFFSET 2
#define STEP_OFFSET 3
#define HOLD_OFFSET 4

#define MAXANIMATECOLORSARRAY MAX_ANI_COLORS*ARRAY  // 25 (MaxColors) * 4 (r,g,b,steps, hold OR h,s,v,steps,hold)

class simtronyx_RGB_LED{

public:
  simtronyx_RGB_LED(int pinR,int pinG,int pinB);

  void loop();

  void setRGBAdjust(int r=256,int g=256,int b=256);
  void useRGBAdjust(boolean isAdjust);
  void setBrightness(int b=100);
  void setOnOff(boolean o);

  void setRGB(int r,int g,int b);
  void setHSV(float h,float s,float v);

  void animateStart();
  void animateStop();

  void animateColorsClear();
  boolean animateColorAdd(int col1,int col2,int col3,int steps, int hold=10, int num=-1);

  void setAnimateDelay(int delay);
  void setAnimateColorType(byte type);
  
  boolean rgbAdjustActive;

private:
  boolean on;
  int brightness;
  int pinRed;
  int pinGreen;
  int pinBlue;

  void writeRGB(int r,int g,int b);
  int getAdjustedColor(int index, int value );
  void updateStrip();

  void animateColorSet();
  void animateCalculateRGB();
  void animateRun();

  void HSV2RGB(float h,float s,float v,int *r,int *g,int *b);	

  // actual colors
  int c0;
  int c1;
  int c2;
  
  // actual deltas in current transition 
  int deltaC0;
  int deltaC1;
  int deltaC2;
  
  // actual rgb
  int red,green,blue;

  boolean isAnimateActive;

  int animateColors[MAXANIMATECOLORSARRAY];
  int animateColorsActual;
  int animateColorsCount;

  byte animateColorType;
  int animateDelay;

  long animateStepCount; // counts actual animation steps for actual color transition

  unsigned long nextTime;

  // adjust factors to get real white
  int adjust[3];
};

#endif // SIMTRONYX_RGB_LED_h


