#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <LiquidCrystal.h>
#define PIN        8
#define NUMPIXELS 10
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500
#define DHT11_PIN 9
DHT dht;
int TrigL = 11;  //pin 2 Arduino połączony z pinem Trigger czujnika
int EchoL = 10;  //pin 3 Arduino połączony z pinem Echo czujnika
int TrigR = 13;
int EchoR = 12;
int CML;        //odległość w cm
int CMR;
long CZASL;     //długość powrotnego impulsu w uS
long CZASR;
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
void setup()
{
  Serial.begin(9600);                         //inicjalizaja monitora szeregowego
  lcd.begin(16, 2);
  lcd.print("Inicjalizacja...");              // Print a message to the LCD.
  pinMode(TrigL, OUTPUT);                     //ustawienie pinu 2 w Arduino jako wyjście
  pinMode(EchoL, INPUT);                      //ustawienie pinu 3 w Arduino jako wejście
  pinMode(TrigR, OUTPUT);
  pinMode(EchoR, INPUT);
  dht.setup(DHT11_PIN);                       //ustawienie pin 11 jako czujnik dht
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
}
  
void loop()
{
  int wilgotnosc = dht.getHumidity();
  int temperatura = dht.getTemperature();
  lcd.clear(); 
	lcd.setCursor(0,0);                         // ustawienie kursora na ekranie w punkcie 0.0
	lcd.print("Temperatura="); 
	lcd.print(temperatura);                     // pokazanie wartosci temperatury 
  lcd.print((char)223);                       // komenda by wstawic znak "°"
  lcd.print("C");
	lcd.setCursor(0,1);                         //ustawienie kursora na ekranie w punkcie 0.1 czyli 2 rząd
	lcd.print("Wilgotnosc="); 
	lcd.print(wilgotnosc);                      //pokazanie wartosci wilgotnosci powietrza 
	lcd.print("%RH"); 
	delay(2000); 

  pixels.setPixelColor(0, pixels.Color(30, 0, 30));   // inicjalizacja pierwszych dwóch diod aby świeciły (jako że zasilanie działa)
  pixels.setPixelColor(1, pixels.Color(30, 0, 30));
  pixels.show(); 
  if (dht.getStatusString() == "OK")          // sprawdzenie czy czujka działa poprawnie
  {
    Serial.println("Temperatura i wilgotnosc");   //to samo co na ekranie LCD tylko w programie
    Serial.print(temperatura);
    Serial.print("*C | ");
    Serial.print(wilgotnosc);
    Serial.println("%RH");
    
  }
  delay(dht.getMinimumSamplingPeriod());

  pomiar_odleglosciL();              //pomiar odległości
  Serial.print("OdlegloscLewy: ");      //wyświetlanie wyników na ekranie w pętli co 200 ms
  Serial.print(CML);
  Serial.println(" cm");

  pomiar_odleglosciR();
  Serial.print("OdlegloscPrawy: ");
  Serial.print(CMR);
  Serial.println(" cm");
  delay(1000);

  if (CML <= 20)
  {
    pixels.setPixelColor(2, pixels.Color(0, 0, 30));  // zabawa diodami w celu odpowiedniego zapalania sie
    pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(4, pixels.Color(30, 0, 0));
    pixels.setPixelColor(5, pixels.Color(30, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }
  else if (CML > 21 && CML < 40)
  {
    pixels.setPixelColor(2, pixels.Color(0, 30, 0));
    pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    pixels.setPixelColor(4, pixels.Color(30, 0, 0));
    pixels.setPixelColor(5, pixels.Color(30, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }
  else
  {
    pixels.setPixelColor(2, pixels.Color(30, 0, 0));
    pixels.setPixelColor(3, pixels.Color(30, 0, 0));
    pixels.setPixelColor(4, pixels.Color(30, 0, 0));
    pixels.setPixelColor(5, pixels.Color(30, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }

  if (CMR <= 20)
  {
    pixels.setPixelColor(6, pixels.Color(0, 0, 30));
    pixels.setPixelColor(7, pixels.Color(0, 0, 0));
    pixels.setPixelColor(8, pixels.Color(30, 0, 0));
    pixels.setPixelColor(9, pixels.Color(30, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }
  else if (CMR > 21 && CMR < 40)
  {
    pixels.setPixelColor(6, pixels.Color(0, 30, 0));
    pixels.setPixelColor(7, pixels.Color(0, 0, 0));
    pixels.setPixelColor(8, pixels.Color(30, 0, 0));
    pixels.setPixelColor(9, pixels.Color(30, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }
  else
  {
    pixels.setPixelColor(6, pixels.Color(30, 0, 0));
    pixels.setPixelColor(7, pixels.Color(30, 0, 0));
    pixels.setPixelColor(8, pixels.Color(30, 0, 0));
    pixels.setPixelColor(9, pixels.Color(30, 0, 0));
    pixels.show();
    delay(DELAYVAL);
  }

}
  
void pomiar_odleglosciL ()
{
  digitalWrite(TrigL, LOW);        //ustawienie stanu wysokiego na 2 uS - impuls inicjalizujacy - patrz dokumentacja
  delayMicroseconds(2);
  digitalWrite(TrigL, HIGH);       //ustawienie stanu wysokiego na 10 uS - impuls inicjalizujacy - patrz dokumentacja
  delayMicroseconds(10);
  digitalWrite(TrigL, LOW);
  digitalWrite(EchoL, HIGH); 
  CZASL = pulseIn(EchoL, HIGH);
  CML = CZASL / 58;                //szerokość odbitego impulsu w uS podzielone przez 58 to odleglosc w cm - patrz dokumentacja
}

void pomiar_odleglosciR ()
{
 digitalWrite(TrigR, LOW);
 delayMicroseconds(2);
 digitalWrite(TrigR, HIGH);
 delayMicroseconds(10);
 digitalWrite(TrigR, LOW);
 digitalWrite(EchoR, HIGH);
 CZASR = pulseIn(EchoR, HIGH);
 CMR = CZASR / 58;
}


