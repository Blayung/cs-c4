#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// SETTINGS
    // Pins
    #define BUZZER A0
    #define DIODE_R A1
    #define DIODE_G A2
    #define DIODE_B A3
    #define KEYPAD_ROW_PINS {7, 6, 5, 4}
    #define KEYPAD_COL_PINS {11, 10, 9, 8}
    // The LCD Address
    #define LCD_ADDRESS 0x3F
    // Durations, delays and sound frequencies (all values are in either hertz or milliseconds)
    #define EXPLOSION_DIODE_DURATION 3000
    #define EXPLOSION_SOUND_FREQUENCY 100
    #define EXPLOSION_SOUND_DURATION 2750
    #define BEEP_DIODE_DURATION 100
    #define BEEP_SOUND_FREQUENCY 440
    #define BEEP_SOUND_DURATION 250
    #define BOMB_PLANTED_DURATION 1000
    #define BOMB_DEFUSED_DURATION 1500
    #define BOMB_DEFUSED_SOUND_FREQUENCY 700
    #define BOMB_DEFUSED_SOUND_DURATION 1500
    #define TYPING_SOUND_FREQUENCY 200
    #define TYPING_SOUND_DURATION 100
    #define INVALID_CODE_DURATION 750
    #define INVALID_CODE_SOUND_FREQUENCY 150
    #define INVALID_CODE_SOUND_DURATION 700
    // The bomb code
    #define CODE_1 '7'
    #define CODE_2 '3'
    #define CODE_3 '5'
    #define CODE_4 '5'
    #define CODE_5 '6'
    #define CODE_6 '0'
    #define CODE_7 '8'
    // Other
    #define EXPLOSION_TIME 40000 // Milliseconds
    #define BEEPING_SPEED 2000

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(LCD_ADDRESS, 16, 2);

char keypadHexaKeys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
byte keypadRowPins[4] = KEYPAD_ROW_PINS;
byte keypadColPins[4] = KEYPAD_COL_PINS;
Keypad keypad = Keypad(makeKeymap(keypadHexaKeys), keypadRowPins, keypadColPins, 4, 4);

uint8_t cursorPos = 0;
char code[7] = {'*', '*', '*', '*', '*', '*', '*'}; // * = empty
bool isPlanted = false;
unsigned long scheduledExplosion;
unsigned long previousBeep;

void clearCode() {
    for (int i = 0; i < 7; i++) {
        code[i] = '*';
    }
}

void setup() {
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);
    pinMode(DIODE_R, OUTPUT);
    digitalWrite(DIODE_R, LOW);
    pinMode(DIODE_G, OUTPUT);
    digitalWrite(DIODE_G, LOW);
    pinMode(DIODE_B, OUTPUT);
    digitalWrite(DIODE_B, HIGH);

    lcd.init();
    lcd.backlight();
    lcd.cursor();
    lcd.setCursor(4, 0);
    lcd.print("*******");
    lcd.setCursor(4, 0);
}

void loop() {
    if (isPlanted) {
        unsigned long currentMillis = millis();

        if (currentMillis >= scheduledExplosion) {
            isPlanted = false;
            tone(BUZZER, EXPLOSION_SOUND_FREQUENCY, EXPLOSION_SOUND_DURATION);
            digitalWrite(DIODE_R, LOW);
            digitalWrite(DIODE_G, LOW);
            digitalWrite(DIODE_B, LOW);
            delay(EXPLOSION_DIODE_DURATION);
            digitalWrite(DIODE_B, HIGH);
        } else if (currentMillis - previousBeep >= max((scheduledExplosion - currentMillis) * BEEPING_SPEED / EXPLOSION_TIME, max(BEEP_SOUND_DURATION, BEEP_DIODE_DURATION) + 10)) {
            previousBeep = currentMillis;
            tone(BUZZER, BEEP_SOUND_FREQUENCY, BEEP_SOUND_DURATION);
            digitalWrite(DIODE_R, LOW);
            delay(BEEP_DIODE_DURATION);
            digitalWrite(DIODE_R, HIGH);
        }
    }

    lcd.setCursor(cursorPos + 4, 0);
    char input = keypad.getKey();
    if (input) {
        if (input == '*') {
            tone(BUZZER, TYPING_SOUND_FREQUENCY, TYPING_SOUND_DURATION);
            if (cursorPos != 0) {
                cursorPos--;
                code[cursorPos] = '*';
                lcd.clear();
                lcd.setCursor(4, 0);
                for (int i = 0; i < 7; i++) {
                    lcd.print(code[i]);
                }
            }
        } else if (input == '#') {
            tone(BUZZER, TYPING_SOUND_FREQUENCY, TYPING_SOUND_DURATION);
            clearCode();
            cursorPos = 0;
            lcd.clear();
            lcd.setCursor(4, 0);
            lcd.print("*******");
        } else {
            tone(BUZZER, TYPING_SOUND_FREQUENCY, TYPING_SOUND_DURATION);
            code[cursorPos] = input;
            lcd.print(input);

            if (cursorPos < 6) {
                cursorPos++;
            } else {
                cursorPos = 0;
                if (code[0] == CODE_1 && code[1] == CODE_2 && code[2] == CODE_3 && code[3] == CODE_4 && code[4] == CODE_5 && code[5] == CODE_6 && code[6] == CODE_7) {
                    if (isPlanted) {
                        isPlanted = false;
                        clearCode();
                        tone(BUZZER, BOMB_DEFUSED_SOUND_FREQUENCY, BOMB_DEFUSED_SOUND_DURATION);
                        lcd.clear();
                        lcd.setCursor(2, 0);
                        lcd.print("THE BOMB HAS");
                        lcd.setCursor(2, 1);
                        lcd.print("BEEN DEFUSED!");
                        delay(BOMB_DEFUSED_DURATION);
                        digitalWrite(DIODE_R, LOW);
                        digitalWrite(DIODE_G, LOW);
                        lcd.clear();
                        lcd.setCursor(4, 0);
                        lcd.print("*******");
                        lcd.setCursor(4, 0);
                    } else {
                        isPlanted = true;
                        clearCode();
                        digitalWrite(DIODE_R, HIGH);
                        digitalWrite(DIODE_G, HIGH);
                        lcd.clear();
                        lcd.setCursor(2, 0);
                        lcd.print("THE BOMB HAS");
                        lcd.setCursor(2, 1);
                        lcd.print("BEEN PLANTED!");
                        delay(BOMB_PLANTED_DURATION);
                        unsigned long currentMillis = millis();
                        previousBeep = currentMillis;
                        scheduledExplosion = currentMillis + EXPLOSION_TIME;
                        lcd.clear();
                        lcd.setCursor(4, 0);
                        lcd.print("*******");
                        lcd.setCursor(4, 0);
                    }
                } else {
                    clearCode();
                    tone(BUZZER, INVALID_CODE_SOUND_FREQUENCY, INVALID_CODE_SOUND_DURATION);
                    lcd.clear();
                    lcd.setCursor(1, 0);
                    lcd.print("INVALID CODE!");
                    delay(INVALID_CODE_DURATION);
                    lcd.clear();
                    lcd.setCursor(4, 0);
                    lcd.print("*******");
                    lcd.setCursor(4, 0);
                }
            }
        }
    }
}
