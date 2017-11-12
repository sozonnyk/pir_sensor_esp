// Do not remove the include below
#include "pir_sensor_esp.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "credentials.h"

#define DEBUG
#ifdef DEBUG
#define DEBUG_SPEED 115200
#define D(input) {Serial.print(input); Serial.flush();}
#define Dln(input) {Serial.println(input); Serial.flush();}
#else
#define D(input)
#define Dln(input)
#endif

#if !defined(WIFI_SSID) && !defined(WIFI_PASS)
#error "You must define WIFI_SSID and WIFI_PASS in credentials.h";
#endif

#define SENSOR_ON "sensor1,ON"
#define SENSOR_OFF "sensor1,OFF"
#define SENSOR_PIN 0

WiFiUDP Udp;

IPAddress group(224, 1, 1, 1);
unsigned int port = 5007;

void send_message(const char* message) {
	D("Sending message: ")
	Dln(message)
	Udp.beginPacketMulticast(group, port, WiFi.localIP());
	Udp.write(message, strlen(message));
	Udp.endPacket();
}

void sensor_isr() {
	//Inverted logic for button
	switch (digitalRead(SENSOR_PIN)) {
	case LOW:
		send_message(SENSOR_ON);
		break;
	case HIGH:
		send_message(SENSOR_OFF);
		break;
	}
}

void setup() {
#ifdef DEBUG
	Serial.begin(DEBUG_SPEED);
#endif
	D("Connecting to ");
	Dln(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PASS);
	while (WiFi.status() != WL_CONNECTED) {
		delay(200);
		D(".");
	}
	Dln();
	Dln("WiFi connected");
	D("IP address: ");
	Dln(WiFi.localIP());

	pinMode(BUILTIN_LED, OUTPUT);
	pinMode(SENSOR_PIN, INPUT_PULLUP);
	attachInterrupt(SENSOR_PIN, sensor_isr, CHANGE);

}

void loop() {
	delay(100);
}
