
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <Servo.h>

int RF24_CHIP_ENABLE_PIN = 9;
int RF24_CHIP_SELECT_PIN = 10;
RF24 radio(RF24_CHIP_ENABLE_PIN, RF24_CHIP_SELECT_PIN); 

int ELEVATOR_SERVO_PIN = 3; // настроить пин сервопривода руля высоты как выход 3
int AILERONS_SERVO_PIN = 4; // настроить пин сервопривода элеронов как выход 4 
int ENGINE_REGULATOR_PIN = 5; // настроить пин регулятора оборотов винта как выход 5

int SG90_MIN_ANGLE_IMPULSE = 640;
int SG90_MAX_ANGLE_IMPULSE = 2260;


Servo aileronsServo;
Servo elevatorServo;
Servo engine;

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

int recievedData[3];

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  elevatorServo.attach(ELEVATOR_SERVO_PIN, SG90_MIN_ANGLE_IMPULSE, SG90_MAX_ANGLE_IMPULSE); 
  aileronsServo.attach(AILERONS_SERVO_PIN, SG90_MIN_ANGLE_IMPULSE, SG90_MAX_ANGLE_IMPULSE);
  engine.attach(ENGINE_REGULATOR_PIN);

  radio.begin(); // активировать модуль
  radio.setAutoAck(0); // режим подтверждения приёма, 1 вкл 0 выкл

  radio.openReadingPipe(1, address[0]); // хотим слушать трубу 0
  radio.setChannel(0x60); // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening(); //начинаем слушать эфир, мы приёмный модуль
}
 
void loop() 
{  
  byte pipeNo;
  // слушаем эфир со всех труб
  while(radio.available(&pipeNo))
  { 
    radio.read(&recievedData, sizeof(recievedData));

    elevatorServo.write(recievedData[0]);
    aileronsServo.write(recievedData[1]); 
    engine.writeMicroseconds(recievedData[2]);
  }
}
