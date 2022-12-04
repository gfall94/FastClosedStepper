#include "FastClosedStepper.h"

#define MAX_RETRIES 5
#define MAX_ACCELERATION 10000



FastClosedStepper::FastClosedStepper(uint8_t enc_pin1, uint8_t enc_pin2, uint8_t step_pin, uint8_t en_pin, uint8_t dir_pin) {
  _en_pin = en_pin;
  _step_pin = step_pin;
  _dir_pin = dir_pin;
  _encoder = new Encoder(enc_pin1, enc_pin2);
  _engine = FastAccelStepperEngine();
}


void FastClosedStepper::init() {
  _currentTarget = _encoder->read();
  _engine.init();
  _stepper = _engine.stepperConnectToPin(_step_pin);
  _stepper->setDirectionPin(_dir_pin);
  _stepper->setEnablePin(_en_pin);
  _stepper->setAutoEnable(true);
  _stepper->setDelayToEnable(50);
  _stepper->setDelayToDisable(1000);
  _stepper->setAcceleration(MAX_ACCELERATION);
  _stepper->setCurrentPosition(_currentTarget);
  _stepper->setSpeedInHz(60);
}

void FastClosedStepper::setTarget(long target) {
  if (target != _currentTarget) {
    _failed = false;
    _nmissed = 0;
    _currentTarget = target;
    _stepper->moveTo(_currentTarget);
  }
  
}

long FastClosedStepper::getTarget() {
    return _currentTarget;
}


float FastClosedStepper::getMaxSpeed() {
  return _stepper->getMaxSpeedInHz();
}

void FastClosedStepper::setMaxSpeed(float speed) {
  _stepper->setSpeedInHz(speed);
}


void FastClosedStepper::setAcceleration(float accel) {
  _stepper->setAcceleration(accel);
}

long FastClosedStepper::getTolerance(){
  return _maxError;
}

void FastClosedStepper::setTolerance(long tolerance) {
  _maxError = tolerance;
}

long FastClosedStepper::getEncoderSPR() {
  return _encoderSpr;
}

void FastClosedStepper::setEncoderSPR(long spr) {
  _encoderSpr = spr;
}

long FastClosedStepper::getStepperSPR() {
  return _stepperSpr;
}

void FastClosedStepper::setStepperSPR(long spr) {
  _stepperSpr = spr;
}

FastAccelStepper* FastClosedStepper::getStepper() {
  return _stepper;
}

Encoder* FastClosedStepper::getEncoder() {
  return _encoder;
}

long FastClosedStepper::getCurrentPosition() {
  return _encoder->read()*_stepperSpr/_encoderSpr;
}

void FastClosedStepper::setCurrentPosition(long position) {
  long enc_pos = position*_encoderSpr/_stepperSpr;
  _encoder->write(enc_pos);
}

long FastClosedStepper::distanceToGo() {
  return _currentTarget - getCurrentPosition();
}
bool FastClosedStepper::success() {
  return !_failed;
  }

void FastClosedStepper::closeLoop(){
  _updateCount = 0;
  if ((abs(distanceToGo()-(_currentTarget-_stepper->getCurrentPosition()))>_maxError)){
    _nmissed += abs(distanceToGo() - (_currentTarget-_stepper->getCurrentPosition()));
    if (_nmissed<_maxMissed){
      _stepper->setCurrentPosition(getCurrentPosition());
      _stepper->moveTo(_currentTarget);
    } else {
      _failed = true;
    }
        }
}

bool FastClosedStepper::run(){
  if (_updateCount > _testInterval) {
    closeLoop();
    if (_failed) return false;
  }
    // Serial.println(abs(distanceToGo()));
  // if (!_stepper->moveTo(_currentTarget)) {
  if (_maxError<abs(distanceToGo())) {
    _updateCount++;
    return true; 
  }
  return false;
}


void FastClosedStepper::runToTarget() {
  _nmissed = 0;
  while (run());
}

void FastClosedStepper::stop(){
    _stepper->stopMove();
    while (_stepper->move(_currentTarget));
    setCurrentPosition(_stepper->getCurrentPosition());

}
