// Cargar librerias
#include "BluetoothSerial.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// Comprueba si la configuración Bluetooth está activada
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Objeto serie Bluetooth
BluetoothSerial SerialBT;

// GPIO al que está conectado el LED
const int ledPin =  2;

// GPIO al que está conectado el DS18B20
const int oneWireBus = 4;          
// Configurar una instancia oneWire para comunicarse con cualquier dispositivo OneWire.
OneWire oneWire(oneWireBus);
// Pasar nuestra referencia oneWire al sensor de temperatura de Dallas 
DallasTemperature sensors(&oneWire);

// Gestionar los mensajes recibidos y enviados
String message = "";
char incomingChar;
String temperatureString = "";

// Temporizador: variables auxiliares
unsigned long previousMillis = 0;    // Almacena la última vez que se publicó la temperatura
const long interval = 10000;         // intervalo en el que publicar las lecturas de los sensores

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  // Nombre del dispositivo Bluetooth
  SerialBT.begin("ESP32");
  Serial.println("El dispositivo arrancó, ¡ahora puedes emparejarlo con bluetooth!");
}

void loop() {
  unsigned long currentMillis = millis();
  // Enviar lecturas de temperatura
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    sensors.requestTemperatures(); 
    temperatureString = String(sensors.getTempCByIndex(0)) + "C  ";
    
    SerialBT.println(temperatureString); 
  }
  // Lectura de los mensajes recibidos (comando de control LED)
  if (SerialBT.available()){
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message = "";
    }
    Serial.write(incomingChar);  
  }
  // Comprueba el mensaje recibido y controla la salida en consecuencia
  if (message =="led_on"){
    digitalWrite(ledPin, HIGH);
  }
  else if (message =="led_off"){
    digitalWrite(ledPin, LOW);
  }
  delay(20);
}
