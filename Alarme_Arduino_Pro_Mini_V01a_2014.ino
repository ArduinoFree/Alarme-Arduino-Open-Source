
/*
ALARME ARDUINO - OPEN SOURCE - Pro Mini Versão 01a_2014
Versão para Desenvolvimento

CRIADO POR: GUSTAVO MURTA
DATA: 30/03/2014
USANDO ARDUINO Pro Mini - IDE Versão 1.05
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

//Declaração das variáveis

#include <Wire.h>

byte pinRF;                              // Pino Digital para conexão com Módulo de RF
boolean startbit;
boolean dataok;
boolean anticodeok;
int counter;                             //received bits counter: 22 of Address + 2 of Data + 4 of EndCode (Anti-Code)
int lambda;                              // on pulse clock width (if fosc = 2KHz than lambda = 500 us)
int dur0, dur1;                          // Duração de pulsos (auxiliar) 
unsigned long buffer=0;                  // Buffer para recebimento dos bytes do Código do Controle Remoto

const int Porta_I2C = 27;                // Endereço da Porta I2C do Arduino Pro Mini


void setup() 
{
  Serial.begin(9600);                    //Inicializando o monitor serial (opcional) - use interface USB-Serial    
  pinRF = 2;                             // Pino Digital 2 conectado ao Módulo de RF
  pinMode(pinRF, INPUT);                 // Configurando o Pino de RF como entrada   
  Wire.begin();                          // Inicialização da Interface I2C   
  Serial.println("Modulo de Descodificacao Pronto ");   
}



void Monitor_RF()
{  
  if (!startbit)
  {// Verifica o Código Piloto até o bit de Start ;
    dur0 = pulseIn(pinRF, LOW);        // Verifica a duração de tempo para nível Zero
        
     // Se o nível Zero estiver entre 10350 us (23 ciclos de 450us) e 12650 us (23 ciclos de 550 us).
     if((dur0 > 10350) && (dur0 < 12650) && !startbit)
    {    
      lambda = dur0 / 23;              // Cálculo do comprimento de onda - lambda
      
      dur0 = 0;
      buffer = 0;
      counter = 0;
      
      dataok = false;
      startbit = true;
    }
  }

  // Se o Start bit estiver OK, inicia medida de tempo do nível UM e verifica se o o valor é aceitável.
  if (startbit && !dataok && counter < 28)
  {
    ++counter;
    
    dur1 = pulseIn(pinRF, HIGH);
    
    //If pulse width at "1" is between "0.5 and 1.5 lambda", means that pulse is only one lambda, so the data Ã© "1".
    if((dur1 > 0.5 * lambda) && (dur1 < (1.5 * lambda)))  
    {
      buffer = (buffer << 1) + 1;      // adiciona UM ao buffer de código
    }
    //If pulse width at "1" is between "1.5 and 2.5 lambda", means that pulse is two lambdas, so the data Ã© "0".
    else if((dur1 > 1.5 * lambda) && (dur1 < (2.5 * lambda)))  
    {
      buffer = (buffer << 1);        // adiciona ZERO ao buffer de código 
    }
    else
    {
      startbit = false;               //Reset do laço 
      digitalWrite(13, LOW);          // Led L do Arduino apagado 
    }
  }
  
  //Verifica se todos os 28 bits foram recebidos (22 de endereço + 2 de dados + 4 de Anti-Code)
  if (counter==28) 
  { 
    // Verifica se o Anti-Code esta OK ( ultimos 4 bits do buffer igual à "0101")
    if ((bitRead(buffer, 0) == 1) && (bitRead(buffer, 1) == 0) && (bitRead(buffer, 2) == 1) && (bitRead(buffer, 3) == 0))
    {
      anticodeok = true;
    }
    else
    {
      startbit = false;               //Reset do laço
      digitalWrite(13, LOW);          // Led L do Arduino apagado 
    }
    
  if (anticodeok)
    {
      dataok = true;      
      counter = 0;
      startbit = false;
      anticodeok = false;
      
      byte Codigo_1,Codigo_2,Codigo_3,Codigo_4 = 0; 
      
      digitalWrite(13, HIGH);         // Led L Arduino aceso
      
      Serial.print("Codigo enviado : ");
      Serial.println(buffer, HEX);
             
      Codigo_1 = buffer ;
      Serial.print("Codigo1 = ");
      Serial.println(Codigo_1,HEX);
      
      Codigo_2 = buffer>>8 ;
      Serial.print("Codigo2 = ");
      Serial.println(Codigo_2,HEX);
      
      Codigo_3 = buffer>>16 ;
      Serial.print("Codigo3 = ");
      Serial.println(Codigo_3,HEX);
      
      Codigo_4 = buffer>>24 ;
      Serial.print("Codigo4 = ");
      Serial.println(Codigo_4,HEX);
      Serial.println();
      
      
      Wire.beginTransmission(Porta_I2C);       // Começa a transmissão através da Porta I2C - endereço 27 
      Wire.write(Codigo_1);                    // envia primeiro Byte
      Wire.write(Codigo_2);                    // envia segundo Byte
      Wire.write(Codigo_3);                    // envia terceiro Byte
      Wire.write(Codigo_4);                    // envia quarto Byte
      Wire.endTransmission();                  // Finaliza a transmissão 
      
      digitalWrite(13, LOW);                   // Led L do Arduino apagado 
           
    }
  }
}

void loop() 
{ 
  Monitor_RF();
}



