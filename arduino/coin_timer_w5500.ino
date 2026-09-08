/*
 * Coin Timer for PS2 Open Loader via Arduino + W5500 Ethernet
 * 
 * Hardware:
 * - Arduino (Nano, Mega, or compatible)
 * - W5500 Ethernet Module (SPI)
 * - Coin acceptor/impulse detector on analog pin A0
 * - PS2 with OPL running and network enabled
 * 
 * Protocol:
 * - UDP packets to port 9999 on PS2
 * - 2-byte payload: number of minutes to add (little-endian 16-bit)
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// ============== CONFIGURATION ==============

// Ethernet configuration
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ps2_ip(192, 168, 1, 100);        // Change to your PS2's IP
unsigned int ps2_port = 9999;              // OPL coin timer UDP port
unsigned int local_port = 8888;            // Local UDP port

// Coin sensor configuration
const int COIN_PIN = A0;                   // Analog input from coin acceptor
const int COIN_THRESHOLD = 500;            // Threshold for coin detection (0-1023)
const int DEBOUNCE_TIME_MS = 200;          // Debounce delay in milliseconds
const int MINUTES_PER_COIN = 5;            // Minutes to add per coin

// ============== GLOBAL VARIABLES ==============

EthernetUDP Udp;
long last_coin_time = 0;

// ============== SETUP ==============

void setup() {
  // Initialize serial for debugging
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("\\n===== Coin Timer Arduino + W5500 =====");
  Serial.println("Initializing Ethernet...");
  
  // Initialize Ethernet
  Ethernet.init(10);  // CS pin for W5500 (change if needed)
  
  if (Ethernet.begin(mac)) {
    Serial.print("Ethernet initialized. Local IP: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("ERROR: Failed to initialize Ethernet");
    while (1) {
      delay(1000);
      Serial.println("Retrying...");
    }
  }
  
  // Start UDP
  Udp.begin(local_port);
  
  // Configure coin sensor pin
  pinMode(COIN_PIN, INPUT);
  
  Serial.print("Waiting for PS2 at ");
  Serial.print(ps2_ip);
  Serial.println("...");
  Serial.println("System ready. Waiting for coins...\\n");
}

// ============== MAIN LOOP ==============

void loop() {
  // Check coin sensor
  int coin_value = analogRead(COIN_PIN);
  
  if (coin_value > COIN_THRESHOLD) {
    // Check debounce time
    long current_time = millis();
    if (current_time - last_coin_time > DEBOUNCE_TIME_MS) {
      
      // Double-check coin detection
      delay(50);
      if (analogRead(COIN_PIN) > COIN_THRESHOLD) {
        Serial.println("\\n*** COIN DETECTED ***");
        sendCoinToPS2(MINUTES_PER_COIN);
        last_coin_time = current_time;
        
        // Wait for coin release
        while (analogRead(COIN_PIN) > COIN_THRESHOLD) {
          delay(10);
        }
        delay(100);
      }
    }
  }
  
  // Small delay to prevent overwhelming the loop
  delay(10);
}

// ============== FUNCTIONS ==============

void sendCoinToPS2(int minutes) {
  // Verify Ethernet connection
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("ERROR: Ethernet cable disconnected!");
    return;
  }
  
  // Create 16-bit payload (little-endian)
  byte packet[2];
  packet[0] = minutes & 0xFF;        // Low byte
  packet[1] = (minutes >> 8) & 0xFF; // High byte
  
  // Send UDP packet
  Udp.beginPacket(ps2_ip, ps2_port);
  Udp.write(packet, 2);
  int result = Udp.endPacket();
  
  if (result > 0) {
    Serial.print("[+] Sent ");
    Serial.print(minutes);
    Serial.print(" minute(s) to PS2 at ");
    Serial.println(ps2_ip);
  } else {
    Serial.println("[-] ERROR: Failed to send UDP packet");
  }
}
