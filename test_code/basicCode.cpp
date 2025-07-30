// --- Pin Definition ---
#define BUTTON_PIN 14

// --- Morse Code Timings (in milliseconds) ---
#define DOT_LENGTH 250
#define DASH_LENGTH (DOT_LENGTH * 3)
#define LETTER_GAP (DOT_LENGTH * 3)
#define WORD_GAP (DOT_LENGTH * 7)

// --- Morse Code Dictionary ---
const char *morse_code[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};
const char morse_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

String current_sequence = "";
String translated_text = "";
unsigned long press_time = 0;
unsigned long release_time = 0;
unsigned long last_input_time = 0;

void setup()
{
    // Start serial communication at a 115200 baud rate
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT);

    Serial.println("Morse Code Translator Ready!");
    Serial.println("----------------------------");
    Serial.println("Start tapping to begin...");
}

void loop()
{
    // --- Check for Button Press ---
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
        if (press_time == 0)
        { // First detection of press
            press_time = millis();
            // Check for gap between letters before this new press
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

            updateSerialMonitor(); // Update the display line
        }
    }

    // --- Check for end of letter or word based on inactivity ---
    if (millis() - last_input_time > WORD_GAP && current_sequence != "")
    {
        translateSequence();
        translated_text += " ";
        Serial.print(" "); // Add a space to the final output
        updateSerialMonitor();
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
            Serial.print(morse_chars[i]); // Print the final translated character
            found = true;
            break;
        }
    }
    if (!found)
    {
        // Optional: indicate an unrecognized sequence
        Serial.print("?");
        translated_text += "?";
    }

    current_sequence = "";
    updateSerialMonitor();
}

void updateSerialMonitor()
{
    // This function just shows the live input buffer. The final text is printed directly.
    // We can use this to show the user what they are currently typing.
    // Note: For a cleaner log, you could remove this function and its calls.
    // For an interactive feel, we can do a trick with carriage returns.
    Serial.print("\rCurrent Input: " + current_sequence + "    "); // The extra spaces clear previous longer sequences
}