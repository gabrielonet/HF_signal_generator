#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define W_CLK 8   // Pin 8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD 9   // Pin 9 - connect to freq update pin (FQ)
#define DATA 10   // Pin 10 - connect to serial data load pin (DATA)
#define RESET 11  // Pin 11 - connect to reset pin (RST) 
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
#define OLED_RESET 4
double frecventa = 14000000;
static byte PinA = 0 ;
static byte PinB = 0 ;  
int time_a;
double res = 10;
int res_temp = 0 ;
int cursor = 83 ;
Adafruit_SSD1306 display(OLED_RESET);

void setup(void) {
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  attachInterrupt (digitalPinToInterrupt(2), encoder, CHANGE);   
  attachInterrupt (digitalPinToInterrupt(3), encoder, CHANGE);  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT); 
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);  // this pulse enables serial mode on the AD9850 - Datasheet page 12.
  //sendFrequency(7010000);
  oled(frecventa);
  ad9850(frecventa);
}


// frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void ad9850(double frequency) {  
  int32_t freq = frequency  * 4294967295/125000000;  // note 125 MHz clock on 9850.  You can make 'slight' tuning variations here by adjusting the clock frequency.
  for (int b_it = 0; b_it < 4; b_it++, freq>>=8) {
    ad9850_serial_send(freq & 0xFF);
  }
  ad9850_serial_send(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}
void ad9850_serial_send(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);
  }
}

void encoder (){
  time_a = millis();
  byte newPinA = digitalRead (2); byte newPinB = digitalRead (3);
  if (PinA == 0 && PinB == 0 ){
    if (newPinA == HIGH && newPinB == LOW ) { frecventa += res; }
    if (newPinA == LOW && newPinB == HIGH ) { frecventa -= res; }  
  }  
PinA = newPinA;  PinB = newPinB;
  int time_b = millis();
  ad9850(frecventa);
}  

void oled (long text){
  display.clearDisplay ();
  display.setCursor (10,0);
  display.setTextSize (2);
  display.setTextColor (WHITE);
  display.print (text);
  display.setCursor (cursor,22);
  display.print ("^");
  display.display ();
}

void rezolution() {
  res *= 10 ;
  if (res  < 10 ){res = 1000000;}
  if (res > 1000000){res = 10 ;}
  if (res == 10 ) {cursor = 83 ;}
  if (res == 100 ) {cursor = 70;}
  if (res == 1000 ) {cursor = 58;}
  if (res == 10000 ) {cursor = 46;}
  if (res == 100000 ) {cursor = 34;}
  if (res == 1000000 ) {cursor = 22;}
  }

void loop() {
  int  time_b=millis();
  // insert a small delay between rotary encoder and i2c display to avoid shared intrerupt pins conflict  (Arduino nano)
  if (time_b - time_a > 0.1) {
    oled(frecventa) ;
    time_a = time_b;
  }

if (digitalRead(4) == LOW  && res_temp == 0 ) {res_temp = 1; rezolution() ;} 
if (digitalRead(4) == HIGH && res_temp == 1 ) {res_temp = 0 ;} 



}
