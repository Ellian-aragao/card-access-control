# Controle de Acesso
O sistema desenvolvido tem como objetivo fazer o controle de autenticação via cartão magnético.

## Fluxo de funcionamento
![flowchat](docs/flowchat.png)

## Funcionalidades
- [X] Cadastro de TAG mestre
  - [X] Inserção
  - [X] Remoção
- [X] Cadastro de TAG usuário
  - [X] Inserção
  - [X] Remoção
- [ ] Interface computacional Amigável
  - [X] Mensagens de debug
  - [ ] Mensagens para usuário
- [ ] API para banco de dados externo

## Infraestrutura
**Plataforma:** Atmel AVR 3.1.0
**Placa:** uno
**Framework:** Arduino

## Pré-requisitos
### Eletrônicos
* Arduino uno
* Sensor RFID522
* Protobord
* 3 Leds (verde,amarelo,azul)
* 3 resistências (200 ohm)
* 12 jumpers

#### Mapeamento sensor RFID para cada microcontroladores Arduino
| Signal    | MFRC522 Reader/PCD | Uno/101      | Mega | Nano V3 | Leonardo/Micro | Pro Micro |
|-----------|--------------------|--------------|------|---------|----------------|-----------|
| RST/Reset |  RST               |  9           |  5   |  D9     |  RESET/ICSP-5  |   RST     |
| SPI SS    |  SDA(SS)           |  10          |  53  |  D10    |  10            |   10      |
| SPI MOSI  |  MOSI              |  11 / ICSP-4 |  51  |  D11    |  ICSP-4        |   16      |
| SPI MISO  |  MISO              |  12 / ICSP-1 |  50  |  D12    |  ICSP-1        |   14      |
| SPI SCK   |  SCK               |  13 / ICSP-3 |  52  |  D13    |  ICSP-3        |   15      |

### Programas
* [ArduinoIDE](https://www.arduino.cc/en/software)
* Java 8 (Dependência arduinoIDE)
* [Vscode](https://code.visualstudio.com/)
* [PlataformIO](https://platformio.org)
> a preferência pelo Vscode vem de sua integração com a plataformaIO que agiliza o processo de desenvolvimento sem dependẽncia do CLI

### Bibliotecas
* [MFRC522](https://github.com/miguelbalboa/rfid?utm_source=platformio&utm_medium=piohome) 1.4.7
* Framework Arduino AVR (Dependências)
  * HID 1.0
  * SPI 1.0
  * Wire 1.0
  * EEPROM 2.0
  * SoftwarSerial 1.0

## Como rodar a aplicação
Levando conta que tudo ocorreu como os conformes nos [Pré-requisitos](Pré-requisitos), temos então o ambiente do Vscode, nele existe os botões na barra inferior que fazem o upload do programa para o microcontrolador, executado este, abrimos o serial monitor para receber as mensagens do arduino, assim inicia-se o [fluxo](Fluxo de funcionamento) da aplicação com sucesso.
> Nota-se que não é necessário a observação pelo serial monitor, os leds indicam o estado da aplicação.

## Licença: The Unlicense
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>


