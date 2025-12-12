#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <SoftwareSerial.h>

// ================== CONFIGURATIONS ==================
#define WIFI_SSID       ""     
#define WIFI_PASSWORD   ""        

#define API_KEY         ""
#define DATABASE_URL    ""


SoftwareSerial uartTo8051(D5, D6); 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String lastValue = "";

void setup() {
  Serial.begin(115200);
  uartTo8051.begin(9600);
  
  delay(1000);
  Serial.println("\n\n--- AUTO-RESET SYSTEM STARTING ---");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Auth OK");
  }
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready()) {
    if (Firebase.RTDB.getString(&fbdo, "/Password/Status")) {
      
      String currentValue = fbdo.stringData();

      if (currentValue != "Idle" && currentValue != lastValue) {
        
        Serial.print("Action Received: ");
        Serial.println(currentValue);

      
        uartTo8051.print(currentValue); 
        
        delay(100); 
        if(Firebase.RTDB.setString(&fbdo, "/Password/Status", "Idle")){
            Serial.println("Status reset to Idle");
            lastValue = "Idle"; 
        }
      }
    }
  }
  delay(200);
}
