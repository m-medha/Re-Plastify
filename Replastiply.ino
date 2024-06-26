/* PROJECT RE-PLASTIFY*/

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <HX711.h>
#include <Servo.h>
#include "HX711.h" 

#define SOUND_SPEED 0.034

//HX711 AMPLIFIER PINS
byte pinData = 3;
byte pinClk = 2;

//ULTRASONIC SENSOR PINS
const int TRIG_PIN =8;
const int ECHO_PIN = 9;
//SERVO PIN
const int SERVO_PIN = 10;

//VARIABLES REQUIRED FOR MEASUREMENTS
long duration;
float distance;
int one_litre;
float weight;

//LCD VARIABLES
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

//SERVO OBJECT
Servo myservo; 
int servoRestPosition = 90; // REST POSITION OF SERVO
int servoPushPosition = 0; // POSITION TO PUSH BOTTLE

//LOAD CELL OBJECT
HX711 scale;

//PARAMETER TO CALIBRATE WEIGHT AND SENSOR
float calibration_factor = 400000; //**IN CASE OF WRONG WEIGHT VALUES, ADJUST THE CALIBRATION FACTOR ACCORDINGLY.**

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  myservo.attach(SERVO_PIN);
   // INITIALIZE LCD
  delay(1000);
  lcd.init();
   //TURN ON LCD BACKLIGHT                     
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("initialising...");
  delay(5000);
  lcd.clear();

  Serial.println("HX711 calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
 
  scale.begin(pinData, pinClk);
  //APPLY CALIBRATION
  scale.set_scale();
  //INITIALIZING TO TARE 
  scale.tare(); //RESET SCALE TO 0

  long zero_factor = scale.read_average(); //GET BASELINE READING
  Serial.print("Zero factor: "); //THIS CAN BE USED TO REMOVE THE NEED TO TARE THE SCALE...FOR PERMANENT SCALE PROJECTS
  Serial.println(zero_factor);
}

void loop() {
  scale.set_scale(calibration_factor); //ADJUST TO THIS CALIBRATION FACTOR
  Serial.print("Reading: ");
  weight=scale.get_units();
  Serial.print(scale.get_units(), 2);
  Serial.print(" kgs"); 
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  digitalWrite(TRIG_PIN,LOW);
  delay(2);
  digitalWrite(TRIG_PIN,HIGH);
  delay(10);
  digitalWrite(TRIG_PIN,LOW);
  duration = pulseIn(ECHO_PIN,HIGH);
  distance= duration * SOUND_SPEED/2 ;
  if(((0.09<weight) && (weight<0.16))&&(distance<20)) 
  {
  lcd.clear();
  lcd.setCursor(2, 0);  // Set cursor to the first line
  Serial.println("1ltr bottle detected");
  lcd.print("1LTR BOTTLE!");
  delay(3500);
  lcd.clear();
  lcd.setCursor(2, 0);  // Set cursor to the second line
  lcd.print("Reward points:");
  lcd.setCursor(2,1);
  lcd.print("20");
  delay(3500);
  lcd.clear();
  
  // PUSHING THE BOTTLE INTO BIN
  myservo.write(servoRestPosition);
  delay(750); 
  //MOVE SERVO TO PUSH POSITION
  myservo.write(servoPushPosition);
  delay(1000); 
  // MOVE SERVO TO REST POSITION
  myservo.write(servoRestPosition);
  delay(1000); // Wait for 1 second
  lcd.setCursor(2,1);
  lcd.print("Bottle In Bin");
  delay(3500);
  lcd.clear();
   }
else if((weight==0)||(weight<0)){
lcd.clear();
delay(100);
 }
    
else if((weight>0.12)||(weight>0 && weight<0.09)||(distance>20)){
  lcd.clear();
  lcd.setCursor(2, 0); //CURSOR ON FIRST LINE
  Serial.println("NOT PLASTIC");
  lcd.print("NOT PLASTIC");
  delay(1000);
  lcd.clear();
  lcd.setCursor(2, 0); //CURSOR ON SECOND LINE 
  lcd.print("PLEASE REMOVE ");
  lcd.setCursor(2, 1);
  lcd.print("THE BOTTLE ");
  delay(1000);
  lcd.clear();
 }

}
