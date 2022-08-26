// v102

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


#define wifi_ssid "xxx"
#define wifi_password  "xxx"

#define mqtt_server "xxx"
#define mqtt_user "xxx"
#define mqtt_password "xxx"
#define mqtt_topic "xxx"

#define INPUT_PIN 5  // Define Pin GPIO5 (=D1) as input pin


WiFiClient espClient;
PubSubClient client(espClient);


const unsigned long INTERVAL = 1000L*60* 1; // 1 Minuten
unsigned long lastRun = 0; 



unsigned long zaehler = 0;




void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(9600);
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
 
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), handleInterruptD05, FALLING);

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, HIGH); // LED ausschalten
  
}



ICACHE_RAM_ATTR void handleInterruptD05() {
  zaehler = zaehler + 1;
}


void sendValues() {
  long zaehler_send = zaehler;
  zaehler = 0;
  
  digitalWrite(LED_BUILTIN, LOW);
  client.publish(mqtt_topic, String(zaehler_send).c_str(), true);
  delay(100); 
  digitalWrite(LED_BUILTIN, HIGH);
    
}



// the loop function runs over and over again forever
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  
  if ( millis() - lastRun >= INTERVAL ) {
    sendValues();
    lastRun += INTERVAL;
  }

  
}
