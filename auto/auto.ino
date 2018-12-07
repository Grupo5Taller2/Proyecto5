/*FUNCIONES ORIGINALES*/
#include <Arduino.h>

/*FUNCIONES EDITADAS O DESARROLLADAS*/

#include <L9110.h>
#include "ESP8266.h"
#include <PID_v1.h>

/*DEFINICIONES DE CONFIGURACION*/
#define BAUDRATE            115200
#define TAM_BUFFER          128

/*DEFINICIONES DE TIEMPO DE EJECUCION*/
#define F_Sensed              27 //revisar valores
#define F_Communication       50
#define F_Movement            97
#define F_Testing             2000


/*VARIABLES FLOAT PARA PID*/

double Kp = 0.5 * 90, Ki = 0.06 / 1.2, Kd = 0;
double Input = 0 , Setpoint = 3, Output = 0;
float rpmi=0,rpm=0;
int distanciai=0,distancia=0;
int pwma = 200, pwmb = 200;

/*HASTA ACA VARIABLES FLOAT*/

// declaracion de variables
const int PinA = 2;
const int PinB = 3;

static volatile unsigned long ReboteA = 0;
static volatile unsigned long ReboteB = 0;

volatile int pulsoA = 0;
volatile int pulsoB = 0;

unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;
unsigned long tiemposegundos = 0;


unsigned long tiempo5 = 0;
unsigned long tiempo8 = 0;
unsigned long tiempoi = 0;






/*INSTANCIA DE OBJECTS*/

L9110 control(6, 7, 5, 4); //MotorDer1,MotorDer2,MotorIzq1,MotorIzq2,PWM_Derecho,PWM_Izquierdo
PID myPID(&Input, &Output,  &Setpoint,  Kp, Ki, Kd, DIRECT);
ESP8266 wifi(Serial);

/*FIN DE INSTANCIA DE OBJECTS*/


uint8_t buffer[TAM_BUFFER] = {0}; //buffer para enviar y recibir informacion
uint32_t len = 0; //Tamaño del mensaje recibido o el mensaje a enviar
uint32_t frecuency = 1; //variables que nos permite implementar un porgramacion de tareas
String data_send;

//variables de velocidad de motores

//variables de desplazamiento -> el vehiculo lee estas variables y decide su movimiento
uint8_t direction = 0; // 0=stop, 1=front, 2=back //si va para adelante o para atras
uint8_t speed = 0;   // 0=stop, 1=slow, 2=fast ,3=fasta&furious
uint8_t turn = 0;    // 0=forward, 1=left, 2=right


//RUTINA DE SERVICIO DE INTERRUPCION (ISR)
//ISR

void cuentA()
{
  if (digitalRead(PinA) && (micros() - ReboteA > 500 ) && digitalRead(PinA)) {
    ReboteA = micros();
    pulsoA++;
  }

}

void cuentB()
{
  if (digitalRead(PinB) && (micros() - ReboteB > 500 ) && digitalRead(PinB)) {
    ReboteB = micros();
    pulsoB++;
  }

}
//RUTINA DE SERVICIO DE INTERRUPCION (ISR)

void setup(void)
{
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(200, 255);
  
  Serial.begin(BAUDRATE);
  Serial.print("setup begin\r\n");
  wifi.Configuration();
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  attachInterrupt(0, cuentA, RISING);
  attachInterrupt(1, cuentB, RISING);
  pulsoA = 0;
  pulsoB = 0;

  tiempo1 = millis();
  tiempo8 = millis();
}

void loop(void)
{

  // MEDICION DE DISTANCIA Y CONTROL DE VELOCIDAD
  if (frecuency % F_Sensed == 0) { //SI es momento de sensar encoders
     if (pulsoA == 3) {
    int desv = pulsoA - pulsoB;

    if (desv < 0) {
      distancia = 10 * 3.14 * desv * -1;
      Serial.println("desviacion hacia la derecha");
      Serial.print(" --> "); Serial.print(distancia);   Serial.print(" cm");
      Serial.println("  ");

      tiempo2 = millis();
      if (tiempo2 > (tiempo1 + 1000)) { // si ha pasado 1 segundo ejecuta el IF
        tiempo1 = millis(); // actualiza el tiempo actual
        tiemposegundos = tiempo1 / 1000;
        Serial.print("ha transcurrido:");
        Serial.print(tiemposegundos);
        float rpm= (float)((distancia / tiemposegundos) / (30 * 3, 14));
        Serial.println("  ");
        Serial.println("desviacion hacia la derecha");
        Serial.print(" --> "); Serial.print(rpm   );   Serial.print(" rpm");
        Serial.println("  ");

      }

    }
    else {
      Serial.println("desviacion hacia la izquierda");
      distanciai=10 * 3.14 * desv;
      Serial.print(" --> "); Serial.print( distanciai );   Serial.print(" cm");
      Serial.println("  ");

      tiempo5 = millis();
      if (tiempo5 > (tiempo8 + 1000)) { // si ha pasado 1 segundo ejecuta el IF
        tiempo8 = millis(); // actualiza el tiempo actual
        tiempoi = tiempo8 / 1000;
        Serial.print("ha transcurrido:");
        Serial.print(tiempoi);
        rpmi= (float)((distanciai / tiempoi) / (30 * 3, 14));
        Serial.println("  ");
        Serial.println("desviacion hacia la derecha");
        Serial.print(" --> "); Serial.print(rpmi   );   Serial.print(" rpm");
        Serial.println("  ");

      }
    }


    Serial.print("  Salida:  ");
    Serial.print(Output);
    Serial.print("   ");
    Serial.println("");


    Input = pulsoB;
    myPID.Compute();
    pwmb = Output;

    pulsoA = 0;
    pulsoB = 0;
  }
  }
  // FIN MEDICION DE DISTANCIA Y CONTROL DE VELOCIDAD


  // DEZPLAZAMIENTO
  if (frecuency % F_Movement == 0 ) { //Si es momento de realizar un movimiento
    if ( direction == 1 && speed != 0  ) { // Si voy hacia adelante
      Serial.print("ADELANTE\r\n");
      if ( turn == 0) { // si voy hacia adelante en linea recta
        control.adelante(pwma, pwmb, 0);
      }
      if ( turn == 1) { // si voy hacia adelante en hacia la izqueirda
        control.izquierda(pwma, pwmb, 0);
      }
      if ( turn == 2) { // si voy hacia adelante en hacia la izqueirda
        control.derecha(pwma, pwmb, 0);
      }
    }
    if ( direction == 2 && speed != 0  ) { // Si voy hacia atrás
      Serial.print("ATRAS\r\n");
      control.atras(pwma, pwmb, 0);
    }
    if (direction == 0 || speed == 0) {
      control.frenar(0);
    }
    Serial.print(direction);
    Serial.print(speed);
    Serial.print(turn);
    Serial.print("\r\n");

  }
  // FIN DEZPLAZAMIENTO


  // COMUNICACION
  if (frecuency % F_Communication == 0 ) { //SI es momento de procesar la comunicacion
    len = wifi.Receive_data(buffer);
    if (len == 3) { //SI SE RECIBE ORDEN
      Serial.print("ORDEN");
      Serial.print("\r\n");
      //REcibí un mensaje de desplazamiento, almacenamiento de los movimientos que decidió el usuario
      direction = buffer[0] - 48; //se resta 48, pues el valor que se recibe está en ascci
      speed = buffer[1] - 48;
      turn = buffer[2] - 48;
      Serial.print(direction);
      Serial.print(speed);
      Serial.print(turn);


    }
    if (len == 1) { //SI PIDE INFORMACION
      Serial.print("Estadisticas");
      Serial.print("\r\n");
      data_send = (String(rpm, 0) + '/' + String(rpmi, 0) + '/' + String(distancia, 0) + '/' + String(distanciai, 0) + '/' + String(direction, DEC) + '/' + String(speed, DEC) + '/' + String(turn, DEC) + '/'); //preparación de la informacion a enviar
      for (len = 0; len < data_send.length(); len++) { // Se coloca el string en el buffer para su proximo envio
        if (data_send[len] != 32) { //en el caso de que la conversion a string me de un caracater espacio, lo reemplazo por '0'(En la práctica sucede)
          buffer[len] = data_send[len];
        } else {
          buffer[len] = '0';
        }
        //Serial.print((char)buffer[len]);
      }
      //Serial.print("\r\n");
      //envio del estado
      Serial.print(data_send.length());
      Serial.print("\r\n");

      wifi.Send_data(buffer, data_send.length());
      Serial.print("Envio de datos");
      Serial.print("\r\n");
      for (len = 0; len < data_send.length(); len++) {
        Serial.print((char)buffer[len]);
      }
      Serial.print("\r\n");


    }

  }
  // FIN COMUNICACION


  // ESCANEO DEL ESTADO DE LA COMUNICACION DEL VEHICULO
  /*
    if (frecuency % F_Testing == 0 ){
        wifi.Comprobation();
    }
  */
  // FIN ESCANEO DEL ESTADO DE LA COMUNICACION DEL VEHICULO

  // CONFIGURACION DE TIEMPOS
  //delayMicroseconds(1000);
  // delay(1); //espera entre iteraciones

  frecuency++;//Aumento de la iteracion realizada
  if (frecuency == 6300) {
    frecuency = 0;
  }
  // FIN CONFIGURACION DE TIEMPOS
}
