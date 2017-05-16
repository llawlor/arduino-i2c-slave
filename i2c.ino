#include <Wire.h>
#include <Servo.h>

// byte address for the i2c communication
#define SLAVE_ADDRESS 0x04

// define a servo
Servo my_servo;

// initialize
void setup() {
  // set onboard LED light (pin 13) to output mode
  pinMode(13, OUTPUT);
  // set the baud rate and start the serial monitor
  //Serial.begin(9600);
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);
  
  // define callbacks for i2c communication
  Wire.onReceive(receiveCallback);
  Wire.onRequest(requestCallback);
}

// main loop of the program
void loop() {
  delay(10);
}

// callback for receiving data
void receiveCallback() {

  // message string
  // message format will appear like: pin:5,servo:60
  String message = "";

  // while there is another character to read
  while(Wire.available()) {
    // read the character
    char c = Wire.read();
    // add the character to the message
    message += c;
  }

  // print the data to the serial line
  //Serial.print("data received: ");
  //Serial.println(message);

  // get the pin number
  int pin_number = message.substring(message.indexOf("pin:") + 4, message.indexOf(",")).toInt();
  // get the index of the servo command
  int servo_message_position = message.indexOf("servo:");

  // if we received the "on" command
  if (message == "on") {
    // turn the onboard light on
    digitalWrite(13, HIGH);
  // else if we received the "off" command
  } else if (message == "off") {
    // turn the onboard light off
    digitalWrite(13, LOW);
  // if we received a servo command
  } else if (servo_message_position >= 0) {
    // get the servo position
    int position = message.substring(servo_message_position + 6).toInt();
    // set the servo to the correct pin
    my_servo.attach(pin_number);
    // send the command to the servo
    my_servo.write(position);
   }
}

// callback for requests
void requestCallback() {
  Wire.write("sending no data back. . .");
}
