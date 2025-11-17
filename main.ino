/*
  Sketch unificado: controle por sensor ultrassônico (modo SENSOR)
  ou por comandos via Bluetooth/Serial (modo BT).
  Use comando "SENSOR" via Bluetooth para ativar modo ultrassom,
  ou "BT" para ativar controle por comandos.

  Comandos BT (quando em modo BT):
  - A, ANDAR, ON       -> mover frente
  - P, PARAR, STOP     -> parar
  - E, ESQUERDA        -> virar esquerda
  - D, DIREITA         -> virar direita
  - STATUS             -> retorna modo atual
  - SENSOR             -> alterna para modo SENSOR
  - BT                 -> alterna para modo BT
*/

#include <Arduino.h>

// declaracao dos pinos do sensor ultrassononico
const int PINO_SENSOR_ECHO = 8;
const int PINO_SENSOR_TRIGGER = 9;

// declaracao dos pinos do driver de motor
const int PIN_MOTOR_IN1 = 2;
const int PIN_MOTOR_IN2 = 3;
const int PIN_MOTOR_IN3 = 6;
const int PIN_MOTOR_IN4 = 7;

// constantes auxiliares
const int DISTANCIA_SEGURA = 30; // [cm]
const int PAUSA = 25; // [ms]

// tempo de giro (ms) usado para virar esquerda/direita
const int TURN_DELAY = 200; // ajuste conforme o hardware

// buffer serial / bluetooth
String btBuffer = ""; // buffer para compor comandos recebidos

// modo de operação
enum Mode { MODE_SENSOR, MODE_BT };
Mode currentMode = MODE_BT; // inicia em modo sensor (pode mudar via BT)

// protótipos
int ler_distancia(void);
void mover_frente(void);
void parar(void);
void virar_esquerda();
void virar_direita();
void processar_comando(String cmd);

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando o carrinho (modo unificado)...");
  Serial.print("Modo atual: "); Serial.println(currentMode == MODE_SENSOR ? "SENSOR (ultrassom)" : "BT (comandos)");

  // configura os pinos do sensor ultrassonico
  pinMode(PINO_SENSOR_ECHO, INPUT);
  pinMode(PINO_SENSOR_TRIGGER, OUTPUT);
  digitalWrite(PINO_SENSOR_TRIGGER, LOW);

  // configura os pinos do driver de motor
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_MOTOR_IN3, OUTPUT);
  pinMode(PIN_MOTOR_IN4, OUTPUT);

  parar(); // robo parado por padrao
}

void loop() {
  // leitura de comandos via Serial/Bluetooth (sempre ativa para permitir troca de modo)
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (btBuffer.length() > 0) {
        processar_comando(btBuffer);
        btBuffer = "";
      }
    } else {
      btBuffer += c;
      if (btBuffer.length() > 64) btBuffer = btBuffer.substring(0,64);
    }
  }

  // comportamento conforme modo atual
  if (currentMode == MODE_SENSOR) {
    int distancia = ler_distancia();
    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" cm");

    if (distancia < DISTANCIA_SEGURA) {
      Serial.println("Obstáculo detectado! Parando...");
      parar();
      delay(1000);
    } else {
      mover_frente();
    }

    delay(PAUSA);
  } else { // MODE_BT
    // Em modo BT, o controle é por comandos recebidos.

    // pequena espera para não lotar CPU
    delay(10);
  }
}


// Ler a distancia com o sensor ultrassonico
int ler_distancia(void){
  digitalWrite(PINO_SENSOR_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_SENSOR_TRIGGER, LOW);

  unsigned long duracao = pulseIn(PINO_SENSOR_ECHO, HIGH, 30000UL); // timeout de 30ms
  if (duracao == 0) return 999; // sem retorno (muito longe)
  return duracao / 58; // [cm]
}


// Mover o robo para a frente
void mover_frente(void){
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}


// Parar o robo (todos LOW = free / coasting). Ajuste se quiser frenagem ativa.
void parar(void){
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}


// Gira o robo para a esquerda por um tempo fixo
void virar_esquerda(){
  // roda direita pra frente, roda esquerda pra tras
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(TURN_DELAY);
  parar();
}


// Gira o robo para a direita por um tempo fixo
void virar_direita(){
  // roda esquerda pra frente, roda direita pra tras
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
  delay(TURN_DELAY);
  parar();
}


// Processa comandos recebidos via Serial/Bluetooth
void processar_comando(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  Serial.print("Comando recebido: "); Serial.println(cmd);

  // comandos sempre válidos para troca de modo
  if (cmd == "SENSOR") {
    currentMode = MODE_SENSOR;
    Serial.println("Modo alterado para: SENSOR (ultrassom)");
    return;
  } else if (cmd == "BT") {
    currentMode = MODE_BT;
    Serial.println("Modo alterado para: BT (comandos)");
    return;
  }

  // se estiver em modo BT, aceita comandos de movimento
  if (currentMode == MODE_BT) {
    if (cmd == "A" || cmd == "ANDAR" || cmd == "ON") {
      mover_frente();
    } else if (cmd == "P" || cmd == "PARAR" || cmd == "STOP" ) {
      parar();
    } else if (cmd == "E" || cmd == "ESQUERDA") {
      virar_esquerda();
    } else if (cmd == "D" || cmd == "DIREITA") {
      virar_direita();
    } else if (cmd == "STATUS") {
      Serial.print("STATUS: MODO=");
      Serial.println(currentMode == MODE_SENSOR ? "SENSOR" : "BT");
    } else {
      Serial.println("Comando desconhecido no modo BT");
    }
  } else {
    // em modo SENSOR, outros comandos (exceto SENSOR/BT tratados acima) são ignorados
    Serial.println("Ignorado: sistema em modo SENSOR. Envie 'BT' para controle por comandos.");
  }
}