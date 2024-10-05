/*
This amazing project was made by Carson Brown
brownc3@rose-hulman.edu

This program blinks LEDs in the pattern "Green Blue Green Yellow Green Red" at a user indicated speed.
Because there is no initial input when powering up, the LEDs do not blink until an initial input, except for some deployment code initializing on the power up following programming.
After recieving a number in ASCII encoded format (no decimals, no non-numeric characters), the string is converted to an integer, which is treated as a value of milliseconds.
This 'delay value' is represented by delayMS, which represents the time for each state in the LED pattern.

Additionally, this program handles user input immediately upon recieving a new delay value, rather than waiting for a change in the pattern to accept new input.
This is due to the timeVar variable, which is set to the current runtime of the program in milliseconds each time the LED pattern changes, and is used with the
delay value in order to avoid the delay() function.

Input rejection is handled before delayMS is ever updated to avoid the situation where an accidental letter turns the delayMS to 0.
*/

//pinColor is the pin number of the Color LED
const int pinRed = 27;
const int pinYellow = 26;
const int pinGreen = 25;
const int pinBlue = 33;

//the leds integer array is used to avoid writting pinRed, pinYellow... when changing large amounts of pins at once.
int leds[4] = {pinRed, pinYellow, pinGreen, pinBlue};

/*
delayMS and timeVar work together to allow user input to be handled without waiting till the end of the delay() function.
At the start of each step in the LED cycle, timeVar is set to the current program runtime in milliseconds. 
Once the current program time is greater than or equal to delayMS + timeVar, the next LED pattern is shown and timeVar is set to the current program runtime. 
*/
int delayMS = 1000; //In milliseconds
int timeVar = 0;

//initialBlink works as a function to demonstrate the delay time to the end user, by blinking all LEDs on and off once, 
//before transitioning into blinkPattern, which is a fairly easy to modify switch statement holding the pattern of the LEDs 
void blinkPattern(); //My own pattern to blink : Green Blue Green Yellow Green Red
void initialBlink(); //All off, delay, All on, delay, all off, delay

//stringValidater and stringValidater
bool stringValidater(); //checks if a serial message was valid, handles feedback through Serial and returns whether or not delayMS was updated
void stringValidaterInfinite(); //repeats what stringValidater does if garbage is sent

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Value in Tutorial 5.1 

  pinMode(pinRed, OUTPUT);
  pinMode(pinYellow, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);

  //digitalWrite(pinBlue, HIGH);
  delay(1000); //stuff is wack, make sure the laptop has time to setup
  while (!Serial) {} //Make sure I'm all good to go
    
  Serial.println("You best be putting in whatever speed you want this to go in, in milliseconds of course.");
  //while (!Serial.available()) {} Done in stringValidaterInfinite();
  stringValidaterInfinite();
  //Serial.println("You have chosen " + delayMS + " milliseconds."); Done in stringValidater
  //timeVar = millis(); Done in initialBlink 
  initialBlink();

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    if(!stringValidater()){ //Would love to just go Serial.available && stringValidater, but stringValidater consumes a serial message and would screw up if called early
      initialBlink();
    }
  }

  if((millis() - delayMS) >= timeVar) {
    blinkPattern(); //Cycle the pattern
    timeVar = millis();
  }
}

void blinkPattern() {
  static int cycle = 0;
  switch (cycle) { //Pins will all be LOW at starting cycle. blinkPattern turns all off again, so cycle can start anywhere regardless, and only has to turn off the previous one
    case 0:
    digitalWrite(pinGreen, HIGH);
    digitalWrite(pinRed, LOW); //and of course, setting LOW to LOW is okie dokie, because no one will know but us.
    break;
    case 1:
    digitalWrite(pinGreen, LOW);//I wonder if I could set green High and all else low as a default to shorten this code. However, I am not going to fix what isn't broken.
    digitalWrite(pinBlue, HIGH);
    break;
    case 2:
    digitalWrite(pinBlue, LOW);
    digitalWrite(pinGreen, HIGH);
    break;
    case 3:
    digitalWrite(pinGreen, LOW);
    digitalWrite(pinYellow, HIGH);
    break;
    case 4:
    digitalWrite(pinYellow, LOW);
    digitalWrite(pinGreen, HIGH);
    break;
    case 5:
    digitalWrite(pinGreen, LOW);
    digitalWrite(pinRed, HIGH);
    break;
  }
  
  cycle++;
  if (cycle > 5) {
    cycle = 0;
  }
}

void initialBlink() {
  for (int i = 0; i < 4; i++){
    digitalWrite(leds[i], LOW); //All off
  }

  timeVar = millis();
  while (!(millis() - delayMS >= timeVar)) {} //stall

    for (int i = 0; i < 4; i++){
    digitalWrite(leds[i], HIGH); //All on

  }

  timeVar = millis();
  while (!(millis() - delayMS >= timeVar)) {} //stall
    
    for (int i = 0; i < 4; i++){
      digitalWrite(leds[i], LOW); //All off
  
    }

  timeVar = millis(); //Set it up for the loop (Be kind, Please Rewind)
}

bool stringValidater() {
  if(Serial.available()) {
    
    String message = Serial.readStringUntil('\n');
    //Serial.print(message);
    message.trim();
    int userInput = message.toInt(); //converts "1000" to 1000, and "One Thousand" to 0
    //Serial.print(userInput);
    if(userInput) { //true if not 0
      delayMS = userInput;
      Serial.print("You have chosen... "); 
      Serial.print(delayMS); //I think these three can be just one line. Not sure how though
      Serial.println(" milliseconds.");
      return false; //this success will break the while loop of stringValidaterInfinite
    
    } else {
      Serial.println("You have chosen... poorly");
      Serial.println("(Make sure to input only numbers, not letters. '" + message + "' is not a valid input.");
      Serial.println("Now what delay do ya need? (milliseconds)");
      return true; //and this failure will keep the loop running
    
    }
  } else {
    return true;
  }
}

void stringValidaterInfinite() { //Made into a function to keep setup clean
  while(stringValidater()) {
  //Serial.println("Spam");
    delay(1000); //Power Saving
  }
}