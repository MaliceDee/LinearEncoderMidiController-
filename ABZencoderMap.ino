#include <avr/interrupt.h>

#define encoder0PinA  3            //Quadrature Track A
#define encoder0PinB  4            //Quadrature Track B
#define encoder0PinC  2            //Index Track

//encoder variables
volatile unsigned int  encoder0Pos = 0;   //the encoder position variable.
int start = true;                        //variable used to indicate the first time the encoder sees the index track.
int index = 0;                          //the total index.
int decimal = 0;                       //tenths of a index.
int encoderHome = 0;                  //used to reset encoder0Pos if any counts are missed during a cycle.
int ccw = false;                     //which direction the encoder is moving.
int indexCount = 0;                 //variable for index count.
static long lastCount = 0L;        //used to remember previous index count.
void setup() {

  //encoder pinModes
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  pinMode(encoder0PinC, INPUT);


  attachInterrupt(1, doEncoder, CHANGE);  // encoder track A on interrupt 1 - pin 3
  attachInterrupt(0, doIndex, FALLING);  // encoder Index track on interupt 0 - pin 2


  Serial.begin(115200);                  //start serial communication

}

void loop() {
    Serial.println ("Count =");          //print "index =" to the serial monitor
    decimal = (encoder0Pos / 32);        //divide position into tenths
    if (index <= -1) {                   //if index goes into negative numbers.
      indexCount = (abs(index + 1));
      decimal = (9 - decimal);           //tenths now counts in the reverse direction when index is negative
      Serial.print ("-");                //print a negative sign when index is negative.
    } else {
      indexCount = index;                //if index is not negative then the indexCount equals the index.
    }
    Serial.print (indexCount);
    Serial.print (".");                 //print a decimal point after the index.
    Serial.print (decimal);             //print the tenths of a index after the decimal point.
}


void doIndex() {                          //every time the index track comes around doIndex will run.
  if (start == true) {                   //if this is the first time the index track has come around
    encoderHome = encoder0Pos;          //the encoder Home position will equal the current encoder position.
    start = false;                     //tell the arduino that start is over.
  } else {
    encoder0Pos = encoderHome;
  }                                    //if this is not the first time index has come around reset the encoder position
}                                     //so that if any counts are missed because of high speed or too high of resolution.
//they do not mess up the total index.

void doEncoder() {                         //every time a change happens on encoder pin A doEncoder will run.

  if (digitalRead(encoder0PinA) == HIGH) {        // found a low-to-high on channel A
    if (digitalRead(encoder0PinB) == LOW) {       // check channel B to see which way encoder is moving
      ccw = true;         // CCW;
    } else {
      ccw = false;        // CW;
    }
  } else {                                       // found a high-to-low on channel A
    if (digitalRead(encoder0PinB) == LOW) {      // check channel B to see which way encoder is moving
      ccw = false;        // CW;
    } else {
      ccw = true;        // CCW;
    }
  }
  if (ccw == true) {
    encoder0Pos = (encoder0Pos - 1);          //if encoder is spinning CCW subtract a count from encoder position
    if (encoder0Pos == -1) {                 //if encoder position is equal to -1
      encoder0Pos = 319;                    //make encoder position equal to 319. (change if needed)
      index = (index - 1);
    }                                         //if encoder position is changed to 319 subtract 1 from index.
                                             //this keeps rolling over the encoder positions so that it never
                                            //gets below 0 counts
  } else {
    encoder0Pos = (encoder0Pos + 1);           //if encoder is spinning CW add a count to encoder position
    if (encoder0Pos == 320) {                 //if encoder position is equal to 320.(change if needed)
      encoder0Pos = 0;                       //make encoder position equal to 0.
      index = (index + 1);
    }                                         //if encoder position is changed to 0 then add 1 to index.
  }                                          //this keeps rolling over the encoder positions so that it never
}                                           //gets over max count

//reset the counter (indexCount) with start=true, encoder0Pos=0, and index=0
