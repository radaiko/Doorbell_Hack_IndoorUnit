#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <Config.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
float oldVoltage = 0;
float newVoltage = 0;


void callback(char *topic, byte *payload, unsigned int length)
{
    String msg;
    for (byte i = 0; i < length; i++)
    {
        char tmp = char(payload[i]);
        msg += tmp;
    }
    Serial.println(msg);
}

void getVoltage(){
    int sensorValue = analogRead(A0);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.2V) and round to 1 after coma:
    newVoltage = (float)((int)(sensorValue * (3.2 / 1023.0) * 10 + .5)) /10; ;
}

void sendData(){
    // Send Alive status
    sprintf(msg, "false");
    client.publish("/SmartHome/Doorbell/IndoorUnit/Dead", msg);

    // Send Uptime status
    sprintf(msg, "%ld", millis());
    client.publish("/SmartHome/Doorbell/IndoorUnit/UptimeInMillis", msg);

    // Send Voltage
    sprintf(msg, "%f", newVoltage);
    client.publish("/SmartHome/Doorbell/IndoorUnit/Voltage", msg);

    // Check if doorbell engaged
    if (newVoltage < 13.6 && newVoltage > 13.4)
    {
        sprintf(msg, "1");
        client.publish("/SmartHome/Doorbell/IndoorUnit/Switch", msg);
    }
}

void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PW);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Reconnecting...");
        client.connect("ESP8266Client", NULL, NULL, "/SmartHome/Doorbell/IndoorUnit/Dead", 0, false, "true", true);
        client.subscribe("/SmartHome/Doorbell/IndoorUnit/Command");
        if (!client.connected())
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        } else
        {
            sendData();
        }        
    }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    // GetVoltage
    getVoltage();
    if (newVoltage != oldVoltage)
    {
        sendData();
    }

    delay(100);
}