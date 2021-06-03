#include <LiquidCrystal_I2C.h>
#include <ThreadController.h>
#include <Thread.h>
#include <dht.h>

#define threadSleep 700
#define DHT11_PIN 2
#define address 0x27       
#define row 2
#define column 16
#define buzzer_pin 13
#define cf .2

LiquidCrystal_I2C lcd(address, column, row);
ThreadController threadController = ThreadController();
Thread showLcdThread = Thread();
Thread buzzerThread = Thread();
dht DHT;

void createThread();
void startBuzzer();
void setInitialValues();
void setLcd();
void showCelsius();
float minCelsius();
float maxCelsius();


volatile float minValueCelsius;
volatile float maxValueCelsius;
volatile float celsius;

void setup(){
  Serial.begin(9600);

  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW); 
  
  setInitialValues();
  setLcd();

  createThread();
}


void loop() {
  threadController.run();
  Serial.println(celsius, 2);
  
}


void createThread(){
  showLcdThread.setInterval(threadSleep);
  buzzerThread.setInterval(threadSleep);
  
  showLcdThread.onRun(showCelsius);
  buzzerThread.onRun(startBuzzer);

  
  threadController.add(&showLcdThread);
  threadController.add(&buzzerThread);
}


void startBuzzer(){
  if (celsius < 25.2 || celsius > 32.6){
    digitalWrite(buzzer_pin, HIGH); 
    delay(400);
    digitalWrite(buzzer_pin, LOW);
    delay(200);
    Serial.print(celsius, 2); // debug
    Serial.println(" °C"); // debug
  }
}


void setInitialValues(){
  DHT.read11(DHT11_PIN);
  minValueCelsius = DHT.temperature;
  maxValueCelsius = DHT.temperature;  
}


void setLcd(){
  lcd.init();
  lcd.backlight();
  lcd.clear(); 
}


void showCelsius(){
  DHT.read11(DHT11_PIN);
  celsius = DHT.temperature + cf;
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(celsius);
  lcd.write(223);
  lcd.setCursor(0, 0);
  lcd.print("");

  lcd.setCursor(8, 1);
  lcd.print("");
  lcd.print(maxCelsius());
  lcd.write(223);


  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.print(minCelsius());
  lcd.write(223);

}


float minCelsius(){
  if (celsius < minValueCelsius){
    minValueCelsius = celsius;   
    
    Serial.print(minValueCelsius, 2); // debug
    Serial.println(" °C minValueCelsius"); // debug
  }  
  return minValueCelsius;
}


float maxCelsius(){
  if (celsius > maxValueCelsius){
    maxValueCelsius = celsius;
    
    Serial.print(maxValueCelsius, 2); // debug
    Serial.println(" °C maxValueCelsius"); // debug
  } 
  
  return  maxValueCelsius;
}