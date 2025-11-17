# Carrinho RC — Documentação atualizada

## Resumo

Este repositório contém um sketch unificado (`main.ino`) que permite controlar um carrinho de duas formas:

- Modo BT (padrão): controle manual por comandos via Serial/Bluetooth (por exemplo, o módulo HC-05).
- Modo SENSOR: comportamento autônomo usando um sensor ultrassônico (HC-SR04). No modo SENSOR o carrinho anda para frente e, ao detectar um obstáculo, tenta desviar automaticamente (busca um caminho livre à direita ou à esquerda).

Arquivo principal: `main.ino` — contém toda a lógica dos dois modos e a possibilidade de alternar entre eles via comando Bluetooth.

## Componentes necessários

- Placa Arduino (Uno, Nano, etc.)
- Módulo Bluetooth (HC-05) para controle por serial (opcional, mas usado para o modo BT)
- Sensor ultrassônico HC-SR04 (Trigger / Echo)
- Driver de motor compatível (ex: L293D, L298N)
- 2 motores DC (ou conjunto compatível)
- Fonte de alimentação adequada para os motores
- Cabos e protoboard

## Conexões (mapeamento de pinos)

Sensor ultrassônico HC-SR04:

- Trigger -> pino `PINO_SENSOR_TRIGGER` (ver `main.ino`)
- Echo -> pino `PINO_SENSOR_ECHO` (ver `main.ino`)

Driver de motor (entradas IN do driver -> Arduino):

- Motor A IN1 -> `PIN_MOTOR_IN1`
- Motor A IN2 -> `PIN_MOTOR_IN2`
- Motor B IN3 -> `PIN_MOTOR_IN3`
- Motor B IN4 -> `PIN_MOTOR_IN4`

## Observações de hardware

- Alimente os motores com uma fonte externa adequada ao motor; NÃO alimente os motores diretamente pelo 5V do Arduino.
- Sempre compartilhe o GND entre a fonte dos motores e o Arduino.
- Ao usar o HC-05, proteja o pino RX do módulo (3.3V) com um divisor de tensão ou conversor de nível se o Arduino estiver em 5V.

## Funcionalidades atuais (resumo)

- Modo BT (padrão no boot):

  - Controle por comandos via Serial/Bluetooth (baud 9600).
  - Comandos suportados (envie a string seguida de CR/LF):
    - `A`, `ANDAR`, `ON` — avançar
    - `P`, `PARAR`, `STOP` — parar
    - `E`, `ESQUERDA` — virar esquerda (giro por tempo)
    - `D`, `DIREITA` — virar direita (giro por tempo)
    - `STATUS` — retorna o modo atual (BT ou SENSOR)
    - `SENSOR` — muda para modo SENSOR
    - `BT` — muda para modo BT

- Modo SENSOR:
  - O carrinho anda para frente automaticamente lendo o ultrassom.
  - Se detectar obstáculo a uma distância menor que `DISTANCIA_SEGURA` (constante em `main.ino`), ele executa a rotina `desviar_obstaculo()`:
    - Tenta virar à direita em pequenos passos (cada passo usa `TURN_DELAY` ms de rotação) e verifica a distância após cada passo.
    - Se não encontrar caminho à direita, tenta o mesmo à esquerda.
    - Ao encontrar um ponto livre (distância >= `DISTANCIA_SEGURA`), avança um pouco para sair do obstáculo e retorna ao comportamento normal.
    - Se nenhuma rota for encontrada após as tentativas, o robô para e aguarda intervenção.

## Notas importantes sobre velocidade

- A funcionalidade de controle de velocidade por tempo (simulação de PWM com `VELOCIDADE_FIXA`) foi removida/disabled — o sketch não usa PWM para os pinos de controle do driver. O controle de velocidade não está ativo por padrão.

## Parâmetros ajustáveis

- `DISTANCIA_SEGURA` — distância em cm usada como limite para detectar obstáculo.
- `TURN_DELAY` — tempo (ms) usado por `virar_esquerda()`/`virar_direita()` para cada passo de giro.
- `PAUSA` — período de espera usado pelo loop (pequeno delay entre leituras).
- `attempts` (definido na função `desviar_obstaculo()` em `main.ino`) — número de passos de rotação tentados para cada lado ao desviar.

## Como usar

1. Carregue `main.ino` na placa Arduino com a fiação correta.
2. Pareie/connecte o HC-05 no celular/PC (PIN padrão: `1234` ou `0000`).
3. Abra um terminal serial (ou app Serial Bluetooth) a 9600 baud.
4. Por padrão o carrinho inicia no modo BT. Envie comandos (ex.: `ON`, `P`, `E`, `D`, `STATUS`) para controlar.
5. Para alternar ao modo autônomo por ultrassom envie `SENSOR`. Envie `BT` para voltar ao controle por comandos.

## Testes recomendados

- Teste os comandos básicos via Serial antes de usar com o HC-05 para garantir que os pinos do driver respondem corretamente.
- Ajuste `TURN_DELAY` e `attempts` conforme o comportamento real do robô (tamanho, raio de giro, atrito).
- Teste o desvio em superfícies seguras, com obstáculos macios no início.

## Referências no código

- Arquivo principal: `main.ino`
- Funções-chave: `ler_distancia()`, `mover_frente()`, `parar()`, `virar_esquerda()`, `virar_direita()`, `desviar_obstaculo()`

Se quiser, eu posso também gerar uma seção curta com dicas de calibração (como ajustar `TURN_DELAY` usando um marcador no chão) ou adicionar exemplos de apps Android recomendados para controlar o HC-05.
