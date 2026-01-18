#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <animation.h>
#include <setting.h> // WIFI ssid,password | API weather_api_key,city

// oled設定
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// アニメーション設定
// WiFi
#define WIFI_ANIMATION_WIDTH 48
#define WIFI_ANIMATION_HEIGHT 48
#define WIFI_ANIMATION_COUNT 28
#define WIFI_ANIMATION_DELAY 100

// Weather
#define WEATHER_ANIMATION_COUNT 30
#define WEATHER_ANIMATION_DELAY 100

// モード
#define HOME 0
#define SCREEN_SAVER 2
#define DETAIL 1
#define SETTING 3

#define MAX_SCREEN_SAVER_MODE 2

uint8_t mode = HOME;
uint8_t screenSaverMode = 0;

// DVD Bounce variables
int dvd_x = 10;
int dvd_y = 10;
int dvd_dx = 3;
int dvd_dy = 3;

// 天気データ
float currentTemp = 0;
float currentMaxTemp = 0;
float currentMinTemp = 0;
int currentHumidity = 0;
int currentPressure = 0;
float currentWindSpeed = 0;
String currentWeather = "--";

// ディスプレイの設定
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int frame = 0;

HTTPClient http;

const int buttons[] = {12, 14, 27, 26};
const int NUM = 4;

int nowBrightness = 3;

// 明るさ変更
void setBrightness(uint8_t value) {
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(value);
}

struct LenWords {
  int x;
  int y;
  int speed;
  int size;
  int c[10];
};

LenWords lenWord[16];

void setLenWords() {
  for (int i = 0; i < 16; i++) {
    lenWord[i].speed = random(2, 3);
    lenWord[i].size = random(5, 10);
    for (int j = 0; j < lenWord[i].size; j++) {
      lenWord[i].c[j] = random(0, 2) ? '1' : '0';
    }
    lenWord[i].x = random(0, 128);
    lenWord[i].y = 64;
  }
}

// 天気APIからデータを取得する
void getWeatherData() {
  String url = "http://api.openweathermap.org/data/2.5/weather?q=";
  url += city;
  url += "&appid=";
  url += weatherAPI;
  url += "&units=metric";

  http.begin(url);
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);

    JsonDocument doc;
    deserializeJson(doc, payload);

    currentTemp = doc["main"]["temp"];
    currentMaxTemp = doc["main"]["temp_max"];
    currentMinTemp = doc["main"]["temp_min"];
    currentHumidity = doc["main"]["humidity"];
    currentPressure = doc["main"]["pressure"];
    currentWindSpeed = doc["wind"]["speed"];
    const char *weather = doc["weather"][0]["main"];
    currentWeather = String(weather);

    Serial.print("気温: ");
    Serial.print(currentTemp);
    Serial.println(" ℃");

    Serial.print("天気: ");
    Serial.println(currentWeather);
  } else {
    Serial.println("HTTPリクエスト失敗");
  }

  http.end();
}

// ボタンを押したら
void pressButton(int command) {
  if (command == HOME) {
    if (mode == HOME) {
      getWeatherData();
    }
  }
  if (command == SCREEN_SAVER) {
    if (mode == SCREEN_SAVER) {
      if (screenSaverMode >= MAX_SCREEN_SAVER_MODE) {
        screenSaverMode = 0;
      } else {
        screenSaverMode++;
      }
    }
  }
  if (command == SETTING) {
    if (mode == SETTING) {
      if (nowBrightness >= 4) {
        nowBrightness = 0;
      } else {
        nowBrightness++;
      }
      setBrightness((nowBrightness + 1) * 51);
      Serial.print((nowBrightness + 1) * 51);
    }
  }
  mode = command;
}

void calculationEndPoint(float angle, int length, int &x, int &y) {
  float rad = (angle - 90) * DEG_TO_RAD;
  x = 64 + cos(rad) * length;
  y = 32 + sin(rad) * length;
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  WiFi.begin(ssid, password);

  // oled displayをセット
  Wire.begin(22, 23);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // WiFiをセット
  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    display.drawBitmap(40, 8, wifiAnimation[frame], WIFI_ANIMATION_WIDTH,
                       WIFI_ANIMATION_HEIGHT, 1);
    display.display();
    frame = (frame + 1) % WIFI_ANIMATION_COUNT;
    delay(WIFI_ANIMATION_DELAY);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Connected!\n");
  display.println("IP adderss");
  display.println(WiFi.localIP());
  display.display();

  // 時刻設定
  configTime(9 * 3600, 0, "pool.ntp.org");

  delay(2000);
  display.clearDisplay();
  display.display();
  HTTPClient http;
  setLenWords();
  getWeatherData();
}

void loop() {
  display.clearDisplay();

  // ボタンを検知
  for (int i = 0; i < NUM; i++) {
    if (digitalRead(buttons[i]) == LOW) {
      pressButton(i);
    }
  }

  // HOMEモードなら
  if (mode == HOME) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      return;
    }

    // 時間表示
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

    // 秒表示 (小さく)
    display.setTextSize(1);
    display.setCursor(90, 15);
    display.printf(":%02d", timeinfo.tm_sec);

    // 天気と気温
    display.setCursor(0, 40);
    display.setTextSize(1);
    display.printf("Weather: %s", currentWeather.c_str());

    display.setCursor(0, 52);
    display.printf("Temp: %.1f C", currentTemp);

    // 天気アイコン表示
    const unsigned char (*icon)[128] = icon_weather_cloudy; // Default
    if (currentWeather == "Clear") {
      icon = icon_weather_sunny;
    } else if (currentWeather == "Clouds") {
      icon = icon_weather_cloudy;
    } else if (currentWeather == "Rain" || currentWeather == "Drizzle") {
      icon = icon_weather_rain;
    } else if (currentWeather == "Thunderstorm") {
      icon = icon_weather_thunderstorm;
    } else if (currentWeather == "Snow") {
      icon = icon_weather_snow;
    }
    display.drawBitmap(96, 32, icon[frame % WEATHER_ANIMATION_COUNT], 32, 32, SSD1306_WHITE);
    frame = (frame + 1) % WEATHER_ANIMATION_COUNT;

    delay(WEATHER_ANIMATION_DELAY); // 更新頻度
  }
  if (mode == SCREEN_SAVER) {
    if (screenSaverMode == 0) {
      dvd_x += dvd_dx;
      dvd_y += dvd_dy;

      if (dvd_x <= 0 || dvd_x >= SCREEN_WIDTH - 36) {
        dvd_dx = -dvd_dx;
        if (dvd_x < 0)
          dvd_x = 0;
        if (dvd_x > SCREEN_WIDTH - 36)
          dvd_x = SCREEN_WIDTH - 36;
      }

      if (dvd_y <= 0 || dvd_y >= SCREEN_HEIGHT - 16) {
        dvd_dy = -dvd_dy;
        if (dvd_y < 0)
          dvd_y = 0;
        if (dvd_y > SCREEN_HEIGHT - 16)
          dvd_y = SCREEN_HEIGHT - 16;
      }

      display.setTextSize(2);
      display.setCursor(dvd_x, dvd_y);
      display.print("DVD");

      delay(100);
    }
    if (screenSaverMode == 1) {
      for (int i = 0; i < 16; i++) {
        display.setTextSize(1);
        if (lenWord[i].y > 64) {
          lenWord[i].y = 0 - (lenWord[i].size * 8 + random(10, 20));
          lenWord[i].x = random(0, 128);
          lenWord[i].speed = random(2, 4);
        };
        for (int j = 0; j < lenWord[i].size; j++) {
          display.setCursor(lenWord[i].x, lenWord[i].y + j * 8);
          display.write(lenWord[i].c[j]);
        };
        lenWord[i].y += lenWord[i].speed;
      };
      delay(50);
    }
    if (screenSaverMode == 2) {
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
        return;
      }
      display.drawCircle(64, 32, 31, SSD1306_WHITE);
      for (int i = 0; i < 12; i++) {
        int sx, sy, ex, ey;
        calculationEndPoint(i * 30, 27, sx, sy);
        calculationEndPoint(i * 30, 31, ex, ey);
        display.drawLine(sx, sy, ex, ey, SSD1306_WHITE);
      }

      int secLineX, secLineY;
      int minLineX, minLineY;
      int hourLineX, hourLineY;

      calculationEndPoint(timeinfo.tm_sec * 6, 25, secLineX, secLineY);
      calculationEndPoint(timeinfo.tm_min * 6, 22, minLineX, minLineY);
      calculationEndPoint((timeinfo.tm_hour % 12) * 30 + timeinfo.tm_min * 0.5,
                          15, hourLineX, hourLineY);

      display.drawLine(64, 32, secLineX, secLineY, SSD1306_WHITE);
      display.drawLine(64, 32, minLineX, minLineY, SSD1306_WHITE);
      display.drawLine(64, 32, hourLineX, hourLineY, SSD1306_WHITE);
      display.fillCircle(64, 32, 2, SSD1306_WHITE);
    }
  }
  if (mode == DETAIL) {
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("WEATHER DETAIL\n");

    display.printf("Max:%.1fC Min:%.1fC\n", currentMaxTemp, currentMinTemp);
    display.printf("Hum:%d%% Pre:%dhPa\n", currentHumidity, currentPressure);
    display.printf("Wind:%.1fm/s\n", currentWindSpeed);

    delay(200);
  }
  if (mode == SETTING) {
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("SETTING INFO\n");

    display.setTextSize(1);

    display.println("SSID: ");
    display.println(ssid);

    display.print("City: ");
    display.println(city);

    display.print("IP: ");
    display.println(WiFi.localIP());

    display.print("LIGHT: ");
    display.print(nowBrightness + 1);
    display.println("/5");
    delay(200);
  }
  display.display();
}