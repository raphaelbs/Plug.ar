/**
 * Conector para lembrar de diferentes redes Wifi.
 */
#ifndef WIFINDER_H
#define WIFINDER_H

#include <arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

class WiFinder{
private:
	int _led_pin;
	char* (*_wifis)[2];
	void printDots();
	void ota();
public:
	WiFinder(char* (*wifis)[2]);
	void connect();
	void handle();
	void setLed(int led_pin);
  String getLocalIp();
};

#endif
