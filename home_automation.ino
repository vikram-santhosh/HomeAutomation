
#include<Wire.h>
#include<Servo.h>
#include<math.h>
#include<rgb_lcd.h>
#include <SPI.h>
#include <Ethernet.h>


byte mac[] = {0x98, 0x4F, 0xEE, 0x01, 0x82, 0xD7 };
IPAddress ip(192,168,0,8);
EthernetClient client;
EthernetServer server(23);

rgb_lcd lcd;
Servo gate ;

float temperature;
int B=3975;                  //B value of the thermistor
float resistance;
boolean connection = false;
int mode = 0;
int threshold = 400 ;

int sound = 2;
int led = 2 ;
int buzzer = 3;
int serv = 5;
int temp = 1 ;
int photo = 3;

String ctrl;

void setup() {
 
  Serial.begin(9600);
  
  pinMode(led,OUTPUT);
  Serial.println("Initializing LED........");
  
  pinMode(photo,INPUT);
  Serial.println("Initializing Photo Sensor........");
  
  pinMode(temp,OUTPUT);
  Serial.println("Initializing Temperature Sensor........");
  
  gate.attach(5);
  Serial.println("Initializing Servo Motor ........");
  
  lcd.begin(16,2);
  Serial.println("Initializing LCD ........");
  lcd.setRGB(0,0,125);
  Serial.println("Initializing Setting RGB ........");
  
  lcd.print("Initializing ..");
  
  delay(2000);

  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.println("Initializing Server........");
 
  }
void instr()
{
  server.println("ledOn - Turn on LED");
  server.println("ledOff - Turn off LED");
  server.println("temp - Display ambient temperature");

}

void prompt()
{
  lcd.setRGB(0,100,100);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mode 1 : REMOTE");
  Serial.println("Enter 1 for REMOTE ACCESS");
  
  lcd.setCursor(0,1);
  lcd.print("Mode 2 : LOCAL");
  Serial.println("Enter 2 for LOCAL ACCESS");
  delay(2000);
  
  if(Serial.available())
  {
    mode = Serial.read()-'0';
  }
  
  Serial.print("Mode value : ");
  Serial.println(mode);
  
}

void mode_1()
{
  while(true)
  {
    Serial.println("Looking for Cients......");
    client = server.available();
    delay(1000);
    if(client)
      break;
  }
  if(client)
  {
    Serial.println("Client found......");
    if(connection==false)
    {
      Serial.println("Establishing connection .......");
      connection=true;
    }
  
  }
  int flag;
  while(connection==true)
  {
    client.flush();
    ctrl="";
    server.println("Enter a Command and hit Enter ");
    Serial.println("Prompting for command .....");
    Serial.println("Enter 0 to quit terminal mode");
    while(Serial.available())
    {
        flag = Serial.read() - '0';
    }
    
    if(!flag)
        {
          Serial.println("Disconnecting Client .......");
          client.stop();
          break;
        }
    else 
      getCommand();
  }
  
  
  
}

void getCommand()
{
  while(client.available())
  {
    char next = client.read();
    if(next==0x0D)
    {
      server.print("Recieved Command is : ");
      server.println(ctrl);
      Serial.print(" Received Command is ");
      Serial.println(ctrl);
      processCommand(ctrl);
    }
    else
    {
      ctrl+=next;
    }
  }
  
}

void processCommand(String ctrl)
{
  server.println("Procssing Command ..........");
  if(ctrl.indexOf("ledOn")>-1)
  {
    digitalWrite(led,HIGH);
    Serial.println("LED ON");
    ctrl="";
  }
  else if(ctrl.indexOf("ledOff")>-1)
  {
    digitalWrite(led,LOW);
    Serial.println("LED OFF");
    ctrl="";
  }
  
  else if(ctrl.indexOf("temp")>-1)
  {
    int a=analogRead(temp);
    resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
    temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    lcd.clear();
    lcd.setRGB(0,100,0);
    lcd.setCursor(0,0);
    lcd.print("Temp is : ");
    lcd.setCursor(0,10);
    lcd.print(temperature);
  
    server.print("Current temperature is : ");
    server.print(temperature);  
    server.println(" deg Cel");
      
    Serial.print("The current temperature is ");
    Serial.println(temperature);
    delay(2000);
  }
  
  else
  {
    server.println("Invalid Command");
    instr();
    getCommand();
  }
}

void mode_2()
{
  
    Serial.println("Enter 1 for Rotatary Motion");
    Serial.println("Enter 2 to monitor temperature ");
    Serial.println("Enter 3 to enable the proximity sensor");
    delay(3000);
    
    int ch;
    while(Serial.available()>0)
    {
      ch = Serial.read() - '0';
    }
    int val , level;
    switch(ch)
    {
      case 1 :{
                int flag=1;
                  while(1)
                  {
                        Serial.println("Enter 0 to stop Rotatry Sensing ......");
                        while(Serial.available())
                          {
                             flag = Serial.read() - '0'; 
                          }
                          
                        if(!flag)
                        {
                          Serial.println("Stopping Rotatry Motion");
                          break;
                        }

                        val = analogRead(photo);
                        constrain(val,50,400);
                        level = map(val,50,400,0,255);
                        if(level<175)
                        {
                          gate.write(180);
                        }
                        
                        else if(level>=175)
                        {
                          gate.write(0);
                        }
                        
                        val = analogRead(photo);
                  }
                  
                  break;
      
              }
              
       case 2 :{
       
                  int a=analogRead(temp);
                  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
                  temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
                  Serial.print("Ambient Temperature is ");
                  Serial.println(temperature);
                  lcd.clear();
                  lcd.setRGB(0,100,0);
                  lcd.setCursor(0,0);
                  lcd.print(temperature);
                  delay(5000);
                  break;                
         
                }
       case 3 : 
               {
                  lcd.setRGB(0,100,0);
                  lcd.clear();
                  lcd.print("Proximity Sensor");
                   
                    while(true)
                    {
                        int flag = 1;
                        Serial.println("Enter 0 to quit");
                        while(Serial.available()>0)
                        {
                          flag = Serial.read() - '0';
                        }
                        
                        if(!flag)
                          break;
                          
                          
                      val = analogRead(photo);
                      constrain(val,50,400);
                      level = map(val,50,400,0,255);
                      
                      if(level<=100)
                      {
                          lcd.clear();
                          lcd.setRGB(100,0,0);
                          lcd.setCursor(0,1);
                          lcd.print("CLOSE");    
                 
                      }
                      
                      if(level>100)
                      {
                          lcd.clear();
                          lcd.setRGB(0,0,100);
                          lcd.setCursor(0,1);
                          lcd.print("FAR");    
                 
                      }
                      
                      val = analogRead(photo);
                      
                      }
                      
                      break ;
               }
      default : Serial.println("Invalid Entry ");
                mode_2();
                break ;
    
    }
    
}


void loop()
{
  
  Serial.println("Looping ......");
  
  int noise ;
  
  noise = analogRead(sound);
  Serial.println(noise);
  if(noise > threshold)
  {
    lcd.setRGB(100,0,0);
    lcd.clear();
    Serial.println("Abnormal Noise Dedected ! Alert !!");
    lcd.print("ALERT");
    delay(3000);
  }
  noise = analogRead(sound);
  
  mode = 0;
  prompt();
  
    lcd.setRGB(0,100,100);
    lcd.clear();
    lcd.setCursor(0,0);
    
  if(mode==1)
  {
    
    lcd.print("Mode - 1");
    Serial.println("Entering Mode-1");
    mode_1();
  }
  
  if(mode==2)
  {

    lcd.print("Mode - 2");
    Serial.println("Entering Mode-2");
    mode_2();
  }
  
  if(mode == 0)
  {
    Serial.println("Waiting .........");
  }
  else 
  {
    Serial.println("Invalid choice . Try Again .... ");
  }
  
  
}








