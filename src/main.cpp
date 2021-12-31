#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>

#include "credentials.hpp"

const std::string TOOL_ID = "bandsaw";
const int NUM_READS = 20;
const float OFF_THRESHOLD = 50.0;
const float ON_THRESHOLD = 300.0;

WiFiClient net;
MQTTClient client;

void publish(const std::string& topic, const std::string& payload) {
    auto fullTopic = topic + "/" + TOOL_ID;
    client.publish(fullTopic.c_str(), payload.c_str());
}

void connect() {
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nconnected! Local IP: ");
    Serial.print(WiFi.localIP());

    Serial.print("\nconnecting...");
    auto clientId ="savadhan-nodemcu-tool-" + TOOL_ID;
    while (!client.connect(clientId.c_str())) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nconnected!");

    publish("/heartbeat", "hello");
}

void setup() {
    Serial.begin(9600);
    pinMode(A0, INPUT);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    client.begin(MQTT_CONTROLLER_IP, net);
    connect();
}

int lastMillis = 0;
int toolStatus = 0;

void loop() {
    if (!client.connected()) {   
        connect();    
    } else {
        client.loop();
        delay(10);  // <- fixes some issues with WiFi stability
    }

    float total = 0.0;
    for (int i=0; i < NUM_READS; ++i) {
        total += analogRead(A0);
        delayMicroseconds(100);
    }
 
    float avgReading = total/NUM_READS;
    if (avgReading > ON_THRESHOLD && toolStatus == 0) {
        Serial.print("Detected tool ON! avgReading = ");
        Serial.println(avgReading);
        toolStatus = 1;
        publish("/tool_sensor", "on");
    } else if (avgReading < OFF_THRESHOLD && toolStatus == 1) {
        Serial.print("Detected tool OFF! avgReading = ");
        Serial.println(avgReading);
        toolStatus = 0;
        publish("/tool_sensor", "off");
    }

    auto millisNow = millis();
    if (millisNow - lastMillis > 3000) {
        Serial.print("Tool status: "); 
        Serial.print("avgReading = "); Serial.print(avgReading);
        Serial.print(", toolStatus = "); Serial.println(toolStatus);

        publish("/heartbeat", "tick");
        lastMillis = millisNow;
    }    
}