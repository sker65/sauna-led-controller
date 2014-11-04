/*
 simtronyx_RGB_LED.cpp - Library RGB LEDs & Strips.
 Based on a Library by Ronny Simon, 2013.
 (c) Ronny Simon  http://blog.simtronyx.de
 extended by sr (c): added hold time, removed delay calls. 
 */

#include "Arduino.h"
#include "simtronyx_RGB_LED.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Init

simtronyx_RGB_LED::simtronyx_RGB_LED(int pinR,int pinG,int pinB){

  pinRed=pinR;
  pinGreen=pinG;
  pinBlue=pinB;

  pinMode(pinRed,OUTPUT);
  pinMode(pinGreen,OUTPUT);
  pinMode(pinBlue,OUTPUT);

  setRGBAdjust();
  useRGBAdjust(false);
  
  c0=c1=c2=0;
  red=green=blue=0;
  deltaC0=deltaC1=deltaC2=0;

  animateColorType=ANIMATETYPE_RGB;
  animateColorsClear();

  animateDelay=100;
  brightness = 100; // max
  on = false;
  nextTime = millis();
  updateStrip();
}

void simtronyx_RGB_LED::setBrightness(int b ) {
  brightness = b;
  //SERIALPORT.print("brightness ");SERIALPORT.println(b);
  updateStrip();
}

void simtronyx_RGB_LED::setOnOff(boolean b ) {
  on = b;
  updateStrip();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Loop (if animate is active)

void simtronyx_RGB_LED::loop(){
  if((isAnimateActive)&&(animateColorsCount))animateRun();
}

//////////////////////////////////////////////////////////////////////////////////////////
// adjust R,G,B Values to make Color Correction for different LED Strips
// normally you do not have a clear white light if you set all colors to 255
// with this function you can adjust this behaviour

void simtronyx_RGB_LED::setRGBAdjust(int r,int g,int b){

  adjust[0] = r;
  adjust[1] = g;
  adjust[2] = b;

  rgbAdjustActive=true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// rgb adjust - on/off

void simtronyx_RGB_LED::useRGBAdjust(boolean isAdjust){
  rgbAdjustActive=isAdjust;
}

//////////////////////////////////////////////////////////////////////////////////////////
// SET RGB VALUES

void simtronyx_RGB_LED::setRGB(int r,int g,int b){
  animateStop();
  writeRGB(r*100,g*100,b*100);
}

void simtronyx_RGB_LED::updateStrip() {
  SERIALPORT.print(getAdjustedColor(0,red/100));
  SERIALPORT.print(",");
  SERIALPORT.print(getAdjustedColor(1,green/100));
  SERIALPORT.print(",");
  SERIALPORT.print(getAdjustedColor(2,blue/100));
  SERIALPORT.println("");
  if( on ) {
    analogWrite(pinRed,getAdjustedColor(0,red/100));
    analogWrite(pinGreen,getAdjustedColor(1,green/100));
    analogWrite(pinBlue,getAdjustedColor(2,blue/100));
  } 
  else {
    analogWrite(pinRed,0);
    analogWrite(pinGreen,0);
    analogWrite(pinBlue,0);
  }
}

int simtronyx_RGB_LED::getAdjustedColor(int index, int value ) {
  if( rgbAdjustActive ) {
    return (  value * adjust[index] * brightness  ) / 25600;
  } 
  else {
    return ( value * brightness ) / 100;
  }
}

void simtronyx_RGB_LED::writeRGB(int r,int g,int b){

  if(r>25500)r=25500;
  else if(r<0)r=0;
  if(g>25500)g=25500;
  else if(g<0)g=0;
  if(b>25500)b=25500;
  else if(b<0)b=0;

  red = r;
  green = g;
  blue = b;

  updateStrip();
}

//////////////////////////////////////////////////////////////////////////////////////////
// SET HSV VALUES

void simtronyx_RGB_LED::setHSV(float h,float s,float v){

  int r,g,b;

  HSV2RGB(h,s,v,&r,&g,&b);

  writeRGB(r,g,b);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ANIMATE FUNCTIONS

void simtronyx_RGB_LED::setAnimateDelay(int v){
  if(v<50)v=50;
  animateDelay=v;
}

void simtronyx_RGB_LED::setAnimateColorType(byte type){
  animateColorType=type;
}

void simtronyx_RGB_LED::animateStart(){
  isAnimateActive=true;
  animateColorsActual=0;
  animateStepCount=0;
  animateCalculateRGB();
}

void simtronyx_RGB_LED::animateStop(){
  isAnimateActive=false;
}

void simtronyx_RGB_LED::animateColorsClear(){
  animateColorsCount=0;
  animateColorsActual=0;
  animateStepCount=0;
  isAnimateActive=false;
}

boolean simtronyx_RGB_LED::animateColorAdd(int col1,int col2,int col3,int steps, int hold, int num){

  if((num>=0)&&(num<animateColorsCount)){
    animateColors[num*ARRAY+0]=col1*100;
    animateColors[num*ARRAY+1]=col2*100;
    animateColors[num*ARRAY+2]=col3*100;
    animateColors[num*ARRAY+STEP_OFFSET]=steps;
    animateColors[num*ARRAY+HOLD_OFFSET]=hold;
    return true;
  }
  else if(num!=-1)return false;
  else if(animateColorsCount<(int)(MAXANIMATECOLORSARRAY/ARRAY)){

    animateColors[animateColorsCount*ARRAY+0]=col1*100;
    animateColors[animateColorsCount*ARRAY+1]=col2*100;
    animateColors[animateColorsCount*ARRAY+2]=col3*100;
    animateColors[animateColorsCount*ARRAY+STEP_OFFSET]=steps;
    animateColors[animateColorsCount*ARRAY+HOLD_OFFSET]=hold;
    animateColorsCount++;
    return true;
  }
  return false;
}

void simtronyx_RGB_LED::animateColorSet(){

  switch(animateColorType){

  case ANIMATETYPE_HSV:

    setHSV(c0/100,c1/100,c2/100);
    break;

  case ANIMATETYPE_RGB:

    writeRGB(c0,c1,c2);
    break;
  }
}

void simtronyx_RGB_LED::animateRun(){

  unsigned long now = millis();
  if( now > nextTime ) { 
    if(animateStepCount-1>0){

      c0+=deltaC0;
      c1+=deltaC1;
      c2+=deltaC2;

      animateColorSet();
      nextTime = now + animateDelay; 
      animateStepCount--;
    } 
    else {
      // set target color
      SERIALPORT.print("set target ");
      SERIALPORT.println(animateColorsActual);
      c0=animateColors[animateColorsActual*ARRAY+0];
      c1=animateColors[animateColorsActual*ARRAY+1];
      c2=animateColors[animateColorsActual*ARRAY+2];

      animateColorSet();

      animateColorsActual++;
      if( animateColorsActual == animateColorsCount) {
        animateColorsActual=0;
      }

      animateCalculateRGB();

      // hold this color for a while      
      nextTime = now + (animateDelay*animateColors[animateColorsActual*ARRAY+HOLD_OFFSET]);
    }
  } // now > nextTime
}

void simtronyx_RGB_LED::animateCalculateRGB(){
  animateStepCount=animateColors[animateColorsActual*ARRAY+3];
  deltaC0=(animateColors[animateColorsActual*ARRAY+0]-c0)/animateStepCount;
  deltaC1=(animateColors[animateColorsActual*ARRAY+1]-c1)/animateStepCount;
  deltaC2=(animateColors[animateColorsActual*ARRAY+2]-c2)/animateStepCount;
  SERIALPORT.print(animateColorsActual);SERIALPORT.print(" ");SERIALPORT.print(deltaC0);SERIALPORT.print(" ");SERIALPORT.print(deltaC1);SERIALPORT.print(" ");SERIALPORT.println(deltaC2);
}

//////////////////////////////////////////////////////////////////////////////////////////
// HSV2RGB  h=0.0..360.0 , s=0.0..100.0 , v=0.0..100.0

void simtronyx_RGB_LED::HSV2RGB(float h,float s,float v,int *r,int *g,int *b) {

  int i;
  float m, n, f;

  h=max(0.0, min(360.0, h));
  s=max(0.0, min(100.0, s));
  v=max(0.0, min(100.0, v));

  s/=100;
  v/=100;

  if(s==0){
    *r=*g=*b=round(v*25500);
    return;
  }

  h/=60;
  i=floor(h);
  f=h-i;

  if(!(i&1)){
    f=1-f;
  }

  m=v*(1-s);
  n=v*(1-s*f);

  switch (i) {

  case 0: 
  case 6:
    *r=round(v*25500);
    *g=round(n*25500);
    *b=round(m*25500);
    break;

  case 1: 
    *r=round(n*25500);
    *g=round(v*25500);
    *b=round(m*25500);
    break;

  case 2: 
    *r=round(m*25500);
    *g=round(v*25500);
    *b=round(n*25500);
    break;

  case 3: 
    *r=round(m*25500);
    *g=round(n*25500);
    *b=round(v*25500);
    break;

  case 4: 
    *r=round(n*25500);
    *g=round(m*25500);
    *b=round(v*25500);
    break;

  case 5: 
    *r=round(v*25500);
    *g=round(m*25500);
    *b=round(n*25500);
    break;
  }
}




