#ifndef ___FastClosedStepper__H___
#define ___FastClosedStepper__H___

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#endif  // #ifndef ___FastClosedStepper__H___

#include <Encoder.h>
#define ENCODER_OPTIMIZE_INTERRUPTS

#include <FastAccelStepper.h>

class Encoder;

class FastClosedStepper {
  public:
    FastClosedStepper::FastClosedStepper(uint8_t enc_pin1, uint8_t enc_pin2, uint8_t step_pin, uint8_t en_pin, uint8_t dir_pin);
    void init();
    void setTarget(long target);
    long getTarget();

    long distanceToGo();

    long getCurrentPosition();
    void setCurrentPosition(long position);

    float getMaxSpeed();
    void setMaxSpeed(float speed);

    void setAcceleration(float accel);

    long getTolerance();
    void setTolerance(long tolerance);

    long getEncoderSPR();
    void setEncoderSPR(long spr);

    long getStepperSPR();
    void setStepperSPR(long spr);

    FastAccelStepper* getStepper();
    Encoder* getEncoder();

    bool run();
    bool success();
    void runToTarget();
    void stop();
    void closeLoop();
    
  private:
    long _currentTarget = 0;
    long _maxError = 2;
    long _updateCount = 0;
    long _testInterval = 100;
    long _encoderSpr = 400;
    long _stepperSpr = 200;
    bool _failed = false;
    long _nmissed = 0;
    long _maxMissed = 100000;
    
    uint8_t _en_pin;
    uint8_t _step_pin;
    uint8_t _dir_pin;
    FastAccelStepperEngine _engine;
    FastAccelStepper* _stepper = NULL;
    Encoder* _encoder;
    
};