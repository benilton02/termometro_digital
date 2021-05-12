#include <Thread.h>
#include <ThreadController.h>
#include <idDHT11.h>


#define buzzer_pin 3       // Definir a porta 03 para acionar Buzzer
#define idDHT11pin 2       //Porta Digital do Arduino onde o Sinal do Sensor DHT esta conectado
#define idDHT11intNumber 0 //Número da interrupção respectiva à porta definida no parametro anterior (veja tabela acima)

void dht11_wrapper();
void loopDHT();
void outOfRange();
void minTemperature();
void maxTemperature();

ThreadController threadController = ThreadController();
ThreadController groupOfThreads = ThreadController();
Thread minTemperatureThread = Thread();
Thread maxTemperatureThread = Thread();
Thread outOfRangeThread = Thread();

idDHT11 DHT11(idDHT11pin, idDHT11intNumber, dht11_wrapper);   //Instanciação do Objeto de Controle do Sensor

float max_temp = 0;
float temperaturaC;
float min_temp;
int controller = 0;


void outOfRange(){
  if (temperaturaC < 25.2 || temperaturaC > 32.6){
    Serial.print(temperaturaC, 2); // debug
    Serial.println(" °C"); // debug
    delay(200);
    digitalWrite(buzzer_pin, HIGH); 
    delay(200);
    digitalWrite(buzzer_pin, LOW);   
  }
  else{
    Serial.print(temperaturaC, 2); // debug
    Serial.println(" °C"); // debug
  }
}


void minTemperature(){
  if (temperaturaC < min_temp){
    min_temp = temperaturaC;
    Serial.print(min_temp, 2); // debug
    Serial.println(" °C MIN_TEMP"); // debug
  }  
}


void maxTemperature(){
  if (temperaturaC > max_temp){
    max_temp = temperaturaC;
    Serial.print(max_temp, 2); // debug
    Serial.println(" °C MAX_TEMP"); // debug
  } 
}


void setup(){
  Serial.begin(9600);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW); 

  minTemperatureThread.setInterval(700);
  minTemperatureThread.onRun(minTemperature);

  maxTemperatureThread.setInterval(700);
  maxTemperatureThread.onRun(maxTemperature);

  outOfRangeThread.setInterval(700);
  outOfRangeThread.onRun(outOfRange);

  groupOfThreads.add(&minTemperatureThread);
  groupOfThreads.add(&maxTemperatureThread);
  groupOfThreads.add(&outOfRangeThread);
  
  threadController.add(&groupOfThreads);
  
}


void loop(){
  loopDHT(); 
  threadController.run();
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
        Serial.println(" °C MIN init"); // debug
    
        Serial.print(max_temp, 2); // debug
        Serial.println(" °C MAX init"); // debug
        controller = 1;
      }
    }

    delayLeitura = millis();     

  }
}