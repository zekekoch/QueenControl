#include "FastSPI_LED2.h"
#include <Wire.h>

const int NUM_LEDS = 300;
const int ledCount = NUM_LEDS;
CRGB leds[NUM_LEDS];

// constants won't change. They're used here to 
// set pin numbers:
const int intensifierPin = 6; //was 9     // the number of the pushbutton pin
const int emissionPin = 7; // was 11     // the number of the pushbutton pin
const int lifePin = 8; // was 10     // the number of the pushbutton pin

const int cutOffPin = A0;

const int meterPin = 23;

const int isntLowPin = 3;
const int isMediumPin = 4;
const int isContinuityPin = 5;
const int isntDynamicPin = 2;


// variables will change:
byte intensifierState = 0;         // variable for reading the pushbutton status
byte lifeState = 0;         // variable for reading the pushbutton status
byte emissionState = 0;         // variable for reading the pushbutton status
int cutOffState = 0;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(intensifierPin, INPUT);     
  pinMode(lifePin, INPUT);   
  pinMode(emissionPin, INPUT); 
  pinMode(cutOffPin, INPUT); 
  pinMode(isntLowPin, INPUT);
  pinMode(isMediumPin, INPUT);
  pinMode(isContinuityPin, INPUT);
  pinMode(isntDynamicPin, INPUT);   

  pinMode(meterPin, OUTPUT);

  digitalWrite(intensifierPin, HIGH);
  digitalWrite(lifePin, HIGH);
  digitalWrite(emissionPin, HIGH);
  digitalWrite(isntLowPin, HIGH);
  digitalWrite(isMediumPin, HIGH);
  digitalWrite(isContinuityPin, HIGH);
  digitalWrite(isntDynamicPin, HIGH);

  Wire.begin();

  LEDS.setBrightness(255);
  LEDS.addLeds<WS2811, 15, RGB>(leds, NUM_LEDS);
  LEDS.addLeds<WS2811, 2, RGB>(leds, NUM_LEDS);


  Serial.begin(9600);
}

/*
If BottomCF then Dynamic
  If TopAF, Dynamic Low
  If TopHF, Dynamic Medium
  If TopCF, Dynamic High

If TopEB then Continuity
If TopCE then Emission
*/
byte dialState()
{

  Serial.print(" [F");Serial.print(digitalRead(isntDynamicPin));
  Serial.print(" C");Serial.print(digitalRead(isContinuityPin));
  Serial.print(" A");Serial.print(digitalRead(isntLowPin));
  Serial.print(" H");Serial.print(digitalRead(isMediumPin));
  Serial.print("] ");

  if (digitalRead(isntDynamicPin) == HIGH)
  {
      return 2;
  } 
  else 
  {
    if (digitalRead(isContinuityPin) == HIGH)
    {
      return 1;
    }
    if (digitalRead(isntLowPin) == LOW)
    {
      return 3;
    }
    else if (digitalRead(isMediumPin) == LOW)
    {
      return 4;
    }
    else
    {
      return 5;
    }

  }
}

void color_bounce() { //-BOUNCE COLOR (SINGLE LED)
  static bool bounceForward = true;
  static int idex = 0;
  if (bounceForward)
  {
      idex++;
      if (idex == ledCount)
      {
          bounceForward = false;
          idex--;
      }
  }
  else
  {
      idex--;
      if (idex == 0)
      {
          bounceForward = true;
      }
  }

  for(int i = 0; i < ledCount; i++ ) {
      if (i == idex) {
          setPixel(i, CRGB::Red);
      }
      else {
          setPixel(i, CRGB::Black);
      }
  }
}

void setPixel(int adex, CRGB c) {
  if (adex < 0 || adex > ledCount-1)
      return;

  leds[adex] = c;
} 

void loop(){
  // read the state of the pushbutton value:
  readState();
  delay(5);
}

byte readState()
{
  intensifierState = !digitalRead(intensifierPin);
  lifeState = digitalRead(lifePin);
  emissionState = !digitalRead(emissionPin);
  cutOffState = map(analogRead(cutOffPin),0,1024,0,255);

  analogWrite(meterPin, cutOffState);

  Serial.print(" M");Serial.print(dialState());
  Serial.print(" I");Serial.print(intensifierState);
  Serial.print(" L");Serial.print(lifeState);
  Serial.print(" E");Serial.print(emissionState);
  Serial.print(" C");Serial.print(cutOffState);
  Serial.println();


  Wire.beginTransmission(47);
  Wire.send('E');
  Wire.send(emissionState);
  Wire.endTransmission();

  Wire.beginTransmission(47);
  Wire.send('C');
  Wire.send(cutOffState);
  Wire.endTransmission();

  Wire.beginTransmission(47);
  Wire.send('I');
  Wire.send(intensifierState);
  Wire.endTransmission();

  Wire.beginTransmission(47);
  Wire.send('L');
  Wire.send(lifeState);
  Wire.endTransmission();

  Wire.beginTransmission(47);
  Wire.send('D');
  Wire.send(dialState());
  Wire.endTransmission();


  return 0;
}
