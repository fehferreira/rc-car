/*
  Sketch modificado para controlar o carrinho via módulo Bluetooth HC-05

  Wiring (example for Arduino UNO):
  - HC-05 TX  -> Arduino D10 (RX of SoftwareSerial)
  - HC-05 RX  -> Arduino D11 (TX of SoftwareSerial)  *Use um divisor de tensão no RX do HC-05 se o Arduino fornece 5V*
  - HC-05 VCC -> 5V
  - HC-05 GND -> GND

  Emparelhamento:
  - Pareie com o HC-05 usando senha 1234 ou 0000 (varia por módulo)

  Comandos suportados (envie por app Bluetooth ou via Serial USB):
  - F, FRENTE, ON    -> mover para frente
  - P, PARAR, STOP   -> parar
  - E, ESQUERDA      -> virar/esquerda
  - D, DIREITA       -> virar/direita
  - V=<0-100>        -> ajustar velocidade fixa (porcentagem)
  - STATUS           -> responde com velocidade atual

  O sketch envia respostas tanto para o Serial USB quanto para o HC-05.
*/

// declaracao dos pinos do driver de motor
const int PIN_MOTOR_IN1 = 2;
const int PIN_MOTOR_IN2 = 3;
const int PIN_MOTOR_IN3 = 7;
const int PIN_MOTOR_IN4 = 6;


// declaracao das constantes auxiliares para controlar os motores
const int DISTANCIA_SEGURA = 75; // [cm]
const int PAUSA = 25; // [ms]


// Variável global para armazenar a velocidade fixa (0 a 100)
int VELOCIDADE_FIXA = 1; // Valor padrão de 50% da velocidade máxima

// tempo de giro (ms) usado para virar esquerda/direita
const int TURN_DELAY = 200; // ajuste conforme o hardware


// ---------------------------------------------------


int ler_distancia(void);
void mover_frente(void);
void parar(void);
void ajustar_velocidade_fixa();
void virar_esquerda();
void virar_direita();


String btBuffer = ""; // buffer para compor comandos recebidos


// ---------------------------------------------------


void setup() {
  // Inicia a comunicação serial
  Serial.begin(9600);
  Serial.println("Iniciando o carrinho (modo HC-05)...");

  // OBS: removido sensor ultrassonico. Controle por comandos Bluetooth.
  // configura os pinos do driver de motor
  pinMode(PIN_MOTOR_IN1, OUTPUT); // saida
  pinMode(PIN_MOTOR_IN2, OUTPUT); // saida
  pinMode(PIN_MOTOR_IN3, OUTPUT); // saida
  pinMode(PIN_MOTOR_IN4, OUTPUT); // saida
  parar(); // robo parado por padrao
}


// ---------------------------------------------------


void loop() {
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

  // pequena espera para não lotar a CPU
  delay(10);
}


// ---------------------------------------------------
// ---------------------------------------------------


// ---------------------------------------------------
// Função para processar comandos recebidos
void processar_comando(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  Serial.print("Comando recebido: "); Serial.println(cmd);

  if (cmd == "A" || cmd == "ANDAR" || cmd == "ON") {
    mover_frente();
  } else if (cmd == "P" || cmd == "PARAR" || cmd == "STOP" ) {
    parar();
  } else if (cmd == "E" || cmd == "ESQUERDA") {
    virar_esquerda();
  } else if (cmd == "D" || cmd == "DIREITA") {
    virar_direita();
  } else if (cmd.startsWith("V=")) {
    // ajustar velocidade: V=50
    int v = cmd.substring(2).toInt();
    if (v < 0) v = 0; if (v > 100) v = 100;
    VELOCIDADE_FIXA = v;
    Serial.print("Velocidade ajustada para: "); Serial.println(VELOCIDADE_FIXA);
  } else if (cmd == "STATUS") {
    Serial.print("STATUS: VELOCIDADE="); Serial.println(VELOCIDADE_FIXA);
  } else {
    Serial.println("Comando desconhecido");
  }
}


// ---------------------------------------------------


// Mover o robo para a frente
void mover_frente(void){
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
}


// Parar o robo
void parar(void){
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}


// Gira o robo para a esquerda por um tempo fixo
void virar_esquerda(){
  // para um diferencial de motores simples: roda direita pra frente, roda esquerda pra tras
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
  delay(TURN_DELAY);
  // para após completar o giro
  parar();
}


// Gira o robo para a direita por um tempo fixo
void virar_direita(){
  // inverso da esquerda: roda esquerda pra frente, roda direita pra tras
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(TURN_DELAY);
  // para após completar o giro
  parar();
}


// ---------------------------------------------------


// Função para ajustar a velocidade fixa dos motores
void ajustar_velocidade_fixa() {
  // Calcula o tempo de ativação e desativação com base na velocidade fixa
  int tempo_ativo = map(VELOCIDADE_FIXA, 0, 100, 0, PAUSA);
  int tempo_inativo = PAUSA - tempo_ativo;


  // Liga os motores por tempo_ativo
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(tempo_ativo);


  // Desliga os motores por tempo_inativo
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(tempo_inativo);
}
