#include <DHT.h>          //Declarate DHT Library
#include <ESP8266WiFi.h>  //Declarate ESP8266 Board Library

#include <BlynkSimpleEsp8266.h>
#define DHTPIN D4      //Pin of DHT 22
#define DHTTYPE DHT11  //Type of DHT

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLumb3_xht"
#define BLYNK_DEVICE_NAME "weather"
#define BLYNK_AUTH_TOKEN "ftTO1kUr0FDheEt7BimYKyc8xwFm7jjQ"

DHT dht(DHTPIN, DHTTYPE);  //Declarate dht

//Declarate val variable
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "OnePlus 8";
char pass[] = "suhasraj";  //Put your Password of your connection

int buzzer = D3;
int smokeA0 = A0;

int state = HIGH;
int buttonpin = D7;

int sensorThres = 30;
int transistor = D6;

int red_light_pin = D0;
int green_light_pin = D1;
int blue_light_pin = D2;

int flame_pin = D5;

unsigned long previousMillis = 0;
int mq2;

int flamval;

int Alert_Val = 0;

float t;
float h;
int i = 0;

int flag = 0;

BlynkTimer timer;

BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(D0, value);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value) {
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void DHT_11() {
  h = dht.readHumidity();     //Read humidity and put it in h variable
  t = dht.readTemperature();  //Read temperature and put it in t variable

  Blynk.virtualWrite(V5, t);  //Send t value to blynk in V5 virtual pin
  Blynk.virtualWrite(V6, h);  //Send h value to blynk in V6 virtual pin
                              //Send q value to blynk in V0 virtual pin
}
void MQ2() {
  int analogSensor = analogRead(smokeA0);
  int readbutton = digitalRead(buttonpin);
  Serial.print("Value:");
  Serial.println(readbutton);
  if (readbutton != HIGH) {
    state = !state;
    Serial.print("State:");
    Serial.println(state);
  }
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  mq2 = map(analogSensor, 0, 1024, 0, 100);
  Blynk.virtualWrite(V7, mq2);
  // Checks if it has reached the threshold value

  delay(100);
}

void flame() {
  flamval = digitalRead(flame_pin);
  if (flamval != 0) {
    Blynk.virtualWrite(V8, LOW);
  } else {
    Blynk.virtualWrite(V8, HIGH);
  }
}


void alert_situation() {
  unsigned long currentMillis = millis();
  unsigned long previousMillis1;
  unsigned long previousMillis2;
  unsigned long previousMillis3;
  unsigned long previousMillis4;
  unsigned long previousMillis5;
  unsigned long previousMillis6;
  if (flamval != 1) {
    digitalWrite(transistor, HIGH);

    if (state != LOW) {
      tone(buzzer, 1000, 200);
      if (currentMillis - previousMillis1 >= 1000) {
        Blynk.notify("Alert: Fire is detected take immediate action!!");
      }
      previousMillis1 = currentMillis;
    }
    RGB_color(0, 1024, 1024);  // Red
  }

  else if (mq2 > sensorThres) {
    digitalWrite(transistor, HIGH);
    if (state != LOW) {
      tone(buzzer, 1000, 200);
      if (currentMillis - previousMillis2 >= 1000) {
        Blynk.notify("Alert: CO2 Level is more. Please move out!!");
        previousMillis2 = currentMillis;
      }
      RGB_color(0, 1024, 1024);  // Red
    }
  } else if (t > 40 || t < 10) {
    digitalWrite(transistor, HIGH);
    if (state != LOW) {
      tone(buzzer, 1000, 200);
      if (t > 40) {
        if (currentMillis - previousMillis3 >= 1000) {
          Blynk.notify("Alert: Temperature is too HOT.!!");
          previousMillis3 = currentMillis;
        }
      } else {
        if (currentMillis - previousMillis4 >= 1000) {
          Blynk.notify("Alert: Temperature is too COLD.!!");
          previousMillis4 = currentMillis;
        }
      }
      RGB_color(0, 1024, 1024);  // Red
    }
  }

  else if (h > 80 || h < 30) {
    digitalWrite(transistor, HIGH);
    if (state != LOW) {
      tone(buzzer, 1000, 200);
      if (h > 80) {
        if (currentMillis - previousMillis5 >= 1000) {
          Blynk.notify("Alert: Humidity is too HIGH.!!");
          previousMillis5 = currentMillis;
        }
      } else {
        if (currentMillis - previousMillis6 >= 1000) {
          Blynk.notify("Alert: Temperature is too LOW.!!");
          previousMillis6 = currentMillis;
        }
        RGB_color(0, 1024, 1024);  // Red
      }
    }
  }

  else {
    digitalWrite(transistor, LOW);
    noTone(buzzer);
    if (currentMillis - previousMillis >= 1000) {
      i += 1;
      switch (i) {
        case 1:
          RGB_color(0, 0, 1024);  // Yellow
          break;
        case 2:
          RGB_color(0, 1024, 1024);  // Red
          break;
        case 3:
          RGB_color(0, 1024, 0);  // Magenta
          break;
        case 4:
          RGB_color(1024, 0, 1024);  // Green
          break;
        case 5:
          RGB_color(1024, 0, 0);  // Cyan
          break;
        case 6:
          RGB_color(1024, 1024, 0);  //  Blue
          break;
        case 7:
          RGB_color(0, 0, 0);  // White / Off
          break;
        default:
          i = 0;
      }
      previousMillis = currentMillis;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud",80);  //Connect the blynk to esp8266 board
  dht.begin();
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  pinMode(transistor, OUTPUT);
  pinMode(buttonpin, INPUT);
  pinMode(flame_pin, INPUT);
}

void loop() {
  Blynk.run();  //Run the Blynk
  MQ2();
  DHT_11();
  flame();
  alert_situation();
}