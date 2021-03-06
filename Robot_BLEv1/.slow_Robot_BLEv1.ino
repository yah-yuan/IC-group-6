//********************************************
//* Robotic Arm with BLE control v1
//* for robotic arm 0
//* By Benny Lo
//* Jan 14 2018
//********************************************
#include <CurieBLE.h>
#include <Servo.h>
#define ROBOT_NAME "Group6"
#define CRAW_MIN 0  //open
#define CRAW_MAX 58 //close
#define ELBOW_MIN 0
#define ELBOW_MAX 140
#define SHOULDER_MIN 0
#define SHOULDER_MAX 165
#define WRIST_X_MIN 0
#define WRIST_X_MAX 180
#define WRIST_Y_MIN 0
#define WRIST_Y_MAX 90
#define WRIST_Z_MIN 0
#define WRIST_Z_MAX 180
#define BASE_MIN 0
#define BASE_MAX 180
#define ELBOW_DEFAULT 60
#define SHOULDER_DEFAULT 100
#define WRIST_X_DEFAULT 80
#define WRIST_Y_DEFAULT 90
#define WRIST_Z_DEFAULT 66
#define BASE_DEFAULT 90
#define CRAW_DEFAULT CRAW_MIN //fully opened
Servo myservoA;
Servo myservoB;
Servo myservoC;
Servo myservoD;
Servo myservoE;
Servo myservoF;
Servo myservoG; //the craw
int i, pos, myspeed;
int sea, seb, sec, sed, see, sef, seg;

BLEPeripheral blePeripheral;                                      //create peripheral instance
BLEService BrobotService("47452000-0f63-5b27-9122-728099603712"); //BLE service
//characteristics for the servo motors
BLECharCharacteristic BLE_ServoA("47452001-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_ServoB("47452002-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_ServoC("47452003-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_ServoD("47452004-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_ServoE("47452005-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_ServoF("47452006-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_ServoG("47452008-0f63-5b27-9122-728099603712", BLERead | BLEWrite);
BLECharCharacteristic BLE_Reset("47452007-0f63-5b27-9122-728099603712", BLEWrite);

int BLE_connected = 0;

void myservosetup() //set up the servo motors
{
  sea = myservoA.read();
  seb = myservoB.read();
  sec = myservoC.read();
  sed = myservoD.read();
  see = myservoE.read();
  sef = myservoF.read();
  seg = myservoG.read();

  for (pos = 0; pos <= myspeed; pos += 1)
  // 缓慢回到初始值
  {
    myservoA.write(int(map(pos, 1, myspeed, sea, ELBOW_DEFAULT)));
    myservoB.write(int(map(pos, 1, myspeed, seb, SHOULDER_DEFAULT)));
    myservoC.write(int(map(pos, 1, myspeed, sec, WRIST_X_DEFAULT)));
    myservoD.write(int(map(pos, 1, myspeed, sed, WRIST_Y_DEFAULT)));
    myservoE.write(int(map(pos, 1, myspeed, see, WRIST_Z_DEFAULT)));
    myservoF.write(int(map(pos, 1, myspeed, sef, BASE_DEFAULT)));
    myservoG.write(int(map(pos, 1, myspeed, seg, CRAW_DEFAULT)));
    delay(1);
  }
}

void Write_servo(Servo servo, int origin, int pwm, int step)
{
  int time_delay = 1;
  if (origin < pwm)
  {
    while (1)
    {
      servo.write(origin);
      origin += step;
      delay(time_delay);
      if (origin >= pwm)
      {
        break;
      }
    }
  }
  else
    {
      while (1)
      {
        servo.write(origin);
        origin -= step;
        delay(time_delay);
        if (origin <= pwm)
        {
          break;
        }
      }
    }
}

void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT); //LED control

  myspeed = 5000;

  myservoA.attach(2);
  myservoB.attach(3);
  myservoC.attach(4);
  myservoD.attach(5);
  myservoE.attach(6);
  myservoF.attach(7);
  myservoG.attach(8);

  myservoA.write(ELBOW_DEFAULT);
  myservoB.write(SHOULDER_DEFAULT);
  myservoC.write(WRIST_X_DEFAULT);
  myservoD.write(WRIST_Y_DEFAULT);
  myservoE.write(WRIST_Z_DEFAULT);
  myservoF.write(BASE_DEFAULT);
  myservoG.write(CRAW_DEFAULT);
  // set the local name peripheral advertises
  blePeripheral.setLocalName(ROBOT_NAME);
  // set the UUID for the service this peripheral advertises
  blePeripheral.setAdvertisedServiceUuid(BrobotService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(BrobotService);
  blePeripheral.addAttribute(BLE_ServoA);
  blePeripheral.addAttribute(BLE_ServoB);
  blePeripheral.addAttribute(BLE_ServoC);
  blePeripheral.addAttribute(BLE_ServoD);
  blePeripheral.addAttribute(BLE_ServoE);
  blePeripheral.addAttribute(BLE_ServoF);
  blePeripheral.addAttribute(BLE_ServoG);
  blePeripheral.addAttribute(BLE_Reset);

  // assign event handlers for connected, disconnected to peripheral
  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  BLE_ServoA.setEventHandler(BLEWritten, ServoACharacteristicWritten);
  BLE_ServoB.setEventHandler(BLEWritten, ServoBCharacteristicWritten);
  BLE_ServoC.setEventHandler(BLEWritten, ServoCCharacteristicWritten);
  BLE_ServoD.setEventHandler(BLEWritten, ServoDCharacteristicWritten);
  BLE_ServoE.setEventHandler(BLEWritten, ServoECharacteristicWritten);
  BLE_ServoF.setEventHandler(BLEWritten, ServoFCharacteristicWritten);
  BLE_ServoG.setEventHandler(BLEWritten, ServoGCharacteristicWritten);
  BLE_Reset.setEventHandler(BLEWritten, ResetCharacteristicWritten);

  // set an initial value for the characteristic
  BLE_ServoA.setValue(ELBOW_DEFAULT);
  BLE_ServoB.setValue(SHOULDER_DEFAULT);
  BLE_ServoC.setValue(WRIST_X_DEFAULT);
  BLE_ServoD.setValue(WRIST_Y_DEFAULT);
  BLE_ServoE.setValue(WRIST_Z_DEFAULT);
  BLE_ServoF.setValue(BASE_DEFAULT);
  BLE_ServoG.setValue(CRAW_DEFAULT);

  // advertise the service
  blePeripheral.begin();
  Serial.println(("Bluetooth device active, waiting for connections..."));
}

void loop()
{
  //Serial.println("Loop");
  blePeripheral.poll();

  i = 10;
  myservosetup();
  while (1)
  {
    if (BLEConnected)
    {
      digitalWrite(13, HIGH); // turn the LED on (HIGH is the voltage level)
      delay(250);             // wait for a second
      digitalWrite(13, LOW);  // turn the LED off by making the voltage LOW
      delay(250);
    }
    else
    {
      digitalWrite(13, HIGH); // turn the LED on (HIGH is the voltage level)
      delay(1000);            // wait for a second
      digitalWrite(13, LOW);  // turn the LED off by making the voltage LOW
      delay(1000);
    }
  }
}

void blePeripheralConnectHandler(BLECentral &central)
{
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
  BLE_connected = 1;
}

void blePeripheralDisconnectHandler(BLECentral &central)
{
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
  myservosetup(); //return to original position
}

void ServoACharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoA.value();
  long se = myservoA.read();
  Write_servo(myservoA, se, to, 1);
  Serial.print("ServoA Control: ");
  Serial.println((byte)BLE_ServoA.value(), DEC);
}

void ServoBCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoB.value();
  long se = myservoB.read();
  Write_servo(myservoB, se, to, 1);
  Serial.print("ServoB Control: ");
  Serial.println((byte)BLE_ServoB.value(), DEC);
}
void ServoCCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoC.value();
  long se = myservoC.read();
  Write_servo(myservoC, se, to, 1);
  Serial.print("ServoC Control: ");
  Serial.println((byte)BLE_ServoC.value(), DEC);
}
void ServoDCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoD.value();
  long se = myservoD.read();
  Write_servo(myservoD, se, to, 1);
  Serial.print("ServoD Control: ");
  Serial.println((byte)BLE_ServoD.value(), DEC);
}

void ServoECharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoE.value();
  long se = myservoE.read();
  Write_servo(myservoA, se, to, 1);
  Serial.print("ServoE Control: ");
  Serial.println((byte)BLE_ServoE.value(), DEC);
}

void ServoFCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoF.value();
  long se = myservoF.read();
  Write_servo(myservoF, se, to, 1);
  Serial.print("ServoF Control: ");
  Serial.println((byte)BLE_ServoF.value(), DEC);
}

void ServoGCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // central wrote new value to characteristic, update LED
  long to = (byte)BLE_ServoG.value();
  long se = myservoG.read();
  Write_servo(myservoG, se, to, 1);
  Serial.print("ServoG Control: ");
  Serial.println((byte)BLE_ServoG.value(), DEC);
}

void ResetCharacteristicWritten(BLECentral &central, BLECharacteristic &characteristic)
{
  // Reset - back to original position
  Serial.println("Reset");
  myservosetup(); //return to original position
}
