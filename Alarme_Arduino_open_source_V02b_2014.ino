/*
ALARME ARDUINO - OPEN SOURCE - Versão 02b_2014

CRIADO POR: GUSTAVO MURTA
DATA: 23/02/2014
USANDO ARDUINO 2009 - Versão 1.05
E-MAIL: jgustavoam@gmail.com
Permitido o uso público, mas devem-se manter os nomes dos autores.

http://labdegaragem.com/forum/topics/projeto-alarme-sem-fio-arduino-open-source

Rotinas de DECODER para HT6P20B 
CRIADAS POR:  
AFONSO CELSO TURCATO 
http://acturcato.wordpress.com/2013/12/20/decodificador-para-o-encoder-ht6p20b-em-arduino/
JACQUES MORESCO
http://forum.arduino.cc/index.php/topic,54788.msg707885.html#msg707885

*/

//Declaração das variáveis referentes às entradas e saídas do Arduino 2009

int SensorSF = 3;             // digital pino 03 - botão teste 01
int Controle_remoto = 4;      // digital pino 04 - botão teste 02
int Led_Verde = 8;            // digital pino 08
int Led_Verm = 9;             // digital pino 09
int Apito = 10;               // digital pino 10
int Sirene = 11;              // digital pino 11 - simulado por led Azul 
int valor_SensorSF = 1;
int valor_Controle_remoto = 1;
unsigned int tempo_apito = 0;
boolean Alarme_ativado = false;
int i = 0;

byte pinRF;                  // Pin where RF Module is connected
boolean startbit;
boolean dataok;
boolean anticodeok;
int counter;                //received bits counter: 22 of Address + 2 of Data + 4 of EndCode (Anti-Code)
int lambda;                 // on pulse clock width (if fosc = 2KHz than lambda = 500 us)
int dur0, dur1;             // pulses durations (auxiliary)
unsigned long buffer=0;     //buffer for received data storage


void setup() 
{
  Serial.begin(9600);       //Inicializando o monitor serial 
  
  //Definindo pinos como entrada ou saída
 
  //pinMode(SensorSF,INPUT);
  pinMode(Controle_remoto,INPUT);
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
  
}

// Função Apito (Buzzer)

void Buzzer(unsigned int tempo_apito)
{
   tone(Apito,4000);                  // Ligando o Apito com uma frequencia de 4000 hz.
   delay(tempo_apito);                // Duração do Apito 
   noTone(Apito);                     // Desliga o apito
}


//Função de monitoramento dos Sensores sem fio 

void Monitor_SensorSF()
{
  valor_SensorSF = digitalRead(SensorSF);
  if (valor_SensorSF == 0 && Alarme_ativado == 1){  // Somente dispara se o Alarme estiver ativado
      Serial.println("Sensor sem fio acionado"); 
      Dispara_Alarme() ;     
  }       
}

// Função de verificação do estado do Controle Remoto 

void CR_acionado()
{
   valor_Controle_remoto = digitalRead(Controle_remoto);  // Verifica se Controle remoto foi acionado      
}   

// Função de monitoramento do Controle Remoto 

void Monitor_CR() 
{
   CR_acionado();
   if (valor_Controle_remoto == 0)   
   {
   Alarme_ativado = !Alarme_ativado;                      // Controle remoto foi acionado
   Serial.print("Controle remoto acionado = ");
     if (Alarme_ativado == 1)
     {
       Liga_Alarme();
        } else {       
       Desliga_Alarme();
        }        
     }  
}
 
// Função de Disparo do Alarme  

void Dispara_Alarme() 
{
  
  Serial.println("Alarme disparado - acione o controle remoto em  5 segundos para cancelar");
  for (int i=0; i<5; i++)                 // tempo de espera para cancelamento do acionamento da Sirene
  {     
  
  digitalWrite(Led_Verm, LOW);            // Acende o led vermelho
  Buzzer(200);                            // Duração do Apito = 200 ms  
  CR_acionado();
    if (valor_Controle_remoto == 0){ break; }
   
  digitalWrite(Led_Verm, HIGH);          // Apaga o led vermelho
  delay(200);       // Atraso de 200 ms  
  CR_acionado();
    if (valor_Controle_remoto == 0){ break; }
         
  digitalWrite(Led_Verm, LOW);          // Acende o led vermelho
  Buzzer(400);                          // Duração do Apito = 400 ms
  CR_acionado();
    if (valor_Controle_remoto == 0){ break; }
    
  digitalWrite(Led_Verm, HIGH);        // Apaga o led vermelho
  delay(200);                          // Atraso de 200 ms
  CR_acionado();
    if (valor_Controle_remoto == 0){ break; }
  }       
  Dispara_Sirene();
}  
  
void Dispara_Sirene()
{  
  digitalWrite(Sirene, LOW);          // Liga a Sirene (Led Azul) 
  Serial.println("Sirene ligada");
  
  for (int i=0; i<20; i++){           // Tempo que a Sirene ficará ligada
 
    digitalWrite(Led_Verm, LOW);      // Acende o led vermelho
    Buzzer(100);                      // Duração do Apito = 100 ms 
    
    CR_acionado();
    if (valor_Controle_remoto == 0){ break; }
    
    digitalWrite(Led_Verm, HIGH);     // Apaga o led vermelho
    delay(100);                       // Duração do Apito = 100 ms
    
    CR_acionado();
    if (valor_Controle_remoto == 0){ break; }
  } 
   Alarme_ativado = !Alarme_ativado;
   Desliga_Alarme(); 
}

void Liga_Alarme() 
{
  digitalWrite(Led_Verm,!Alarme_ativado); 
  Serial.println("Alarme ativo");
  Buzzer (200);       // Um Bip 
  noTone(Apito);                           // Desliga o apito
}

// Função de desativação do Alarme  

void Desliga_Alarme() 
{
  digitalWrite(Sirene, HIGH);              // Desliga a Sirene (Led Azul)
  Serial.println("Alarme desativado");
  Buzzer (100);                            // Dois bips 
  delay(100);
  Buzzer (100);
  noTone(Apito);                           // Desliga o apito
  digitalWrite(Led_Verm,!Alarme_ativado);  // Led Vermelho apagado = Alarme Desativado 
}

void Monitor_RF()
{  
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

    {
      buffer = (buffer << 1) + 1;      // add "1" on data buffer
    }

   //If pulse width at "1" is between "1.5 and 2.5 lambda", means that pulse is two lambdas, so the data Ã© "0".

    else if((dur1 > 1.5 * lambda) && (dur1 < (2.5 * lambda)))  

    {
      buffer = (buffer << 1);       // add "0" on data buffer
    }
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
    {
      anticodeok = true;
    }
    else
    {
      //Reset the loop
      startbit = false;
      digitalWrite(13, LOW);                  // Led Vermelho apagado
    }
    
  if (anticodeok)
    {
      dataok = true;
      
      counter = 0;
      startbit = false;
      anticodeok = false;
      digitalWrite(13, HIGH);                  // Led Vermelho aceso
      
      Serial.print("Data: ");
      Serial.println(buffer, BIN);
      
      unsigned long addr = buffer >> 6;
      
      //Serial.println("Controle remoto acionado ! ");
      
      Serial.print("-Address: ");
      Serial.println(addr, HEX);
      
      Serial.println("-Button1: " + (String)bitRead(buffer, 4));
      Serial.println("-Button2: " + (String)bitRead(buffer, 5));
      Serial.println("-----------------------------------");
      
      // Ativa ou desativa Alarme  - coloque o código do seu controle remoto no lugar de 123456
      
         
      if ((addr == 0x123456) && (bitRead(buffer, 4)==1) && (bitRead(buffer, 5)==0))  // Botão 1 do controle remoto acionado 
       {
        Alarme_ativado = true;
        Liga_Alarme();
       }
      
      if ((addr == 0x123456) && (bitRead(buffer, 4)==0) && (bitRead(buffer, 5)==1))  // Botão 2 do controle remoto acionado
       {
        Alarme_ativado = false;
        Desliga_Alarme();
       }
         
    }
  }
}

void loop() 
{ 
  Monitor_RF();
}



