#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000 
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

int vIn = 5;
float r1 = 10000;
//float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
  Serial.begin(9600);
  //pinMode(A0,OUTPUT); //output to cooler
  analogWriteResolution(10);
  analogWrite(A0, 511);
  pinMode(A3, INPUT); //THERMISTER 1
  pinMode(A4, INPUT); //THERMISTER 2
  pinMode(A5, INPUT); //THERMISTER 3
  pinMode(A6, INPUT); //THERMISTER 4
  pinMode(1, INPUT_PULLUP); //Increase Button
  pinMode(4, INPUT_PULLUP); //Decrease Button
  pinMode(5, INPUT_PULLUP); //Flip Button
  attachInterrupt(digitalPinToInterrupt(1), increaseB, FALLING);
  attachInterrupt(digitalPinToInterrupt(4), decreaseB, FALLING);
  attachInterrupt(digitalPinToInterrupt(5), flipB, FALLING);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  
  delay(2000);
  display.clearDisplay();
  display.setTextSize(.25);
  display.setTextColor(WHITE);
  display.setCursor(0,10);   
  display.println("\nset T = 2 C");
  display.println("\ncur T = 21 C");
  display.display();
  
}

double lastVal = 0;
double error = 0;
double runningSum = 0;
int setTemp = 20; //degrees celcius

void loop() {
  delay(10);

  //read * scale to 3.3v - resistor error * scale to 5v
  float thermister1 = analogRead(A3)*.0033*1.52;
  //Serial.println(analogRead(A3));
  //Serial.println(thermister1);
  float thermister2 = analogRead(A4)*.0033*1.52;
  float thermister3 = analogRead(A5)*.0033*1.52;
  float thermister4 = analogRead(A6)*.0033*1.52;

  //Serial.println(thermister1);
  //Serial.println(thermister2);
  //Serial.println(thermister3);
  //Serial.println(thermister4);

  float thermisterAve = (thermister1 + thermister2 + thermister3 + thermister4) / 4;
  //float vOut = (thermisterAve * 0.0038 - .3)*1.52;
  float vOut = thermisterAve;
  //Serial.println(vOut);

  //float vOut = analogRead(A3)*.0033*1.52;
  
  float r2 = vOut * r1 / ( vIn - vOut );

  float temp;
  temp = r2 / THERMISTORNOMINAL;     // (R/Ro)
  temp = log(temp);                  // ln(R/Ro)
  temp /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  temp += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  temp = 1.0 / temp;                 // Invert
  temp -= 273.15;                         // convert absolute temp to C
  //Serial.println(temp);

  temp = temp - 4;
  
  error = setTemp - temp;
  runningSum = runningSum + error;
  float deriv = error-lastVal;
  float pid = 512 + 35*error + .05*runningSum + .05*deriv;
  if(pid > 1023) {
    pid = 1023;
  }
  if(pid < 1) {
    pid = 1;
  }
  /*if( pid < 200 ) {
    pid = 200;
  }
  if( pid > 900 ) {
    pid = 900;
  }*/
  //Serial.print("Pid: ");
  //Serial.println(pid);
  analogWriteResolution(10);
  analogWrite(A0, pid);
  lastVal = error;

  display.clearDisplay();
  display.setCursor(0,10);
  display.print("set = ");
  display.print(setTemp);
  display.print(" C\n");
  display.print("cur = ");
  display.print(temp);
  display.print(" C");
  display.display();
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("set temp: ");
//  lcd.print(setTemp);
//  lcd.setCursor(0,1);
//  lcd.print("cur temp: ");
//  lcd.print("NA");
}

void increaseB() {
  if( setTemp > 29 ) {
    return;
  }
  setTemp++;
  delayMicroseconds(2000);
}

void decreaseB() {
  if( setTemp < 3 ) {
    return;
  }
  setTemp--;
  delayMicroseconds(2000);
}

void flipB() {
  if( setTemp < 16 ) {
    setTemp = 30;
  } else {
    setTemp = 2;
  }
  delayMicroseconds(2000);
}
