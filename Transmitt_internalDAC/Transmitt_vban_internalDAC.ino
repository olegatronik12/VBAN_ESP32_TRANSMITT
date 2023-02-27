#include <WiFi.h>
#include <WiFiUdp.h>
#include <driver/adc.h>
#include <driver/dac.h>

// Параметры Wi-Fi и VBAN
const char* ssid = "имя_сети";
const char* password = "пароль_сети";
const IPAddress destinationIP(192, 168, 0, 10); // IP-адрес приемника
const uint16_t destinationPort = 6980; // Порт VBAN

// Параметры АЦП и ЦАП
const int adcPin = 36; // Пин микрофона
const int dacPin = 25; // Пин ЦАП
const int adcMax = 4095; // Максимальное значение АЦП
const int dacMax = 255; // Максимальное значение ЦАП

// Объекты Wi-Fi и VBAN
WiFiUDP udp;
vbnc_t vbnc;

void setup() {
  // Инициализация АЦП и ЦАП
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  dac_output_enable(dacPin);

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Настройка VBAN
  vbnc_set_sample_rate(&vbnc, 44100);
  vbnc_set_channels(&vbnc, 1);
  vbnc_set_payload_type(&vbnc, VBNC_PAYLOAD_TYPE_PCM_FLOAT);
  vbnc_set_stream_name(&vbnc, "Microphone");

  // Начало передачи звука
  udp.beginPacket(destinationIP, destinationPort);
  vbnc_send_header(&vbnc, &udp);
  udp.endPacket();
}

void loop() {
  // Чтение звука с микрофона
  int adcValue = analogRead(adcPin);
  float voltage = adcValue / (float)adcMax;
  int dacValue = voltage * dacMax;

  // Запись звука в ЦАП
  dac_output_voltage(dacPin, dacValue);

  // Отправка звука по протоколу VBAN
  udp.beginPacket(destinationIP, destinationPort);
  vbnc_send_data(&vbnc, (char*)&voltage, sizeof(voltage), &udp);
  udp.endPacket();
}
