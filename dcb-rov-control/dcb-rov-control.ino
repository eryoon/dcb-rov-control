#include <AFMotor.h>

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
  Serial.begin(9600);                           //set baud rate for the software serial port to 9600
  // Calibrate Joystick
  nullX = analogRead(A0);
  nullY = analogRead(A1);
  nullZ = analogRead(A3);
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



}
