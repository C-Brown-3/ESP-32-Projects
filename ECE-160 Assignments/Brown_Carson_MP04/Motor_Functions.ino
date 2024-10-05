/* Motor_Functions.ino
  Author : Carson Brown
  This file contains the functions related to the Motor and the PWM controller.

  The current duty cycle is around 50-60%, to avoid burning up the L239D chip if the motor is left running for too long
  Additionally, the preference for turning the motor off is to set both timers to a duty cycle of 0% by setting the go low bit to 0.

  Remember that the motor is connected between the forwards and backwards output of the L239D, and runs based off of the potential between them.
*/

//motorForwards drives the motor forwards after shutting off power to the backwards controller
void motorForwards() { 
  ledcWrite(motorBackwardPin, 0);
  ledcWrite(motorForwardPin, dutyCycle);
}

//motorBackwards drives the motor Backwards after shutting off power to the Forwards controller
void motorBackwards() {
  ledcWrite(motorForwardPin, 0);
  ledcWrite(motorBackwardPin, dutyCycle);
}

//motorStop sets both controllers to have a go low bit of 0, meaning no power ever flows to the motor
void motorStop() {
  ledcWrite(motorForwardPin, 0);
  ledcWrite(motorBackwardPin, 0);
}