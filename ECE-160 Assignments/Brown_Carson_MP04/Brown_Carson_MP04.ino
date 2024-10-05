/* Brown_Carson_MP04.ino
  This program implements a state machine which controls a motor by comparing the values read by an ultrasonic sensor to some constants, which creates a mock up of 
  the autonomous controls that the final project will have later this quarter.

  Author : Carson Brown
  Last Revised : 10/3/2024

  The filter is almost identical to the IIR filter sent to the class emails, and works pretty darn well. Otherwise, portions of this code also come from previous mini projects,
  as many of the tasks we were asked to do in this project had been completed in other projects

*/

#include <Adafruit_NeoPixel.h> //LED Library
//States
#define STOP 1      // Motor is shut off, Center LEDS are yellow, all others off
#define GO 2        // Motor is Spinning Forwards, First two LEDS are Green, all others off
#define REVERSE 3   // Motor is Spinning Backwards, Last two LEDS are RED, all others off

//pins
const int motorForwardPin = 26;   // This is connected to pin 2 of the L293D
const int motorBackwardPin = 27;  // This is connected to pin 7 of the L293D

const int sonarTrigPin = 14;      // Connects to the TRIG pin on the Ultrasonic Sensor, this is the OUTPUT from the ESP-32
const int sonarEchoPin = 12;      // Connects to the ECHO pin on the Ultrasonic Sensor, this is the INPUT to the ESP-32

const int ledStripPin = 25;       // Connects to the yellow input wire on the LED strip

const int inputs[] = {sonarEchoPin};  // I suspect this code may get re-used in the future, and so this sets up a bit of future proofing when I have more inputs
const int outputs[] = {motorForwardPin, motorBackwardPin, sonarTrigPin, ledStripPin}; // This will be iterated over to set up the pin configurations

//LED strip setup
#define NUM_LEDS 8 // Standard practice for Adafruit examples is using the define keyword, so I will follow. 
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(NUM_LEDS, ledStripPin, NEO_GRB + NEO_KHZ800);  // Creates the ledStrip object so that I can flash lights. I'd like to create a motor object for the final project for ease of use

//Global Variables
int state = STOP; // state controls the entire program instruction, and should be handled carefully. Technically it is just 1, 2, or 3.
float distance;   // distance in centimeters from the Ultrasonic Sensor to whatever it can detect. Gets filtered, so is not the true reading in order to avoid bad reads
float prevDistance = 9.0; //Arbitrary to my knowledge, but may be worthwhile to play around with. Currently set to 9 centimeters to initialize in the STOP state as per instructions

float goDistance = 12.0; // Beyond this distance in centimeters from the Ultrasonic Sensor, the ESP-32 will transition into the GO state
float reverseDistance = 10.0; // Anything less than this distance in centimeters from the Ultrasonic Sensor, the ESP-32 will transition into the STOP state

const int freq = 500; //Frequency of Hertz to use for the DC motor
const int resolution = 8; // the number of bits for the motor PWM counter, which gives 256 steps from 0 -> 255
int dutyCycle = 150; // This is the bit on which the motor PWM counter transitions from HIGH to LOW. A higher number means the motor is more likely to burn components. This currently gives at least 10 seconds of safe spinning
                     // dutyCycle can be changed into the duty cycle of the PWM in percent by dividing dutyCycle by 255

//Funcion Declarations, More expansive comments exist on their seperate function files
void motorForwards();   // Defined in Motor_Functions.ino, sets the motor to go forwards until otherwise instructed
void motorBackwards();  // Defined in Motor_Functions.ino, sets the motor to go backwards until otherwise instructed
void motorStop();       // Defined in Motor_Functions.ino, sets the motor to stop until otherwise instructed

void ledPatternForwards();  // Defined in LED_Functions.ino, clears all LED colors and sets the first two to green.
void ledPatternBackwards(); // Defined in LED_Functions.ino, clears all LED colors and sets the middle two to yellow.
void ledPatternStop();      // Defined in LED_Functions.ino, clears all LED colors and sets the last two to red.

float sonarDistance();          // Defined in Sonar_Functions.ino, this returns the distance in centimeters from the Ultrasonic Sensor to whatever it hit
float sonarDistanceFiltered();  // Defined in Sonar_Functions.ino, this calls sonarDistance() and filters the input with an infinite impulse response filter.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 

  for(int i = 0; i < 4; i++) { //This loops through the output pin array and configures the pin numbers inside as outputs. The middle expression should have i as less than the number of pins to be outputs
    Serial.print(outputs[i]);
    Serial.println(" configured as output."); //Best seen through hitting the reset button on the ESP-32
    pinMode(outputs[i], OUTPUT);
  }

  Serial.print(inputs[0]);                  //This chunk of code is together so that it can be encapsulated by a for loop if more inputs are attatched to the circuit.
  Serial.println(" configured as input.");  //Same logic inside the for loop above, but only one input exists
  pinMode(inputs[0], INPUT);

  ledcAttach(motorForwardPin, freq, resolution);  // Never connect the motor pins directly to the motor from the ESP-32. Always use the L239D
  ledcAttach(motorBackwardPin, freq, resolution); // The motor is connected on both sides to the L239D. This means setting both pins to the same state will turn off the motor, as there will be no potential between the poles of the motor

  ledStrip.begin(); //Start up the NeoPixel Strip
  ledStrip.show();  //Make sure NeoPixel Strip is empty if starting from button push reset, as the LEDs have there own memory

  Serial.println("Setup phase complete.");
}

void loop() {
  /*
  This switch statement is the state machine that controlls the entire system. Each state is designed to call functions of the same state name, one function per thing done,
  so that updating states for future projects is fairly easy to do, and to improve readability.

  the state variable the this switch statement reads from is a simple integer behind the scenes, with each state being a define macro.
  STOP = 1
  GO = 2
  REVERSE = 3
  state initializes to STOP

  If everything works, the default case will never run.
  */
  switch (state) {
    case STOP:
      motorStop();
      ledPatternStop();
      Serial.println("State = STOP");
      break;
    
    case GO:
      motorForwards();
      ledPatternForwards();
      Serial.println("State = GO");
      break;
    
    case REVERSE:
      motorBackwards();
      ledPatternBackwards();
      Serial.println("State = REVERSE");
      break;
    
    default :
      Serial.println("You should never see this, Error : DEFAULT");
      break;
  }

  delay(10);                //Like any good project, this delay increases the code execution speed. Just try removing it. The Ultrasonic Sensor will yell at you. (it just will give big pauses every other call)
  sonarDistanceFiltered();  //Because sonarDistanceFiltered updates global variables, it doesn't need to do anything but be called before the state decider.

  /*
  Below is the state decider. If it was any more complicated than two comparisons, I'd move it into its own function, but I don't think this state decider will get reused.
  If statement means ONLY one block will execute, regardless of programmer error.

  If the distance updated by sonarDistanceFiltered() is greater than 12.0 cm, then the state will change to GO.
  If the distance is less than 10.0 cm, then the state will change to REVERSE.
  If the distance is between those two values, or somehow null or otherwise NaN, the state will change to STOP.
  */
  if (goDistance < distance) {
    state = GO;

  } else if (reverseDistance > distance) {
    state = REVERSE;

  } else {
    state = STOP;

  }
}
