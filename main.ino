// declaracao dos pinos do sensor ultrassononico
const int PINO_SENSOR_ECHO = 8;
const int PINO_SENSOR_TRIGGER = 9;


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
const int TURN_DELAY = 400; // ajuste conforme o hardware


// ---------------------------------------------------


int ler_distancia(void);
void mover_frente(void);
void parar(void);
void ajustar_velocidade_fixa();
void virar_esquerda();
void virar_direita();


// ---------------------------------------------------


void setup() {
  // Inicia a comunicação serial
  Serial.begin(9600);
  Serial.println("Iniciando o carrinho...");


  // configura os pinos do sensor ultrassonico
  pinMode(PINO_SENSOR_ECHO, INPUT); // entrada
  pinMode(PINO_SENSOR_TRIGGER, OUTPUT); // saida
  digitalWrite(PINO_SENSOR_TRIGGER, LOW); // por padrao em nivel baixo (sem sinal)


  // configura os pinos do driver de motor
  pinMode(PIN_MOTOR_IN1, OUTPUT); // saida
  pinMode(PIN_MOTOR_IN2, OUTPUT); // saida
  pinMode(PIN_MOTOR_IN3, OUTPUT); // saida
  pinMode(PIN_MOTOR_IN4, OUTPUT); // saida
  parar(); // robo parado por padrao
}


// ---------------------------------------------------


void loop() {
  // le a distancia
  int distancia = ler_distancia();
 
  // Mostra a distância lida no monitor serial
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");


  // verifica se ha um obstaculo na frente
  if(distancia < DISTANCIA_SEGURA){
    Serial.println("Obstáculo detectado! Parando...");
    // tenta primeiro virar para a esquerda e verificar novamente
    virar_esquerda();
    delay(200); // pequena estabilização
    int distancia_pos_virada = ler_distancia();
    Serial.print("Distancia apos virar_esquerda: ");
    Serial.print(distancia_pos_virada);
    Serial.println(" cm");

    if(distancia_pos_virada < DISTANCIA_SEGURA){
      // se ainda bloqueado, tenta virar para a direita
      virar_direita();
      delay(200);
      distancia_pos_virada = ler_distancia();
      Serial.print("Distancia apos virar_direita: ");
      Serial.print(distancia_pos_virada);
      Serial.println(" cm");

      if(distancia_pos_virada < DISTANCIA_SEGURA){
        // ambos lados bloqueados: parar e aguardar
        Serial.println("Ambos lados bloqueados. Parando e aguardando...");
        parar();
        delay(1000);
      } else {
        // direita liberada
        Serial.println("Direita liberada. Indo para frente...");
        mover_frente();
      }
    } else {
      // esquerda liberada
      Serial.println("Esquerda liberada. Indo para frente...");
      mover_frente();
    }
  } else { // senao
    // move o robo para a frente
    mover_frente();
  }


  // pausa para a proxima leitura
  delay(PAUSA);
}


// ---------------------------------------------------
// ---------------------------------------------------


// Ler a distancia com o sensor ultrassonico
int ler_distancia(void){
  // realiza o pulso de 10 microsegundos no trigger do sensor
  digitalWrite(PINO_SENSOR_TRIGGER,HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_SENSOR_TRIGGER,LOW);


  // mede o pulso em microsegundos retornado para o echo do sensor
  // e converte o tempo para distancia ao dividir por 58
  return pulseIn(PINO_SENSOR_ECHO, HIGH) / 58; // [cm]
}


// ---------------------------------------------------


// Mover o robo para a frente
void mover_frente(void){
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);
}


// Parar o robo
void parar(void){
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
}


// Gira o robo para a esquerda por um tempo fixo
void virar_esquerda(){
  // para um diferencial de motores simples: roda direita pra frente, roda esquerda pra tras
  digitalWrite(PIN_MOTOR_IN1, LOW);  // motor esquerdo parar/tras dependendo do driver
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, HIGH); // motor direito pra frente
  digitalWrite(PIN_MOTOR_IN4, LOW);
  delay(TURN_DELAY);
  // para após completar o giro
  parar();
}


// Gira o robo para a direita por um tempo fixo
void virar_direita(){
  // inverso da esquerda: roda esquerda pra frente, roda direita pra tras
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);
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
