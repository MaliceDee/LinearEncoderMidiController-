#include <MIDIUSB2.h>
/*
HIGH RESOLUTION LINEAR ENCODER AS A LARGE SLIDING MIDI CONTROLLER
 * Created: Jim Foster (feb, 2020) 
 * Purpose/ Intended functions;
  * To read values from a 400mm TTL linear encoder with a resolution of 5um and a force sensitive resistor
    to be translated into corresponding Midi data
 * Encoder; 
   * Position determines a value between 0-127 to be sent via CC1 (modulation) 
 * Fsr;
   * Senses Velocity and continuous pressure to be sent as aftertouch values 
 -------------------------------------------------------------------------------
* Problems;
  * Encoder values can be unstable, sometimes needs to be re-zero'd 
   encoder has a/b/z channels but only a/b are being used, not sure how to implement 
   the index(z) channel for more accurate readings, in the meantime will 
   work on writing a function to reset the base position with a button press. 
  * not sure if how I am using interupts in my sketch is sufficient, sketch might be
   slowing down the interupt functions? I don't know, the encoder has an incredibly 
   high resolution which puts out a large number of counts over a small travel length
   and can miss pulses at high speeds. 
 */
//Define a bunch of things 
#define MIDI_CHANNEL 1 
#define Threshold 150 
#define MaxPressure 1000 
//States to check what state the fsr is in
#define NOTE_OFF 1
#define RISE_WAIT 2
#define NOTE_ON 3 
//Time to check/ update states 
#define AT_INTERVAL 40
#define RISE_TIME 3 
//PIN's definition
// encoder pins 
#define encoder0PinA  2
#define encoder0PinB  3
// fsr pin 
#define sensorPin A0

//State machine variables 
int noteSounding; 
int sensorValue; 
int state;
unsigned long breath_on_time = 0L;
int initial_breath_value;
int atVal;
unsigned long atSendTime = 0L;

//Encoder variables
volatile int encoder0Pos = 0;
volatile long lastPos = 0;
volatile boolean PastA = 0;
volatile boolean PastB = 0;
volatile int CCval = 0L;
volatile long lastCCval = 0L;

void setup()
{
  state = NOTE_OFF;  
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  PastA = (boolean)digitalRead(encoder0PinA); //initial value of channel A;
  PastB = (boolean)digitalRead(encoder0PinB); //and channel B
  // encoder A channel on interrupt 0 (Arduino's pin 2)
  attachInterrupt(0, doEncoderA, FALLING);
  // encoder B channel pin on interrupt 1 (Arduino's pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
  
//uncomment next line for debugging 
//Serial.begin(115200);

}

void loop()
{ 
  CCval = encoder0Pos/20; //Scale encoder pulses down to familiar range ( 0-1023)
  CCval = CCval/8;       //Divide by 8 to get range within workale midi values (0-127) 
  if (CCval != lastCCval) { //if interupt routine returns a scaled value greater than last value
  MidiUSB.sendControlChange(1, CCval, MIDI_CHANNEL); //Send Midi
  lastCCval = CCval; //Remember last value to be compared in next loop 
  sensorValue = analogRead(sensorPin);  //Read Fsr in & check state machine;  
  stateMachine();   //not sure if this slows down interupts or vice versa?
  //Seems to work ok for now, need to implement a re-zero button or something 
  
//uncomment next line for debugging
//debug();
}

int get_velocity(int initial, int final, unsigned long time_delta) {
  return map(final, Threshold, MaxPressure, 0, 127);
}
void stateMachine() {
  if (state == NOTE_OFF) {
    if (sensorValue > Threshold) {
      breath_on_time = millis();
      initial_breath_value = sensorValue;
      state = RISE_WAIT;  
    }
  } else if (state == RISE_WAIT) {
    if (sensorValue > Threshold) {
      if (millis() - breath_on_time > RISE_TIME) {
        noteSounding = 36;
        int velocity = get_velocity(initial_breath_value, sensorValue, RISE_TIME);
        MidiUSB.sendNoteOn(noteSounding, velocity, MIDI_CHANNEL);
        state = NOTE_ON;
      }
    } else {

      state = NOTE_OFF;
    }
  } else if (state == NOTE_ON) {
    if (sensorValue < Threshold) {
      MidiUSB.sendNoteOff(noteSounding, 100, MIDI_CHANNEL);  
      state = NOTE_OFF;
    } else {
      if (millis() - atSendTime > AT_INTERVAL) {
        atVal = map(sensorValue, Threshold, 1023, 0, 127);
        MidiUSB.sendAfterTouch(atVal, MIDI_CHANNEL);
        atSendTime = millis();
        }
      }
    }
  }
}
void doEncoderA() {
  PastB ? encoder0Pos++ :  encoder0Pos--;
}

void doEncoderB() {
  PastB = !PastB;
}
void debug() {
  if (encoder0Pos != lastPos) {
 Serial.print(CCval);
 Serial.print(" ]");
 Serial.println("CCval; ");
 lastPos = encoder0Pos;
  }
}
