/* Sonar_Functions.ino
  Author : Carson Brown
  This file contains the functions related to the Ultrasonic Senor and the data processing related to it.

*/

/*  sonarDistance() is the only function in the code which should use the sonarTrigPin, and probably the only function to use the sonarEchoPin.
    sonarDistance() runs the activation sequence for the Ultrasonic sensor, delaying the ESP-32 by at least 12 microseconds plus the return signal pulse time, 
    and returns the distance in centimeters from the Ultrasonic sensor to the object it reflects off of it. 

    The speed of sound is assumed to be a constant
*/
float sonarDistance(){
  digitalWrite(sonarTrigPin, LOW); //This is the pattern for activating the Ultrasonic sensor module
  delayMicroseconds(2);
  digitalWrite(sonarTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sonarTrigPin, LOW);
  
  unsigned long microsecond = pulseIn(sonarEchoPin, HIGH); //This reads the pulse time of the Ultrasonic sensor module.
  float centimetersDistance = microsecond / 29.00 / 2.0; //This converts the time in microseconds recorded into the centimeters from the object to the sensor

  // Serial.print("Current Distance : ");
  // Serial.print(centimetersDistance);
  // Serial.println(" cm");

  return centimetersDistance;
}

/* sonarDistanceFiltered() updates global variables for ease of access inside the main program, and filters the response of sonarDistance() with an IIR filter
  if values seem strange, changing or disabling the filter can help. Otherwise, hitting the reset button on the ESP-32 has also solved my issues.
  The fractions within the IIR filter should always add up to 1. */
float sonarDistanceFiltered() {
  distance = sonarDistance();  //Call sonarDistance, the only function allowed to touch the trig and echo pins

  distance = (7.0/8.0)*prevDistance + (1.0/8.0)*distance; // Because the distance is read so often, bad data can be siphoned out by only taking only 1/8 of the input each time

  prevDistance = distance; //And this sets up the filter for the next run through. This filter is also the reason why prevDistance NEEDs to be initialized with a value.

  Serial.print("Current Distance : ");
  Serial.print(distance);             //This block of code reports the distance read to the user
  Serial.println(" cm");
  
  return distance;
}