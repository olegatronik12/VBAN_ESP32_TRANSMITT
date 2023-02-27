#include <WiFi.h>
#include <WiFiUdp.h>
#include <Audio.h>

// Задаем параметры подключения к WiFi
const char* ssid = "ваш_SSID";
const char* password = "ваш_пароль";
const IPAddress remoteIP(192, 168, 0, 100); // IP-адрес получателя
const uint16_t remotePort = 6980; // Порт получателя

// Создаем объекты для работы с WiFi и UDP
WiFiUDP udp;
WiFiClient client;

void setup() {
  // Инициализируем микрофон
  Audio.begin();
  Audio.setPinout(I2S_PINS);

  // Подключаемся к WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Подключение к WiFi...");
    delay(1000);
  }
  Serial.println("Подключение к WiFi установлено");

  // Настраиваем соединение UDP
  udp.begin(9000);
}

void loop() {
  // Считываем звук с микрофона
  int16_t buffer[512];
  int bytesRead = Audio.read(buffer, 512);

  // Отправляем звук по протоколу VBAN
  if (bytesRead > 0) {
    udp.beginPacket(remoteIP, remotePort);
    udp.write("VBAN");
    udp.write(0); // StreamID = 0
    udp.write(0); // PacketCounter = 0
    udp.write((byte*)buffer, bytesRead * 2); // Записываем данные звука в пакет UDP
    udp.endPacket();
  }

  delay(10); // Небольшая задержка для стабильности работы
}
