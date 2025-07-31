#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Pin Definition ---
#define BUTTON_PIN 14

// --- Morse Code Timings (in milliseconds) ---
#define DOT_LENGTH 250
#define DASH_LENGTH (DOT_LENGTH * 3)
#define LETTER_GAP (DOT_LENGTH * 3)
#define WORD_GAP (DOT_LENGTH * 7)

// --- LCD Setup ---
// Set the LCD address to 0x27 or 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Morse Code Dictionary ---
const char *morse_code[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};
const char morse_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

String current_sequence = "";
String translated_text = "";
unsigned long press_time = 0;
unsigned long release_time = 0;
unsigned long last_input_time = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);

    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Ready!");
}

void loop()
{
    // --- Check for Button Press ---
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
        if (press_time == 0)
        { // First detection of press
            press_time = millis();
            if (press_time - last_input_time > LETTER_GAP && current_sequence != "")
            {
                translateSequence();
            }
        }
    }
    else
    { // Button is released
        if (press_time != 0)
        {
            release_time = millis();
            unsigned long press_duration = release_time - press_time;
            if (press_duration < DOT_LENGTH)
            {
                current_sequence += ".";
            }
            else
            {
                current_sequence += "-";
            }
            last_input_time = millis();
            press_time = 0; // Reset for next press
            updateDisplay();
        }
    }

    // --- Check for end of letter or word ---
    if (millis() - last_input_time > WORD_GAP && current_sequence != "")
    {
        translateSequence();
        translated_text += " ";
        updateDisplay();
    }
    else if (millis() - last_input_time > LETTER_GAP && current_sequence != "" && press_time == 0)
    {
        translateSequence();
    }
}

void translateSequence()
{
    bool found = false;
    for (int i = 0; i < 26; i++)
    {
        if (current_sequence.equals(morse_code[i]))
        {
            translated_text += morse_chars[i];
            found = true;
            break;
        }
    }
    if (!found)
        translated_text += "?"; // Add a placeholder for unknown sequences

    current_sequence = "";
    updateDisplay();
}

void updateDisplay()
{
    lcd.clear();
    // Display the translated text on the top row
    lcd.setCursor(0, 0);
    lcd.print(translated_text);

    // Display the current live input on the bottom row
    lcd.setCursor(0, 1);
    lcd.print("In: " + current_sequence);
}