#include <Thread.h>
#include <ThreadController.h>
#include <idDHT11.h>


#define buzzer_pin 3       
#define idDHT11pin 2       
#define idDHT11intNumber 0 
#define threadSleep 700


void minTemperature();
void maxTemperature();
void dht11_wrapper();
void outOfRange();
void loopDHT();


idDHT11 DHT11(idDHT11pin, idDHT11intNumber, dht11_wrapper);   
ThreadController threadController = ThreadController();
ThreadController groupOfThreads = ThreadController();
Thread minTemperatureThread = Thread();
Thread maxTemperatureThread = Thread();
Thread outOfRangeThread = Thread();


float temperaturaC;
float max_temp;
float min_temp;
static bool controller = false;


void setup(){
  Serial.begin(9600);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW); 

  createThread();
  createGroup();
}


void loop(){
  loopDHT(); 
  threadController.run();
}


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


void createThread(){
  minTemperatureThread.setInterval(threadSleep);
  maxTemperatureThread.setInterval(threadSleep);
  outOfRangeThread.setInterval(threadSleep);
  
  minTemperatureThread.onRun(minTemperature);
  maxTemperatureThread.onRun(maxTemperature);
  outOfRangeThread.onRun(outOfRange);
}


void createGroup(){
  groupOfThreads.add(&minTemperatureThread);
  groupOfThreads.add(&maxTemperatureThread);
  groupOfThreads.add(&outOfRangeThread);
  
  threadController.add(&groupOfThreads);
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

      if (!controller){   
        min_temp = temperaturaC;
        max_temp = temperaturaC;
    
        Serial.print(min_temp, 2); // debug
        Serial.println(" °C MIN init"); // debug
    
        Serial.print(max_temp, 2); // debug
        Serial.println(" °C MAX init"); // debug
        
        controller = true;
      }
    }

    delayLeitura = millis();     

  }
}