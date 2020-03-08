#include <MIDIUSB2.h>

//PIN's definition
#define encoder0PinA  3
#define encoder0PinB  2
//20450 max 

volatile int encoder0Pos = 0;
volatile long lastPos = 0;
volatile int Value = 0;
volatile long lastValue = 0;

volatile boolean PastA = 0;
volatile boolean PastB = 0;

int MidiChannel = 2;
void setup()
{
//  Serial.begin(115200);
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  PastA = (boolean)digitalRead(encoder0PinA); //initial value of channel A;
  PastB = (boolean)digitalRead(encoder0PinB); //and channel B

  // encoder A channel on interrupt 0 (Arduino's pin 2)
  attachInterrupt(0, doEncoderA, FALLING);
  // encoder B channel pin on interrupt 1 (Arduino's pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);
}
void debug() {
Serial.print(Value);
Serial.print(";t");
Serial.println("p");
}

void loop(){
  if (encoder0Pos >= 20450) {
    encoder0Pos = 20450;
  }
  if (encoder0Pos <= 0) {
    encoder0Pos = 0;
  } else {
    Value = encoder0Pos/20;
    Value = Value/8;
    if (Value != lastValue) {
    MidiUSB.sendControlChange(1, Value, MidiChannel);
    lastValue = Value;
    }
  }
}

void doEncoderA()
{
  PastB ? encoder0Pos++ :  encoder0Pos--;

}

void doEncoderB()
{
  PastB = !PastB;
}
