# Carrinho Ultrassônico

Resumo
-----
Este projeto controla um carrinho simples com leitor ultrassônico. O código está em [main.ino](main.ino). Ele lê a distância à frente e:
- Se a distância for menor que [`DISTANCIA_SEGURA`](main.ino) o carrinho para usando [`parar`](main.ino).
- Caso contrário, o carrinho anda para frente com [`mover_frente`](main.ino).
A distância lida é obtida por [`ler_distancia`](main.ino) e exibida no Monitor Serial.

Componentes necessários
-----------------------
- Placa Arduino (Uno, Nano, etc.)
- Sensor ultrassônico HC-SR04 (Trigger / Echo)
- Driver de motor compatível (ex: L293D, L298N)
- 2 motores DC (ou conjunto de motores compatíveis com o driver)
- Fonte de alimentação adequada para os motores
- Cabos e protoboard

Conexões (mapeamento de pinos)
------------------------------
Sensor ultrassônico HC-SR04:
- Trigger -> pino [`PINO_SENSOR_TRIGGER`](main.ino)
- Echo    -> pino [`PINO_SENSOR_ECHO`](main.ino)

Driver de motor (entradas IN do driver -> Arduino):
- Motor A IN1 -> [`PIN_MOTOR_IN1`](main.ino)
- Motor A IN2 -> [`PIN_MOTOR_IN2`](main.ino)
- Motor B IN3 -> [`PIN_MOTOR_IN3`](main.ino)
- Motor B IN4 -> [`PIN_MOTOR_IN4`](main.ino)

Observações:
- Ligue a alimentação do driver/motores à fonte externa conforme especificado pelo driver.
- Compartilhe o GND entre a placa Arduino e a fonte dos motores.

Comportamento do código
-----------------------
- O loop principal chama [`ler_distancia`](main.ino) e imprime no Monitor Serial (baud 9600).
- Se a distância < [`DISTANCIA_SEGURA`](main.ino) (padrão 75 cm), chama [`parar`](main.ino) e aguarda 1 s.
- Caso contrário chama [`mover_frente`](main.ino).
- Há uma função [`ajustar_velocidade_fixa`](main.ino) que usa a variável [`VELOCIDADE_FIXA`](main.ino) para simular PWM via tempos ligados/desligados (não chamada no loop por padrão). A constante [`PAUSA`](main.ino) controla o período usado por essa lógica.

Como usar / ajustar
-------------------
- Abra [main.ino](main.ino) na IDE do Arduino e carregue para a placa.
- Abra o Monitor Serial em 9600 baud para ver as leituras.
- Ajuste [`DISTANCIA_SEGURA`](main.ino) para alterar a distância mínima segura.
- Ajuste [`VELOCIDADE_FIXA`](main.ino) (0–100) para controlar a velocidade se decidir usar [`ajustar_velocidade_fixa`](main.ino).
- Verifique a alimentação dos motores antes de testar para evitar danos.

Referências no código
---------------------
- Arquivo principal: [main.ino](main.ino)  
- Funções e variáveis: [`ler_distancia`](main.ino), [`mover_frente`](main.ino), [`parar`](main.ino), [`ajustar_velocidade_fixa`](main.ino), [`PINO_SENSOR_TRIGGER`](main.ino), [`PINO_SENSOR_ECHO`](main.ino), [`PIN_MOTOR_IN1`](main.ino), [`PIN_MOTOR_IN2`](main.ino), [`PIN_MOTOR_IN3`](main.ino), [`PIN_MOTOR_IN4`](main.ino), [`DISTANCIA_SEGURA`](main.ino), [`PAUSA`](main.ino), [`VELOCIDADE_FIXA`](main.ino)