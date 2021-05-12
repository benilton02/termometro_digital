/*
                  int.0   int.1   int.2   int.3   int.4   int.5
 Uno              2       3
 Mega2560         2       3       21      20      19      18
 Leonardo         3       2       0       1
*/

#include <idDHT11.h>

#define buzzer_pin 3       // Definir a porta 03 para acionar Buzzer
#define idDHT11pin 2       //Porta Digital do Arduino onde o Sinal do Sensor DHT esta conectado
#define idDHT11intNumber 0 //Número da interrupção respectiva à porta definida no parametro anterior (veja tabela acima)

void dht11_wrapper();
void loopDHT();

idDHT11 DHT11(idDHT11pin, idDHT11intNumber, dht11_wrapper);   //Instanciação do Objeto de Controle do Sensor

float max_temp = 0;
float temperaturaC;
float min_temp;
int controller = 0;

void setup(){
  Serial.begin(9600);
  pinMode(buzzer_pin, OUTPUT);
}


void loop(){

  loopDHT(); 

    if (temperaturaC < 25.2 || temperaturaC > 32.6){
      Serial.print(temperaturaC, 2); // debug
      Serial.println(" °C"); // debug
      delay(300);
      digitalWrite(buzzer_pin, LOW); 
      delay(600);
      digitalWrite(buzzer_pin, HIGH);
   
  }
    if (temperaturaC > max_temp){
      max_temp = temperaturaC;
      Serial.print(max_temp, 2); // debug
      Serial.println(" °C MAX_TEMP"); // debug
    } 
  
    if (temperaturaC < min_temp){
      min_temp = temperaturaC;
      Serial.print(min_temp, 2); // debug
      Serial.println(" °C MIN_TEMP"); // debug
    }

    else{
      digitalWrite(buzzer_pin, LOW);  
      Serial.print(temperaturaC, 2); // debug
      Serial.println(" °C"); // debug
      delay(500);
    } 
}


void dht11_wrapper() {
  DHT11.isrCallback();
}


void loopDHT() {
  #define tempoLeitura 1000
  static unsigned long delayLeitura = millis() + tempoLeitura + 1;
  static bool request = false;

  if ((millis() - delayLeitura) > tempoLeitura) { 
      if (!request) {
        DHT11.acquire(); 
        request = true;
      }
  }

  if (request && !DHT11.acquiring()) {
    request = false;

    float valor = DHT11.getCelsius();
     
    if (!isnan(valor)) {             
      temperaturaC = valor;

      if (controller == 0 ){   
        min_temp = temperaturaC;
        max_temp = temperaturaC;
    
        Serial.print(min_temp, 2); // debug
        Serial.println(" °C MIN"); // debug
    
        Serial.print(max_temp, 2); // debug
        Serial.println(" °C MAX"); // debug
        controller = 1;
      }
    }

    delayLeitura = millis();     

  }
}