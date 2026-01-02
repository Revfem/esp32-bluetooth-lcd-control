// ======================= Libraries =======================
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>
#include <Ticker.h>

// ======================= Global Objects =======================

BluetoothSerial ESP_BT;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define teamId "A03"

// ======================= Data Structures =======================

struct LCDInfo {
    String type;        
    String interface;   
    String resolution;  
    int id;             
    String teamId;     
};

LCDInfo lcds[] = {
    {"16x2", "I2C", "128x16", 3, "A03"}
};

int numLCDs = sizeof(lcds) / sizeof(lcds[0]);
Ticker scrollTicker;

// ======================= Setup Function =======================

void setup() {
    Serial.begin(9600);

    ESP_BT.begin("ESP32_LCD_Control");
    Serial.println("Bluetooth Device is Ready to Pair");

    lcd.init();
    lcd.backlight();
}

// ======================= Main Loop =======================

void loop() {
    if (ESP_BT.available()) {
        String command = ESP_BT.readStringUntil('\n');
        executeCommand(command);
    }
}

// ======================= Command Handler =======================

// Parses and executes incoming JSON commands
void executeCommand(String command) {
    StaticJsonDocument<100> jsonDoc;

    DeserializationError error = deserializeJson(jsonDoc, command);
    if (error) {
        Serial.println("Failed to parse JSON");
        return;
    }

    if (jsonDoc.containsKey("action")) {

        String action = jsonDoc["action"].as<String>();
        if (action == "getLCDs") {
            sendLCDList(teamId);
        }

        else if (action == "setText") {
            int lcdId = jsonDoc["id"].as<int>();
            JsonArray textArray = jsonDoc["text"].as<JsonArray>();
            set_text(lcdId, textArray, teamId);
        }

        else if (action == "setIcons") {
            int lcdId = jsonDoc["id"].as<int>();
            JsonArray iconsArray = jsonDoc["icons"].as<JsonArray>();
            set_icons(lcdId, iconsArray, teamId);
        }

        else if (action == "scroll") {
            int lcdId = jsonDoc["id"].as<int>();
            String direction = jsonDoc["direction"].as<String>();
            control_scroll(lcdId, direction, teamId);
        }
    }
}

// ======================= LCD Icon Handling =======================

// Creates and displays custom icons on the LCD
void set_icons(int lcdId, JsonArray iconsArray, String teamId) {
    lcd.clear();

    int lcdIndex = -1;
    for (int i = 0; i < numLCDs; i++) {
        if (lcds[i].id == lcdId) {
            lcdIndex = i;
            break;
        }
    }

    if (lcdIndex != -1) {

        for (int i = 0; i < iconsArray.size(); i++) {
            JsonObject icon = iconsArray[i].as<JsonObject>();
            JsonArray data = icon["data"].as<JsonArray>();

            byte customChar[8];
            for (int j = 0; j < 8; j++) {
                customChar[j] = data[j].as<byte>();
            }

            lcd.createChar(i, customChar);

            lcd.setCursor(i * 2, 0);
            lcd.write(byte(i));
        }

        StaticJsonDocument<200> responseDoc;
        responseDoc["id"] = lcdId;
        responseDoc["number_icons"] = iconsArray.size();
        responseDoc["teamId"] = teamId;

        String response;
        serializeJson(responseDoc, response);
        ESP_BT.println(response);
    } else {
        Serial.println("LCD not found");
    }
}

// ======================= LCD Text Handling =======================

// Displays text received via Bluetooth on the LCD
void set_text(int lcdId, JsonArray textArray, String teamId) {

    int lcdIndex = -1;
    for (int i = 0; i < numLCDs; i++) {
        if (lcds[i].id == lcdId) {
            lcdIndex = i;
            break;
        }
    }

    if (lcdIndex != -1) {
        lcd.clear();

        for (int row = 0; row < textArray.size(); row++) {
            String text = textArray[row].as<String>();
            lcd.setCursor(0, row);
            lcd.print(text);
        }

        StaticJsonDocument<200> responseDoc;
        responseDoc["id"] = lcdId;
        responseDoc["text"] = textArray;
        responseDoc["teamId"] = teamId;

        String response;
        serializeJson(responseDoc, response);
        ESP_BT.println(response);
    } else {
        Serial.println("LCD not found");
    }
}

// ======================= Scrolling Control =======================

// Enables or disables LCD scrolling
void control_scroll(int lcdId, String direction, String teamId) {

    if (direction == "Left") {
        scrollTicker.attach_ms(500, scrollLeft);
    } else if (direction == "Right") {
        scrollTicker.attach_ms(500, scrollRight);
    } else if (direction == "Off") {
        scrollTicker.detach();
    }

    StaticJsonDocument<200> responseDoc;
    responseDoc["id"] = lcdId;
    responseDoc["scrolling"] = direction;
    responseDoc["teamId"] = teamId;

    String response;
    serializeJson(responseDoc, response);
    ESP_BT.println(response);
}

void scrollLeft() {
    lcd.scrollDisplayLeft();
}

void scrollRight() {
    lcd.scrollDisplayRight();
}

// ======================= LCD Discovery =======================

// Sends information about available LCDs via Bluetooth
void sendLCDList(String teamId) {

    StaticJsonDocument<200> jsonDoc;

    for (int i = 0; i < numLCDs; i++) {
        jsonDoc["type"] = lcds[i].type;
        jsonDoc["interface"] = lcds[i].interface;
        jsonDoc["resolution"] = lcds[i].resolution;
        jsonDoc["id"] = lcds[i].id;
        jsonDoc["teamId"] = teamId;
    }

    String response;
    serializeJson(jsonDoc, response);
    ESP_BT.println(response);
}
