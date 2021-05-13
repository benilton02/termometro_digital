#include <ThreadController.h>
#include <idDHT11.h>
#include <Thread.h>


#define idDHT11intNumber 0 
#define readingTime 1000
#define threadSleep 700
#define idDHT11pin 2       
#define buzzer_pin 3       


void dht11Wrapper();
void minCelsius();
void maxCelsius();
void outOfRange();
void loopDHT();


idDHT11 DHT11(idDHT11pin, idDHT11intNumber, dht11Wrapper);   
ThreadController threadController = ThreadController();
ThreadController groupOfThreads = ThreadController();
Thread minCelsiusThread = Thread();
Thread maxCelsiusThread = Thread();
Thread outOfRangeThread = Thread();


static bool controller = false;
float maxValueCelsius;
float minValueCelsius;
float celsius;


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
  if (celsius < 25.2 || celsius > 32.6){
    Serial.print(celsius, 2); // debug
    Serial.println(" °C"); // debug
    delay(200);
    digitalWrite(buzzer_pin, HIGH); 
    delay(200);
    digitalWrite(buzzer_pin, LOW);   
  }
  else{
    Serial.print(celsius, 2); // debug
    Serial.println(" °C"); // debug
  }
}


void minCelsius(){
  if (celsius < minValueCelsius){
    minValueCelsius = celsius;
    Serial.print(minValueCelsius, 2); // debug
    Serial.println(" °C minValueCelsius"); // debug
  }  
}


void maxCelsius(){
  if (celsius > maxValueCelsius){
    maxValueCelsius = celsius;
    Serial.print(maxValueCelsius, 2); // debug
    Serial.println(" °C maxValueCelsius"); // debug
  } 
}


void createThread(){
  minCelsiusThread.setInterval(threadSleep);
  maxCelsiusThread.setInterval(threadSleep);
  outOfRangeThread.setInterval(threadSleep);
  
  minCelsiusThread.onRun(minCelsius);
  maxCelsiusThread.onRun(maxCelsius);
  outOfRangeThread.onRun(outOfRange);
}


void createGroup(){
  groupOfThreads.add(&minCelsiusThread);
  groupOfThreads.add(&maxCelsiusThread);
  groupOfThreads.add(&outOfRangeThread);
  
  threadController.add(&groupOfThreads);
}


void dht11Wrapper() {
  DHT11.isrCallback();
}


void loopDHT() {
  static unsigned long readingDelay = millis() + readingTime + 1;
  static bool request = false;

  if ((millis() - readingDelay) > readingTime) { 
      if (!request) {
        DHT11.acquire(); 
        request = true;
      }
  }

  if (request && !DHT11.acquiring()) {
    request = false;

    float valor = DHT11.getCelsius();
     
    if (!isnan(valor)) {             
      celsius = valor;

      if (!controller){   
        minValueCelsius = celsius;
        maxValueCelsius = celsius;
    
        Serial.print(minValueCelsius, 2); // debug
        Serial.println(" °C MIN init"); // debug
    
        Serial.print(maxValueCelsius, 2); // debug
        Serial.println(" °C MAX init"); // debug
        
        controller = true;
      }
    }

    readingDelay = millis();     

  }
}