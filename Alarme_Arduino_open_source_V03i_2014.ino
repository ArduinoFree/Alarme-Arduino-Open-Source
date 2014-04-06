/*
ALARME ARDUINO - OPEN SOURCE - Versão 03i_2014
Versão para Desenvolvimento

CRIADO POR: GUSTAVO MURTA
DATA: 03/04/2014
USANDO ARDUINO 2009 - IDE Versão 1.05
E-MAIL: jgustavoam@gmail.com
Permitido o uso público, mas devem-se manter os nomes dos autores.

Informações sobre o Projeto:
http://labdegaragem.com/forum/topics/projeto-alarme-sem-fio-arduino-open-source

Fontes dos programas:
https://github.com/ArduinoFree/Alarme-Arduino-Open-Source

Rotinas de DECODER para HT6P20B 
CRIADAS POR:  
AFONSO CELSO TURCATO 
http://acturcato.wordpress.com/2013/12/20/decodificador-para-o-encoder-ht6p20b-em-arduino/
JACQUES MORESCO
http://forum.arduino.cc/index.php/topic,54788.msg707885.html#msg707885

*/

#include <Wire.h>

//Declaração das variáveis referentes às entradas e saídas do Arduino 2009

int Led_Verde = 8;            // digital pino 08
int Led_Verm = 9;             // digital pino 09
int Apito = 10;               // digital pino 10
int Sirene = 11;              // digital pino 11 - simulado por led Azul 
int valor_SensorSF = 1;
int valor_Controle_remoto = 1;
long tempo_apito = 0;         // Duração de tempo do Apito  
boolean Alarme_ativado = false;
unsigned long Proc_timer = 0;          // Valor do Timer do Processador 
unsigned long duracao=0;
int i = 0;

byte pinRF;                  // Pino onde o Módulo de Recepção de RF é conectado
boolean startbit;
boolean dataok;
boolean anticodeok;
int counter;                // Contador dos pulsos recebidos : 22 de endereçamento + 2 de dados + 4 de Anti-Code
int lambda;                 // Período do clock do chip HT6P20B  (fosc = 2KHz => Período = 500 us)
int dur0, dur1;             // Duração de pulsos (auxiliar)
unsigned long buffer=0;     // Buffer para armazeanmento do Código recebido 
unsigned long addr=0;       // Endereçamento do Chip HT6P20B

const int Porta_I2C = 27;   // Endereço da Porta I2C do Arduino

void setup() 
{
  Serial.begin(9600);       //Inicializando o monitor serial 
  
  //Definindo pinos como entrada ou saída
  
  pinMode(Led_Verde,OUTPUT);
  pinMode(Led_Verm,OUTPUT);
  pinMode(Apito,OUTPUT);
  pinMode(Sirene,OUTPUT);           // Led Azul simulando a Sirene
  
  pinRF = 2;                        // digital pino 02 - Conectado ao circuito de recepção do sinal de RF
  pinMode(pinRF, INPUT);

  
  // Inicialização do Alarme
  
  digitalWrite(Led_Verde, LOW);                  // Led Verde aceso = Alarme Operacional 
  digitalWrite(Led_Verm, !Alarme_ativado);       // Led Vermelho apagado = Alarme Desativado 
  digitalWrite(Sirene, HIGH);                    // Sirene desativada (Led Azul) 
  noTone(Apito);                                 // Desliga o apito   
  Serial.println("Alarme Operacional "); 
  
  Wire.begin(Porta_I2C);                // Conecta a interface I2C com o endereço 27
  Wire.onReceive(Recebe_codigo);        // Função para receber os dados do codigo
   
}

 void Tempo_duracao()
 {
   //duracao = (millis() - Proc_timer);
   Serial.println ((millis() - Proc_timer));
 }  

// Função Apito (Buzzer)

 void Buzzer(long tempo_apito)
{
   tone(Apito,4000);                  // Ligando o Apito com uma frequencia de 4000 hz.
   delay(tempo_apito);                // Duração do Apito 
   noTone(Apito);                     // Desliga o apito
}

// Rotina usada para teste do Alarme

void Teste_ativado()                                     
  {
   if (Alarme_ativado == true)  Serial.println("Alarme Ativado = true");
   if (Alarme_ativado == false) Serial.println("Alarme Ativado = false"); 
  } 
 
// Função de Disparo do Alarme  

void Dispara_Alarme() 
{
  digitalWrite(Sirene, LOW);  
  delay (2000); 
  digitalWrite(Sirene, HIGH);
  //Serial.println("Alarme disparado !");
  //Serial.println("Acione o controle remoto em  ate 5 segundos para cancelar");
  
  
  /*for (int i=0; i<5; i++)                     // tempo de espera para cancelamento do acionamento da Sirene = 5 segundos
  {       
  Proc_timer = millis();                      // Timer do Processador 
  
  if  ((millis() - Proc_timer) > 1)
    {
    digitalWrite(Led_Verm, LOW);                // Acende o led vermelho
    tone (Apito,4000,200);                      // Um Bip 4 KHz 200ms  
    }
  
  if ((millis() - Proc_timer) > 250)
    {
    digitalWrite(Led_Verm, HIGH);               // Apaga o led vermelho
    noTone(Apito);  
    }
  
  if  ((millis() - Proc_timer) > 450)
    {
    digitalWrite(Led_Verm, LOW);                // Acende o led vermelho
    tone (Apito,4000,200);                      // Um Bip 4 KHz 200ms  
    }
  
  if  ((millis() - Proc_timer) > 650)
    {
    digitalWrite(Led_Verm, HIGH);               // Apaga o led vermelho
    noTone(Apito);  
    }  
    
    
  if  ((millis() - Proc_timer) > 1050)  
    {
    Serial.print("Espera = ");
    Serial.println(i);                          // Contador de tempo de espera 
    Serial.println ((millis() - Proc_timer));   // Duração de tempo da rotina    
    }       
  Dispara_Sirene();
  } */
}  
  
void Dispara_Sirene()
{  
  digitalWrite(Sirene, LOW);          // Liga a Sirene (Led Azul) 
  Serial.println("Sirene ligada");
  
  for (int i=0; i<20; i++){           // Tempo que a Sirene ficará ligada
        
    digitalWrite(Led_Verm, HIGH);     // Apaga o led vermelho
    delay(100);                       // Duração do Apito = 100 ms
    
    digitalWrite(Led_Verm, LOW);     // Acende o led vermelho
    Buzzer(100);                     // Duração do Apito = 100 ms 
   
    } 
    
   digitalWrite(Sirene, HIGH);          // Desliga a Sirene (Led Azul) 
   Serial.println("Sirene desligada");   
   if (Alarme_ativado == true)  Serial.println("Alarme Ligado");
      
}

void Liga_Alarme() 
{
  Alarme_ativado = true;
  digitalWrite(Led_Verm,!Alarme_ativado); 
  Serial.println("Alarme Ligado ");
  tone (Apito,4000,300);                           // Um Bip 4 KHz 200ms
}

void Desliga_Alarme() 
{
  Alarme_ativado = false;
  digitalWrite(Led_Verm,!Alarme_ativado); 
  digitalWrite(Sirene, HIGH);                      // Desliga a Sirene (Led Azul)
  Serial.println("Alarme Desligado");
  tone (Apito,4200,300);                           // Um bip 4 KHz 200ms
    
}

void Recebe_codigo (int Numero_bytes)              //   Recebe quatro bytes de Codigo
{
  Numero_bytes = 4;                                // 4 bytes para receber
  byte Codigo_1,Codigo_2,Codigo_3,Codigo_4 = 0;    // Bytes do codigo recebido
  digitalWrite(13, HIGH);                          // Led L do Arduino aceso
  
  while(Wire.available() > 0)
  {  
    Codigo_1 = Wire.read();                        // recebe o primeiro byte
    Codigo_2 = Wire.read();                        // recebe o segundo byte
    Codigo_3 = Wire.read();                        // recebe o terceiro byte
    Codigo_4 = Wire.read();                        // recebe o quarto byte
  }    
    
  buffer = Codigo_4 ;                             // remontando o codigo recebido 
  buffer = Codigo_3 + (buffer << 8);              // deslocamento dos bytes para a esquerda
  buffer = Codigo_2 + (buffer << 8);
  buffer = Codigo_1 + (buffer << 8);
  
  Serial.println (); 
  Serial.println(buffer,HEX); 
  digitalWrite(13, LOW);                          // Led L do Arduino apagado
  
   // Verifica o Código do dispositivo sem Fio 
  
  switch(buffer)
       {      
        case 0x0835C815:                                              // Código do Controle Remoto 
             Liga_Alarme(); 
             break;
            
        case 0x0835C825:  
             Desliga_Alarme(); 
             break; 
          
       case 0x08516E55:                                              // Sensor Sem Fio 
            if (Alarme_ativado == true) Dispara_Alarme();  
            break;  
           
       default: 
            break;
       }    
}

void Monitor_RF()
{  
  //Proc_timer = millis();                   // Timer do Processador 
  if (!startbit)
  {// Check the PILOT CODE until START BIT;
    dur0 = pulseIn(pinRF, LOW);            //Check how long DOUT was "0" (ZERO) (refers to PILOT CODE)
        
     //If time at "0" is between 10350 us (23 cycles of 450us) and 12650 us (23 cycles of 550 us).

     if((dur0 > 10350) && (dur0 < 12650) && !startbit)
    {    
      lambda = dur0 / 23;  //calculate wave length - lambda
      
      dur0 = 0;
      buffer = 0;
      counter = 0;
      
      dataok = false;
      startbit = true;
    }
  }

  // If Start Bit is OK, then starts measure os how long the signal is level "1" and check is value is into acceptable range.

  if (startbit && !dataok && counter < 28)
  {
    ++counter;
    
    dur1 = pulseIn(pinRF, HIGH);

    //If pulse width at "1" is between "0.5 and 1.5 lambda", means that pulse is only one lambda, so the data Ã© "1".
    
    if((dur1 > 0.5 * lambda) && (dur1 < (1.5 * lambda)))  

    //{
      buffer = (buffer << 1) + 1;      // add "1" on data buffer
    //}

   //If pulse width at "1" is between "1.5 and 2.5 lambda", means that pulse is two lambdas, so the data Ã© "0".

    else if((dur1 > 1.5 * lambda) && (dur1 < (2.5 * lambda)))  

    //{
      buffer = (buffer << 1);       // add "0" on data buffer
    //}
    else
    {
      //Reset the loop
      startbit = false;
      digitalWrite(13, LOW);   // Led Vermelho apagado
    }
  }
  
  //Check if all 28 bits were received (22 of Address + 2 of Data + 4 of Anti-Code)

  if (counter==28) 
  { 
    // Check if Anti-Code is OK (last 4 bits of buffer equal "0101")

    if ((bitRead(buffer, 0) == 1) && (bitRead(buffer, 1) == 0) && (bitRead(buffer, 2) == 1) && (bitRead(buffer, 3) == 0))
    //{
      anticodeok = true;
    //}
    else
    {
      //Reset the loop
      startbit = false;
      digitalWrite(13, LOW);                  // Led Vermelho apagado
    }
    
  if (anticodeok)
    {
                 
      addr = buffer >> 6;      
                  
      Serial.println();
      Serial.print("-Codigo: ");
      Serial.println(addr, HEX);
      
           
    }
  }
}

void loop() 
{ 
}



