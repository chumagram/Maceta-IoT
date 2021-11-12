//Alumno: Gonzalo Leonel Gramajo
#define   BLYNK_PRINT Serial
//Declaración de bibliotecas
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_BMP085.h>
//GLOBALES PARA PINES
#define   inMux        A0
#define   ctrlA          D3
#define   ctrlB          D6
#define   ctrlC          D5
#define   bombR      D7
#define   bombL       D8
//Parámetros para la conexión WiFi y a Blynk
char auth[] = "WO4vwEGFweASFyqyMof_oGeEaaysRMma";
char ssid[] = "TP-LINK_486E";
char pass[] = "87191905";

Adafruit_BMP085 bmp; //objeto del sensor BMP180
float pres, temp; // variables para el sensor BMP180
int hume_aux, hume; //variables para el sensor de humedad
float lux_Volt, lux_Amp, lux_uAmp, lux; // variables para el sensor de luz
bool bomba = 0; // variables del motor

BLYNK_WRITE(V0){
    bomba = param.asInt();
}

void setup() {
    pinMode(ctrlA, OUTPUT);
    pinMode(ctrlB, OUTPUT);
    pinMode(ctrlC, OUTPUT);
    pinMode(bombR,  OUTPUT);
    pinMode(bombL, OUTPUT);
    Blynk.begin(auth, ssid, pass);
    if (!bmp.begin()) {
        while (1) {}
    }
}

void loop() {
    //CONTROL DE MOTOR
    WidgetLED led(V4);
    if (bomba == 1){
       digitalWrite(bombR,  LOW);
       digitalWrite(bombL, HIGH);
       led.on();
    }
    else{
       digitalWrite(bombR,  LOW);
       digitalWrite(bombL, LOW);
       led.off();
    }
    
    //LECTURA DEL SENSOR DE HUMEDAD
    digitalWrite (ctrlA, LOW);
    digitalWrite (ctrlB, HIGH);
    digitalWrite (ctrlC, HIGH);
    hume_aux = (analogRead(inMux) - 1024)*(-1);
    hume = map (hume_aux, 0, 444, 0, 100);

    //LECTURA DE SENSOR TEMT6000
    digitalWrite(ctrlA, LOW);
    digitalWrite(ctrlB, LOW);
    digitalWrite(ctrlC, HIGH);
    lux_Volt = analogRead(inMux) * 5.0 / 1024.0;
    lux_Amp = lux_Volt / 10000.0; // porque la resistencia es de 10k
    lux_uAmp = lux_Amp * 1000000;
    lux = lux_uAmp * 2.0;

    //LECTURA DEL SENSOR BMP180
    pres = bmp.readPressure() / 100; // en hectopascales
    temp = bmp.readTemperature();
    
    Blynk.run();
    Blynk.virtualWrite(V1,hume);
    Blynk.virtualWrite(V2,lux);
    Blynk.virtualWrite(V3, String(pres) + " HPa - " + String(temp) + " °C");
}
