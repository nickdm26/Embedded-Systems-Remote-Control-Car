/*
 * Author Nick Muldrew
 * Reciever Program for Remote Control Car
 * Uses the nRF24L01 to recieve a data Struct sent from the Transmitter and then using that data sends a call to the various methods which control the car.
 */

// Imports Libraries which are needed
#include <SPI.h>
#include "RF24.h"

// Creates a address for the radio pipe
byte addresses[][6] = {"1Node","2Node"};

// Set up nRF24L01 radio on SPI pins 7 and 8
RF24 radio(7,8);

// Data fields which assign variable names to the output pins
int EnableA = 6;
int EnableB = 5;
int MotorTurn1 = 4;
int MotorTurn2 = 3;
int MotorForward1 = 10;
int MotorForward2 = 9;

// A Struct is created so that the data recieved is able to be used correctly
// The Struct contains 4 boolean values which represent if a button was pressed and the micros is the time since the program started.
struct dataStruct{
  unsigned long _micros;
  boolean YL;
  boolean BR;
  boolean Go;
  boolean Back;
}myData;

/*
 * Sets the pins up as Outputs. 
 * Also starting the Serial Monitor so information may be printed to the computer screen when neccessary. 
 * Starts the actuall radio up, sets the Power amplifer level, opens the pipe to read and write from. 
 * Then starts the radio listening for data.
 */
void setup() {
  pinMode(MotorTurn1, OUTPUT);
  pinMode(MotorTurn2, OUTPUT);
  pinMode(MotorForward1, OUTPUT);
  pinMode(MotorForward2, OUTPUT);
  pinMode(EnableA, OUTPUT);
  pinMode(EnableB, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Reciever");
  
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);  
  radio.openWritingPipe(addresses[0]);      // Open a writing pipe on the radio. Has a opposite address to the Transimitter radio.
  radio.openReadingPipe(1,addresses[1]);    // Open a reading pipe on the radio. Has a opposite address to the Transimitter radio.
  radio.startListening();
}

/*
 * Loop which runs forever is used to read in the data from the radio.
 * It then prints apporiate information to the Serial Mointor and passes the info to the CarControls Method.
 * Sends the information back to the Transmitter so it knows it was recieved.
 */
void loop() {  
  if( radio.available()){
    while (radio.available()) {
      radio.read( &myData, sizeof(myData) );
    }   
    radio.stopListening();
    radio.write( &myData, sizeof(myData) );
    radio.startListening();
    Serial.print(F("Sent response "));
    Serial.print(myData._micros);  
    Serial.print(F(" : "));
    Serial.print(myData.YL);
    Serial.print("  ");
    Serial.print(myData.BR);
    Serial.print("  ");
    Serial.print(myData.Go);
    Serial.print("  ");
    Serial.println(myData.Back);  
    CarControls(myData.YL, myData.BR, myData.Go, myData.Back);
  } 
}

/**
 * CarControls is called from the loop passing in the data read from the radio.
 * CarControls recieves 4 boolean values.
 * It then checks if the data is true or false and calls the correct method to deal with the movement of the car.
 * Also print to the Serial Monitor what method was called for troubleshooting.
 */
void CarControls(boolean YL, boolean BR, boolean Go, boolean Back){
    if(!YL){
    Serial.println("LEFT");
    Left();
  }else if(!BR){
    Serial.println("RIGHT");
    Right();
 }else if(YL == 0 && BR == 0){
    Serial.println("Both Middle");
    Middle();
 }else{
    Serial.println("Middle");
    Middle();
 }

  if(!Go){
    Serial.println("Go");
    Forward();
  }else if(!Back){
    Serial.println("Back");
    Reverse();
 }else if(Go == 0 && Back == 0){
    Serial.println("Both Go");
    Stop();
 }else{
    Serial.println("STOP");
    Stop();
 }
// delay(50);
}

/*
 * Right method is used to turn on the corect motor and change the polarity so that it turns right.
 * The Enable pin is turned on and the Control pins, MotorTurn1 and 2 are set.
 */
void Right(){
  digitalWrite(EnableA, HIGH);
  digitalWrite(MotorTurn1, HIGH);
  digitalWrite(MotorTurn2, LOW);
}

/*
 * Left method is used to turn on the front motor and change the polarity so that it turns left.
 * The Enable pin is turned on and the Control pins, MotorTurn1 and 2 are set.
 */
void Left(){
  digitalWrite(EnableA, HIGH);
  digitalWrite(MotorTurn1, LOW);
  digitalWrite(MotorTurn2, HIGH);
}

/*
 * Middle is used to turn off the motor so that it does not turn at all and rests in the middle.
 * This is done by setting the enable pin to low which means the motor gets no power.
 */
void Middle(){
  digitalWrite(EnableA, LOW);
}

/*
 * Forward is uesd to turn on the rear motor and use the correct polartiy to drive the car forward.
 * The Enable pin is is turned on and the Control pins, MotorForward1 and 2 are set.
 */
void Forward(){
  digitalWrite(EnableB, HIGH);
  digitalWrite(MotorForward1, LOW);
  digitalWrite(MotorForward2, HIGH);
}

/*
 * Reverse is used to turn on the rear motor and use the correct polartiy needed to drive the car backwards.
 * The Enable pin is is turned on and the Control pins, MotorForward1 and 2 are set.
 */
void Reverse(){
  digitalWrite(EnableB, HIGH);
  digitalWrite(MotorForward1, HIGH);
  digitalWrite(MotorForward2, LOW);
}

/*
 * Stop is used to turn off the rear motor so it does nothing  at all and stays it place.
 * Turns the Enable pin off so the motor get no power.
 */
void Stop(){
  digitalWrite(EnableB, LOW);
}
