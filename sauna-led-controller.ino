
#include <IRremote.h>

int RECV_PIN = 11;

#define RED 5
#define GREEN 6
#define BLUE 9

IRrecv irrecv(RECV_PIN);

decode_results results;
int r = 0;
int g = 0;
int b = 0;
boolean on = false;
unsigned long lastCode = 0;
int brightness = 255;
#define STEP 5 // brightness steps

int fadeDelay = 300;
int holdDelay = 20000;

boolean fading = false;
boolean inTransition = false;
int fadingSpeed = 100;
int actPaletteIndex = 0;
int fadingSteps = 50;
int actStep = 0;

// deltas for fading
int dr;
int dg;
int db;

unsigned long pal[] = { 
  0xD92727, 0xFC8F12, 0xFFE433, 0x6FCC43, 0x0DB8B5,0x8000FF, 0x006E96, 0x452C57, 0xCC0052, 0xFF6136 };
unsigned long pal1[] = { 
  0xFF0000, 0x0000FF }; //0x00FF00, 0x00FFFF, 0xFF00FF };

void update() {
  if( on ) {
    unsigned int red = ( r*brightness / 255 ) & 0xFF;
    unsigned int green = ( g*brightness / 255) & 0xFF;
    unsigned int blue = ( b*brightness / 255) & 0xFF;
    analogWrite(RED, red); 
    analogWrite(GREEN, green); 
    analogWrite(BLUE, blue);
    Serial.print( "b rgb: " );
    Serial.print( brightness );
    Serial.print( " " );
    Serial.print( red,HEX );
    Serial.print( " " );
    Serial.print( green,HEX );
    Serial.print( " " );
    Serial.println( blue,HEX );
  }
  else {
    Serial.println("off");
    analogWrite(RED, 0); 
    analogWrite(GREEN, 0); 
    analogWrite(BLUE, 0);
  } 
}

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  on = false;
  update();
}

int red( unsigned long rgb ) {
  return rgb >> 16;
}

int green( unsigned long rgb ) {
  return (rgb >> 8) & 0xFF;
}

int blue( unsigned long rgb ) {
  return rgb & 0xFF;
}

void startFading() {
  fading = true;
  // calc deltas
  int fromIndex = actPaletteIndex;
  int toIndex = fromIndex + 1;
  if( toIndex == sizeof(pal)/sizeof(unsigned long) ) toIndex = 0;

  r = red( pal[fromIndex] );
  g = green( pal[fromIndex] );
  b = blue( pal[fromIndex] );
  update();

  Serial.print( "fade from / to "); 
  Serial.print(fromIndex); 
  Serial.print(" "); 
  Serial.println(toIndex);
  dr = ( red( pal[toIndex] ) - red( pal[fromIndex] ) )  / fadingSteps;
  dg = ( green( pal[toIndex] ) - green( pal[fromIndex] ) )  / fadingSteps;
  db = ( blue( pal[toIndex] ) - blue( pal[fromIndex] ) )  / fadingSteps;
  Serial.print( "dr/dg/db: "); 
  Serial.print(dr); 
  Serial.print(" "); 
  Serial.print(dg); 
  Serial.print(" ");
  Serial.println(db);
  actStep = 0;
}

void endFading() {
  fading = false;
}

unsigned long nextTime = 0;

void handleFading() {
  unsigned long now = millis();
  if( now > nextTime ) {
    if( inTransition ) {
      nextTime = now + fadeDelay + (100-fadingSpeed) * 30 ;
      r += dr;
      g += dg;
      b += db;
      if( actStep++ >= fadingSteps ) {
        inTransition = false;
        nextTime = now + holdDelay + (100-fadingSpeed) * 1000;
      }
      update();
    } 
    else { // we was in hold
      actPaletteIndex += 1;
      if( actPaletteIndex == sizeof(pal)/sizeof(unsigned long) ) actPaletteIndex = 0;
      inTransition = true;
      startFading();
    }
  }
}

void loop() {
  if (irrecv.decode(&results)) {
    if( results.value != 0xFFFFFFFF ) Serial.println(results.value, HEX);
    process(results.value);
    irrecv.resume(); // Receive the next value
  }
  if( fading) handleFading();
  delay(100);
}

void process(unsigned long value) {
  if( value != 0xFFFFFFFF ) {
    lastCode = value;
  } 
  else {
    value = lastCode;
  }
  switch(value) {
  case 0xF7C03F: //on
    on = true;
    break;
  case 0xF740BF: // off
    on = false;
    break;

  case 0xF720DF: //red
    r = 255; 
    g = b = 0; 
    endFading();
    break;
  case 0xF7A05F: //green
    r = b = 0; 
    g = 255; 
    endFading();
    break;
  case 0xF7609F: //blue
    r = g = 0;
    b = 255; 
    endFading();
    break;
  case 0xF710EF: // orange
    r = 255;
    g = 116;
    b = 21;
    endFading();
    break;
  case 0xF7906F: // mint
    r = 140;
    g = 253;
    b = 153;
    fading = false;
    break;
  case 0xF750AF: // purp blue
    r = 7;
    g = 47;
    b = 122;
    endFading();
    break;

  case 0xF700FF: // brighter
    brightness = inc( brightness, STEP, 255 );
    break;
  case 0xF7807F: // darker
    brightness = dec( brightness, STEP );
    break;

  case 0xF7D02F: // slower
    fadingSpeed = dec(fadingSpeed,10);
    break;
  case 0xF7F00F: // faster
    fadingSpeed = inc(fadingSpeed, 10,100);
    break;

  case 0xF7E817: // fade smooth
    startFading();
    break;

  case 0xF708F7: // red up
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
  }
  update();
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











