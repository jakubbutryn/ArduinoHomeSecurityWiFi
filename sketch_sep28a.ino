#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Joystick.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#define XPIN A0
#define YPIN A1
#define SWPIN 30
#define buzzer 31
#define Menu1Screen 0
#define Menu2Screen 1
#define ActivatingAlarmScreen 2
#define ActivatedAlarmScreen 3
#define ChangePasswordScreen 4
#define PasswordChangedScreen 5
#define AlarmOnScreen 6
#define TrigPin 32
#define EchoPin 33
#define ServoPin 2
#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN          53  
#define IRQ_PIN         19   




MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myservo;
boolean ActiveAlarm = false;
String password = "1111";
String tempPassword;
boolean passChangeMode = false;
boolean passChanged = false;
int screenOffMsg = 0;
boolean AlarmActivated = false;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keypressed;
int Screen = 0;
boolean AuthorizedAccess=false;
int xJoystickPosition = 0;
int yJoystickPosition = 0;
long Duration;
int Distance;
int InitDistance;

LiquidCrystal_I2C lcd(0x27, 16, 2);

//define the cymbols on the buttons of the keypads
char keyMap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {29, 28, 27, 26}; //Row pinouts of the keypad
byte colPins[COLS] = {25, 24, 23, 22}; //Column pinouts of the keypad

Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);


void setup() {
  myservo.write(180);
  lcd.init();
  lcd.backlight();
  pinMode(XPIN, INPUT_PULLUP);
  pinMode(YPIN, INPUT_PULLUP);
  pinMode(SWPIN, INPUT);
  digitalWrite(SWPIN, HIGH);
  pinMode(TrigPin,OUTPUT);
  pinMode(EchoPin,INPUT);
  myservo.attach(2);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.begin(9600);
  

}

void loop() {


  if ( Screen == Menu1Screen) {
    AuthorizedAccess=false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Activate Alarm");
    delay(500);
    lcd.clear();

    Serial.print(map(analogRead(YPIN), 0, 1023, -1, 2));
    delay(500);
    digitalRead(SWPIN) == LOW ? Screen = ActivatingAlarmScreen : Screen = Menu1Screen;
    ScroolUpDown();
  }




  else if (Screen == Menu2Screen) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Change Password ");
    delay(500);
    lcd.clear();
    delay(500);
    digitalRead(SWPIN) == LOW ? Screen = ChangePasswordScreen : Screen = Menu2Screen;
    ScroolUpDown();
  }
  else if (Screen == ActivatingAlarmScreen) {
    //tone(buzzer, 2000, 100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm activated");
    lcd.setCursor(0, 1);
    lcd.print("in:");
    int countdown = 5; // 9 seconds count down before activating the alarm
    while (countdown != 0) {
      lcd.setCursor(13, 1);
      lcd.print(countdown);
      countdown--;
      tone(buzzer, 700, 100);
      delay(1000);
      if(countdown==0){
         InitDistance = calculateDistance();
         ScreenTitleChange("***ARMED***",2,0);
         Screen = ActivatedAlarmScreen;
      }
     
    
  }
  }
  else if (Screen == ActivatedAlarmScreen)
  {

    
    Distance=calculateDistance()<InitDistance-10 ? Screen=AlarmOnScreen:Screen=ActivatedAlarmScreen;
    RotateServo();
    
  }
  else if (Screen=AlarmOnScreen){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ALARM TRIGGERED");
  tone(buzzer, 700, 50);
  delay(100);
  
  }
  else if (Screen == ChangePasswordScreen)
  {

    lcd.clear();
    int i = 1;
    tone(buzzer, 2000, 100);
    tempPassword = "";
    lcd.setCursor(0, 0);
    lcd.print("Current Password");
    lcd.setCursor(0, 1);
    lcd.print(">");
    passChangeMode = true;
    passChanged = true;
    while (passChanged) {
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY) {
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
            keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
            keypressed == '8' || keypressed == '9' ) {
          tempPassword += keypressed;
          lcd.setCursor(i, 1);
          lcd.print("*");
          i++;
          tone(buzzer, 2000, 100);
        }
      }
      if (i > 5 || keypressed == '#') {
        tempPassword = "";
        i = 1;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Current Password");
        lcd.setCursor(0, 1);
        lcd.print(">");
      }
      if ( keypressed == '*') {
        i = 1;
        tone(buzzer, 2000, 100);
        if (password != tempPassword) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password!");
          tone(buzzer, 2000, 2000);
          delay(2000);

          return;
        }
        if (password == tempPassword) {
          tempPassword = "";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Set New Password");
          lcd.setCursor(0, 1);
          lcd.print(">");
          while (passChangeMode) {
            keypressed = myKeypad.getKey();
            if (keypressed != NO_KEY) {
              if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' ||
                  keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' ||
                  keypressed == '8' || keypressed == '9' ) {
                tempPassword += keypressed;
                lcd.setCursor(i, 1);
                lcd.print("*");
                i++;
                tone(buzzer, 2000, 100);
              }
            }
            if (i > 5 || keypressed == '#') {
              tempPassword = "";
              i = 1;
              tone(buzzer, 2000, 100);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Set New Password");
              lcd.setCursor(0, 1);
              lcd.print(">");
            }
            if ( keypressed == '*') {
              i = 1;
              tone(buzzer, 2000, 100);
              password = tempPassword;
              passChangeMode = false;
              passChanged = false;
              Screen = PasswordChangedScreen;
            }
          }
        }
      }
    }
  }

  if (Screen == PasswordChangedScreen) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Changed");
    delay(2000);
    Screen = Menu1Screen;
  }



}

int ScroolUpDown() {
  int tempJoystickPosition = map(analogRead(YPIN), 0, 1023, -1, 2);

  switch (tempJoystickPosition) {
    case 0:
      break;
    case 1:
      Screen + 1 >= 1 ? Screen = 1 : Screen = Screen + 1;
      break;
    case -1:
      Screen - 1 <= 0 ? Screen = 0 : Screen = Screen - 1;
      break;


  }
}

void RotateServo(){
 for (int i = 0; i <= 180; i++) { // goes from 0 degrees to 180 degrees
    
      
       myservo.write(i);              // tell servo to go to position in variable 'pos'
       delay(10);
    
    
  }
  
  for (int i = 180; i >= 0; i--) { // goes from 180 degrees to 0 degrees
    
    
    myservo.write(i);              // tell servo to go to position in variable 'pos'
                delay(10); 
                serial         // waits 15 ms for the servo to reach the position
    }
  
}

int calculateDistance(){ 
  int duration;
  int distance;
  digitalWrite(TrigPin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TrigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  duration = pulseIn(EchoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;
  return distance;
}

void ScreenTitleChange(char StringToDisplay[],int Column,int Row){

 lcd.clear();
 lcd.setCursor(Column, Row);
 lcd.print(StringToDisplay);

}

void LookForCard() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "83 61 9A 16") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    AuthorizedAccess=true;
    Screen = Menu1Screen;
    Serial.println();
   
  }
 
 else   {
    Serial.println(" Access denied");
    Screen = ActivatedAlarmScreen;
    
  }
} 