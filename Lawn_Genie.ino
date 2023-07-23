#include <Servo.h>
#define vcc A0
#define trigpin A1
#define echopin A2
#define gnd A3
#define m10 2
#define m11 3
#define m20 4
#define m21 5
#define grassCutter 6
#define irrigationPump 7
Servo myRadar,mySeedDisperser;
char c='\0';
bool manualMode=true;
void manualModeCommands(char c);
float distance=0,duration=0;
float getDistance();
void scanRadar();
void moveForward();
void moveRight();
void moveLeft();
void rotate180();
void autoMode();
void setup()
{
  myRadar.attach(8);//servo motor pin for radar
  mySeedDisperser.attach(9);//servo motor pin for seed dispenser
  pinMode(vcc, OUTPUT);// mode of pin set to output
  pinMode(trigpin, OUTPUT);// trig pin of ultrasonic sensor set to output
  pinMode(echopin, INPUT);// echopin  of ultrasonic sensor set to input
  pinMode(gnd, OUTPUT);
  pinMode(m10, OUTPUT);// left hand side motors 1st pin
  pinMode(m11, OUTPUT);// left hand side motors 2nd pin
  pinMode(m20, OUTPUT);// right hand side motors 1st pin
  pinMode(m21, OUTPUT);// right hand side motors 2nd pin
  pinMode(grassCutter,OUTPUT);
  digitalWrite(grassCutter,HIGH);
  pinMode(irrigationPump,OUTPUT);
  digitalWrite(irrigationPump,HIGH);
  myRadar.write(90);// set angle of radar
  mySeedDisperser.write(0);// set angle of seed feeder
  delay(1000);// waits for all sensors to turn on
}

void loop()
{
  if(Serial.available()>0)// Conditon for Serial communication
  {
    c=Serial.read();// reads input character 
    if(c=='U')
    {
      manualMode=false;
    }
    else if(c=='u')
    {
      stop();
      manualMode=true;
    }
  }
    if(manualMode)
    {
      manualModeCommands(c);// passing value of c
    }
    else
    {
      autoMode();
    }
}
float getDistance()
{
  digitalWrite(trigpin,LOW);// creating a pulse
  delayMicroseconds(10);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);
  duration=pulseIn(echopin,HIGH);// reading time of pulse
  distance=duration*0.034/2;// finding distance through formula speed * time
  delay(20);//only for mainatining accuracy due to change of motion
  return distance;
}

void scanRadar()
{
  bool obstacleOnRight=false,obstacleOnLeft=false;// boolean variables
  myRadar.write(0);// radar angle set to 0
  delay(1000);
  for(int i=0;i<181;i++)// incrementing radar angle by 1 till 180 degree
  {
    myRadar.write(i);// writing radar angle
    if(i<30)// to observe right side till 30 degree angle
    {
      if(getDistance()<20.0)// distance limit threshold
      {
        obstacleOnRight=true;
        i=46;
      }
    }
    else if(i>150)// observe left side obstacle from 151 degrees to 180
    {
      if(getDistance()<20.0)
      {
        obstacleOnLeft=true;
        i=181;
      }
    }
    if(i>180)
    {
      break;
    }
  }
  if(obstacleOnRight && obstacleOnLeft)
  {
    rotate180();// u turn
    delay(9850);// adjust timings accordingly
  }
  else if(obstacleOnRight)
  {
    moveLeft();
    delay(4850);// adjust time according to tyre friction with ground
    moveForward();
    delay(1000);// adjust time according to tyre friction with ground
    moveLeft();
    delay(4850);// adjust time according to tyre friction with ground
  }
  else
  {
    moveRight();
    delay(4800);// adjust time according to tyre friction with ground
    moveForward();
    delay(1000);// adjust time according to tyre friction with ground
    moveRight();
    delay(4800);// adjust time according to tyre friction with ground
  }
  myRadar.write(90);// radar set to 90 degree
}
void moveForward()
{
  digitalWrite(m10,HIGH);// turn on motor in forward direction of left side
  digitalWrite(m11,LOW);
  digitalWrite(m20,HIGH);// turn on motor in forward direction of right side
  digitalWrite(m21,LOW);
}
void rotate180()
{
  digitalWrite(m10,LOW);
  digitalWrite(m11,HIGH);// turn on motor in backward direction of left side
  digitalWrite(m20,HIGH);// turn on motor in forward direction of right side
  digitalWrite(m21,LOW);
}
void moveLeft()
{
  digitalWrite(m10,LOW);
  digitalWrite(m11,HIGH);// turn on motor in backward direction of left side
  digitalWrite(m20,HIGH);// turn on motor in forward direction of right side
  digitalWrite(m21,LOW);
}
void moveRight()
{
  digitalWrite(m10,HIGH);// turn on motor in forward direction of left side
  digitalWrite(m11,LOW);
  digitalWrite(m20,LOW);
  digitalWrite(m21,HIGH);// turn on motor in backward direction of right side
}
void stop()
{
  digitalWrite(m10,LOW);// turn off motors
  digitalWrite(m11,LOW);
  digitalWrite(m20,LOW);
  digitalWrite(m21,LOW);
}
void autoMode()
{
  if(getDistance()<30)//in centimeters
  {
    scanRadar();
  }
  else
  {
    moveForward();
  }
}
void manualModeCommands(char c)
{
  if(c=='F')
  {
    moveForward();
  }
  else if(c=='L')
  {
    moveLeft();  
  }
  else if(c=='R')
  {
    moveRight();  
  }
  else if(c=='B')
  {
    digitalWrite(m10,LOW);
    digitalWrite(m11,HIGH);
    digitalWrite(m20,LOW);
    digitalWrite(m21,HIGH);
  }
  else if(c=='S')
  {
    stop();
  }
  else if(c=='W')
  {
    digitalWrite(grassCutter,LOW);// turns on grass cutter . NOT gate operated
  }
  else if(c=='w')
  {
    digitalWrite(grassCutter,HIGH);// turns off grass cutter . NOT gate operated
  }
  else if(c=='V')
  {
    digitalWrite(irrigationPump,LOW);// turns on pump . NOT gate operated
  }
  else if(c=='v')
  {
    digitalWrite(irrigationPump,HIGH);// turns off pump . NOT gate operated
  }
  else if(c=='X')
  {
    mySeedDisperser.write(45);
    delay(50);
    mySeedDisperser.write(0);
  }
}
