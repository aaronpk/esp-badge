#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

// https://github.com/lewisxhe/GxEPD
#include <GxEPD.h>
#include <GxGDE0213B72B/GxGDE0213B72B.h> // 2.13" b/w
#include <Fonts/FreeMonoBold12pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>





// Define the URL to pull the image data from
String imageURL = "";

// Define your WiFi
char wifiSSID[] = "";
char wifiPassword[] = "";

// Refresh the image every 5 minutes
#define TIME_TO_SLEEP 60 * 5




// These pins depend on the specific TTGO board you have
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17
#define ELINK_SS 5
const int buttonPin = 39;
const int ledPin = 19;

WiFiMulti WiFiMulti;

GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

#define uS_TO_S_FACTOR 1000000

void wifiStart()
{
  WiFiMulti.addAP(wifiSSID, wifiPassword);

  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }

  Serial.println("");
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void displayImage() {
  digitalWrite(ledPin, HIGH);
  delay(100);

  HTTPClient http;
  http.begin(imageURL);
  int httpCode = http.GET();
  String imageData;
  if(httpCode > 0) {
    imageData = http.getString();
    Serial.print("Downloaded image: ");
    Serial.print(imageData.length());
    Serial.println(" bytes");
  } else {
    Serial.print("[HTTP] GET... failed, error:");
    Serial.println(http.errorToString(httpCode).c_str());
    return;
  }

  int i;
  int x = 0;
  int y = 0;

  Serial.println("Attempting to display image");
  delay(500);

  // display.init(115200);
  display.init();

  for(int c = 0; c < imageData.length(); c++) {
    char ch = imageData.charAt(c);
    for(i = 7; i >= 0; i--) {
      int bit = bitRead(ch, i);
      if(bit == 1) {
        display.drawPixel(x, y, GxEPD_WHITE);
      } else {
        display.drawPixel(x, y, GxEPD_BLACK);
      }
      x++;
      if(x == 122) {
        y++;
        x = 0;
      }
    }
  }

  display.update();

  digitalWrite(ledPin, LOW);
}

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  delay(100);

  wifiStart();

  delay(500);

  if(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
  {
    // Woke up from sleep
    wifiStart();
    displayImage();
  }
  else
  {
    // First launch
    wifiStart();
    displayImage();
  }

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("Going to sleep now");

  esp_deep_sleep_start();
}

void loop() {
  // This isn't run because the device sleeps after the setup() function

  /*
  int buttonState = 0;
  buttonState = digitalRead(buttonPin);
  if(buttonState == LOW) {
    Serial.println("Button PRESSED");
    displayImage();
  } else {
    Serial.println("Button not pressed");
  }
  */

}
