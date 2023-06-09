#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// COMPILER CONSTANTS
#define BEEPER A0
#define DIODE_R A1
#define DIODE_G A2
#define DIODE_B A3

// THE KEYPAD
char keypad_hexa_keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte keypad_row_pins[4] = {7, 6, 5, 4}; // ROW PINS
byte keypad_col_pins[4] = {11, 10, 9, 8}; // COLUMN PINS
Keypad keypad = Keypad(makeKeymap(keypad_hexa_keys), keypad_row_pins, keypad_col_pins, 4, 4);

// THE LCD SCREEN
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F,16,2);

// NORMAL VARS
int i;
char input;
uint8_t cursor_pos=0;
char code[7]={'*','*','*','*','*','*','*'}; // * = empty
bool is_planted=false;
int timer=0;
int beep_timer=0;

void clear_code(){ // Function that clears the bomb code
  for(i=0;i<7;i++){
    code[i]='*';
  }
}

void beep(){ // What happens when the bomb beeps
  beep_timer=0;
  tone(BEEPER,440,250);
  digitalWrite(DIODE_R,LOW);
  delay(100);
  digitalWrite(DIODE_R,HIGH);
}

void setup(){
  // Pins
  pinMode(BEEPER,OUTPUT);
  digitalWrite(BEEPER,LOW);
  pinMode(DIODE_R,OUTPUT);
  digitalWrite(DIODE_R,LOW);
  pinMode(DIODE_G,OUTPUT);
  digitalWrite(DIODE_G,LOW);
  pinMode(DIODE_B,OUTPUT);
  digitalWrite(DIODE_B,HIGH);

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.cursor();
  lcd.setCursor(4,0);
  lcd.print("*******");
  lcd.setCursor(4,0);
}
  
void loop(){
  if(is_planted){ //If the bomb is planted
    timer++;
    beep_timer++;

    // Beeping
    if(timer<5001 && beep_timer>1000){
      beep();
    }else if(timer>5000 && timer<10001 && beep_timer>800){
      beep();
    }else if(timer>10000 && timer<15001 && beep_timer>600){
      beep();
    }else if(timer>15000 && timer<20001 && beep_timer>400){
      beep();
    }else if(timer>20000 && timer<22501 && beep_timer>200){
      beep();
    }else if(timer>22500 && beep_timer>100){
      beep();
    }

    if(timer>25000){ // The bomb goes off
      is_planted=false;
      timer=0;
      beep_timer=0;
      tone(BEEPER,100,2750);
      digitalWrite(DIODE_R,LOW);
      digitalWrite(DIODE_G,LOW);
      digitalWrite(DIODE_B,LOW);
      delay(3000);
      digitalWrite(DIODE_B,HIGH);
    }
  }

  lcd.setCursor(cursor_pos+4,0);
  input = keypad.getKey();
  if (input){
    if((input=='A')||(input=='B')||(input=='C')||(input=='D')){} // Empty buttons
    else if(input=='*'){ // The delete button
      tone(BEEPER,200,100);
      if (cursor_pos!=0){
        cursor_pos--;
        code[cursor_pos]='*';
        lcd.clear();
        lcd.setCursor(4,0);
        for(i=0;i<7;i++){
          lcd.print(code[i]);
        }
      }
    }else if(input=='#'){ // The clear button
      tone(BEEPER,200,100);
      clear_code();
      cursor_pos=0;
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("*******");
    }else{ // A number was provided
      tone(BEEPER,200,100);
      code[cursor_pos]=input;
      lcd.print(input);

      if(cursor_pos<6){
        cursor_pos++;
      }else{
        cursor_pos=0;
        if((code[0]=='7')&&(code[1]=='3')&&(code[2]=='5')&&(code[3]=='5')&&(code[4]=='6')&&(code[5]=='0')&&(code[6]=='8')){ // Valid code
          if(is_planted){
            //DEFUSED
            is_planted=false;
            timer=0;
            beep_timer=0;
            tone(BEEPER,700,1500);
            lcd.clear();
            lcd.setCursor(2,0);
            lcd.print("THE BOMB HAS");
            lcd.setCursor(2,1);
            lcd.print("BEEN DEFUSED!");
            delay(1500);
            digitalWrite(DIODE_R,LOW);
            digitalWrite(DIODE_G,LOW);
            lcd.clear();
            lcd.setCursor(4,0);
            lcd.print("*******");
            lcd.setCursor(4,0);
            clear_code();
          }else{
            //PLANTED
            is_planted=true;
            digitalWrite(DIODE_R,HIGH);
            digitalWrite(DIODE_G,HIGH);
            lcd.clear();
            lcd.setCursor(2,0);
            lcd.print("THE BOMB HAS");
            lcd.setCursor(2,1);
            lcd.print("BEEN PLANTED!");
            delay(1000);
            lcd.clear();
            lcd.setCursor(4,0);
            lcd.print("*******");
            lcd.setCursor(4,0);
            clear_code();
          }
        }else{ // Invalid code
          tone(BEEPER,150,700);
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("INVALID CODE!");
          delay(750);
          lcd.clear();
          lcd.setCursor(4,0);
          lcd.print("*******");
          lcd.setCursor(4,0);
          clear_code();
        }
      }
    }
  }
}
