#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define RXPIN 10
#define TXPIN 11

SoftwareSerial esp(RXPIN, TXPIN);
DHT dht(DHTPIN, DHTTYPE);

String IP = "";
String WIFI_SSID = "";
String WIFI_PASSWORD = "";
String SN = "";


float temp, hum;
long lastMsg = 0;

void setup() {

  Serial.begin(9600);
  Serial.println("Started");
  esp.begin(115200);
  esp.println("AT");
  Serial.println("AT sent");
  while(!esp.find("OK")){
    esp.println("AT");
    Serial.println("ESP8266 module not found!");
  }
  Serial.println("OK command received");
  esp.println("AT+CWMODE=1");
  while(!esp.find("OK")){
    esp.println("AT+CWMODE=1");
    Serial.println("Configuring...");
  }
  Serial.println("Client configured");
  Serial.println("Connecting to WiFİ...");
  esp.println("AT+CWJAP=\""+WIFI_SSID+"\",\""+WIFI_PASSWORD+"\"");
  while(!esp.find("OK"));
  Serial.println("Connected to WiFi");
  
  dht.begin();
  delay(1000);
}
void loop() {

  esp.println("AT+CIPSTART=\"TCP\",\""+IP+"\",5000");
  if(esp.find("Error")){
    Serial.println("AT+CIPSTART Error");
  }

  temp = (float)dht.readTemperature();
  hum = (float)dht.readHumidity();

  // Check failure.
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float hic = dht.computeHeatIndex(temp, hum, false);

  String req = "GET /api/gateway/devices";
  req += "?serialNumber=";
  req += SN;
  req += "&temp=";
  req += temp;
  req += "&hum=";
  req += hum;
  req += "\r\n\r\n"; 

  esp.print("AT+CIPSEND=");
  esp.println(req.length()+2);
  //delay(2000);
  if(esp.find(">")){
    long now = millis();
    if (now - lastMsg > 10000) {
      lastMsg = now;
      Serial.println(lastMsg);
      esp.print(req);
      Serial.println(req);
      Serial.println("Telemetry sent");
    }
  }
  //Serial.println("Connector closed.");
  //esp.println("AT+CIPCLOSE");
  //delay(500);
}
