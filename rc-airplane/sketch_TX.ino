#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

int RF24_CHIP_ENABLE_PIN = 9;
int RF24_CHIP_SELECT_PIN = 10;
RF24 radio(RF24_CHIP_ENABLE_PIN, RF24_CHIP_SELECT_PIN); 

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; // возможные номера труб

int LEFT_VERTICAL_SLIDER_PIN = 2; // потенциометр на 2 аналоговом пине, вперед-назад  (мотор)
// int LEFT_HORIZONTAL_SLIDER_PIN = 3; // левый потенциометр на 2 аналоговом пине, вправо-влево  (элероны)
int RIGHT_VERTICAL_SLIDER_PIN = 1;  // правый потенциометр на 1 аналоговом пине, вперед-назад  (руль высоты)
int RIGHT_HORIZONTAL_SLIDER_PIN = 8; // левый потенциометр на 2 аналоговом пине, вправо-влево  (элероны)

int SERVO_MIN_ANGLE = 0;
int SERVO_MAX_ANGLE = 180;

int ENGINE_STOP = 800;
int ENGINE_FULL_SPEED = 2200;

int controlPanelParams[3];

void setup() 
{
  Serial.begin(9600); // открываем порт для связи с ПК
  
  radio.begin(); // активировать модуль
  radio.setAutoAck(0);         // режим подтверждения приёма, 1 вкл 0 выкл

  radio.openWritingPipe(address[0]);   // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  // выбираем канал (в котором нет шумов!)

  radio.setPALevel(RF24_PA_MAX); // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate(RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  // должна быть одинакова на приёмнике и передатчике!
  // при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); // начать работу
  radio.stopListening();  // не слушаем радиоэфир, мы передатчик
}

int readAnalogPin(int pinNumber, long toLow, long toHigh, bool isInvert) 
{
  return isInvert
    ? map(analogRead(pinNumber), 0, 1023, toHigh, toLow)
    : map(analogRead(pinNumber), 0, 1023, toLow, toHigh);
}

/**
  * Прочитать значения пульта управления
  *
  * @return массив с текущими значениями снятыми с пульта управления
  */
void readControlPanelValues()
{
  // руль высоты
  controlPanelParams[0] = readAnalogPin(RIGHT_VERTICAL_SLIDER_PIN,
    SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, true); 
    
  // элероны
  controlPanelParams[1] = readAnalogPin(RIGHT_HORIZONTAL_SLIDER_PIN, 
    SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, false); 
    
  // мотор
  controlPanelParams[2] = readAnalogPin(LEFT_VERTICAL_SLIDER_PIN,
    ENGINE_STOP, ENGINE_FULL_SPEED, false); 
}

/**
  * Отправить данные на передатчик
  *
  * @param массив с текущими значениями снятыми с пульта управления
  */
void sendData() 
{
  radio.powerUp();
  radio.write(&controlPanelParams, sizeof(controlPanelParams));
  radio.powerDown();
}

void loop() 
{
  readControlPanelValues();
  sendData();
}
