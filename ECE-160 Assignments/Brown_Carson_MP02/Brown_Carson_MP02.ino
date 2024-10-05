/*
Carson Brown, MP-O2
--Hardware
Pins 27, 26, 25, and 33 are all the outputs to LEDs with 220ohm resistors
Pin 32 is an output to a Piezo Buzzer, which then passes through a 220 ohm resistor
Pin 35 conncects to the center terminal of a variable resistor
Pins 34 and 39 connect to pull-down push buttons and 10kohm pull-up resistors. The buttons also each have two 104 ceramic capacitors in parallel between the terminals to debounce the switches a little bit. 

This program listens for button inputs, and then plays a short series of notes or a dancing LED pattern. The sound output is prioritized when both buttons are pressed, and only one
of the outputs can play at a time. (However, a short rewrite of the code would allow both to play together.) The speed at which these modes activate is determined by the ADC on pin
35, which expects a variable resistor to be connected, however other variable voltage sensors could be used to determine the note and flash delay if connected in place of the variable
resistor.
*/
const int pinRed = 27; //LED pins
const int pinYellow = 26;
const int pinGreen = 25;
const int pinBlue = 33;

const int pinButtonOne = 39; //Button pins, connected to Positive by default, will read FALSE when active
const int pinButtonTwo = 34;

const int pinVarResistor = 35; //Input, controls delay by being converted straight to MS (delay range of 0 to 4095 milliseconds)
const int pinBuzzer = 32; //Output, Controlled only through tone() and noTone(). Probably don't use the time control on tone() to be able to control delay during a tone

const int inputs[] = {pinButtonOne, pinButtonTwo, pinVarResistor}; //Going to do this for pin config in the future. 
const int outputs[] = {pinRed, pinYellow, pinGreen, pinBlue, pinBuzzer}; //by seeing the human names next to output or input, then looping through them to set to the right mode.
const int musicNotes[8] = {262, 294, 330, 349, 392, 440, 494, 523};

bool HandleLED_Mode[2] = {false, false}; //Buttons can be pressed at any time, but if the circuit is busy, it seems like the goal is to only handle one at a time
bool HandleSound_Mode[2] = {false, false};//So for these arrays, x[0] is if the button has been pushed, and x[1] is if the mode is active. Shame to do only one at a time tho

void LED_Mode(); //Blinks Red to Yellow to Green to Blue, then blinks Blue back to Red. Doesn't halt checking for button presses, so while sound mode cannot activate while LED Mode is active, it can still be queued
void sound_Mode(); //Cycles through the array of musicNotes, then cycles through backwords. Similar to LED Mode, LED mode can be queued while sound Mode is active

//If both Sound and LED mode are pressed, Sound mode has priority and plays first, but will not interrupt LED mode to play
//Buttons must be pressed within 50ms of each other and held down for said duration for reliability.

bool timeChecker(); //Returns if the delay period has elapsed

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Couple of status messages, doesn't affect the program.

  for (int i = 0; i < 5; i++) {
    pinMode(outputs[i], OUTPUT);
  }
//Iterate through pin arrays that need configuration
  for (int i = 0; i < 3; i++) {
    pinMode(inputs[i], INPUT);
  }
//These also are my demonstration for arrays and loops, because I wrote this code off of the proj reqs and then read the instructions
//Taught me a new way to do pin config though!

  Serial.println("End of Setup.");
  timeChecker(); //Sets the initial timestamp and reads the VarResistor for the first time. May be unnecessary, but calling it here won't hurt either.
}

void loop() {

  if(!digitalRead(pinButtonOne)) { //read the button, and check the other one with 50 ms delay to give coyote time for pressing both buttons
    HandleLED_Mode[0] = true;      
    if (digitalRead(pinButtonTwo)) {
      delay(50);                   //Could probably iterate for 50 delays of 1 second and check each time, but this is reliable 'enough' for pressing both buttons
    }
    if (!digitalRead(pinButtonTwo)) { //Just a reminder that digitalRead(pinButtonTwo) for a pushed button evaluates to false
      HandleSound_Mode[0] = true;  
    }
  }

  if(!digitalRead(pinButtonTwo)) { //read the other button
    HandleSound_Mode[0] = true;
  }

  if(HandleSound_Mode[0] && !HandleLED_Mode[1]) { //If Sound mode is queued and LED mode is not currently active, run sound mode
    HandleSound_Mode[1] = true; //Happens before the LED mode to give sound mode priority, as buttons have just been checked
    if (timeChecker()) {
      sound_Mode();
    }
  }

  if(HandleLED_Mode[0] && !HandleSound_Mode[1]) { //If LED mode is queued and Sound mode is not currently active, run sound mode
    HandleLED_Mode[1] = true;
    if (timeChecker()) {
      LED_Mode();
    }
  }
}

void LED_Mode() {
  static int cycle = 0;
/* Shoulda read the instructions first, supposed to use arrays and for loops */
  switch (cycle) {
    case 0:
      digitalWrite(pinRed, HIGH); //This is the same sort of easily modifyable pattern, quick to write and simple to debug
      break;                      //Wait I totally could have just run cycle through a math equation and put it into an array. Whatever, this works.
    case 1:
      digitalWrite(pinYellow,HIGH);
      digitalWrite(pinRed, LOW);
      break;
    case 2:
      digitalWrite(pinGreen, HIGH);
      digitalWrite(pinYellow, LOW);
      break;
    case 3:
      digitalWrite(pinBlue, HIGH);
      digitalWrite(pinGreen, LOW);
      break;
    case 4: 
      digitalWrite(pinBlue, LOW);
      break;
    case 5:
      digitalWrite(pinBlue, HIGH);
      break;
    case 6:
      digitalWrite(pinGreen, HIGH);
      digitalWrite(pinBlue, LOW);
      break;
    case 7:
      digitalWrite(pinYellow, HIGH);
      digitalWrite(pinGreen, LOW);
      break;
    case 8:
      digitalWrite(pinRed, HIGH);
      digitalWrite(pinYellow, LOW);
      break;
    case 9: 
      digitalWrite(pinRed, LOW);
      break;
  }

  cycle++; //This is a use of a static variable that make me happy. Keeps everything nice and in scope,
  if(cycle > 9) { //and lets me name mangle like c++ intended to avoid variables like cycleLED and cycleSound that do the same function
    cycle = 0;
    HandleLED_Mode[0] = false;
    HandleLED_Mode[1] = false; //Ran into an array initializer list error when doing these as one statement. I'll watch a youtube video on it someday.
  }
}

void sound_Mode() {
  static int cycle = 1; //starts at 1 so that I play a note on the first time the button is pressed, because modulo overcomplicated my life
  static bool forward = true; //increase through the array on true, decrement on false
  int note = (cycle/2); //take cycle/2 (no remainder due to integers) 
  if ((cycle == 0) && !forward) {//Oddly enough, the last step in the cycle has to be the first case checked
    noTone(pinBuzzer);
    cycle = 1;
    forward = true;
    HandleSound_Mode[0] = false; //Mark that sound mode has been completed [1], and that Sound mode is not requested [0].
    HandleSound_Mode[1] = false; //Also apparently you cannot assing to an array from an initializer list... weird
    Serial.println("Final Cycle"); 

  }else if(!(cycle % 2) && forward) { //modulo 2 is a funny lil guy, making a zero (false) or a one (true) letting me know if I'm on an even or odd cycle
    noTone(pinBuzzer);
    cycle++;

  } else if (!(cycle % 2) && !forward) { //I should mention that these even gaps are to make sure that each tone is seperated by a no tone of user length.
    noTone(pinBuzzer);
    cycle--;

  } else if ((cycle % 2) && forward) { //Tone cycle and positive cycle
    tone(pinBuzzer, musicNotes[note]);
    cycle++;
    if(cycle > 15) {
      forward = !forward; //Leaves the code at cycle = 16 with direction pointed the other way
    }

  } else {
    tone(pinBuzzer, musicNotes[note]); //Tone cycle and negative
    cycle--;

  }
  Serial.println("forward");

}

bool timeChecker() { //Just like MP01, make a mark in time with millis, then wait for timestamp plus the delay time (analog read of VarResistor, 0 - 4095 ms) to be less than millis 
  static int timeStamp = millis();
  int delayMS = analogRead(pinVarResistor);
  if (millis() >= timeStamp + delayMS) {
    timeStamp = millis();
    return true; //Functions like this are what allow ESP32 to be so responsive. delay() is a trap.
  }
  return false;

}