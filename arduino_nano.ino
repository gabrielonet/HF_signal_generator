#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>



#define W_CLK 8   // Pin 8 - connect to AD9850 module word load clock pin (CLK)
#define FQ_UD 9   // Pin 9 - connect to freq update pin (FQ)
#define DATA 10   // Pin 10 - connect to serial data load pin (DATA)
#define RESET 11  // Pin 11 - connect to reset pin (RST) 
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

double frecventa = 7010000;
static byte PinA = 0 ;
static byte PinB = 0 ;  


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

void setup(void) {
  Serial.begin(9600) ;
  //digitalWrite (20, HIGH);   //  Encoder intrerupt pin 2
  //digitalWrite (21, HIGH);   //  Encoder intrerupt pin 3 
  attachInterrupt (digitalPinToInterrupt(2), encoder, CHANGE);   
  attachInterrupt (digitalPinToInterrupt(3), encoder, CHANGE);  
//  attachInterrupt (5, sw_press, CHANGE);   // pin 18


    
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
 sendFrequency(frecventa);

}



// frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {  
  int32_t freq = frequency  * 4294967295/125000000;  // note 125 MHz clock on 9850.  You can make 'slight' tuning variations here by adjusting the clock frequency.
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
    //Serial.println(freq);
  }
  
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}
// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}


void encoder (){
  byte newPinA = digitalRead (2); byte newPinB = digitalRead (3);
  if (PinA == 1 && PinB == 1 )
  {
   if (newPinA == HIGH && newPinB == LOW ) { frecventa-= 50; }
   if (newPinA == LOW && newPinB == HIGH ) { frecventa += 50; }  
  }  
  PinA = newPinA;  PinB = newPinB;
  //Serial.println(frecventa);
  
  //oled(frecventa) ;
  sendFrequency(frecventa);

}  



void oled (long text){
display.clearDisplay ();
display.setCursor (10,5);
display.setTextSize (2);
display.setTextColor (WHITE);
display.print (text);
display.print("Hz");
display.display ();
}






void loop() {
}
