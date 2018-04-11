#include <Wire.h>
#include <Servo.h>

// byte address for the i2c communication
#define SLAVE_ADDRESS 0x04

// initialize a maximum of 70 input pins
// when referencing pins by pin number, use [pin_number - 1] since array index starts at 0 and pin numbering starts at 1
byte input_pins[70];
//int input_values[70];

// define an array of 48 servos
// when referencing servos by pin number, use [pin_number - 1] since array index starts at 0 and pin numbering starts at 1
Servo my_servos[48];

void setup() 
{
  // set each pin to 0 (not an active input pin)
  for (int current_pin = 1; current_pin <= 70; current_pin++) {
    input_pins[current_pin - 1] = 0;
  }

  // set the input pin
  // Pin connected to voltage divider output; pin #14
  input_pins[13] = 1;
  pinMode(14, INPUT);
  // add another reading to test out the library
  input_pins[14] = 1;
  pinMode(15, INPUT);
  
  // set the baud rate and start the serial monitor
  //Serial.begin(9600);
  // initialize i2c as slave
  Wire.begin(SLAVE_ADDRESS);

  // define callbacks for i2c communication
  Wire.onReceive(receiveCallback);
  Wire.onRequest(requestCallback);
}

void loop() { 
}

// callback for requests
void requestCallback() {
  
  //Serial.println("request");
  
  // initialize a 32 character string
  String stringOutput = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  // create the message one string at a time to avoid problems with the String class and garbage values
  stringOutput = "";
  // for each pin
  for (int current_pin = 1; current_pin <= 70; current_pin++) {
    // if the pin is active
    if (input_pins[current_pin - 1] == 1) {
      // add the pin number
      stringOutput += String(current_pin);
      // add the separator
      stringOutput += ":";
      // add the pin value
      stringOutput += String(analogRead(current_pin));
      stringOutput += ",";
    }
  }
  // add the end of the string
  stringOutput += "end";

  // write the output after converting the String to a character array
  Wire.write(stringOutput.c_str());
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
  // get the index of the digital command
  int digital_message_position = message.indexOf("digital:");
  // get the index of the servo command
  int servo_message_position = message.indexOf("servo:");

  // if we received a digital command
  if (digital_message_position >= 0) {
    // get the digital command
    String command = message.substring(digital_message_position + 8);
    // set pin to output mode
    pinMode(pin_number, OUTPUT);

    // if the message is "on", write "HIGH"
    if (command == "on") {
      digitalWrite(pin_number, HIGH);
    // if the message is "off", write "LOW"
    } else if (command == "off") {
      digitalWrite(pin_number, LOW);
    }

  // if we received a servo command
  } else if (servo_message_position >= 0) {
    // get the servo position
    int position = message.substring(servo_message_position + 6).toInt();

    // set the servo to the correct pin
    my_servos[pin_number - 1].attach(pin_number);
    // send the command to the servo
    my_servos[pin_number - 1].write(position);

  }
}
