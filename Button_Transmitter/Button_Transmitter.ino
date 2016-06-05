/*
 * Author Nick Muldrew
 * Transmitter Program for Remote Control Car
 * Uses the nRF24L01 to transmit a data Struct To the Reciever which controls a Car.
 * The Struct which is sent contains boolean values representing if a button is pressed.
 */

// Imports Libraries which are needed
#include <SPI.h>
#include "RF24.h"

// Creates a address for the radio pipe
byte addresses[][6] = {"1Node","2Node"};

// Set up nRF24L01 radio on SPI pins 7 and 8
RF24 radio(7,8);

// A Struct is created so that the data recieved is able to be used correctly
// The Struct contains 4 boolean values which represent if a button was pressed and the micros is the time since the program started.
struct dataStruct{
  unsigned long _micros;
  boolean YL;
  boolean BR;
  boolean Go;
  boolean Back;
}myData;

// Data fields which assign variable names to the Input pins connected to buttons
int yellow_left = 6;
int blue_right = 5;
int green = 4;
int red = 3;

/*
 * Sets the pins up as Inputs using the Unos internal resistors to read what state a button is in.
 * Also starting the Serial Monitor so information may be printed to the computer screen when neccessary. 
 * Starts the actuall radio up, sets the Power amplifer level, opens the pipe to read and write from. 
 * Then starts the radio listening for data.
 */
void setup() {
  pinMode(yellow_left, INPUT_PULLUP);
  pinMode(blue_right, INPUT_PULLUP);
  pinMode(green, INPUT_PULLUP);
  pinMode(red, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Transmitter");
  
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);  
  radio.openWritingPipe(addresses[1]);   // Open a writing pipe on the radio. Has a opposite address to the Transimitter radio.
  radio.openReadingPipe(1,addresses[0]); // Open a reading pipe on the radio. Has a opposite address to the Transimitter radio.
  radio.startListening();
}

/*
 * Loop which runs forever is used to write data to the radio.
 * Reads in the digital input from the buttons connected to the Uno and puts the value in the myData Struct.
 * Sends the Struct with data in to the Reciever radio.
 * Waits for a response to the data sent, if nothing is recieved in 200ms the Failed timed out is printed to the Serial Monitor.
 * OtherWise Other useful troubleshooting information is printed to the Serial Monitor if the data is successfully sent.
 */
void loop() {
  myData.YL = digitalRead(yellow_left);
  myData.BR = digitalRead(blue_right);
  myData.Go = digitalRead(green);
  myData.Back = digitalRead(red);
   
  radio.stopListening();     
  Serial.println(F("Now sending"));
  myData._micros = micros();
   if (!radio.write( &myData, sizeof(myData) )){   // If the data is not sent, print to the Serial Monitor failed.
     Serial.println(F("failed"));
   }
       
  radio.startListening();
  unsigned long started_waiting_at = micros();               // Set up a timeout and get the current time
  boolean timeout = false;                                   // Set up a variable to indicate if a response was received
    
  while ( ! radio.available() ){                             // While nothing is received
    if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
        timeout = true;
        break;
    }      
  }
  
// Print Various information to The Serial Monitor for TroubleShooting Purposes
  if ( timeout ){                                            
      Serial.println(F("Failed, response timed out."));
  }else{            
      radio.read( &myData, sizeof(myData) );
      unsigned long time = micros();
      Serial.print(F("Sent "));
      Serial.print(time);
      Serial.print(F(", Got response "));
      Serial.print(myData._micros);
      Serial.print(F(", Round-trip delay "));
      Serial.print(time-myData._micros);
      Serial.print(F(" microseconds Value "));
      Serial.print(myData.YL);
      Serial.print("  ");
      Serial.print(myData.BR);
      Serial.print("  ");
      Serial.print(myData.Go);
      Serial.print("  ");
      Serial.println(myData.Back);
  }
  delay(50);  
}
