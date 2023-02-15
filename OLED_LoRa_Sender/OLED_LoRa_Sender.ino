#include <SPI.h> //lib para comunicação serial
#include <LoRa.h> //lib para comunicação com o WIFI Lora
#include <Wire.h>  //lib para comunicação i2c
#include "SSD1306.h" //lib para comunicação com o display

//Descomente as linhas abaixo para o tipo do sensor DHT que você estiver usando!

// Definição dos pinos 
#define SCK     5    // GPIO5  -- SX127x's SCK
#define MISO    19   // GPIO19 -- SX127x's MISO
#define MOSI    27   // GPIO27 -- SX127x's MOSI
#define SS      18   // GPIO18 -- SX127x's CS
#define RST     14   // GPIO14 -- SX127x's RESET
#define DI00    26   // GPIO26 -- SX127x's IRQ(Interrupt Request)

#define sensor1 36    //pino digital do sensor de umidade

#define BAND    915E6  //Frequencia do radio - podemos utilizar ainda : 433E6, 868E6, 915E6

int dig_value;       //variável usada para receber o sinal digital do sensor
int solo_limit = 0;   //usado para indicar o valor mínimo de acionamento do sensor de umidade (este valor varia conforme for ajustado o parafuso do sensor)

String packSize; //variável usada para receber tamanho do pacote convertido em String
String packet = "OK";   //variável usada para armazenar a string enviada, ao final ela é concatenada com a variável "values" abaixo
String values = "|-|-"; //valores de humidade e temperatura são separados por pipe

//parametros: address,SDA,SCL 
SSD1306 display(0x3c, 4, 15); //construtor do objeto que controlaremos o display

void setup()
{  
  //inicia serial com 9600 bits por segundo
  Serial.begin(115200);
  
  //configura pino digital do sensor como entrada
  pinMode(sensor1, INPUT_PULLUP);
  //configura os pinos oled como saida
  pinMode(16,OUTPUT); //pino RST do oled display
  //reseta o OLED
  digitalWrite(16, LOW);    
  //aguarda 50ms
  delay(50); 
  //enquanto o OLED estiver ligado, GPIO16 deve estar HIGH
  digitalWrite(16, HIGH); 
  //inicializa o display
  display.init(); 
  //inverte verticalmente o display (de ponta cabeça)
  display.flipScreenVertically();
  //configura a fonte para um tamanho maior
  display.setFont(ArialMT_Plain_10); 
  //aguarda 1500ms
  delay(1500);
  //apaga todo o conteúdo da tela do display
  display.clear();
  
  //inicia a comunicação serial com o Lora
  SPI.begin(SCK,MISO,MOSI,SS); 
  //configura os pinos que serão utlizados pela biblioteca (deve ser chamado antes do LoRa.begin)
  LoRa.setPins(SS,RST,DI00); 
  
  //inicializa o Lora com a frequencia específica
 
}

void loop()
{ 
  //apaga o conteúdo do display
  display.clear();
  //alinha texto do display à esquerda
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  //configura fonte do texto
  display.setFont(ArialMT_Plain_16);
  //exibe na pos 0,0 a mensagem entre aspas
  display.drawString(0, 0, "Running...");
  
  //se no pino digital do sensor receber sinal baixo, significa que foi detectado umidade
  if(HumidadeSolo())
  {
    //atribui à variável o texto SOL1
    packet = "SOL1";   
    //concatena com o pacote os valores, separados por pipe 
    packet+=values;
    //envia pacote por LoRa
    sendPacket();
    //chama função para exibir no display com flag true, indicando umidade detectado
    showDisplay(true);
    Serial.println("SOL1");
  }
  else
  { 
    //atribui à variável o texto OK
    packet = "OK"; 
    //concatena com o pacote os valores, separados por pipe 
    packet+=values;
    //envia pacote por LoRa
    sendPacket();
    //chama função para exibir no display, com flag false, indicando umidade não detectado
    showDisplay(false);
    Serial.println("ok");
  }
  //aguarda 250ms
  delay(250);
}

void sendPacket()
{
  //beginPacket : abre um pacote para adicionarmos os dados para envio
  LoRa.beginPacket();
  //envia pacote
  LoRa.print(packet);
  //endPacket : fecha o pacote e envia
  LoRa.endPacket(); //retorno= 1:sucesso | 0: falha
}
    
void showDisplay(bool HumidadeSolo)
{
  //apaga o conteúdo do display
  display.clear();
  //alinha texto do display à esquerda
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  //configura fonte do texto
  display.setFont(ArialMT_Plain_16);
  //exibe na pos 0,0 a mensagem
  display.drawString(0, 0, "Running...");

  //se o umidade foi detectado
  if(HumidadeSolo)
  {
    //escreve na pos 0,20 a mensagem
    display.drawString(0, 20, "Status: SOL1");
    //escreve na pos 0,40 a mensagem
    display.drawString(0, 40, "Baixa Humidade!");
    //exibe display
    display.display(); 
    Serial.println("SOL1");
    
  }  
  else
  {
    //escreve na pos 0,20 a mensagem
    display.drawString(0, 20, "Status: OK");    
    //escreve na pos 0,40 a mensagem  
   
    display.display(); 
    Serial.println("ok");
  }
}
//função responsável por disparar mensagem caso o sensor detectar umidade
bool HumidadeSolo()
{  
  dig_value = digitalRead(sensor1);
  if (sensor1 == 0){
    if(solo_limit == 1);
  //lê o valor digital do sensor, caso sinal baixo o umidade foi detectado
  Serial.println("Baixa umidade1 !!!");
  return true;

}
else
{
  Serial.println("Umidade normal...");
  if(solo_limit == 1)
  return false;
}
}
