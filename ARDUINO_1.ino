#include <dht11.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_I2CDevice.h>
#include <SoftwareSerial.h>
//#include <SPI.h>
//#include <MFRC522.h>
//#include <WiFiUdp.h>


//#define WIFI_SSID "ToxiFi"
//#define WIFI_PASSWORD "pranto11223344"
//const String YOUR_PHONE = "8801624846897"; //Without +
//const String YOUR_APIKEY = "91935970"; //Your API KEY
//const String URL = "https://api.whatabot.net/whatsapp/sendMessage?apikey=91935970&text=Hello%20from%20Whatabot&phone=8801624846897";
//WiFiUDP ntpUDP;

//#define SS_PIN 10 //RX slave select
//#define RST_PIN 9

//MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

/*byte card_ID[4]; //card UID size 4byte
byte Name1[4]={0x77,0x74,0x70,0x63};//first UID card
byte Name2[4]={0x09,0x93,0xBF,0xB9};//second UID card
byte Name3[4]={0xD3,0x1A,0x08,0x0C};
byte Name4[4]={0xC3,0xBF,0xC5,0x11};
//if you want the arduino to detect the cards only once
int NumbCard[4];//this array content the number of cards. in my case i have just two cards.
int j=0;        
*/
//int const RedLed=6;
//int const GreenLed=5;
//int const Buzzer=8;

//String Name;//user name
//long Number;//user number
//int n ;//The number of card you want to detect (optional) 
int RX=3;
int TX=2;

dht11 dhtObject;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial esp8266(RX,TX);  

int TRIGGER_PIN=4;
int ECHO_PIN=5;


int dht_apin=6;
int CUT_OFF_TEMPERATURE =95; //This is temperature in degreeC
int MAX_DISTANCE=5; 
float duration, distance;

String AP = "OnePlus 9 5G";       // AP NAME
String PASS = "235146175"; // AP PASSWORD
String API = "F1NJ35JTXVDXVV3O";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
  

  


void setup()
{
  Serial.begin (9600);
  esp8266.begin(9600);   
  mlx.begin();

   lcd.init();
   lcd.backlight();
   //Wire.setClock(400000);
  lcd.print("IoT Temp-Env System");
  delay(2000);
  //lcd.clear();

  lcd.clear();
    lcd.setCursor(0,0);
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);

  
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  
  
}

void loop()
{
 String getData = "GET /update?api_key="+ API +"&field1="+temp()+"&field2="+getTemperatureValue()+"&field3="+getHumidityValue();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");

    temp();
    getTemperatureValue();
    getHumidityValue();
   
    
}

String temp()
{
    digitalWrite(TRIGGER_PIN,LOW);
    delay(2);
    digitalWrite(TRIGGER_PIN,HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN,LOW);
    duration=pulseIn(ECHO_PIN,HIGH);
    distance=(duration*0.034)/2;
    Serial.print(distance);
    float temperature=mlx.readObjectTempF(); 
      Serial.print("Temperature");
      Serial.print(temperature);
  if(distance<=MAX_DISTANCE)
    {
      Serial.print("Temperature");
      Serial.print(temperature);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("T:");
       lcd.setCursor(7,0);
      lcd.print(temperature);
      lcd.println("F ");
      lcd.setCursor(13,0);
        delay (100);
    }
    else
    {
    
    lcd.setCursor(0,0);
    lcd.print("****BeInRange**** ");
    delay(200);
    //lcd.setCursor(0,0);

    //buzzer();
    }

    
    //delay(200);
     return String(mlx.readObjectTempF()); 
    }

String getTemperatureValue()
{
  

    dhtObject.read(dht_apin);
    float temp = dhtObject.temperature;
    lcd.setCursor(0,1);
    Serial.print("RT:");
   Serial.println( temp); 
   lcd.print("C");
   //lcd.setCursor(10,1);
   //delay(2000);
   return String(temp); 
  
}


String getHumidityValue()
{
 

   dhtObject.read(dht_apin);
   float humidity = dhtObject.humidity;
    lcd.setCursor(7,1);
   lcd.print("H%=");
   lcd.println( humidity);

   return String(humidity);
  
}
/*void buzzer()

{
 //int t=mlx.readObjectTempF();
  if ( mlx.readObjectTempF()>CUT_OFF_TEMPERATURE)
  {
    digitalWrite(BUZZER_PIN,HIGH);
    delay(1000);
  }
  else 
  {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1000);
}*/

 void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
