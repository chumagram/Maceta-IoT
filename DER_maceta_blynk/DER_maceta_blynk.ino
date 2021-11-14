/*     
_________________________________________________
                MACETA IoT
        Alumno: Gonzalo Leonel Gramajo
        Para: Diplomatura en Robótica (DER)
        Cohorete n°1 de la UNLaR
        Fecha: 12/11/2021
_________________________________________________
*/
#define   BLYNK_PRINT Serial  // permite visualizar datos en el monitor serial de Arduino o Blynk
// Declaración de bibliotecas
#include <ESP8266WiFi.h>    // biblioteca para la gestión de WiFi
#include <BlynkSimpleEsp8266.h>   // biblioteca para la conexión a Blynk
#include <Adafruit_BMP085.h>    // biblioteca para la lectura del sensor BMP180
// Redefinición de los pines que se usaran de la placa NodeMCU
#define   ledPlaca    D4  // definición del pin D4 que tiene un led negado en la placa
#define   ledBomb    D0  // led negado a D0 para indicar que la bomba esta encendida
#define   inMux        A0   // definición del pin A0 como la entrada desde el multiplexor
#define   ctrlA          D3   // definición del pin A de control del multiplexor
#define   ctrlB          D6   // definición del pin B de control del multiplexor
#define   ctrlC          D5   // definición del pin C de control del multiplexor
#define   bombR      D7   // definición del pin para el control de la bomba ya sea en 0 o 1
#define   bombL       D8  // definición del pin para el control de la bomba ya sea en 0 o 1
//Parámetros para la conexión WiFi y a Blynk
char auth[] = "WO4vwEGFweASFyqyMof_oGeEaaysRMma";   // token brindado por Blynk
char ssid[] = "TP-LINK_486E";   // nombre de la red WiFi a la que nos conectaremos
char pass[] = "87191905";   // contraseña de la red WiFi a la que nos conectaremos
// Otras variables para el manejo de sensores y actuador
Adafruit_BMP085 bmp; //objeto del sensor BMP180
float pres, temp; // variables para el sensor BMP180
int hume_aux, hume; //variables para el sensor de humedad
float lux_Volt, lux_Amp, lux_uAmp, lux; // variables para el sensor de luz
bool bomba = 0; // variable para accionar la bomba
int cont = 0;

BLYNK_WRITE(V0){  // función que recibe un dato por el pin virtual V0 de Blynk
    bomba = param.asInt(); // se asigna V0 a la variable bomba
}

void setup() {
    pinMode(ledPlaca, OUTPUT);  // definimos PIN de la placa como salida
    analogWrite(ledPlaca, 0);  // se enciende con LOW porque está negado
    pinMode(ledBomb, OUTPUT); // led indicador de la bomba como salida
    pinMode(ctrlA, OUTPUT); // definimos el PIN de control A del mux como salida
    pinMode(ctrlB, OUTPUT); // definimos el PIN de control B del mux como salida
    pinMode(ctrlC, OUTPUT); // definimos el PIN de control C del mux como salida
    pinMode(bombR,  OUTPUT); // definimos el PIN R de control de la bomba como salida
    pinMode(bombL, OUTPUT); // definimos el PIN L de control de la bomba como salida
    Blynk.begin(auth, ssid, pass);  // función que inicializa Blynk
    if (!bmp.begin()) { // inicializa el sensor BMP y lo condiciona:
        while (1) {}  // si el sensor no se encuentra, se entra en un bucle infinito
    }
    analogWrite(ledPlaca, 255); // se apaga el led y termina la inicialización del programa
}

void loop() {
    // Control de la bomba
    WidgetLED led(V4);  // se inicia el pin virtual V4 como led
    if (bomba == 1){  // si la variable bomba esta en 1, se enciede la misma
       digitalWrite(bombR,  LOW);
       digitalWrite(bombL, HIGH);
       digitalWrite(ledBomb, LOW); // LOW porque está negado
       led.on();  // se enciende el led en Blynk correspondiente al pin V4
    }
    else{
       digitalWrite(bombR,  LOW); // se apaga la bomba
       digitalWrite(bombL, LOW);  // los 2 pines deben estar en bajo
       digitalWrite(ledBomb, HIGH); // LOW porque está negado
       led.off(); // se apaga el led en Blynk correspondiente al pin V4
    }
    //Control automático de la bomba
    cont ++;
    while (hume < 20 && cont > 100){  // si la humedad es menor que 20 se enciende la bomba
       digitalWrite(bombR,  LOW);
       digitalWrite(bombL, HIGH);
       digitalWrite(ledBomb, LOW); // LOW porque está negado
       led.on();  // se enciende el led en Blynk correspondiente al pin V4
       delay(4000);
       digitalWrite(bombR,  LOW); // se apaga la bomba
       digitalWrite(bombL, LOW);
       digitalWrite(ledBomb, HIGH); // apagamos el led de la bomba
       led.off(); // se apaga el led en Blynk
       cont = 0;
    }  

    // Lectura del sensor TEMT6000
    digitalWrite(ctrlA, LOW); // pin de control A del mux en bajo
    digitalWrite(ctrlB, LOW); // pin de control B del mux en bajo
    digitalWrite(ctrlC, HIGH);  // pin de control C del mux en alto
    lux_Volt = analogRead(inMux) * 5.0 / 1024.0; // por 5V y dividido los 1024 valores posibles
    lux_Amp = lux_Volt / 10000.0; // porque la resistencia en la placa es de 10k Ohms
    lux_uAmp = lux_Amp * 1000000; // se lo multiplica por 10^6 para pasar de A a uA
    lux = lux_uAmp * 2.0;

    // Lectura del sensor de humedad de suelo
    digitalWrite (ctrlA, LOW);  // pin de control A del mux en bajo
    digitalWrite (ctrlB, HIGH); // pin de control B del mux en alto
    digitalWrite (ctrlC, HIGH); // pin de control C del mux en alto
    hume_aux = (analogRead(inMux) - 1024)*(-1); // se lee el pin analógico, se le resta el valor maximo y se lo pasa positivos
    hume = map (hume_aux, 0, 444, 0, 100);  // los valores van de 0 (seco) a 444 (mojado), pero se tomara de 0 a 100

    // Lectura del sensor BMP180
    pres = bmp.readPressure() / 100; // se lo divide en 100 para pasar a hectopascales
    temp = bmp.readTemperature(); // lectura de la temperatura

    digitalWrite(ledPlaca, LOW);  // apagado del pin indicador

    // Encendido de PIN indicador
    analogWrite(ledPlaca, 230); // se lo enciende acá para que no afecte la lectura de la luminosidad
    Blynk.run();  // llama a Blynk
    Blynk.virtualWrite(V1,hume);  // envía el valor de humedad por el pin virtual V1 a Blynk
    Blynk.virtualWrite(V2,lux); // envía el valor de luminosidad por el pin virtual V2 a Blynk
    Blynk.virtualWrite(V4, pres); // envía el valor de presión por el pin virtual V4 a Blynk
    Blynk.virtualWrite(V5, temp); // // envía el valor de temperatura por el pin virtual V5 a Blynk
    Blynk.virtualWrite(V3, String(pres) + " HPa - " + String(temp) + " °C"); // envía el valor de presión y temperatura concatenados
    delay(2);
    analogWrite(ledPlaca, 255);  // apagado del pin indicador
}
