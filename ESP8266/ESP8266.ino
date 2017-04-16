#include "WiFinder.h"
#include <FirebaseArduino.h>

#define FIREBASE_HOST "plugar-e4795.firebaseio.com"
#define FIREBASE_AUTH "lgshhtREzTrtdGzHEV5jyAzmC16SlTg0ndgVYQuk"
#define LED_PIN 10
#define BTN_PIN 2

char* WIFIS[][2] = {
  {"Private", "666conexaoprivate"},
  {"rapha", "rapha123"},
  {"livs", "asdf1234"},
	{"Find", "find@2016"},
	{"Aja", "0135792468aja"}  
};
WiFinder wifinder(WIFIS);

volatile bool led_status, alterado = false;
volatile int fb_delay = 1000, prev_delay = 1000;

void setup() {
	Serial.begin(115200);
  wifinder.setLed(LED_PIN);
	wifinder.connect();
	pinMode(BTN_PIN, INPUT_PULLUP);
	Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.setString("localIp", wifinder.getLocalIp());
	attachInterrupt(BTN_PIN, readBtn, FALLING);
}

void loop() {
	wifinder.handle();
	readLEDFirebase();
	updateFirebase();
	readDelayFirebase();

	delay(fb_delay);
}

// Lê do firebase: LED
void readLEDFirebase(){
	bool cled_status = Firebase.getBool("LED");
	if(led_status == cled_status) return;
	led_status = cled_status;
	Serial.print("LED alterado para: ");
	Serial.println(led_status);
	digitalWrite(LED_PIN, led_status);
}

// Lê do firebase: Delay
void readDelayFirebase(){
	fb_delay = Firebase.getInt("delay");
	if(fb_delay < 1000) fb_delay = 1000;
	if(fb_delay == prev_delay) return;
	Serial.print("Delay alterado para: ");
	Serial.println(fb_delay);
	prev_delay = fb_delay;
}

// Atualiza do firebase: LED
void updateFirebase(){
	if(!alterado) return;
	Firebase.setBool("LED", !led_status);
	Serial.print("Definindo LED no firebase: ");
	Serial.println(!led_status);
	if (Firebase.failed()) {
		Serial.print("Erro ao setar LED:");
		Serial.println(Firebase.error());
		return;
	}
	alterado = false;
	attachInterrupt(BTN_PIN, readBtn, FALLING);
}

void readBtn(){
	Serial.println("Alterando LED via interrupt");
	alterado = true;
	detachInterrupt(BTN_PIN);
}
