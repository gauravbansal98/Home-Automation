#include <LiquidCrystal.h>
#include<SPI.h>
#include<MFRC522.h>
#include <IRremote.h>
#include <TimerOne.h>
#include<String.h>
#include <Servo.h>
#include <SPI.h>

#define manual_control_switch A4
#define manual_button_for_bulb1 A3
#define manual_button_for_bulb2 A2
#define RST_PIN   9
#define SS_PIN    10
#define RECV_PIN 8
#define buzzer_pin A5
#define lm35_pin A0

void ReadRemote();

Servo main_gate_servo;

unsigned int Time = 0;
int manual_auto = 0;

//for rfid module
byte readCard[4];
char* myTags[2] = {"B51C1A77", "3DBE4052"};
int tagsCount = 0;
String tagID = "";
boolean successRead = false;
boolean correctTag = false;

//for ir_controlled_password
uint8_t enteredPassword[4];
uint8_t passwordLength = 0;
uint8_t password[4] = {2, 4, 6, 8};
uint8_t wrongAttempts = 0;

//for doors
uint8_t front_door_state = 0;

//for lcd_screen
uint8_t lcd_screen = 0;
uint8_t lcd_screen_change = 0;
unsigned int time_for_screen_change = 0;
float temperature = 0;
String no_of_person ;
String real_time = "Wait";
uint8_t bulb_one_state = 0;
uint8_t bulb_two_state = 0;
uint8_t bulb_three_state = 0;
String last_action = "NOTHING";
String day = "Wait";
String date = "Wait";


int door_is_opened = 0;
int door_is_closed = 1;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
IRrecv irrecv(RECV_PIN);
decode_results results;
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
  Serial.begin(115200);
  lcd.begin(16, 2);
  main_gate_servo.attach(A1);
  main_gate_servo.write(10);
  irrecv.enableIRIn(); // Start the receiver of tsop


  pinMode(manual_control_switch, INPUT);
  pinMode(manual_button_for_bulb1, INPUT);
  pinMode(manual_button_for_bulb2, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(lm35_pin, INPUT);

  SPI.begin();        // SPI bus
  mfrc522.PCD_Init(); //  MFRC522
}

void loop()
{
  get_temperature();
  print_on_lcd();
  check_rfid();
  read_serial_data();
  ReadRemote();
  aag_lagi();
  manual_auto_switch();
}

void check_rfid()
{
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return;
  }
  tagID = "";
  // The MIFARE PICCs that we use have 4 byte UID
  for ( uint8_t i = 0; i < 4; i++)
  { //
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  correctTag = false;
  // Checks whether the scanned tag is the master tag
  if (tagID == myTags[0] || tagID == myTags[1]) {
    open_front_door();
  }
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  // Serial.println(strdup(tagID.c_str()));
  return 1;
}

int states[2] = {0, 0};

void manual_auto_switch()
{
  int control_button = digitalRead(manual_control_switch);
  if (control_button == 1)
  {
    manual_auto = 1;
    if (digitalRead(manual_button_for_bulb1) != states[0] && digitalRead(manual_button_for_bulb1) == 1)
    {
      states[0] = 1;
      Serial.write("bulb 1 on\n");
      last_action = "Bulb 1 on";
      bulb_one_state = 1;
    }
    else if(digitalRead(manual_button_for_bulb1) != states[0] && digitalRead(manual_button_for_bulb1) == 0)
    {
      states[0] = 0;
      Serial.write("bulb 1 off\n");
      last_action = "Bulb 1 off";
      bulb_one_state = 0;
    }
    if (digitalRead(manual_button_for_bulb2)== 1 && digitalRead(manual_button_for_bulb2) != states[1])
    {
      states[1] = 1;
      Serial.write("bulb 2 on\n");
      last_action = "bulb 2 on";
      bulb_two_state = 1;
    }
    else if(digitalRead(manual_button_for_bulb2) == 1 && digitalRead(manual_button_for_bulb2) != states[1])
    {
      states[1] = 0;
      Serial.write("bulb 2 off\n");
      last_action = "bulb 2 off";
      bulb_two_state = 0;
    }
  }
  else
  {
    manual_auto = 0;
  }
}

void get_temperature()
{
  int value = analogRead(lm35_pin);
  float millivolts = (value / 1024.0) * 5000;
  temperature = millivolts / 10;
}

void aag_lagi()
{
  if (temperature > 50)
  {
    Time = millis();
    lcd.clear();
    lcd.print("Home is at fire");
    delay(100);
    digitalWrite(buzzer_pin, HIGH);
    Serial.write("aag lagi");
    //open_front_door();
  }
  else
  {
    digitalWrite(buzzer_pin, LOW);
  }
}

void read_serial_data()
{
  if (Serial.available())
  {
    Time = millis();
    String s = Serial.readStringUntil('\n');
  //  lcd.clear();
  // lcd.print(s);
      if ( s == "bulb 1 is on" )
      {
        last_action = "bulb 1 on";
        bulb_one_state = 1;
      }
      else if ( s == "bulb 1 is off")
      {
        bulb_one_state = 0;
        last_action = "bulb 1 off";
      }
      else if ( s == "bulb 2 is on")
      {
        bulb_two_state = 1;
        last_action = "bulb 2 on";
      }
      else if ( s == "bulb 2 is off")
      {
        bulb_two_state = 0;
        last_action = "bulb 2 off";
      }
      else if (s == "bulb 3 is on")
      {
        bulb_three_state = 1;
        last_action = "bulb 3 on";
      }
      else if (s == "bulb 3 is off")
      {
        bulb_three_state = 0;
        last_action = "bulb 3 off";
      }
      else if (s == "open front door")
      {
        open_front_door();
        last_action = "door opened";
      }
      else if (s == "close front door")
      {
        last_action = "door closed";
        close_front_door();
      }
      else if (s == "no of person")
      {
        int y = Serial.read();
        no_of_person = y-48;
      }
      else if (s == "time is")
      {
        real_time = Serial.readStringUntil('\n');
        day = real_time.substring(0, 3);
        date = real_time.substring(5, 16);
        real_time = real_time.substring(17);
      }
    }
}

void print_on_lcd()
{
  unsigned int current_time = millis();

  // Serial.println(current_time);
  // Serial.println(time_for_screen_change);
  //Serial.println(Time);
  // Serial.println("");
  if (current_time > Time + 2000)
  {
    if (lcd_screen == 0)
    {
      if (lcd_screen_change == 0)
      {
        lcd_screen_change = 1;
        time_for_screen_change = current_time;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tem:");
      lcd.print(temperature);
      lcd.setCursor(10, 0);
      lcd.print("N.P.:");
      lcd.print(no_of_person);
      lcd.setCursor(0, 1);
      lcd.print("Day:");
      lcd.print(day);
      delay(10);
    }
    else if (lcd_screen == 2)
    {
      if (lcd_screen_change == 0)
      {
        lcd_screen_change = 3;
        time_for_screen_change = current_time;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("B1:");
      lcd.print(bulb_one_state);
      lcd.setCursor(6, 0);
      lcd.print("B2:");
      lcd.print(bulb_two_state);
      lcd.setCursor(12, 0);
      lcd.print("B3:");
      lcd.print(bulb_three_state);
      lcd.setCursor(0, 1);
      lcd.print("L.A.:");
      lcd.print(last_action);
      delay(10);
    }
    else if (lcd_screen == 1)
    {
      if (lcd_screen_change == 0)
      {
        lcd_screen_change = 2;
        time_for_screen_change = current_time;
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Date:");
      lcd.print(date);
      lcd.setCursor(0, 1);
      lcd.print("Time:");
      lcd.print(real_time);
      delay(10);
    }
  }
  if (time_for_screen_change + 2000 < (current_time) && lcd_screen_change == 1)
  {
    lcd_screen = 1;
    lcd_screen_change = 0;
  }
  else if (time_for_screen_change + 2000 < (current_time) && lcd_screen_change == 2)
  {
    lcd_screen = 2;
    lcd_screen_change = 0;
  }
  else if (time_for_screen_change + 2000 < (current_time) && lcd_screen_change == 3)
  {
    lcd_screen = 0;
    lcd_screen_change = 0;
  }
}

void open_front_door()
{
  if(door_is_opened == 1)
  {
    return;
  }
  int  pos = 0;
  lcd.clear();
  lcd.print("Door Opened");
  for (pos = 0; pos <= 90; pos += 1)
  {
    main_gate_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  door_is_opened = 1;
  door_is_closed = 0;
  last_action = "Door opened";
  Time = millis();
  Serial.write("door is opened\n");
}

void close_front_door()
{
  if(door_is_closed == 1)
  {
    return;
  }
  int pos = 0;
  lcd.clear();
  lcd.print("Door Closed");
  for (pos = 90; pos >= 15; pos -= 1)
  {
    main_gate_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  door_is_closed = 1;
  door_is_opened = 0;
  last_action = "Door closed";
  Serial.write("door is closed");
  Time = millis();
}

void ReadRemote()
{
  if (irrecv.decode(&results))
  {
    translateIR();
    //  Serial.println(results.value);
    irrecv.resume(); // Receive the next value
  }
}

void translateIR() // takes action based on IR code received describing Car MP3 IR codes
{
  switch (results.value)
  {
    //for 1
    case 33444015:
      enteredPassword[passwordLength] = 1;
      passwordLength++;
      break;

    //for 2
    case 33478695:
      enteredPassword[passwordLength] = 2;
      passwordLength++;
      break;

    //for 3
    case 33486855:
      enteredPassword[passwordLength] = 3;
      passwordLength++;
      break;

    //for 4
    case 33435855:
      enteredPassword[passwordLength] = 4;
      passwordLength++;
      break;

    //for 5
    case 33468495:
      enteredPassword[passwordLength] = 5;
      passwordLength++;
      break;

    //for 6
    case 33452175:
      enteredPassword[passwordLength] = 6;
      passwordLength++;
      break;

    //for 7
    case 33423615:
      enteredPassword[passwordLength] = 7;
      passwordLength++;
      break;

    //for 8
    case 33484815:
      enteredPassword[passwordLength] = 8;
      passwordLength++;
      break;

    //for 9
    case 33462375:
      enteredPassword[passwordLength] = 9;
      passwordLength++;
      break;

    //for 0
    case 33480735:
      enteredPassword[passwordLength] = 0;
      passwordLength++;
      break;

    //for stop
    case 33441975:
      Time = millis();
      lcd.clear();
      lcd.print("Bulb 1 on");
      Serial.write("bulb 1 on\n");
      last_action = "Bulb 1 on";
      bulb_one_state = 1;
      break;

    //for next
    case 33456255:
      Time = millis();
      lcd.clear();
      lcd.print("Bulb 1 off");
      Serial.write("bulb 1 off\n");
      last_action = "Bulb 1 off";
      bulb_one_state = 0;
      break;

    //for 'mode'
    case 33446055:
      Time = millis();
      lcd.clear();
      lcd.print("Bulb 2 on");
      Serial.write("bulb 2 on\n");
      last_action = "Bulb 2 on";
      bulb_two_state = 1;
      break;

    //for rewind
    case 33439935:
      Time = millis();
      lcd.clear();
      lcd.print("Bulb 2 off");
      Serial.write("bulb 2 off\n");
      last_action = "Bulb 2 off";
      bulb_two_state = 0;
      break;

    //for mute
    case 33454215:
      Time = millis();
      lcd.clear();
      lcd.print("Bulb 3 on");
      Serial.write("bulb 3 on\n");
      last_action = "Bulb 3 on";
      bulb_three_state = 1;
      break;

    //for forward
    case 33472575:
      Time = millis();
      lcd.clear();
      lcd.print("Bulb 3 off");
      Serial.write("bulb 3 off\n");
      last_action = "Bulb 3 off";
      bulb_three_state = 0;
      break;

    //for 'vol-'
    case 33464415:
      break;

    //for vol+
    case 33448095:
      break;

    //for 'rpt'
    case 33427695:
      open_front_door();
      break;

    //for u/sd
    case 33460335:
      close_front_door();
      break;


    //for 'EQ'
    case 33431775:
      break;

    default :
      break;
  }
  if (passwordLength == 1)
  {
    Time = millis();
    lcd.clear();
    lcd.print("*");
  }
  else if (passwordLength == 2)
  {
    Time = millis();
    lcd.clear();
    lcd.print("**");
  }
  else if (passwordLength == 3)
  {
    Time = millis();
    lcd.clear();
    lcd.print("***");
  }
  else if (passwordLength == 4)
  {
    Time = millis();
    passwordLength = 0;
    for (int i = 0; i < 4; i++)
    {
      if (password[i] == enteredPassword[i])
        passwordLength++;
    }
    if (passwordLength == 4)
    {
      open_front_door();
      wrongAttempts = 0;
      passwordLength = 0;
    }
    else
    {
      lcd.clear();
      lcd.print("Wrong password");
      ++wrongAttempts;
      passwordLength = 0;
      if (wrongAttempts >= 3)
      {
        Time = millis();
        lcd.clear();
        lcd.print("DON'T TRY. THIS");
        lcd.setCursor(0, 1);
        lcd.print("HOME IS SECURE");
        digitalWrite(buzzer_pin, HIGH);
        delay(2000);
        digitalWrite(buzzer_pin, LOW);
        Serial.write("someone is at your door\n");  ////send data to esp ki chor aya
      }
    }
  }
}

