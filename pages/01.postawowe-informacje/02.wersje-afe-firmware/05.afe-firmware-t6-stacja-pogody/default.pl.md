---
title: 'T6 - Stacja pogody'
recaptchacontact:
    enabled: false
---

**Firmware umożliwia**

* odczytywanie warunków atmosferycznych 
* sterowanie bezprzewodowo 1 urządzeniem / układem elektrycznym

**Sprzęt:**
* ESP8286/8285
	* Dla wersji 2.x wymagany jest ESP: ESP12, 12E, 12F, 13, 14, 07s lub inny z min 4MB pamięci Flash 
* 1 przekaźnik
* do 2 przycisków: mono/bi-stabilnych
* do 2 diod LED
* 1 czujnik Bosch BME680, BME280, BMP180 lub BMP085
	* BMP085, BMP180 - temperatura, ciśnienie, ciśnienie względne (zrobione)
	* BME280 - temperatura, wilgotność, ciśnienie względne, punkt rosy, temperatura odczuwalna  (zrobione)
	* BME680 - temperatura, wilgotność, ciśnienie, ciśnienie względne, Punkt Rosy, Temperatura Odczuwalna, IAQ, Statyczne IAQ, Ekwiwalent CO2, Breath VOC Ekwiwalent 
* 1 czujnik BH1750 (natężenia światła)
* 1 czujnik Honeywell HPMA115S0 (pyłów zawieszonych 2.5 oraz 10)
* 1 czujnik AS3935 (odległości od burzy)

**API**
* HTTP
* MQTT
* Dedykowane API dla Domoticz, z wykorzystanie protokołu HTTP

** Funkcje **
* Pomiary warunków atmosferycznych w zależności od wykorzystanych czujników, patrzy wyżej
* Informacje o ratingu warunków atmosferycznych (HTTP oraz MQTT)
* Możliwość dokonania korekty wartości z czujników
* Wykrywanie odległości od burzy do 50km [[w wersji PRO](/postawowe-informacje/wersja-pro)]
* Usypianie czujnika Honeywell HPMA115S0
* Ustawianie dowolnych GPIO dla portu I2C oraz UART
* Sterowanie przekaźnikiem bezprzewodowo przez sieć WiFi 
* Ustawiania stanu początkowego przekaźnika po uruchomieniu
* Ustawienie stany przekaźnika po podłączeniu się do brokera MQTT
* Automatyczne wyłączanie przekaźnika po zadanym czasie
* Sterowanie przekaźnikiem zewnętrznym włącznikiem/przyciskiem mono (dzwonkowym) lub bistabilnymi
* LED może informwać o stanie przekaźnika
* Odczyty z przetwornika analogowo-cyfrowego [[w wersji PRO](/postawowe-informacje/wersja-pro)]
* Konfiguracja przez przeglądarkę w języku PL lub EN
* Możliwość nadania stałego adresu IP dla urządzenia
* Aktualizacja oprogramowania przez przeglądarkę

** Gotowe moduły **
* [Stacja Pogody v2](https://www.smartnydom.pl/weather-station-stacja-pogodowa-v2/)


!! Wersja 2.0 nie obsługuje ESP8266 1MB, ESP8285 ze względu na ograniczone rozmiary pamięci Flash. Jest możliwość wgrania AFE T6 2.x do ESP z rozmiarem pamięći 1MB natomiast nie będzie możliwa aktualizacja firmware przez przeglądarkę www




---