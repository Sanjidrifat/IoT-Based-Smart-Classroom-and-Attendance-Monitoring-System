
//Defining libraries
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_I2CDevice.h>
#include <dht11.h>
#include <SoftwareSerial.h>
int RX=3;
int TX=2;
// Defining variables, parameters and setting threshold value of the sensors
LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
SoftwareSerial esp8266(RX,TX);
long duration, distance;
const int trigPin=4;
const int echoPin=5;
int dht_apin=6;
int MAX_DISTANCE=10;
int CUT_OFF_TEMPERATURE =99;
dht11 dhtObject;

const int buzzer=8;

//Setting Wi-Fi access points and password

String AP = "Pranto"; // AP NAME
String PASS = "11223344"; // AP PASSWORD
String API = "IEHUU7A6KUXCUFWG"; // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand;
boolean found = false;
int valSensor = 1;

//Initializing the LCD display, ESP8266, mlx90614,Ultrasonic Sensor,Buzzer,and DHT11 sensor

void setup()
{
Serial.begin (9600);
mlx.begin();
esp8266.begin(9600);
lcd.init();
lcd.backlight();
lcd.print("Smart Monitoring");
delay(500);
lcd.clear();
lcd.setCursor(0,0);
pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);
pinMode(buzzer,OUTPUT);
sendCommand("AT",5,"OK");
sendCommand("AT+CWMODE=1",5,"OK");
sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}


//In the void loop we call the four functions that we want to perform repeatedly and send the data instanly to the ThingSpeak Server

void loop ()
{
temp();
getTemperatureValue();
getHumidityValue();
//buzzer();
String getData = "GET /update?api_key="+ API +"&field1="+temp()+"&field2="+getTemperatureValue()+"&field3="+getHumidityValue();
sendCommand("AT+CIPMUX=1",5,"OK");
sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
esp8266.println(getData);
delay(1);
countTrueCommand++;
sendCommand("AT+CIPCLOSE=0",5,"OK");
}

//Function for determing body temperature and printing them on to the LCD

String temp()
{
  digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
distance = duration * 0.034 / 2;
// Prints the distance on the Serial Monitor
delay(100);
if(distance<=MAX_DISTANCE)
{
float temperature=mlx.readObjectTempF();
//buzzer();
lcd.clear();
lcd.setCursor(0,0);
lcd.print("BTemp: ");
lcd.setCursor(7,0);
lcd.print(temperature,2);
lcd.print((char)223 );
lcd.println(" F ");
lcd.setCursor(14,0);
delay (100);
}
else
{
lcd.setCursor(0,0);
lcd.print("**Stay in range**");
delay(50);
}
return String(mlx.readObjectTempF());
}
//Function for measuring room temperature and printing them in LCD

String getTemperatureValue()
{
dhtObject.read(dht_apin);
float temp = dhtObject.temperature;
lcd.setCursor(0,1);
lcd.print("RT:");
lcd.println( temp,1);
lcd.setCursor(7,1);
lcd.print((char) 223);
lcd.print("C");
lcd.setCursor(0,1);
delay(100);
return String(temp);
}
//Function for measuring humidity and printing them in LCD

String getHumidityValue()
{
float humidity = dhtObject.humidity;
dhtObject.read(dht_apin);
lcd.setCursor(11,1);
lcd.print("H:");
lcd.println( humidity,0);
lcd.setCursor(15,1);
lcd.print("%");
lcd.setCursor(11,1);
delay(100); return String(humidity);
}

//Function for Buzzer output

/*void buzzer()
{
int t=mlx.readObjectTempF();
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
}
*/
//Building up connection between the network and ESP8266

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
}
countTimeCommand++;
}
if(found == true)
{
Serial.println("CONNECTED...:D");
countTrueCommand++;
countTimeCommand = 0;
}

if(found == false)
{
Serial.println("NOT CONNECTED....:(");
countTrueCommand = 0;
countTimeCommand = 0;
}
found = false;
}
countTimeCommand++;
}
if(found == true)
{
Serial.println("CONNECTED...:D");
countTrueCommand++;
countTimeCommand = 0;
}
if(found == false)
{
Serial.println("NOT CONNECTED....:(");
countTrueCommand = 0;
countTimeCommand = 0;
}
found = false;
}
}
