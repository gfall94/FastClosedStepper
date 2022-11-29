// Bounce.ino
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2022 Christian Gfall

#include <TMCStepper.h>
#include <FastAccelStepper.h>
#include <AVRStepperPins.h>
#include <FastClosedStepper.h>

/***********STEPSTICK******************/
/* pin mapping
* mega       stepstick
* GND        GND(9)
* 5V         VCCIO(10)
* 5          STEP(2)
* 6          DIR(1)
* 7          DRV_ENN(8)         
* MISO(50)   SDO(4) 
* MOSI(51)   SDI(7)
* SCK(52)    SCK(6)
* SS(SCK(53) CSN(5)*/
#define EN_PIN           7 // Enable
#define DIR_PIN          6 // Direction
#define STEP_PIN         stepPinStepper4C // Step (turned out that only pins from OC4 are working)
#define CS_PIN           53 // Chip select
#define R_SENSE 0.075f // TMC5160 uses 0.075

TMC5160Stepper driver = TMC5160Stepper(CS_PIN, R_SENSE);

/***********ENCODER*******************/
/*  Arduino       Encoder
*   2               A
*   24               B*/
#define ENC_A     2 // Channel A
#define ENC_B     24 // Channel B


/**********Closed Loop Stepper*************/
FastClosedStepper stepper(ENC_A, ENC_B, STEP_PIN, EN_PIN, DIR_PIN);


/*GEARBOX*/
/*  1st Gear: 30
*   2nd Gear: 10
*   3rd Gear: 20
*   4th Gear: 20
*   Ratio Encoder: 1
*   Ratio Stepper: 3*/
/*Spindle*/
/*  Pitch: 25mm*/
/********DEFINITIONS*****************/
constexpr float RATIO_STEPPER = 1.0/3.0;
constexpr float RATIO_ENCODER = 1.0;
constexpr float PITCH_SPINDLE = 25.0; // pitch of spindle

constexpr uint8_t MICROSTEPPING = 8;
constexpr float STEPS_PER_ROT = 200*RATIO_STEPPER*MICROSTEPPING;    // stepper: 200 full steps per rotation * Gear Ratio
constexpr float COUNTS_PER_ROT = 4000*RATIO_ENCODER;  // encoder: 2000 counts per rotation * Gear Ratio

constexpr float STEPS_PER_MM  = STEPS_PER_ROT/PITCH_SPINDLE;
constexpr float COUNTS_PER_MM = COUNTS_PER_ROT/PITCH_SPINDLE;

// #define MAX_ACCEL 3000      // maximum acceleration in mm/s^2
#define MAX_SPEED 200       // maximum speed in mm/s
#define MAX_ACCEL 20      // maximum acceleration in mm/s^2
// #define MAX_SPEED 10       // maximum speed in mm/s
#define CURRENT 1500    // Stepper current in mA


void setup() {

  SPI.begin();
  Serial.begin(9600);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  driver.begin();             // Initiate pins and registeries
  driver.rms_current(CURRENT);    // Set stepper current to 600mA
  driver.en_pwm_mode(1);      // Enable extremely quiet stepping
  driver.pwm_autoscale(1);
  driver.microsteps(MICROSTEPPING);
    
  // Serial.println("Initialized driver");

  stepper.init();
  stepper.setMaxSpeed(MAX_SPEED*STEPS_PER_MM); // set max speed
  stepper.setAcceleration(MAX_ACCEL*STEPS_PER_MM); // set maximum acceleration
  stepper.setEncoderSPR(round(COUNTS_PER_ROT)); // set pulses per mm
  stepper.setStepperSPR(round(STEPS_PER_ROT)); // set steps per mm
  stepper.setTolerance(2);
  // stepper.getStepper()->enableOutputs();
  
    Serial.println("pos_goal, pos_encoder, pos_stepper");
}


long pos_current = 0;
long pos_goal = round(123.456*STEPS_PER_MM);  // Move xx mm
uint32_t count = 0;
uint32_t count2 = 0;

void loop() {

    
    // Serial.println(stepper.run());
    if (!stepper.run()) {
      if (count2>10000)
      {
        pos_goal = -pos_goal;
        stepper.setTarget(pos_goal);
        count2 = 0;
      }
      count2++;        
        // Serial.println(F("Changed Direction"));
    }


    if (count>500)
    {
    Serial.print(pos_goal/STEPS_PER_MM);
    Serial.print(",");
    Serial.print((stepper.getEncoder()->read())/COUNTS_PER_MM);
    Serial.print(",");
    Serial.println(stepper.getStepper()->getCurrentPosition()/STEPS_PER_MM);
    count=0;
    }
    count++;
}