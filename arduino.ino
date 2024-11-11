#include <ESP8266WiFi.h>
#include <ThingSpeak.h>  
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "abc";        
char pass[] = "12345678";    
unsigned long channelID = YOUR_CHANNEL_ID;  
const char *writeAPIKey = YOUR_API_KEY;  

LiquidCrystal_I2C lcd(0x27, 16, 2);
byte degree_symbol[8] = {0b00111, 0b00101, 0b00111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000};

#define DHTPIN 2      
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

int gas = A0;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);

  dht.begin();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.createChar(1, degree_symbol);
  lcd.setCursor(3, 0);
  lcd.print("Air Quality");
  lcd.setCursor(3, 1);
  lcd.print("Monitoring");
  delay(2000);
  lcd.clear();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(gas);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  lcd.setCursor(0, 0);
  lcd.print("Temperature ");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.setCursor(6, 1);
  lcd.write(1);
  lcd.setCursor(7, 1);
  lcd.print("C");
  delay(4000);
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Humidity ");
  lcd.print(h);
  lcd.print("%");
  delay(4000);
  lcd.clear();

  if (gasValue < 600) {
    lcd.setCursor(0, 0);
    lcd.print("Gas Value: ");
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Fresh Air");
    Serial.println("Fresh Air");
    delay(4000);
    lcd.clear();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Gas Value: ");
    lcd.print(gasValue);
    lcd.setCursor(0, 1);
    lcd.print("Bad Air");
    Serial.println("Bad Air");
    delay(4000);
    lcd.clear();
  }
 
  ThingSpeak.setField(1, t);        
  ThingSpeak.setField(2, h);           
  ThingSpeak.setField(3, gasValue);    
  int httpCode = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data to ThingSpeak. HTTP error code: ");
    Serial.println(httpCode);
  }
  delay(30000);
}