#include "WiFinder.h"

WiFinder::WiFinder(char* (*wifis)[2]){
	_led_pin = -1;
	_wifis = wifis;
}

void WiFinder::setLed(int led_pin){
	_led_pin = led_pin;
	pinMode(led_pin, OUTPUT);
}

void WiFinder::handle(){
  ArduinoOTA.handle();
}

String WiFinder::getLocalIp(){
  return WiFi.localIP().toString();
}

#define STR_CON_HEAD "* [WiFinder~connect()] "
#define STR_CON_INIT "iniciando conexao WiFi"
#define STR_CON_TRYI "conectando a: "
#define STR_CON_CONN "Conectado: "
#define STR_CON_ERR  "falha na conexao, nenhum WiFi conectado! Restartando..."
#define STR_CON_DOTS "."
void WiFinder::connect(){
	Serial.println();
	Serial.print(STR_CON_HEAD);
	Serial.println(STR_CON_INIT);
  Serial.print(STR_CON_HEAD);
  Serial.print(STR_CON_TRYI);
	WiFi.mode(WIFI_STA);

	int size = sizeof(_wifis);
	for(int i=0; i<size; i++){
		char* SSID = _wifis[i][0];
		char* PASSWORD = _wifis[i][1];
    Serial.print("\n\t\t\t");
		Serial.print(SSID);
		WiFi.begin(SSID, PASSWORD);
		int status;
		while (true) {
			status = WiFi.status();
			if(status == WL_CONNECTED) break;
      else if(status == WL_NO_SSID_AVAIL) break;
			else if(status == WL_CONNECT_FAILED) break;
			printDots();
		}
		if(status != WL_CONNECTED) continue;
		Serial.println();
		Serial.print(STR_CON_HEAD);
		Serial.print(STR_CON_CONN);
		Serial.println(WiFi.localIP());
		ota();
		return;
	}
	Serial.println();
	Serial.print(STR_CON_HEAD);
	Serial.println(STR_CON_ERR);
	delay(5000);
	ESP.restart();
}

void WiFinder::printDots(){
	Serial.print(STR_CON_DOTS);
	delay(500);
	if(_led_pin < 0) return;
	digitalWrite(_led_pin, !digitalRead(_led_pin));
}

#define STR_OTA_HEAD "* [WiFinder~ota()] "
#define STR_OTA_INIT "OTA inicializado"
#define STR_OTA_FREE "espaco livre: "
#define STR_OTA_START "comecando OTA"
#define STR_OTA_END "finalizando OTA "
#define STR_OTA_PRG "* [WiFinder~ota()] progresso OTA: %u%%\r"
#define STR_OTA_ERR  "falha no OTA, erro [%u]:\n\t\t\tfalha ao "
#define STR_OTA_ERR_AUTH "autenticar"
#define STR_OTA_ERR_BEGIN "inicializar"
#define STR_OTA_ERR_CONNECT "conectar"
#define STR_OTA_ERR_RECEIVE "receber"
#define STR_OTA_ERR_END "terminar"
void WiFinder::ota(){

	ArduinoOTA.onStart([]() {
		Serial.print(STR_OTA_HEAD);
		Serial.println(STR_OTA_START);
	});
	ArduinoOTA.onEnd([]() {
		Serial.print(STR_OTA_HEAD);
		Serial.println(STR_OTA_END);
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf(STR_OTA_PRG, (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.print(STR_OTA_HEAD);
		Serial.printf(STR_OTA_ERR, error);
		if (error == OTA_AUTH_ERROR) Serial.println(STR_OTA_ERR_AUTH);
		else if (error == OTA_BEGIN_ERROR) Serial.println(STR_OTA_ERR_BEGIN);
		else if (error == OTA_CONNECT_ERROR) Serial.println(STR_OTA_ERR_CONNECT);
		else if (error == OTA_RECEIVE_ERROR) Serial.println(STR_OTA_ERR_RECEIVE);
		else if (error == OTA_END_ERROR) Serial.println(STR_OTA_ERR_END);
	});
	ArduinoOTA.begin();

	Serial.println();
	Serial.print(STR_OTA_HEAD);
	Serial.println(STR_OTA_INIT);
  Serial.print(STR_OTA_HEAD);
  Serial.print(STR_OTA_FREE);
  Serial.print(ESP.getFreeSketchSpace());
  Serial.println(" bytes");
}
