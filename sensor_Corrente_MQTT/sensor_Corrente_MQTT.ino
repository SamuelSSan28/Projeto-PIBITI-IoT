// --- Bibliotecas ---
#include "EmonLib.h"            //Biblioteca para o sensor de corrente
#include <ESP8266WiFi.h> //lib para conectar o wifi do ESP201
#include <ESP8266WiFiMulti.h>//lib para as funções addAP e  run
#include <SPI.h>  //protocolo síncrono de dados serial

// =============================================================================================================
// --- Mapeamento de Hardware ---
#define     pin_sct     A0     //sensor de corrente

// --- Declaração de Objetos ---
EnergyMonitor emon1;            //cria objeto para sensor de corrente
ESP8266WiFiMulti nodemcuClient; //cria uma instância da classe ESP8266WiFiMulti

// --- MQTT ---
const char* MQTT_BROKER = "10.94.15.69";
const char* MQTT_ClientID = "esp-dht22-node10";
const char* MQTT_USERNAME = "mqtt_user";
const char* MQTT_PASSWORD = "mqtt_ufpi";
PubSubClient client(nodemcuClient);

const char* topicoTemperatura = "corrente/sensor_01";
// =============================================================================================================

// PUBLICACAO DO VALOR ATUAL DA TEMPERATURA DO AMBIENTE
void publicarCorrenteNoTopico(double corrente) {
  client.publish(topicoTemperatura, String(corrente).c_str(), true);   
}

void conectarWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("UFPI", ""); //parametros: WiFi.softAP(nomeDoAccessPoint, senhaRede)
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFI Conectado!!!\n");
}

void conectarMQTT() {
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



void setup(){
  pinMode(pin_sct, INPUT);             //entrada para sensor de corrente
  Serial.begin(9600);
  //Calibrar
  emon1.current(pin_sct, 88.51063829787235); //Pino, calibracao - Cur Const= Ratio/BurdenR. 2000/33 = 84,255
  
  conectarWifi(); 
  reconectarMQTT();  
}

void loop() {
  double Irms = emon1.calcIrms(2000);   //Calcula a corrente
 
  if (!client.connected()) {
    reconectarMQTT();
  }

  publicarCorrenteNoTopico();

  delay(1000);
 
}
