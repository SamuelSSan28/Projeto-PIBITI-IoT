// --- Bibliotecas ---
#include "DHT.h"                //biblioteca do sensor de umidade e temperatura
#include <ESP8266WiFi.h> //lib para conectar o wifi do ESP201
#include <ESP8266WiFiMulti.h>//lib para as funções addAP e  run
#include <PubSubClient.h>
#include <AddrList.h>
#include <Wire.h>
#include <SPI.h>

// LwIPv2
#include <lwip/dns.h>
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip6_addr.h"

#define LWIP_IPV4 0 
#define LWIP_IPV6 1 
#define LWIP_DHCP 0 
#define LWIP_DHCP6 1 

// =============================================================================================================
// --- Mapeamento de Hardware ---
#define     dht_pin    5       //pino de sinal do dht22 ligado no digital 5

// Definindo o sensor DHT11
#define dht_type DHT22

// --- Declaração de Objetos ---
WiFiClient nodemcuClient;

DHT dht(dht_pin, dht_type, 15);
// --- Variáveis Globais ---
double    temperatura = 0x00,             //armazena a temperatura em inteiro
          umidade     = 0x00;             //armazena a umidade em inteiro

// --- MQTT ---
const char* MQTT_BROKER = "10.94.15.69";
const char* MQTT_ClientID = "esp-dht22-node10";
const char* MQTT_USERNAME = "mqtt_user";
const char* MQTT_PASSWORD = "mqtt_ufpi";
PubSubClient client(nodemcuClient);

const char* topicoTemperatura = "temperatura/sensor_10";
const char* topicoUmidade = "umidade/sensor_10";

// CONEXAO WIFI
void conectarWifi() {
  WiFi.mode(WIFI_STA);
  
  WiFi.begin("UFPI", ""); //parametros: WiFi.softAP(nomeDoAccessPoint, senhaRede)
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFI Conectado!!!\n");
}
void reconectarMQTT() {
  while (!client.connected()) {
    Serial.print("..");  
    client.connect(MQTT_ClientID);
    client.connect(MQTT_ClientID,MQTT_USERNAME,MQTT_PASSWORD);
    client.subscribe(topicoUmidade);
    client.subscribe(topicoTemperatura);
  
    delay(30);
  }
  Serial.print("Broker Conectado\n");  
}


// PUBLICACAO DO VALOR ATUAL DA TEMPERATURA DO AMBIENTE
void publicarTemperaturaacNoTopico() {
  client.publish(topicoTemperatura, String(temperatura).c_str(), true); 
  Serial.print("Publish Temperatura" + String(temperatura));
  client.publish(topicoUmidade, String(umidade).c_str(), true);
  Serial.print("\n\nPublish Umidade"+ String(umidade));
}



void setup(){
  WiFi.hostname("esp-temperatura-node10");
  dht.begin();                          //inicia a comunicação com o dht
    
  Serial.begin(9600);
  
  conectarWifi();

  client.setServer(MQTT_BROKER, 1883);
  client.connect(MQTT_ClientID);
  client.connect(MQTT_ClientID,MQTT_USERNAME,MQTT_PASSWORD);
  client.subscribe(topicoUmidade);
  client.subscribe(topicoTemperatura);  
}

void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  
  temperatura = dht.readTemperature();
  umidade     = dht.readHumidity();
  
  publicarTemperaturaacNoTopico();

  delay(100);

}
