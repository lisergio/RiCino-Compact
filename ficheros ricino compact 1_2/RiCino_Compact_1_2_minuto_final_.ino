#include <IRremote.h>
#include <SD.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#define SD_ChipSelectPin 10
RF24 radio(15,16);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

int m,s,l; 
int mu=0,md=0,su=0,sd=0,lu=0,ld=0,lc=0;
long int tiempo,inicio,tiempo_parcial,tiempo_vuelta,tiempo_minimo_vuelta,tiempo_entrenos,tiempo_inicio,vuelta_rapida,control_vuelta_rapida;
int empezar_crono = LOW;
int numero_vueltas=0,numero_vuelta_rapida=0;
int config_numero_transponder=1;

int led_1=5;
int led_2=6;
int led_3=7;

int pulsador=3;
int selector=4;
int estado_pulsador;
int estado_selector;

File myFile;
boolean aviso_fin=LOW;

int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
int message;
int cronoiniciado=0;
char mensaje[3];
long int guarda_vueltas[55];
boolean a_vueltas;

float voltios_por_analog = 0.00481; //Valor en voltios de uno de los niveles de la entrada analógica (1-1023)
int num_resistores=5; //Número de resistencias de nuestro voltimetro
int entrada=3;
float valor_analog, voltios;
int led_voltios=9;

int menu=1;
long int Tx;

boolean aviso_ultimo_minuto=LOW;

//*********************************************************************************
void(* Resetea) (void) = 0;
//*********************************************************************************

void setup()
{
pinMode(led_1, OUTPUT);
pinMode(led_2, OUTPUT);
pinMode(led_3, OUTPUT);
pinMode(led_voltios, OUTPUT);
pinMode (entrada, INPUT);


vuelta_rapida=0;
control_vuelta_rapida=0;
numero_vuelta_rapida=0;
//*********************************** VALORES AJUSTABLES ****************************
tiempo_minimo_vuelta= 10000; //Tiempo en milisegundos 
tiempo_entrenos=600000;  //Tiempo en milisegundos
Tx=0x110;                // Transponder
//***********************************************************************************
digitalWrite(led_1, LOW);
digitalWrite(led_2, LOW);
digitalWrite(led_3, LOW);
estado_selector=digitalRead(selector);
if (estado_selector==LOW)
{
  a_vueltas=LOW; 
}
else
{
  a_vueltas=HIGH;  
}
radio.begin();
radio.setRetries(15,15);
radio.openWritingPipe(pipes[0]);
radio.openReadingPipe(1,pipes[1]);
delay(100);
radio.stopListening();
delay(100);
menu_1();
}
//*********************************************************************************
void loop()
{

}
//*********************************************************************************

void menu_aviso_ultimo_minuto()
{
aviso_ultimo_minuto=HIGH;
long int campana=6;
radio.stopListening();
bool ok = radio.write( &campana, sizeof(long int) );
Serial.println(campana);
radio.startListening();
}

//*********************************************************************************

void menu_1()
{
digitalWrite(led_1, HIGH);

vuelta_rapida=0;
numero_vuelta_rapida=0;
radio.startListening();
while (menu==1)
  {
    leer_voltios();
    estado_pulsador=digitalRead(pulsador);
    while (radio.available() )
    {      
      int dato;
      radio.read( &dato, sizeof(int) );
      if (dato == 1)
      {
        menu=2;
        radio.stopListening();
        menu_2();
      }
    }
    if (estado_pulsador==LOW)
    {
        menu=2;
        radio.stopListening();
        delay(250);
        menu_2();  
    }
    delay(50);
  }  
}
//*********************************************************************************
void menu_2()
{

irrecv.enableIRIn();
 
if (empezar_crono==LOW)
{
  leer_voltios();
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, HIGH);
  delay(500);
  long int ei=3;
  bool ok = radio.write( &ei, sizeof(long int) );
  delay(100);
  radio.write( &ei, sizeof(long int) );
  delay(100);
  radio.write( &ei, sizeof(long int) );
  delay(100);
  radio.write( &ei, sizeof(long int) );  
  radio.startListening();
  
}  
  while (menu==2)
  { 
   
    if (empezar_crono == HIGH)
    { 
    leer_voltios();
    estado_pulsador=digitalRead(pulsador);  
    while (radio.available() )
    {      
      int dato;
      radio.read( &dato, sizeof(int) );
      if (dato == 1)
      {
       
       final_entreno() ;
       
      }
    }
    if (estado_pulsador==LOW)
    {
        final_entreno() ;  
    } 
    digitalWrite(led_2, LOW);
    digitalWrite(led_3, HIGH);
    tiempo=millis()-tiempo_inicio;
  if (a_vueltas==LOW)
  {  
    if (tiempo>=tiempo_entrenos-60000)
    {
      if(aviso_ultimo_minuto==LOW)
      {
        menu_aviso_ultimo_minuto();
      }
    }
  }  
      if (irrecv.decode(&results))       
      {      
        if(results.value==Tx)contar_vuelta(); 
        irrecv.resume();
      }    
    }         
    else
    { 
    leer_voltios();
    estado_pulsador=digitalRead(pulsador);  
    while (radio.available() )
    {      
      int dato;
      radio.read( &dato, sizeof(int) );
      if (dato == 1)
      {    
       final_entreno() ;       
      }
    }
    if (estado_pulsador==LOW)
    {
        final_entreno() ;  
    } 
      if (irrecv.decode(&results))       
      {
        if(results.value==Tx)contar_vuelta();
             
        irrecv.resume();
      }    
    }  
  }
}
//*********************************************************************************
void contar_vuelta()
{
radio.stopListening();
delay(100);
if (empezar_crono == LOW)
{  
  empezar_crono = HIGH;
  tiempo_inicio = millis();    
  long int ti=4;
  bool ok = radio.write( &ti, sizeof(long int) );
  delay(100); 
  radio.write( &ti, sizeof(long int) );
  delay(100); 
  radio.write( &ti, sizeof(long int) );
  delay(100);
  radio.write( &ti, sizeof(long int) );
  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, HIGH);
  digitalWrite(led_3, HIGH);   
  delay(250);
  digitalWrite(led_1, LOW);
  digitalWrite(led_2, LOW);
  digitalWrite(led_3, LOW); 

}
else
 {
   tiempo_vuelta =(tiempo)-(tiempo_parcial);  
   if (tiempo_vuelta<tiempo_minimo_vuelta)
   {          
    
   } 
   else
   { 
      digitalWrite(led_1, HIGH);
      digitalWrite(led_2, HIGH);
      digitalWrite(led_3, HIGH);   
      delay(250);
      digitalWrite(led_1, LOW);
      digitalWrite(led_2, LOW);
      digitalWrite(led_3, LOW);     
     tiempo_parcial = tiempo;
     int v; 
     numero_vueltas++;
     v = numero_vueltas;
     control_vuelta_rapida=tiempo_vuelta;   
     bool ok = radio.write( &tiempo_vuelta, sizeof(long int) );
     delay(100);
     radio.write( &tiempo_vuelta, sizeof(long int) );
     delay(100);
     radio.write( &tiempo_vuelta, sizeof(long int) );
     delay(100);
     radio.write( &tiempo_vuelta, sizeof(long int) );
     if(numero_vueltas<=50)
     {
       int posicion_array;
       posicion_array=numero_vueltas-1;
       guarda_vueltas[posicion_array]=tiempo_vuelta;  
       if (vuelta_rapida==0)
       {
         vuelta_rapida=control_vuelta_rapida;
       }
       else
       {
         if (control_vuelta_rapida<vuelta_rapida)
         {
           vuelta_rapida=control_vuelta_rapida;
           numero_vuelta_rapida=numero_vueltas;
           long int vr=1;
           delay(3000); 
           bool ok = radio.write( &vr, sizeof(long int) );
           radio.write( &vr, sizeof(long int) );
           delay(200);
           radio.write( &vr, sizeof(long int) );
           delay(200);
           radio.write( &vr, sizeof(long int) );
           delay(200);
           radio.write( &vr, sizeof(long int) );

         }
       }      
     }
     else
     {
     if (numero_vuelta_rapida==0)
     {
      numero_vuelta_rapida=1;
     } 
      
      final_entreno(); 
     }    
     }
    } 
  radio.startListening();
  irrecv.enableIRIn();
  if (a_vueltas==LOW)
  {
    if(tiempo>=tiempo_entrenos)
    {
      final_entreno();
    }
  }

}
//*********************************************************************************
void final_entreno()
{ 
radio.stopListening();
delay(250);
digitalWrite(led_1, HIGH);
digitalWrite(led_2, LOW);
digitalWrite(led_3, LOW);
long int fe=2;
bool ok = radio.write( &fe, sizeof(long int) );
delay(100);
radio.write( &fe, sizeof(long int) );
delay(100);
radio.write( &fe, sizeof(long int) );
delay(100);
radio.write( &fe, sizeof(long int) );
if (numero_vueltas>0)
{
guardar_datos_sd_entreno();
}
Resetea();
}
//*********************************************************************************
void guardar_datos_sd_entreno()
{

if (!SD.begin(SD_ChipSelectPin)) 
{
  digitalWrite(led_1, HIGH);
  delay(500);
  digitalWrite(led_1, LOW);
} 




char nombrefichero[12];
int i = 1;
int exist = LOW ;
while (exist == LOW)
{
  sprintf(nombrefichero, "ENTRENO/%d.txt",i);
  if (SD.exists(nombrefichero))
  {
    i = i+1;
  }
  else
  {
    exist = HIGH;
  }    
}
SD.mkdir("ENTRENO"); 

myFile = SD.open(nombrefichero, FILE_WRITE);
if (myFile) 
  {
   myFile.println(F("******************** INICIO DEL ENTRENO ********************"));
   myFile.println(F(""));
   myFile.println(F(""));
   myFile.print(F("Tiempo total del entreno: "));
   m=tiempo/60000;                     
   mu=m%10;                            
   md=(m-mu)/10;                       
   s=(tiempo/1000)-(m*60);             
   su=s%10;                           
   sd=(s-su)/10;                       
   l=tiempo-(s*1000)-(m*60000);        
   lu=l%10;                           
   ld=((l-lu)/10)%10;                  
   lc=(l-(ld*10)-lu)/100;       
   myFile.print(md);                      
   myFile.print(mu);               
   myFile.print(F(":"));
   myFile.print(sd);
   myFile.print(su);
   myFile.print(F(":"));
   myFile.print(lc);
   myFile.print(ld);
   myFile.println(lu); 
   myFile.println(F(""));
   myFile.print(F("Total de vueltas:         "));   
   myFile.println(numero_vueltas);
   myFile.println(F(""));
   myFile.print(F("Vuelta rapida - Vuelta "));
   if (numero_vuelta_rapida==0)
    {
      numero_vuelta_rapida=1;
    }
   myFile.print(numero_vuelta_rapida);
   myFile.print(F(" en: "));
   m=vuelta_rapida/60000;                     
   mu=m%10;                            
   md=(m-mu)/10;                       
   s=(vuelta_rapida/1000)-(m*60);             
   su=s%10;                           
   sd=(s-su)/10;                       
   l=vuelta_rapida-(s*1000)-(m*60000);        
   lu=l%10;                           
   ld=((l-lu)/10)%10;                  
   lc=(l-(ld*10)-lu)/100;                                                    
   myFile.print(md);                      
   myFile.print(mu);               
   myFile.print(F(":"));
   myFile.print(sd);
   myFile.print(su);
   myFile.print(F(":"));
   myFile.print(lc);
   myFile.print(ld);
   myFile.println(lu);
   myFile.println(F(""));
 
   myFile.print(F("Tiempo Medio:      "));
   int posicion_array = 0;
   long int tiempo_suma_vueltas = 0;
   
   while (posicion_array<numero_vueltas)
    {
     tiempo_suma_vueltas = tiempo_suma_vueltas + guarda_vueltas[posicion_array]; 
     posicion_array=posicion_array+1;
    } 
   
   long int media = 0;
  
   media = tiempo_suma_vueltas / numero_vueltas;

   m=media/60000;                     
   mu=m%10;                            
   md=(m-mu)/10;                       
   s=(media/1000)-(m*60);             
   su=s%10;                           
   sd=(s-su)/10;                       
   l=media-(s*1000)-(m*60000);        
   lu=l%10;                           
   ld=((l-lu)/10)%10;                  
   lc=(l-(ld*10)-lu)/100;       
   myFile.print(md);                      
   myFile.print(mu);               
   myFile.print(F(":"));
   myFile.print(sd);
   myFile.print(su);
   myFile.print(F(":"));
   myFile.print(lc);
   myFile.print(ld);
   myFile.println(lu);    
   myFile.println(F("")); 
   myFile.println(F(""));    
   myFile.println(F("")); 
   myFile.println(F(""));    



   myFile.println(F("RESUMEN DE VUELTAS:"));
   myFile.println(F(""));
   
   posicion_array = 0;
   
   while (posicion_array<numero_vueltas)
    {
     myFile.print(posicion_array+1);
     myFile.print(F(" -- "));
     tiempo_vuelta=guarda_vueltas[posicion_array];
     m=tiempo_vuelta/60000;                     
     mu=m%10;                            
     md=(m-mu)/10;                       
     s=(tiempo_vuelta/1000)-(m*60);             
     su=s%10;                           
     sd=(s-su)/10;                       
     l=tiempo_vuelta-(s*1000)-(m*60000);        
     lu=l%10;                           
     ld=((l-lu)/10)%10;                  
     lc=(l-(ld*10)-lu)/100;                                                    
     myFile.print(md);                      
     myFile.print(mu);               
     myFile.print(F(":"));
     myFile.print(sd);
     myFile.print(su);
     myFile.print(F(":"));
     myFile.print(lc);
     myFile.print(ld);
     myFile.println(lu); 
     posicion_array=posicion_array+1;
     delay(50);
  }
   myFile.println(F(""));
   myFile.println(F("********************* FIN DEL ENTRENO **********************"));
   myFile.println(F(""));
   myFile.println(F(""));
//*******************************************************************************   
   myFile.println(F(""));
   myFile.close();   
  }
}


void leer_voltios()
{
  valor_analog = analogRead (entrada);
  voltios = valor_analog*voltios_por_analog*num_resistores;
  voltios = voltios - 0.38;
  if (voltios<=7.00)
  {
    digitalWrite(led_voltios, HIGH);
  }
  if (voltios<=6.80)
  {
    digitalWrite(led_voltios, HIGH);
    digitalWrite(led_1, LOW);
    digitalWrite(led_2, LOW);
    digitalWrite(led_3, LOW);
    
    while (voltios<=6.80)
    {
      valor_analog = analogRead (entrada);
      voltios = valor_analog*voltios_por_analog*num_resistores;
      voltios = voltios - 0.38;
     digitalWrite(led_voltios, LOW); 
     delay(300);
     digitalWrite(led_voltios, HIGH); 
     delay(300);    
    }
    
  }
}

