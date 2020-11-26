#include <AddrList.h>
#include <IRac.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h> 

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

// WIFI
#define STASSID "UFPI"
#define STAPSK  ""

// --- Declaração de Constantes ---
int IR_Recver = 13;   //Receptor Infravermelho (IR)
int estado_ant = 0;
int estado = 0;
int temperatura = 22;

const uint16_t IrLed = 15; //Emissor Infravermelho (IR) 
//Raw responsavel por cada sinal enviado do emissor IR ao A/C
uint16_t lixo[199] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t powerOffRawData[199] = {4370, 4374,  512, 1662,  512, 576,  514, 1684,  488, 1660,  514, 576,  514, 576,  514, 1684,  488, 580,  538, 548,  514, 1660,  512, 576,  514, 576,  514, 1660,  514, 1658,  514, 576,  514, 1660,  540, 548,  514, 1662,  512, 1684,  488, 1660,  512, 1658,  514, 576,  514, 1658,  514, 1662,  536, 1634,  510, 576,  514, 576,  514, 576,  514, 576,  514, 1684,  490, 576,  514, 578,  538, 1634,  512, 1660,  512, 1662,  510, 576,  514, 576,  514, 576,  514, 576,  514, 578,  540, 548,  514, 576,  514, 574,  514, 1658,  514, 1658,  516, 1658,  514, 1660,  512, 1658,  540, 5194,  4372, 4352,  514, 1660,  512, 576,  514, 1660,  514, 1660,  510, 576,  514, 576,  514, 1660,  512, 578,  538, 550,  514, 1660,  512, 576,  514, 576,  514, 1660,  512, 1684,  490, 574,  514, 1660,  538, 550,  514, 1660,  512, 1660,  512, 1658,  514, 1684,  488, 574,  514, 1660,  512, 1660,  538, 1632,  512, 576,  514, 576,  514, 576,  514, 576,  514, 1660,  512, 576,  514, 578,  538, 1656,  488, 1660,  512, 1684,  488, 576,  514, 576,  514, 576,  514, 578,  512, 578,  538, 550,  514, 576,  514, 576,  514, 1660,  512, 1684,  488, 1660,  512, 1660,  512, 1662,  536};  // COOLIX B27BE0
uint16_t powerOnRawData[199] =  {4372, 4350,  514, 1660,  512, 576,  514, 1658,  514, 1658,  514, 576,  514, 576,  514, 1658,  514, 578,  518, 570,  512, 1684,  490, 574,  514, 576,  514, 1658,  512, 1684,  490, 576,  514, 1660,  538, 550,  514, 576,  514, 576,  514, 1660,  512, 1660,  512, 1682,  490, 1682,  488, 1662,  538, 1632,  512, 1662,  510, 1684,  490, 576,  514, 576,  514, 576,  514, 576,  514, 578,  538, 550,  512, 1662,  512, 1684,  488, 1662,  510, 1660,  512, 576,  514, 576,  514, 576,  538, 1632,  514, 576,  514, 576,  514, 576,  514, 576,  514, 1660,  512, 1662,  510, 1658,  540, 5190,  4374, 4352,  514, 1658,  512, 576,  514, 1658,  514, 1660,  514, 574,  514, 576,  514, 1684,  488, 578,  530, 558,  514, 1658,  512, 576,  514, 576,  516, 1658,  514, 1660,  512, 576,  514, 1662,  536, 550,  514, 576,  514, 576,  514, 1684,  488, 1658,  514, 1660,  514, 1656,  514, 1662,  536, 1634,  512, 1660,  512, 1658,  512, 576,  514, 574,  514, 576,  514, 574,  514, 578,  538, 550,  514, 1660,  510, 1684,  490, 1658,  514, 1660,  514, 576,  514, 576,  514, 578,  516, 1680,  490, 576,  512, 578,  514, 576,  514, 576,  514, 1660,  512, 1682,  490, 1660,  540};  // COOLIX B21F78
uint16_t set17RawData[199] =    {4372, 4360,  514, 1656,  514, 576,  514, 1682,  490, 1660,  512, 576,  514, 576,  514, 1660,  512, 578,  538, 548,  514, 1662,  510, 576,  514, 576,  514, 1682,  490, 1682,  490, 576,  514, 1662,  536, 550,  514, 574,  514, 576,  514, 1682,  490, 1660,  514, 1660,  512, 1682,  490, 1684,  514, 1630,  516, 1658,  514, 1682,  490, 576,  514, 576,  514, 576,  514, 574,  514, 578,  538, 550,  514, 576,  514, 576,  514, 576,  514, 1684,  488, 576,  514, 576,  514, 578,  538, 1630,  516, 1658,  514, 1658,  514, 1658,  514, 574,  514, 1658,  514, 1682,  490, 1658,  538, 5192,  4372, 4350,  516, 1680,  490, 576,  514, 1660,  514, 1658,  512, 576,  514, 574,  514, 1660,  512, 578,  540, 548,  514, 1660,  512, 576,  514, 576,  516, 1656,  514, 1684,  488, 576,  514, 1684,  514, 550,  514, 576,  514, 576,  516, 1656,  514, 1682,  490, 1658,  514, 1684,  488, 1662,  538, 1632,  514, 1658,  514, 1682,  490, 576,  512, 576,  514, 576,  514, 576,  514, 578,  540, 548,  514, 576,  514, 576,  514, 576,  516, 1660,  510, 576,  514, 576,  514, 578,  538, 1656,  490, 1658,  514, 1682,  488, 1684,  490, 574,  514, 1684,  490, 1658,  514, 1660,  538};  // COOLIX B21F08
uint16_t set18RawData[199] =    {4370, 4356,  514, 1658,  512, 576,  514, 1656,  514, 1658,  512, 576,  514, 576,  512, 1658,  514, 578,  538, 548,  514, 1658,  512, 576,  514, 576,  512, 1660,  512, 1656,  514, 576,  512, 1660,  538, 550,  512, 578,  512, 576,  514, 1660,  512, 1656,  514, 1660,  510, 1660,  512, 1662,  536, 1632,  512, 1660,  512, 1660,  510, 576,  512, 576,  514, 576,  514, 576,  512, 580,  538, 548,  514, 574,  514, 576,  512, 1658,  514, 1656,  514, 576,  514, 574,  514, 578,  536, 1634,  512, 1660,  512, 1656,  514, 576,  514, 576,  514, 1660,  512, 1658,  514, 1656,  540, 5190,  4368, 4350,  512, 1658,  512, 576,  514, 1656,  512, 1658,  514, 576,  514, 576,  514, 1658,  512, 578,  538, 548,  514, 1660,  510, 576,  512, 576,  514, 1658,  512, 1658,  514, 574,  514, 1662,  536, 548,  514, 576,  512, 576,  514, 1656,  514, 1660,  510, 1658,  514, 1658,  512, 1660,  538, 1632,  514, 1660,  512, 1658,  512, 576,  514, 576,  514, 576,  514, 574,  514, 578,  536, 550,  514, 576,  514, 576,  514, 1658,  514, 1660,  512, 576,  512, 576,  512, 578,  538, 1632,  514, 1660,  512, 1658,  512, 576,  514, 576,  514, 1660,  512, 1684,  488, 1660,  536};  // COOLIX B21F18
uint16_t set19RawData[199] =    {4370, 4358,  512, 1658,  514, 576,  514, 1658,  512, 1660,  512, 574,  514, 576,  514, 1658,  512, 578,  520, 566,  514, 1656,  514, 576,  514, 576,  514, 1658,  512, 1658,  514, 576,  512, 1660,  516, 570,  514, 576,  512, 576,  514, 1656,  514, 1660,  512, 1658,  512, 1656,  514, 1660,  538, 1632,  514, 1658,  514, 1658,  512, 576,  514, 576,  512, 576,  514, 576,  514, 578,  538, 550,  512, 578,  512, 1658,  512, 1658,  514, 1658,  514, 576,  514, 576,  514, 578,  538, 1632,  512, 1658,  512, 576,  514, 576,  514, 574,  514, 1660,  512, 1658,  512, 1658,  538, 5192,  4368, 4350,  514, 1658,  514, 574,  514, 1660,  512, 1660,  512, 574,  514, 576,  512, 1658,  514, 578,  518, 570,  514, 1658,  512, 576,  514, 576,  514, 1656,  514, 1658,  512, 576,  514, 1660,  512, 574,  514, 576,  514, 576,  514, 1656,  514, 1658,  512, 1658,  512, 1658,  514, 1660,  514, 1654,  514, 1658,  512, 1660,  510, 576,  514, 576,  512, 578,  512, 576,  514, 578,  538, 550,  512, 576,  514, 1656,  514, 1658,  512, 1660,  512, 574,  514, 576,  512, 578,  538, 1630,  514, 1660,  512, 576,  514, 576,  514, 576,  512, 1658,  514, 1656,  514, 1662,  510};  // COOLIX B21F38
uint16_t set20RawData[199] =    {4396, 4344,  512, 1658,  512, 576,  514, 1658,  512, 1656,  514, 576,  514, 574,  514, 1658,  512, 578,  538, 550,  512, 1660,  510, 576,  514, 576,  512, 1658,  512, 1658,  512, 574,  514, 1660,  536, 552,  514, 576,  514, 576,  514, 1658,  512, 1658,  512, 1658,  512, 1658,  512, 1662,  516, 1652,  512, 1658,  512, 1660,  512, 576,  512, 576,  514, 576,  514, 574,  514, 576,  518, 568,  514, 576,  514, 1656,  516, 574,  514, 1658,  514, 574,  516, 574,  514, 576,  514, 1656,  512, 1658,  514, 574,  516, 1658,  510, 576,  514, 1656,  516, 1658,  512, 1656,  538, 5192,  4366, 4348,  514, 1658,  512, 574,  516, 1656,  514, 1660,  512, 574,  514, 576,  514, 1660,  510, 576,  514, 572,  514, 1658,  512, 574,  514, 574,  514, 1658,  512, 1660,  512, 576,  514, 1660,  536, 550,  512, 576,  514, 576,  512, 1660,  512, 1658,  512, 1658,  512, 1658,  514, 1660,  514, 1658,  510, 1658,  512, 1656,  514, 576,  514, 576,  514, 574,  514, 574,  514, 578,  540, 548,  514, 576,  512, 1658,  512, 576,  514, 1660,  510, 576,  514, 576,  512, 578,  530, 1640,  514, 1656,  512, 576,  512, 1656,  514, 576,  512, 1658,  514, 1658,  514, 1658,  538};  // COOLIX B21F28
uint16_t set21RawData[199] =    {4340, 4416,  486, 1660,  512, 574,  514, 1658,  512, 1656,  514, 576,  514, 574,  514, 1658,  514, 578,  516, 572,  514, 1658,  514, 574,  514, 576,  514, 1656,  514, 1658,  512, 576,  514, 1660,  526, 560,  514, 576,  514, 576,  514, 1658,  514, 1658,  514, 1658,  512, 1658,  514, 1660,  538, 1630,  514, 1658,  512, 1656,  516, 576,  514, 574,  514, 576,  514, 576,  514, 578,  538, 550,  514, 1658,  512, 1656,  516, 576,  514, 1658,  512, 576,  514, 576,  514, 578,  538, 1630,  516, 574,  514, 576,  512, 1658,  512, 576,  514, 1656,  514, 1658,  512, 1656,  540, 5192,  4368, 4352,  510, 1656,  514, 574,  514, 1658,  514, 1656,  516, 576,  514, 574,  514, 1660,  512, 576,  514, 574,  514, 1658,  514, 576,  512, 574,  514, 1658,  514, 1658,  512, 578,  512, 1660,  520, 566,  514, 576,  514, 576,  514, 1658,  514, 1656,  514, 1658,  514, 1658,  514, 1660,  536, 1630,  516, 1660,  510, 1656,  516, 574,  514, 576,  512, 576,  514, 576,  514, 578,  514, 572,  514, 1656,  516, 1658,  514, 574,  512, 1658,  514, 576,  514, 574,  514, 578,  514, 1656,  512, 574,  514, 576,  516, 1656,  514, 574,  514, 1656,  514, 1658,  514, 1658,  538};  // COOLIX B21F68
uint16_t set22RawData[199] =    {4340, 4390,  512, 1658,  512, 576,  514, 1658,  512, 1658,  512, 576,  514, 576,  512, 1658,  514, 578,  536, 548,  514, 1658,  514, 576,  514, 576,  512, 1656,  516, 1656,  516, 574,  514, 1658,  538, 550,  512, 576,  514, 576,  514, 1658,  514, 1658,  512, 1658,  514, 1656,  514, 1658,  536, 1632,  514, 1658,  514, 1658,  514, 574,  514, 574,  514, 576,  514, 576,  514, 578,  514, 572,  514, 1656,  514, 1660,  512, 1656,  514, 1658,  512, 574,  514, 576,  514, 576,  518, 1654,  510, 576,  514, 576,  512, 576,  514, 574,  514, 1658,  514, 1656,  514, 1656,  540, 5190,  4366, 4350,  512, 1656,  514, 576,  514, 1656,  514, 1658,  514, 574,  514, 576,  512, 1656,  514, 576,  538, 550,  514, 1656,  514, 574,  514, 576,  514, 1656,  514, 1660,  512, 574,  514, 1660,  520, 566,  514, 574,  516, 574,  514, 1658,  512, 1658,  512, 1658,  512, 1658,  514, 1660,  538, 1632,  512, 1658,  514, 1656,  514, 576,  512, 576,  514, 576,  514, 574,  514, 576,  538, 548,  514, 1658,  512, 1658,  514, 1658,  514, 1660,  510, 574,  516, 574,  514, 578,  538, 1632,  512, 578,  512, 574,  514, 576,  512, 576,  514, 1656,  514, 1656,  514, 1662,  536};  // COOLIX B21F78
uint16_t set23RawData[199] =    {4368, 4366,  516, 1658,  514, 574,  514, 1658,  514, 1656,  514, 576,  512, 576,  514, 1658,  514, 578,  536, 550,  514, 1660,  512, 574,  514, 576,  514, 1660,  512, 1660,  512, 576,  512, 1660,  538, 550,  514, 574,  516, 574,  514, 1660,  510, 1658,  514, 1658,  514, 1658,  512, 1658,  540, 1632,  514, 1656,  514, 1658,  512, 576,  514, 574,  516, 574,  514, 574,  514, 576,  540, 548,  514, 1656,  516, 576,  514, 1660,  510, 1658,  512, 576,  514, 574,  514, 578,  514, 1656,  514, 574,  514, 1660,  512, 574,  516, 574,  514, 1658,  514, 1658,  512, 1656,  540, 5190,  4370, 4350,  514, 1658,  512, 576,  514, 1656,  514, 1656,  514, 576,  514, 574,  514, 1656,  516, 576,  538, 550,  514, 1658,  512, 576,  514, 574,  514, 1660,  512, 1658,  514, 576,  514, 1658,  540, 548,  514, 576,  514, 576,  514, 1658,  512, 1660,  512, 1656,  514, 1658,  514, 1660,  538, 1632,  512, 1656,  514, 1660,  512, 576,  514, 574,  514, 576,  514, 576,  514, 576,  538, 548,  514, 1658,  514, 574,  514, 1660,  512, 1658,  514, 574,  514, 576,  512, 578,  538, 1632,  514, 574,  514, 1658,  514, 574,  514, 576,  514, 1656,  514, 1658,  512, 1660,  538};  // COOLIX B21F58
uint16_t set24RawData[199] =    {4396, 4344,  514, 1656,  514, 574,  514, 1660,  512, 1658,  512, 574,  516, 574,  514, 1656,  514, 576,  540, 548,  514, 1658,  512, 576,  514, 576,  512, 1660,  510, 1658,  512, 576,  514, 1660,  538, 548,  514, 576,  514, 574,  514, 1658,  514, 1656,  516, 1656,  514, 1658,  514, 1658,  538, 1632,  514, 1656,  514, 1656,  514, 576,  514, 574,  516, 574,  514, 576,  514, 578,  526, 560,  514, 1658,  514, 574,  514, 574,  516, 1658,  512, 574,  514, 574,  514, 576,  540, 1630,  514, 574,  514, 1656,  516, 1656,  514, 574,  514, 1656,  514, 1658,  514, 1656,  540, 5190,  4370, 4350,  514, 1658,  514, 574,  514, 1660,  512, 1656,  514, 574,  516, 576,  514, 1658,  514, 578,  516, 570,  514, 1658,  514, 574,  514, 576,  514, 1658,  512, 1658,  514, 576,  512, 1658,  540, 548,  514, 576,  514, 576,  514, 1658,  514, 1658,  512, 1660,  512, 1656,  514, 1658,  538, 1628,  516, 1658,  514, 1658,  514, 574,  514, 576,  514, 574,  514, 574,  516, 576,  538, 548,  514, 1658,  514, 574,  516, 574,  514, 1658,  514, 576,  514, 574,  514, 576,  538, 1632,  512, 574,  516, 1658,  512, 1656,  514, 576,  512, 1660,  512, 1660,  512, 1660,  518};  // COOLIX B21F48
uint16_t set25RawData[199] =    {4372, 4342,  514, 1656,  514, 576,  514, 1658,  514, 1652,  516, 574,  514, 576,  514, 1682,  488, 578,  538, 548,  514, 1658,  512, 576,  514, 574,  514, 1658,  512, 1656,  514, 574,  514, 1658,  538, 550,  514, 574,  514, 574,  514, 1656,  516, 1656,  516, 1656,  514, 1656,  514, 1660,  514, 1654,  514, 1658,  514, 1656,  514, 574,  514, 576,  514, 574,  514, 576,  514, 578,  538, 1630,  516, 1682,  488, 574,  514, 574,  514, 1656,  514, 576,  514, 576,  514, 576,  540, 548,  514, 574,  514, 1656,  516, 1656,  514, 576,  514, 1656,  514, 1656,  514, 1656,  542, 5188,  4370, 4350,  514, 1656,  514, 574,  514, 1656,  514, 1656,  514, 574,  514, 574,  514, 1656,  514, 578,  538, 548,  514, 1656,  514, 576,  514, 576,  514, 1658,  512, 1656,  514, 574,  514, 1658,  538, 548,  516, 576,  514, 574,  514, 1658,  512, 1658,  514, 1656,  514, 1658,  514, 1660,  538, 1630,  514, 1658,  512, 1658,  512, 574,  514, 576,  514, 574,  516, 574,  514, 578,  538, 1630,  514, 1660,  512, 576,  514, 574,  516, 1656,  514, 574,  514, 576,  514, 578,  538, 548,  514, 576,  514, 1656,  514, 1658,  514, 574,  514, 1658,  514, 1656,  514, 1658,  538};  // COOLIX B21FC8

// --- Declaração de Objetos ---
IRsend irsend(IrLed);
IRrecv irrecv(IR_Recver);
decode_results results;
LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient nodemcuClient;

// --- MQTT ---
#include <PubSubClient.h>
//const char* MQTT_BROKER = "fd9e:6c51:2336::1";
//const char* MQTT_BROKER = "fd9e:6c51:2336:0:717b:e02b:9c00:d43a";
const char* MQTT_BROKER = "10.94.5.1";
const char* MQTT_ClientID = "esp-dht22-node05";
const char* MQTT_USERNAME = "mqtt_user";
const char* MQTT_PASSWORD = "mqtt_ufpi";
PubSubClient client(nodemcuClient);

const char* topicoEstado = "controleArCondicionado/Disnel/LD";
const char* topicoTemperatura = "controleArCondicionado/Disnel/temperatura2";

const char* topicoEstadoBD = "controleArCondicionado/Disnel/estadoBD";
const char* topicoTemperaturaBD = "controleArCondicionado/Disnel/temperaturaBD";

// CONEXAO WIFI
void conectarWifi() {
  delay(10);

 WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
void reconectarMQTT() {
  while (!client.connected()) {
    client.connect(MQTT_ClientID);
    client.connect(MQTT_ClientID,MQTT_USERNAME,MQTT_PASSWORD);
    client.subscribe(topicoEstado);
    client.subscribe(topicoTemperatura);
    delay(1000);
  }
}

// PUBLICACAO DO VALOR ATUAL DA TEMPERATURA DO AR COONDICIONADO
void publicarTemperaturaArCondicionadoNoTopico() {
  int aux = estado;
  if(estado == 1) aux = 22;
  if(estado == 0) aux = estado;
  client.publish("controleArCondicionado/Disnel/temperaturaApp", String(aux).c_str(), true);
}

// PUBLICACAO DO ESTADO ATUAL DO AR COONDICIONADO
void publicarEstadoNoTopico() {
  client.publish("controleArCondicionado/Disnel/LDApp", String(estado).c_str(), true);
}

// PUBLICACAO pro bd
void publicarBD() {
  String msg;
  if(estado == 1)  msg = "1 22";
  if(estado > 1)  msg = "1 " + String(estado);
  if(estado == 0)  msg = "0 " + String(estado_ant);
  
  client.publish("controleArCondicionado/Disnel/temp_est_bd",msg.c_str(), true);
}

// RECEBENDO O VALOR ATUAL DA TEMPERATURA DO AR COONDICIONADO
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  String topico = String(topic);
  Serial.print("Message arrived in topic: ");
  
  for (int i = 0; i < length; i++) {
      msg += ((char)payload[i]);
    }
    
  Serial.print(topic);
  Serial.print(" : ");
  Serial.println(msg);

  estado_ant = estado;
  estado = msg.toInt();
  
  Serial.println("Atual e Anterior: "+String(estado)+" "+ String(estado_ant));
  
  if(estado > 1 and estado_ant == 0){
    estado = estado_ant;
    }
  
  
}

void set_Ar(int set){ 
  switch(set) {
    case 0:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(powerOffRawData, 199, 38);
      irsend.sendRaw(powerOffRawData, 199, 38);
      set_Display("Desligando....");
      delay(943);
      set_Display("Desligado");
    break;
    }
    case 1:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(powerOnRawData, 199, 38);
      irsend.sendRaw(powerOnRawData, 199, 38);
      set_Display("Ligando...");
      delay(1000);
      set_Display("Temperatura: 22");
    break;
    }
    case 17:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(50);
      irsend.sendRaw(set17RawData, 199, 38);
      irsend.sendRaw(set17RawData, 199, 38);
      set_Display("Temperatura: 17");
     break;
    }
    case 18:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set18RawData, 199, 38);
      irsend.sendRaw(set18RawData, 199, 38);
      set_Display("Temperatura: 18");
    break;
    }
    case 19:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set19RawData, 199, 38);
       irsend.sendRaw(set19RawData, 199, 38);
      set_Display("Temperatura: 19");
    break;
    }
    case 20:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set20RawData, 199, 38);
      irsend.sendRaw(set20RawData, 199, 38);
      set_Display("Temperatura: 20");
    break;
    }
    case 21:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set21RawData, 199, 38);
      irsend.sendRaw(set21RawData, 199, 38);
      set_Display("Temperatura: 21");
    break;
    }
    case 22:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set22RawData, 199, 38);
      irsend.sendRaw(set22RawData, 199, 38);
      set_Display("Temperatura: 22");
    break;
    }
    case 23:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set23RawData, 199, 38);
      irsend.sendRaw(set23RawData, 199, 38);
      set_Display("Temperatura: 23");
    break;
    }
    case 24:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set24RawData, 199, 38);
      irsend.sendRaw(set24RawData, 199, 38);
      set_Display("Temperatura: 24");
    break;
    }
    case 25:{
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      irsend.sendRaw(lixo, 199, 38);
      delay(100);
      irsend.sendRaw(set25RawData, 199, 38);
      irsend.sendRaw(set25RawData, 199, 38);
      set_Display("Temperatura: 25");
    break;
    }
  }
}

void set_Display(String msg){
  lcd.clear();
  for(int i = 0; i < msg.length(); i++ ) {
    lcd.setCursor(i, 0);
    lcd.print(msg.charAt(i));
  }
}

void controle(){
  if (irrecv.decode(&results))   { 
    Serial.println("Sinal InfraRecebido");
    estado_ant = estado;
    switch (results.value) {
      case 16720605: { //ligar (seta esquerda)          
          estado = 1; 
        break;
      }
      case 16761405:{   //desligar (seta direita) 
          estado = 0;
        
        break;
      }
      case 16736925:   { //seta pra cima
        if(estado == 1 ) estado = 22;
        if(estado < 25) estado++;
        
        break;
      }
      case 16754775:  {//seta pra baixo
        if(estado == 1 ) estado = 22;
        if(estado > 17) estado--;
        break;
      }
     
    }
    irrecv.resume(); // Recebe o proximo valor do botÃ£o que pressionamos
  }
}
void setup() {
  WiFi.hostname("esp-controle-node00");
  irrecv.enableIRIn(); //inicia o receptor infravermelho (IR)
  irsend.begin();
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  Serial.begin(115200);
  client.setServer(MQTT_BROKER, 1883);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  conectarWifi();
  reconectarMQTT();
  Serial.println("OK");
  set_Display("Desligado!");
}

void loop() { 
  
 if (!client.connected()) {
    reconectarMQTT();
 }
 
 controle();
  
 if(estado != estado_ant){
  
  if(estado == 1 and estado_ant == 0){ //está ligando o Ar 
      Serial.println("Ligou o Ar");
      set_Ar(estado);
      estado_ant = estado;
      publicarEstadoNoTopico();
      publicarTemperaturaArCondicionadoNoTopico();
      publicarBD();
    }
  if(estado >= 1 and estado <= 25 and estado >= 17){ //está alterando a temperatura
    Serial.print("Temperatura: ");
    set_Ar(estado);
    estado_ant = estado;
    Serial.println(temperatura);
    publicarTemperaturaArCondicionadoNoTopico();
    publicarBD();
    }
  if(estado == 0 and estado_ant != 0){ //está desligando o Ar 
    Serial.println("Desligou o Ar");
    set_Ar(estado);
    estado_ant = estado;
    publicarEstadoNoTopico();
    publicarBD();
    }
  }
  client.loop();
  yield(); //Dormir um pouco
}


  
