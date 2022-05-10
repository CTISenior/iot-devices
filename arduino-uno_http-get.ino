#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define RXPIN 10
#define TXPIN 11

SoftwareSerial esp(RXPIN, TXPIN);
DHT dht(DHTPIN, DHTTYPE);

String IP = "176.235.202.77";
String WIFI_SSID = "Linksys_ALKAN";
String WIFI_PASSWORD = "159875321a";
String SN = "Arduino-02";


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

  String field = "GET /api/gateway/devices";
  field += "?serialNumber=";
  field += SN;
  field += "&temp=";
  field += temp;
  field += "&hum=";
  field += hum;
  field += "\r\n\r\n"; 

  esp.print("AT+CIPSEND=");
  esp.println(field.length()+2);
  //delay(2000);
  if(esp.find(">")){
    long now = millis();
    if (now - lastMsg > 10000) {
      lastMsg = now;
      Serial.println(lastMsg);
      esp.print(field);
      Serial.println(field);
      Serial.println("Telemetry sent");
    }
  }
  //Serial.println("Connector closed.");
  //esp.println("AT+CIPCLOSE");
  //delay(500);
}
