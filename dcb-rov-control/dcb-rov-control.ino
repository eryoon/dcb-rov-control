#include <AFMotor.h>
#include <Wire.h>
#include "MS5837.h"

MS5837 bar;

#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define rx 2                                          //define what pin rx is going to be
#define tx 3                                          //define what pin tx is going to be


SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float DO;                                             //used to hold a floating point number that is the DO


//CONSTTANTS
int JOY_MIN = 0;
int JOY_MAX = 1023;
int MOTOR_MIN = 0;
int MOTOR_MAX = 255;

// GLOBAL VARIABLES
int x; // X Axis reading
int y; // Y Axis reading
int z; // Z Axis reading
int rate1; // motor 1 speed
int rate2; // motor 2 speed
int rateMod1; // motor 1 speed modifier
int rateMod2; // motor 2 speed modifier
int nullX; // null reading for x axis
int nullY; // null reading for y axis
int nullZ; // null reading for z axis

// Setup Motors
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);        
  Wire.begin();
  // Calibrate Joystick
  nullX = analogRead(A0);
  nullY = analogRead(A1);
  nullZ = analogRead(A3);
  bar.init();
  bar.setFluidDensity(122); // kg/m^3 (997 freshwater, 1029 for seawater)
  Serial.println("pressure,temperature,depth,altitude,light,dissolvedoxygen");
}void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC 
}

void loop() {

  // Get current joystick values
  x = analogRead(A0);
  y = analogRead(A1);
  z = analogRead(A3);

  // Conditional to set motor speeds based on joystick values
  if(x < nullX || y < nullY || y > nullY){
    rate1 = map(y, nullY, JOY_MAX, MOTOR_MIN, MOTOR_MAX);
    rateMod1 = map(x, nullX, JOY_MAX, MOTOR_MIN, MOTOR_MAX);
    rateMod2 = map(x, 0, nullX, MOTOR_MAX, MOTOR_MIN);
    motor1.run(FORWARD);
    motor2.run(FORWARD);

    int rspeed1 = constrain((rate1-rateMod1), MOTOR_MIN, MOTOR_MAX);
    int lspeed1 = constrain((rate1+rateMod2), MOTOR_MIN, MOTOR_MAX);

    motor1.setSpeed(rspeed1);
    motor2.setSpeed(lspeed1);

  } else if (x > nullX){
    rate2 = map(y, nullY, JOY_MIN, MOTOR_MIN, MOTOR_MAX);
    rateMod1 = map(x, nullX, JOY_MAX, MOTOR_MIN, MOTOR_MAX);
    rateMod2 = map(x, 0, nullX, MOTOR_MAX, MOTOR_MIN);
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    int rspeed2 = constrain((rate2-rateMod1), MOTOR_MIN, MOTOR_MAX);
    int lspeed2 = constrain((rate2+rateMod2), MOTOR_MIN, MOTOR_MAX);
    motor1.setSpeed(rspeed2);
    motor2.setSpeed(lspeed2);
  } else { 
    motor1.run(RELEASE);
    motor2.run(RELEASE);
  }

  // Z-axis control
  if(z > nullZ) {
    motor3.run(FORWARD);
  } else if (z < nullZ) {
    motor3.run(BACKWARD);
  } else {
    motor3.run(RELEASE);
  }


  if (input_string_complete && millis() % 1000 < 100){                         //if a string from the PC has been received in its entirety
                   //reset the flag used to tell if we have received a completed string from the PC

    bar.read();
    Serial.print(bar.pressure()); 
    Serial.print(",");
    Serial.print(bar.temperature()); 
    Serial.print(",");
    Serial.print(bar.depth()); 
    Serial.print(",");
    Serial.print(bar.altitude()); 
    Serial.print(",");
    Serial.println(analogRead(A1));
    Serial.print(",");
    myserial.print(inputstring);                      //send that string to the Atlas Scientific product
    myserial.print('\r');                             //add a <CR> to the end of the string 
    inputstring = "";                                 //clear the string
    input_string_complete = false;     
  }

  if (myserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }

  if (sensor_string_complete) {                       //if a string from the Atlas Scientific product has been received in its entirety                 //send that string to the PC's serial monitor
    /*                                                //uncomment this section to see how to convert the DO reading from a string to a float 
    if (isdigit(sensorstring[0])) {                   //if the first character in the string is a digit
      DO = sensorstring.toFloat();                    //convert the string to a floating point number so it can be evaluated by the Arduino
      if (DO >= 6.0) {                                //if the DO is greater than or equal to 6.0
        Serial.println("high");                       //print "high" this is demonstrating that the Arduino is evaluating the DO as a number and not as a string
      }
      if (DO <= 5.99) {                               //if the DO is less than or equal to 5.99
        Serial.println("low");                        //print "low" this is demonstrating that the Arduino is evaluating the DO as a number and not as a string
      }
    }
    */
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }

}
