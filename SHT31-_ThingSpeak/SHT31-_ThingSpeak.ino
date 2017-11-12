#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include "AE_SHT31.h"

#define LED 13
#define SDA 4
#define SCL 5


#define PERIOD 30

// WiFi Connection
const char* ssid = "Weeyble01";
const char* password = "akiba2525";

WiFiClient client;

// SHT31のアドレスを設定
AE_SHT31 SHT31 = AE_SHT31(0x45, SDA, SCL);

// Vcc電圧を読み込めるように設定
ADC_MODE(ADC_VCC);

// ThingSpeak設定
const unsigned long myChannelNumber = 363115;
const char * myWriteAPIKey = "S9HFEY0EFHEVZ5V4";

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println("");
  Serial.println("SHT31 + ThingSpeak Test");

  pinMode(LED, OUTPUT);

  WiFi.mode(WIFI_STA);         // 子機モードに設定
  WiFi.begin(ssid, password);  // 親機(Access Point)に接続する

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // 接続が完了するまでLEDを点滅させる
    delay(500);

    digitalWrite(LED, i++ % 2);
    Serial.print(".");
  }
  Serial.println("");

  digitalWrite(LED, LOW);
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // SHT31をソフトリセット
  SHT31.SoftReset();
  // 内蔵ヒーター 0:OFF 1:ON
  SHT31.Heater(0);
}

void loop()
{
  float temperature, humidity,vcc;
  // SHT31から温湿度データを取得
  SHT31.GetTempHum();
  temperature = SHT31.Temperature();
  humidity = SHT31.Humidity();
  // Vcc取得
  vcc = ESP.getVcc() / 1000.;
  
  digitalWrite(LED, HIGH);
  digitalWrite(LED, LOW);

 //センサーデータアップロード開始
  ThingSpeak.begin(client);

 //フィードに値を設定
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, vcc);

 //ThingSpeakに送信
  Serial.println();
  Serial.print("Write fields...");
  Serial.println("");
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  Serial.print("temperature: ");
  Serial.print(temperature);
  Serial.print("DegC,  humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print(vcc);
  Serial.println(" vcc");
  Serial.println("done");

  delay(PERIOD * 1000);
}
