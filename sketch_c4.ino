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
char code[7]={'#','#','#','#','#','#','#'}; // # = empty
bool is_planted=false;
int timer=0;
int beep_timer=0;

void clear_code(){ // Function that clears the bomb code
  for(i=0;i<7;i++){
    code[i]='#';
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
  lcd.blink();
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

  lcd.setCursor(cursor_pos,0);
  input = keypad.getKey();
  if (input){ // TODO: Make the typing work like in cs (the input field is filled with "*" and numbers appear from the right)
    if((input=='A')||(input=='B')||(input=='C')||(input=='D')){} // Empty buttons
    else if(input=='*'){ // The delete button
      if (cursor_pos!=0){
        cursor_pos--;
        code[cursor_pos]='#';
        lcd.setCursor(0,0);
        lcd.clear();
        for(i=0;i<7;i++){
          if(code[i]!='#'){
            lcd.print(code[i]);
          }
        }
      }
    }else if(input=='#'){ // The clear button
      cursor_pos=0;
      lcd.setCursor(0,0);
      lcd.clear();
      clear_code();
    }else{ // A number was provided
      lcd.print(input);
      code[cursor_pos]=input;

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
            lcd.setCursor(0,0);
            lcd.clear();
            lcd.print("  THE BOMB HAS");
            lcd.setCursor(0,1);
            lcd.print("  BEEN DEFUSED!");
            delay(1500);
            lcd.clear();
            clear_code();
            digitalWrite(DIODE_R,LOW);
            digitalWrite(DIODE_G,LOW);
          }else{
            //PLANTED
            is_planted=true;
            digitalWrite(DIODE_R,HIGH);
            digitalWrite(DIODE_G,HIGH);
            lcd.setCursor(0,0);
            lcd.clear();
            lcd.print("  THE BOMB HAS");
            lcd.setCursor(0,1);
            lcd.print("  BEEN PLANTED!");
            delay(1000);
            lcd.clear();
            clear_code();
          }
        }else{ // Invalid code
          lcd.setCursor(0,0);
          lcd.clear();
          lcd.print(" INVALID CODE!");
          delay(750);
          lcd.clear();
          clear_code();
        }
      }
    }
  }
}
