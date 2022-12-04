// FastMultiClosedStepper.cpp
//
// Copyright (C) 2022 Christian Gfall

#include "FastMultiClosedStepper.h"
#include "FastClosedStepper.h"

FastMultiClosedStepper::FastMultiClosedStepper()
{
    _num_steppers = 0;
}

boolean FastMultiClosedStepper::addStepper(FastClosedStepper &stepper)
{
    if (_num_steppers >= CLOSEDSTEPPER_MAX_STEPPERS)
        return false; // No room for more

    _steppers[_num_steppers++] = &stepper;
    return true;
}

void FastMultiClosedStepper::setTarget(long target[])
{
    // First find the stepper that will take the longest time to move
    float longestTime = 0.0;

    uint8_t i;
    for (i = 0; i < _num_steppers; i++)
    {
        long thisDistance = target[i] - _steppers[i]->getCurrentPosition();
        float thisTime = abs(thisDistance) / _steppers[i]->getStepper()->getMaxSpeedInHz();
        if (thisTime > longestTime)
            longestTime = thisTime;
    }

    if (longestTime > 0.0)
    {
        // Now work out a new max speed for each stepper so they will all
        // arrived at the same time of longestTime
        for (i = 0; i < _num_steppers; i++)
        {
            long thisDistance = target[i] - _steppers[i]->getCurrentPosition();
            float thisSpeed = thisDistance / longestTime;
            _steppers[i]->setTarget(target[i]);                  // New target position (resets speed)
            _steppers[i]->getStepper()->setSpeedInHz(thisSpeed); // New speed
        }
    }
}

// Returns true if any motor is still running to the target position.
boolean FastMultiClosedStepper::run()
{
    uint8_t i;
    boolean ret = false;
    for (i = 0; i < _num_steppers; i++)
    {
        if (_steppers[i]->run())
            ret = true;
    }
    return ret;
}

// Blocks until all steppers reach their target position and are stopped
void FastMultiClosedStepper::runToTarget()
{
    while (run())
        ;
}
