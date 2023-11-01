#include <Servo.h>
#include "image.h"

// Servo
Servo pen;
#define SERVO 11


// Stepper Motor Pins
#define XSTEP 2
#define XDIR 5

#define YSTEP 3 
#define YDIR 6

// Dimensions
#define XLENGTH 16200
#define YLENGTH 21000
#define PEN_HEIGHT 90


void setup() {
  pinMode(XSTEP, OUTPUT);
  pinMode(XDIR, OUTPUT);
  pinMode(YSTEP, OUTPUT);
  pinMode(XSTEP, OUTPUT);

  digitalWrite(XSTEP, LOW);
  digitalWrite(XDIR, LOW);
  digitalWrite(YSTEP, LOW);
  digitalWrite(YDIR, LOW);

  pen.attach(SERVO);
  pen.write(PEN_HEIGHT);

  Serial.begin(9600);
  Serial.println();
}

void loop() {
  Serial.println("-----------------------------------------");
  Serial.println("What would you like to do?");
  Serial.println("1.\tCalibrate X boundaries");
  Serial.println("2.\tCalibrate Y boundaries");
  Serial.println("3.\tCalibrate pen height");
  Serial.println("4.\tMove either axis");
  Serial.println("5.\tSandbox");
  int menu = inputInt(false);

  // Calibrate X boundaries
  if (menu == 1) {
    Serial.print("\nAt initial boundary (Y/N)? ");
    bool boundary = (inputStr(true) == "Y");

    while (!boundary) {
      moveMotor("X");

      Serial.print("\nAt initial boundary (Y/N)? ");
      boundary = (inputStr(true) == "Y");
    }
    
    bool done = false;
    int dx = 0;
    while (!done) {
      int steps = moveMotor("X");

      Serial.print("\nReached the other end (Y/N)? ");
      done = (inputStr(true) == "Y");

      dx += abs(steps);
    }

    String calibrationResult = "\nTotal X width is " + String(dx) + " steps";
    Serial.println(calibrationResult);
  }
  
  // Calibrate Y boundaries
  else if (menu == 2) {
    Serial.print("\nAt initial boundary (Y/N)? ");
    bool boundary = (inputStr(true) == "Y");

    while (!boundary) {
      moveMotor("Y");

      Serial.print("\nAt initial boundary (Y/N)? ");
      boundary = (inputStr(true) == "Y");
    }
    
    bool done = false;
    int dy = 0;
    while (!done) {
      int steps = moveMotor("Y");

      Serial.print("\nReached the other end (Y/N)? ");
      done = (inputStr(true) == "Y");

      dy += abs(steps);
    }

    String calibrationResult = "\nTotal Y width is " + String(dy) + " steps";
    Serial.println(calibrationResult);
  }

  // Calibrate pen height
  else if (menu == 3) {
    pen.write(90);
    Serial.print("\nCalibration complete (Y/N)? ");
    bool done = (inputStr(true) == "Y");

    int penHeight = 90;
    while (!done) {
      Serial.print("Enter a value between 50 and -50: ");
      int change = inputInt(true);

      penHeight += change;
      pen.write(penHeight);
      
      Serial.print("\nCalibration complete (Y/N)? ");
      done = (inputStr(true) == "Y");
    }

    String calibrationResult = "\nThe final pen height is " + String(penHeight);
    Serial.println(calibrationResult);

  }

  // Move X or Y axis
  else if (menu == 4) {
    Serial.print("\nWhich axis do you want to move? ");
    String axis = inputStr(true);

    moveMotor(axis);
  }

  // Sandbox
  else if (menu == 5) {
    checkPixel(0);
  }

  Serial.println();
}

void stepMotor(String axis, int steps) {
  int dir = steps > 0 ? 1 : 0;
  steps = abs(steps);

  for (int i = 0; i < steps; i++) {
    if (axis.equals("X")) {
      digitalWrite(XDIR, dir);
      digitalWrite(XSTEP, LOW);
      digitalWrite(XSTEP, HIGH);
      delayMicroseconds(500);
      digitalWrite(XSTEP, LOW);
    }
    else if (axis.equals("Y")) {
      digitalWrite(YDIR, dir);
      digitalWrite(YSTEP, LOW);
      digitalWrite(YSTEP, HIGH);
      delayMicroseconds(700);
      digitalWrite(YSTEP, LOW);
    }
    else {
      Serial.println("Misunderstood input");
      break;
    }
  }
}

int moveMotor(String axis) {
  Serial.print("Enter a number of steps between -1000 and 1000: ");
  int steps = inputInt(true);
  stepMotor(axis, steps);

  return steps;
}

int inputInt(bool printOutput) {
  while (Serial.available() == 0) {}
  int output = Serial.parseInt();
  Serial.read();

  if (printOutput) {
    Serial.print(output);
  }

  return output;
}

String inputStr(bool printOutput) {
  while (Serial.available() == 0) {}
  String output = Serial.readString();
  output.trim();
  Serial.read();

  if (printOutput) {
    Serial.println(output);
  }

  return output;
}

int checkPixel (long pixel) {
  Serial.print("Checking Pixel #");
  Serial.print(" ");
  unsigned long byteNum = pixel / 8;  //Take the actual pixel number and divide it by 8 to get the Byte location in the Char array
  int remainder = pixel % 8;          //Take the modulus of the pixel number by 8 to get the bit inside of the byte location
  Serial.print(byteNum);
  Serial.print(" -- ");
  unsigned char bufferByte = pgm_read_byte(&bmp[byteNum]);    //Use the byte location to save the byte into a Buffer to read the bits
  Serial.println(bufferByte,HEX);
  Serial.print(" -- Remainder: ");
  Serial.print(remainder);
  Serial.print(" -- Byte: ");
  Serial.print(byteNum);
  int pixelBit = pixel - byteNum;                             
  Serial.print(" -- Bit in Byte:");
  Serial.print(7-remainder);
  int activePixel = bitRead(bufferByte, 7 - remainder);   //We need the bits from left to right so the remained is subtracted by 7 to get the proper bit
  Serial.print(" -- Active Pixel:");
  Serial.println(!activePixel);
  return !activePixel;                                   //Return the 1 or 0 in the bit location
}
