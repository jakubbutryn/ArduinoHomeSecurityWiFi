#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "DHT.h"
#include <String.h>


#define XPIN A0
#define YPIN A1
#define SWPIN 30
#define buzzer 31
#define Menu1Screen 20
#define Menu2Screen 21
#define Menu3Screen 22
#define Menu4Screen 23
#define Menu5Screen 24
#define ActivatingAlarmScreen 2
#define ActivatedAlarmScreen 3
#define ChangePasswordScreen 4
#define PasswordChangedScreen 5
#define AlarmOnScreen 6
#define MappingScreen 7
#define HomeDataScreen 8
#define DisableAlarmScreen 9
#define AddRFIDScreen 10
#define TempControlScreen 11
#define EnterNewTemperatureScreen 12
#define TurnOnHeatingScreen 13
#define TrigPin 32
#define EchoPin 33
#define ServoPin 2
#define RST_PIN 5
#define SS_PIN 53
#define IRQ_PIN 19
#define sensePin 34
#define AlarmTurnOnPin 36
#define AlarmTurnOffPin 35
#define HeatingTurnOnPin 37
#define HeatingTurnOffPin 38
#define WaterLevelPin A2
#define Type DHT11


DHT HT(sensePin, Type);
MFRC522 mfrc522(SS_PIN, RST_PIN);
int numOfRFIDsInBase = 4;
int numOfRFIDs = 1;
int Screen = 20;
int screenOffMsg = 0;
int xJoystickPosition = 0;
int yJoystickPosition = 0;
int SetTemperature = 20;
int TurnOnHeating = 0;
const byte ROWS = 4;
const byte COLS = 4;
const unsigned long eventInterval = 100;
const unsigned long eventInterval2 = 3000;
unsigned long previousTime = 0;
long Duration;
float Distance;
float InitDistance;
float floatHumidity;
float floatTempC;
String password = "1111";
String tempPassword;
String temptemperatureToSet = "";
String previousStateAlarm = "OFF";
String previousStateHeating = "OFF";
char *temperatureToSet = "22";
char *heatingToSet = "OFF";
char *stringWaterLevel;
char *stringAlarmStatus = "Disarmed";
boolean passChangeMode = false;
boolean passChanged = false;
boolean enterPasswordOnAlarm = false;
boolean AlarmActivated = false;
boolean RFIDadded = false;
boolean ActiveAlarm = false;
boolean AuthorizedAccess = false;
boolean DeniedAccess = false;
boolean tempChangeMode = false;
boolean heatingChangeMode = false;
boolean stringSent = false;
char keypressed;
char charHumidity[7];
char charTempC[7];
char tempChar[40];
char tempChar1[40];
char tempChar2[40];
char tempChar3[40];
char tempChar4[40];
char arrayOfRFIDs[5][12];



LiquidCrystal_I2C lcd(0x27, 16, 2);
char keyMap[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 29, 28, 27, 26 };
byte colPins[COLS] = { 25, 24, 23, 22 };

Keypad myKeypad = Keypad(makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);

byte downArrow[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100
};
byte upArrow[] = {
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};



void setup() {

  pinMode(AlarmTurnOnPin, INPUT);
  pinMode(AlarmTurnOffPin, INPUT);
  pinMode(HeatingTurnOnPin, INPUT);
  pinMode(HeatingTurnOffPin, INPUT);
  pinMode(XPIN, INPUT_PULLUP);
  pinMode(YPIN, INPUT_PULLUP);
  pinMode(SWPIN, INPUT);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  strcpy(arrayOfRFIDs[0], "83 61 9A 16");
  digitalWrite(SWPIN, HIGH);
  SPI.begin();
  HT.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);
}

void loop() {
  if (Screen == Menu1Screen) {
    stringAlarmStatus = "Disarmed";
    AuthorizedAccess = false;
    ScreenTitleChange("Activate Alarm", 0, 0);
    Arrow('Down');
    lcdClear500ms();
    digitalRead(SWPIN) == LOW ? Screen = ActivatingAlarmScreen : Screen = Menu1Screen;
    if (digitalRead(SWPIN) == LOW) {
      stringSent = false;
      do {
        stringAlarmStatus = "ManuallyActivatingAlarm";
        fetchHomeData();
      } while (!stringSent == true);
      stringSent = false;
      Screen = ActivatingAlarmScreen;
      fetchHomeData();
    } else {
      Screen = Menu1Screen;
    }
    ScroolUpDown();
    fetchHomeData();
  } else if (Screen == Menu2Screen) {
    ScreenTitleChange("Change Password", 0, 0);
    Arrow('Both');
    lcdClear500ms();
    stringAlarmStatus = "Disarmed";
    digitalRead(SWPIN) == LOW ? Screen = ChangePasswordScreen : Screen = Menu2Screen;
    ScroolUpDown();
    fetchHomeData();
  } else if (Screen == Menu3Screen) {
    ScreenTitleChange("Home Data", 0, 0);
    Arrow('Both');
    lcdClear500ms();
    stringAlarmStatus = "Disarmed";
    digitalRead(SWPIN) == LOW ? Screen = HomeDataScreen : Screen = Menu3Screen;
    ScroolUpDown();
    fetchHomeData();
  } else if (Screen == HomeDataScreen) {
    sprintf(tempChar, "T:%sC", charTempC);
    sprintf(tempChar1, "Hum:%s%%", charHumidity);
    sprintf(tempChar2, "WL:%s", stringWaterLevel);
    lcd.setCursor(0, 0);
    lcd.print(tempChar);
    lcd.setCursor(8, 0);
    lcd.print(tempChar2);
    lcd.setCursor(0, 1);
    lcd.print(tempChar1);
    delay(1000);
    lcd.clear();
    stringAlarmStatus = "Disarmed";
    digitalRead(SWPIN) == LOW ? Screen = Menu3Screen : Screen = HomeDataScreen;
    fetchHomeData();
  } else if (Screen == Menu4Screen) {
    ScreenTitleChange("Add new RFID", 0, 0);
    Arrow('Both');
    lcdClear500ms();
    stringAlarmStatus = "Disarmed";
    digitalRead(SWPIN) == LOW ? Screen = AddRFIDScreen : Screen = Menu4Screen;
    ScroolUpDown();
    fetchHomeData();
  } else if (Screen == Menu5Screen) {
    ScreenTitleChange("Temp Control", 0, 0);
    Arrow('Up');
    lcdClear500ms();
    stringAlarmStatus = "Disarmed";
    digitalRead(SWPIN) == LOW ? Screen = TempControlScreen : Screen = Menu5Screen;
    ScroolUpDown();
    fetchHomeData();

  } else if (Screen == TempControlScreen) {
    lcd.clear();
    sprintf(tempChar3, "Heating: %s", heatingToSet);
    sprintf(tempChar4, "Set Temp: %s", temperatureToSet);
    lcd.setCursor(0, 0);
    lcd.print(tempChar3);
    lcd.setCursor(0, 1);
    lcd.print(tempChar4);
    stringAlarmStatus = "Disarmed";
    fetchHomeData();
    delay(2000);
    digitalRead(SWPIN) == LOW ? Screen = EnterNewTemperatureScreen : Screen = TempControlScreen;
  } else if (Screen == EnterNewTemperatureScreen) {
    lcd.clear();
    ScreenTitleChange("Enter Temperature", 0, 0);
    int i = 0;
    temperatureToSet = "";
    temptemperatureToSet = "";
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set New Temp");
    lcd.setCursor(0, 1);
    lcd.print(">");
    tempChangeMode = true;
    while (tempChangeMode) {
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY) {
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' || keypressed == '8' || keypressed == '9') {
          temptemperatureToSet += keypressed;
          lcd.setCursor(i, 1);
          lcd.print(keypressed);
          i++;
          tone(buzzer, 2000, 100);
        }
      }
      if (i > 2 || keypressed == '#') {
        temptemperatureToSet = "";
        i = 1;
        tone(buzzer, 2000, 100);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Set New Temp");
        lcd.setCursor(0, 1);
        lcd.print(">");
      }
      if (keypressed == '*') {
        i = 1;
        tone(buzzer, 2000, 100);
        temperatureToSet = temptemperatureToSet.c_str();
        tempChangeMode = false;
        ScreenTitleChange("Temp Changed!", 0, 0);
        Screen = TurnOnHeatingScreen;
      }
      stringAlarmStatus = "ChangingTemperature";
      fetchHomeData();
    }
  } else if (Screen == TurnOnHeatingScreen) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Turn On Heating?");
    lcd.setCursor(0, 1);
    lcd.print("1-YES 2-NO");
    heatingChangeMode = true;
    while (heatingChangeMode) {
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY) {
        if (keypressed == '1') {
          heatingToSet = "ON";
          tone(buzzer, 2000, 100);
          heatingChangeMode = false;
        } else if (keypressed == '2') {
          heatingToSet = "OFF";
          tone(buzzer, 2000, 100);
          heatingChangeMode = false;
        }
      }
      stringAlarmStatus = "Disarmed";
    }
    ScreenTitleChange("Heating Changed!", 0, 0);
    delay(2000);
    stringAlarmStatus = "ChangingHeating";
    fetchHomeData();
    Screen = Menu1Screen;
  } else if (Screen == ActivatingAlarmScreen) {
    ScreenTitleChange("Alarm Activated", 0, 0);
    lcd.setCursor(0, 1);
    lcd.print("in:");
    int countdown = 5;
    while (countdown != 0) {
      lcd.setCursor(13, 1);
      lcd.print(countdown);
      countdown--;
      tone(buzzer, 700, 100);
      stringAlarmStatus = "Activating";
      fetchHomeData();
      delay(1000);
      if (countdown == 0) {
        ScreenTitleChange("**MAPPING**", 2, 0);
        Screen = MappingScreen;
      }
    }
  } else if (Screen == MappingScreen) {
    InitDistance = calculateDistance();
    delay(2000);
    ScreenTitleChange("***ARMED***", 2, 0);
    Screen = ActivatedAlarmScreen;
    stringAlarmStatus = "Mapping";
    fetchHomeData();
  } else if (Screen == ActivatedAlarmScreen) {
    delay(2000);
    ScreenTitleChange("***ARMED***", 2, 0);
    do {
      Distance = calculateDistance();
      LookForCard(false);
      stringAlarmStatus = "Armed";
      fetchHomeData();
    } while (Distance > InitDistance - 5 && Distance < InitDistance + 5 && AuthorizedAccess == false && DeniedAccess == false && digitalRead(AlarmTurnOffPin) != HIGH);
    if (AuthorizedAccess == true || digitalRead(AlarmTurnOffPin) == HIGH) {
      ScreenTitleChange("ACCES GRANTED", 0, 0);
      tone(buzzer, 1000, 500);
      delay(700);
      stringSent = false;
      do {
        stringAlarmStatus = "Disarming";
        fetchHomeData();
      } while (!stringSent == true);
      stringSent = false;
      Screen = Menu1Screen;
    } else if (DeniedAccess == true) {

      ScreenTitleChange("ACCES DENIED", 0, 0);
      tone(buzzer, 100, 1500);
      Screen = ActivatedAlarmScreen;
      DeniedAccess = false;
      stringAlarmStatus = "DeniedAccess";
      fetchHomeData();
    } else {
      Screen = AlarmOnScreen;
    }
  } else if (Screen == AlarmOnScreen) {
    ScreenTitleChange("ALARM TRIGGERED", 0, 0);
    tone(buzzer, 700, 50);
    LookForCard(false);

    delay(100);
    digitalRead(SWPIN) == LOW ? Screen = DisableAlarmScreen : Screen = AlarmOnScreen;
    stringAlarmStatus = "TRIGGERED";
    fetchHomeData();
    if (AuthorizedAccess == true) {
      ScreenTitleChange("ACCES GRANTED", 0, 0);
      for (int i = 0; i < numOfRFIDsInBase; i++) {
        tone(buzzer, 1000, 500);
        delay(700);
        i++;
        fetchHomeData();
      }
      Screen = Menu1Screen;
    } else if (DeniedAccess == true) {
      ScreenTitleChange("ACCES DENIED", 0, 0);
      tone(buzzer, 100, 1500);
      DeniedAccess = false;
      delay(2000);
      stringAlarmStatus = "DeniedAccess";
      fetchHomeData();
    }
  } else if (Screen == DisableAlarmScreen) {
    stringAlarmStatus = "Disarming";
    ScreenTitleChange("ENTER PASSWORD", 0, 0);
    enterPasswordOnAlarm = true;
    int i = 1;
    while (enterPasswordOnAlarm) {
      fetchHomeData();
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY) {
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' || keypressed == '8' || keypressed == '9') {
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
        lcd.print("ENTER PASSWORD");
        lcd.setCursor(0, 1);
        lcd.print(">");
      }
      if (keypressed == '*') {
        i = 1;
        tone(buzzer, 2000, 100);
        if (password != tempPassword) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password!");
          tone(buzzer, 2000, 2000);
          delay(2000);
          enterPasswordOnAlarm = false;
          Screen = AlarmOnScreen;
          return;
        }
        if (password == tempPassword) {
          ScreenTitleChange("ACCESS GRANTED", 0, 0);
          delay(2000);
          enterPasswordOnAlarm = false;
          Screen = Menu1Screen;
          return;
        }
      }
    }
  } else if (Screen == ChangePasswordScreen) {
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
      fetchHomeData();
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY) {
        if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' || keypressed == '8' || keypressed == '9') {
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
      if (keypressed == '*') {
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
              if (keypressed == '0' || keypressed == '1' || keypressed == '2' || keypressed == '3' || keypressed == '4' || keypressed == '5' || keypressed == '6' || keypressed == '7' || keypressed == '8' || keypressed == '9') {
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
            if (keypressed == '*') {
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
      stringAlarmStatus = "ChangingPassword";
      fetchHomeData();
    }
  } else if (Screen == AddRFIDScreen) {
    ScreenTitleChange("Place RFID Tag", 0, 0);
    LookForCard(true);
    stringAlarmStatus = "AddingRFID";
    fetchHomeData();
  }
  if (Screen == PasswordChangedScreen) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Changed");
    delay(2000);
    stringAlarmStatus = "ChangingPassword";
    fetchHomeData();
    Screen = Menu1Screen;
  }
}


int ScroolUpDown() {
  int tempJoystickPosition = map(analogRead(YPIN), 0, 1023, -1, 2);
  switch (tempJoystickPosition) {
    case 0:
      break;
    case 1:
      Screen + 1 >= 24 ? Screen = 24 : Screen = Screen + 1;
      tone(buzzer, 700, 100);
      break;
    case -1:
      Screen - 1 <= 20 ? Screen = 20 : Screen = Screen - 1;
      tone(buzzer, 700, 100);
      break;
  }
}

float calculateDistance() {
  unsigned long duration;
  float distance;
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  duration = pulseIn(EchoPin, HIGH);
  distance = duration / 58.00;
  return distance;
}

void ScreenTitleChange(char StringToDisplay[], int Column, int Row) {
  lcd.clear();
  lcd.setCursor(Column, Row);
  lcd.print(StringToDisplay);
}

void Arrow(char UpDownBoth[]) {
  if (UpDownBoth == 'Up') {
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
  } else if (UpDownBoth == 'Down') {
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  } else if (UpDownBoth == 'Both') {
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
}

void LookForCard(boolean WantToAddCard) {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (WantToAddCard == false) {
    for (int i = 0; i < numOfRFIDsInBase; i++) {
      if (content.substring(1) == arrayOfRFIDs[i]) {
        AuthorizedAccess = true;
        break;
      } else {
        DeniedAccess = true;
      }
    }
  } else if (WantToAddCard == true) {
    for (int i = 0; i < numOfRFIDsInBase; i++) {
      if (content.substring(1) == arrayOfRFIDs[i]) {
        RFIDadded = true;
        ScreenTitleChange("ALREADY ADDED!", 0, 0);
        tone(buzzer, 100, 1500);
        delay(2000);
        Screen = Menu4Screen;
        break;
      }
    }
    if (numOfRFIDs < 5 && RFIDadded == false) {
      char buffer[12];
      content.substring(1).toCharArray(buffer, 12);
      strcpy(arrayOfRFIDs[numOfRFIDs], buffer);
      numOfRFIDs++;
      ScreenTitleChange("RFID ADDED", 0, 0);
      tone(buzzer, 1000, 500);
      delay(2000);
      Screen = Menu4Screen;
    } else if (numOfRFIDs >= 5 && RFIDadded == false) {
      ScreenTitleChange("MEMORY FULL", 0, 0);
      tone(buzzer, 100, 1500);
      delay(2000);
      Screen = Menu4Screen;
    }
  }
  RFIDadded = false;
}

void lcdClear500ms() {
  delay(500);
  lcd.clear();
  delay(500);
}

void fetchHomeData() {
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= eventInterval2) {
    int waterLevel;
    floatHumidity = HT.readHumidity();
    floatTempC = HT.readTemperature();
    waterLevel = analogRead(WaterLevelPin);
    if (waterLevel <= 100) {
      stringWaterLevel = "Empty";

    } else if (waterLevel > 100 && waterLevel <= 300) {
      stringWaterLevel = "Low";

    } else if (waterLevel > 300 && waterLevel <= 330) {
      stringWaterLevel = "Medium";

    } else if (waterLevel > 330) {
      stringWaterLevel = "High";
    }
    dtostrf(floatHumidity, 4, 1, charHumidity);
    dtostrf(floatTempC, 4, 1, charTempC);


    String serialToSend = "&value1=" + String(floatTempC) + "&value2=" + String(floatHumidity) + "&value3=" + stringWaterLevel + "&value4=" + stringAlarmStatus + "&value5=" + String(heatingToSet) + "&value6=" + temperatureToSet + "";
    Serial.print(serialToSend);
    stringSent = true;


    if (digitalRead(AlarmTurnOnPin) == HIGH && previousStateAlarm == "OFF") {
      Screen = ActivatingAlarmScreen;
      previousStateAlarm = "ON";
    } else if (digitalRead(AlarmTurnOffPin) == HIGH && previousStateAlarm == "ON") {
      Screen = Menu1Screen;
      previousStateAlarm = "OFF";
    }


    if (digitalRead(HeatingTurnOnPin) == HIGH && previousStateHeating == "OFF") {
      heatingToSet = "ON";
      previousStateHeating = "ON";
    } else if (digitalRead(HeatingTurnOffPin) == HIGH && previousStateHeating == "ON") {
      heatingToSet = "OFF";
      previousStateHeating = "OFF";
    }



    previousTime = currentTime;
    Serial.print('\n');
  }
  return;
}
