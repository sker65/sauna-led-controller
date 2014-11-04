
#include <IRremote.h>
#include "simtronyx_RGB_LED.h"

#define RECV_PIN 11

#define RED 5
#define GREEN 6
#define BLUE 9

IRrecv irrecv(RECV_PIN);

simtronyx_RGB_LED strip(RED,GREEN,BLUE);

// var that holds result from ir remote control
decode_results results;
// remember the last code received from remote to support repeat
unsigned long lastCode = 0;

// overall brightness 
int brightness = 100;
// speed for animation / color transitions
int fadingDelay = 100;

#define FADING_STEPS 80
#define COLOR_HOLD 20

void setup()
{
  // just debug out of code and colors
  Serial.begin(9600);
  // Start the ir receiver
  irrecv.enableIRIn();
  // switch to HSV color model
  strip.setAnimateColorType(ANIMATETYPE_HSV);

  strip.setRGB(128,0,0);

  // add ani colors, for now just one palette
  strip.animateColorAdd(0,100,100,FADING_STEPS,COLOR_HOLD); // red
  strip.animateColorAdd(45,100,100,FADING_STEPS,COLOR_HOLD);
  strip.animateColorAdd(60,100,80,FADING_STEPS,COLOR_HOLD);
  strip.animateColorAdd(120,100,80,FADING_STEPS,COLOR_HOLD);
  strip.animateColorAdd(160,100,60,FADING_STEPS,COLOR_HOLD);
  strip.animateColorAdd(240,100,100,FADING_STEPS,COLOR_HOLD);
  strip.animateColorAdd(280,100,100,FADING_STEPS,COLOR_HOLD);
}

// main loop
void loop() {
  // if button pressed on remote
  if (irrecv.decode(&results)) {
    // if button is hold down, value is -1 so dont print out
    if( results.value != 0xFFFFFFFF ) Serial.println(results.value, HEX);
    // process ir command
    processIRCode(results.value);
    // Receive the next value
    irrecv.resume();
  }
  // trigger loop method of strip (if transition is in progress)
  strip.loop();
  delay(100);
}

void processIRCode(unsigned long value) {
  // remote button was held down, repeat last cmd
  if( value != 0xFFFFFFFF ) {
    lastCode = value;
  } 
  else {
    value = lastCode;
  }

  // switch by ir command / code
  switch(value) {
  case 0xF7C03F: //on
    strip.setOnOff(true);
    break;
  case 0xF740BF: // off
    strip.setOnOff(false);
    break;

    // reihe 2
  case 0xF720DF: //red
    strip.setRGB(255,0,0);
    break;
  case 0xF7A05F: //green
    strip.setRGB(0,255,0);
    break;
  case 0xF7609F: //blue
    strip.setRGB(0,0,255);
    break;

    // reihe 3
  case 0xF710EF: // orange
    strip.setRGB(255,116,21);
    break;
  case 0xF7906F: // mint
    strip.setRGB(140,253,153);
    break;
  case 0xF750AF: // purp blue
    strip.setRGB(7,47,122);
    break;

    // reihe 4
  case 0xF730CF:
    strip.setRGB(217,170,3);
    break;
  case 0xF7B04F: 
    strip.setRGB(88,162,187);
    break;
  case 0xF7708F:
    strip.setRGB(37,12,67);
    break;

    // reihe 5
  case 0xF708F7:
    strip.setRGB(212,151,5);
    break;
  case 0xF78877:
    strip.setRGB(51,122,202);
    break;
  case 0xF748B7:  
    strip.setRGB(39,13,70);
    break;

    // reihe 6
  case 0xF728D7:
    strip.setRGB(239,211,5);
    break;
  case 0xF7A857:
    strip.setRGB(22,90,187);
    break;
  case 0xF76897:
    strip.setRGB(115,17,102);
    break;

  case 0xF700FF: // brighter
    brightness = inc(brightness,10,100);
    strip.setBrightness(brightness);
    break;
  case 0xF7807F: // darker
    brightness = dec(brightness,10);
    strip.setBrightness(brightness);
    break;

  case 0xF7D02F: // slower
    fadingDelay = inc(fadingDelay,100,5000);
    strip.setAnimateDelay(fadingDelay);
    break;
  case 0xF7F00F: // faster
    fadingDelay = dec(fadingDelay, 100 );
    strip.setAnimateDelay(fadingDelay);
    break;

  case 0xF7E817: // fade smooth
    strip.animateStart();
    break;

    /*  case 0xF708F7: // red up
     r = inc(r,10,255);
     break;
     case 0xF728D7: //red down
     r = dec(r,10);
     break;
     
     case 0xF78877: // green up
     g = inc(g,10,255);
     break;
     case 0xF7A857: //green down
     g = dec(g,10);
     break;
     
     case 0xF748B7: // blue up
     b = inc(b,10,255);
     break;
     case 0xF76897: //blue down
     b = dec(b,10);
     break;
     */
  }
}

int inc( int in, int delta, int limit ) {
  int r = in + delta;
  if( r > limit ) r = limit;
  return r;
}

int dec( int in, int delta ) {
  int r = in - delta;
  if( r < 0 ) r = 0;
  return r;
}















