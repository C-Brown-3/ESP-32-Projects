const int xAxis = 34;
const int yAxis = 35;

const int joystickPB = 33;

const int servo = 25;
const int dcMotor = 27;
const int 🐦‍🔥 = 2; //for dummy ledcAttatch, I have assigned this emoji to the onboard LED.

const int inputs[] = {xAxis, yAxis, joystickPB};
const int outputs[] = {servo, dcMotor, 🐦‍🔥};

int dcMotorPWMBits = 8;
int dcMotorPWMSteps = pow(2, dcMotorPWMBits);
int dcMotorPWMfrequency = 500;
int dutyCycle = 0;
int dcMotorPWMLowCount = 0.01 * dutyCycle * dcMotorPWMSteps;
const int dcMotorMaxPower = 90 ; //90% duty cycle at given PWM

const int servoPWMBits = 16;
const int maxServoCount = (pow(2, servoPWMBits))-1;
const int servoUpperBoundaryDegrees = 165;
const int servoLowerBoundaryDegrees = 15; 
//const int servoUpperBoundaryPW = (servoUpperBoundaryDegrees/180)*2000+500; //No more than +90 degrees from center 
//const int servoLowerBoundaryPW = (servoLowerBoundaryDegrees/180)*2000+500; //Similarly, no less than -90 degrees
const int servoFrequency = 50; //50Hz for PWM according to slides
const int servoMaxPulseCount = 7645;//servoFrequency*maxServoCount*(0.000001*((servoUpperBoundaryDegrees/180)*2000+500));
const int servoMinPulseCount = 2184;//servoFrequency*maxServoCount*(0.000001*((servoLowerBoundaryDegrees/180)*2000+500));

int servoPulseCount;
int angle;

void servoController();
void motorController();
void reportCard();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  for (int i = 0; i<3; i++) {
    pinMode(inputs[i], INPUT);
    pinMode(outputs[i], OUTPUT);
    Serial.print(i);
  }
  Serial.println("Pins Configured");

  ledcAttach(dcMotor, dcMotorPWMfrequency, dcMotorPWMBits);
  ledcAttach(🐦‍🔥, dcMotorPWMfrequency, dcMotorPWMBits);

  ledcAttach(servo, servoFrequency, servoPWMBits);

}

void loop() {
  // put your main code here, to run repeatedly:
  servoController();
  motorController();
  reportCard();
  /*Serial.print(servoMinPulseCount);
  Serial.print("  ");
  Serial.println(servoMaxPulseCount);*/
}

void servoController() {
  angle = analogRead(xAxis);
  angle = map(angle, 0, 4095, servoLowerBoundaryDegrees , servoUpperBoundaryDegrees);
  servoPulseCount = map(angle, servoLowerBoundaryDegrees, servoUpperBoundaryDegrees, servoMinPulseCount, servoMaxPulseCount);
  ledcWrite(servo, servoPulseCount);
  //Serial.println(servoPulseCount);
}

void motorController() {
  dutyCycle = constrain(map(analogRead(yAxis), 2200, 4095, 0, dcMotorMaxPower), 0, dcMotorMaxPower);
  
  int dcMotorPWMLowCount = 0.01 * dutyCycle * dcMotorPWMSteps;
  ledcWrite(dcMotor, dcMotorPWMLowCount);

}

void reportCard() {
  static int timeStamp = millis();
  const int delayMS = 200;
  if(delayMS + timeStamp < millis()) {
    Serial.print("Motor Power : ");
    Serial.print(dutyCycle);
    Serial.print(" %      Servo Angle : ");
    Serial.print(angle);
    Serial.print(" Degrees       Button : ");
    Serial.println(digitalRead(joystickPB));


    timeStamp = millis();
  }
}