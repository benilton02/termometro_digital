

/*
                  int.0   int.1   int.2   int.3   int.4   int.5
 Uno              2       3
 Mega2560         2       3       21      20      19      18
 Leonardo         3       2       0       1
*/

#include <idDHT11.h>

#define idDHT11pin 2       //Porta Digital do Arduino onde o Sinal do Sensor DHT esta conectado
#define idDHT11intNumber 0 //Número da interrupção respectiva à porta definida no parametro anterior (veja tabela acima)

void dht11_wrapper();     // Declaração da funcão de controle da interrupção. 
void loopDHT();           // Atualiza a leitura do sensor

idDHT11 DHT11(idDHT11pin, idDHT11intNumber, dht11_wrapper);   //Instanciação do Objeto de Controle do Sensor

void setup(){
  Serial.begin(9600);
}


float temperaturaC;

void loop(){

  loopDHT(); 

  if (temperaturaC < 25.2 || temperaturaC > 32.6){
    Serial.println("Out of Range");
    // Acionar buzina
  }
  else{
    Serial.print(temperaturaC, 2);
    Serial.println(" °C");
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
    }

    delayLeitura = millis();     
  }
}