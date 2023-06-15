#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial bluetooth(8, 7);  // Pin TX y RX del módulo Bluetooth HC-05

Servo servoVentana;      // Servo para controlar la ventana
Servo servoCochera;      // Servo para controlar la cochera
Servo servoVentilador;   // Servo para controlar el ventilador

#define LED1 6   // Pin del LED 1
#define LED2 3   // Pin del LED 2
#define LED3 4   // Pin del LED 3

#define trigPin 13   // Pin TRIGGER del sensor ultrasonido
#define echoPin 12   // Pin ECHO del sensor ultrasonido

int pirPin = 10;          // Pin de entrada digital del sensor PIR
int buzzerPin = 5;       // Pin de salida para el zumbador
int contador = 0;

unsigned long tiempoUltimaActivacion = 0;
unsigned long tiempoInicio = 0;
bool servoCocheraActivado = false;
bool ventiladorEncendido = false;
bool girandoVentilador = false;

void setup() {
  // Configuración de los pines
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicialización de los servos
  servoVentana.attach(9);
  servoCochera.attach(11);
  servoVentilador.attach(2);

  abrirCochera();
  delay(9000);  // Esperar 5 segundos
  
  // Cerrar la cochera
  cerrarCochera();
  delay(9000);  // Esperar 5 segundos

  // Inicia la comunicación serial para el módulo Bluetooth
  bluetooth.begin(9600);

  // Menú de comandos
  Serial.begin(9600);
  Serial.println("--------------Menu de la casa inteligente-----------------------");
  Serial.println("Comandos:");
  Serial.println(" - Encender LED 1: 'a'");
  Serial.println(" - Apagar LED 1: 'A'");
  Serial.println(" - Encender LED 2: 'b'");
  Serial.println(" - Apagar LED 2: 'B'");
  Serial.println(" - Encender LED 3: 'c'");
  Serial.println(" - Apagar LED 3: 'C'");
  Serial.println(" - Encender todos los LEDs: 'd'");
  Serial.println(" - Apagar todos los LEDs: 'D'");
  Serial.println(" - Abrir ventana: 'v'");
  Serial.println(" - Cerrar ventana: 'V'");
  Serial.println(" - Encender ventilador: '1'");
  Serial.println(" - Apagar ventilador: '0'");
  Serial.println(" - La cochera y la puerta se abren automáticamente");
  Serial.println(" - El sensor de movimiento activa la alarma");
  Serial.println(" - Gracias por usar nuestra aplicación");

  // Posición inicial de los servos
  servoVentana.write(0);
  servoCochera.write(1500);
  servoVentilador.writeMicroseconds(500);
}

void loop() {
  if (bluetooth.available()) {
    char command = bluetooth.read();
    executeCommand(command);
  }

  checkPIR();  // Verificar sensor de movimiento
  checkCochera();  // Verificar sensor de cochera
}

void executeCommand(char command) {
  switch (command) {
    case 'p':
      digitalWrite(LED1, HIGH);
      break;
    case 'A':
      digitalWrite(LED1, LOW);
      break;
    case 'b':
      digitalWrite(LED2, HIGH);
      break;
    case 'B':
      digitalWrite(LED2, LOW);
      break;
    case 'c':
      digitalWrite(LED3, HIGH);
      break;
    case 'C':
      digitalWrite(LED3, LOW);
      break;
    case 'd':
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      break;
    case 'D':
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      break;
    case 'v':
      servoVentana.write(180);
      delay(1000);
      break;
    case 'V':
      servoVentana.write(0);
      delay(1000);
      break;
    case '9':
      encenderVentilador();
      break;
    case '8':
      apagarVentilador();
      break;
    default:
      break;
  }
}

void encenderVentilador() {
  if (!ventiladorEncendido) {
    if (!girandoVentilador) {
      girandoVentilador = true;
      for (int i = 90; i >= 0; i--) {
        servoVentilador.write(i);
        delay(10);
      }
    }
    ventiladorEncendido = true;
  }
}

void apagarVentilador() {
  if (ventiladorEncendido) {
    if (!girandoVentilador) {
      girandoVentilador = true;
      for (int i = 0; i <= 90; i++) {
        servoVentilador.write(i);
        delay(10);
      }
    }
    ventiladorEncendido = false;
  }
}

void checkPIR() {
  int sensorValue = digitalRead(pirPin);
  if (sensorValue == HIGH) {
    unsigned long tiempoActual = millis();
    if (tiempoActual - tiempoUltimaActivacion > 5000) {  // Evitar activaciones continuas en intervalos cortos
      tiempoUltimaActivacion = tiempoActual;
      activarAlarma();
    }
  }
}

void activarAlarma() {
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

void checkCochera() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance <= 10 && !servoCocheraActivado) {
    abrirCochera();
    servoCocheraActivado = true;
  } else if (distance > 10 && servoCocheraActivado) {
    cerrarCochera();
    servoCocheraActivado = false;
  }
}

void abrirCochera() {
  for (int i = 0; i <=90; i += 1) {
    servoCochera.write(i);
    delay(30);
  }
}

void cerrarCochera() {
  for (int i = 79; i >= 0; i -= 1) {
    servoCochera.write(i);
    delay(15);
  }
}
