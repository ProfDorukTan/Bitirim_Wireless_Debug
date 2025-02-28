#include <WiFi.h>
#include <WiFiUdp.h>

// Wi-Fi Credentials
const char* ssid = "elma";
const char* password = "elmaarmut";

// UDP Settings
WiFiUDP udp;
const char* laptop_ip = "192.168.1.128";  // Replace with your laptop's IP
const int udp_port = 5005;  // Port to send data

// UART Settings
#define RXD2 16  // ESP32 UART RX pin
#define TXD2 17  // ESP32 UART TX pin
#define BAUD_RATE 1900000    // Updated baud rate

// LED Pin
#define LED_PIN 22

void setup() {
    Serial.begin(115200);  // Debugging
    Serial2.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
    pinMode(LED_PIN, OUTPUT);  // Set LED pin as output
    digitalWrite(LED_PIN, HIGH);  // Turn on LED

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to Wi-Fi");

    digitalWrite(LED_PIN, LOW);  // Turn off LED

}

void loop() {
    static char buffer[1024];
    static int index = 0;
    
    while (Serial2.available()) {
        char c = Serial2.read();
        
        if (c == '\r') {
            continue; // Ignore carriage return
        }
        
        if (c == '\n') {  // End of line
            buffer[index] = '\0';  // Null-terminate the string
            
            udp.beginPacket(laptop_ip, udp_port);
            udp.write((uint8_t*)buffer, index);
            udp.endPacket();
            
            index = 0;  // Reset buffer index
        } else if (index < sizeof(buffer) - 1) {
            buffer[index++] = c;
        } else {
            Serial.println("Warning: Buffer overflow, discarding character.");
            index = 0;  // Reset buffer to prevent corruption
        }
    }
}
