#include <USBComposite.h> // https://github.com/arpruss/USBComposite_stm32f1
#include "src/RotaryEncoder/RotaryEncoder.h"  
#include "src/WS2812/WS2812B.h"  

#define NUM_LEDS 60
#define MSGSIZE 64

WS2812B strip = WS2812B(NUM_LEDS);

USBHID HID;
HIDRaw<MSGSIZE,MSGSIZE> usb(HID);

byte buf[MSGSIZE];

RotaryEncoder encoder1(PA0,PA1,PA2);
RotaryEncoder encoder2(PB3,PB4,PB5);

int8_t count = 0;

boolean is_connected = false;

const uint8_t reportDescription[] = {
   HID_RAW_REPORT_DESCRIPTOR(MSGSIZE,MSGSIZE)
};

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) 
{
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
      strip.setPixelColor(i, c);
      strip.show();
      if(wait != 0)
        delay(wait);
  }
}

void showValue(uint32_t c, uint8_t value){
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  for(uint16_t i=0; i<value; i++) 
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void clearBuffer(){
  for (int i=0;i<MSGSIZE;i++) buf[i] = 0;
}

void setup() {

  for (int i=0;i<MSGSIZE;i++) buf[i] = i;
  
   //Serial.begin (115200);

   strip.begin();// Sets up the SPI
   strip.show();// Clears the strip, as by default the strip data is set to all LED's off.
   strip.setBrightness(8);

   
   colorWipe(strip.Color(0, 0, 255), 10); // Blue

   USBComposite.clear(); // clear any plugins previously registered
   USBComposite.setVendorId( 0xdead ); 
   USBComposite.setProductId( 0xbeef );
   USBComposite.setManufacturerString("datenpirat");
   USBComposite.setProductString("mixer");
   
   HID.begin(reportDescription, sizeof(reportDescription));

   colorWipe(strip.Color(255, 0, 0), 10); // Red  
   while (!USBComposite);

   colorWipe(strip.Color(0, 255, 0), 10); // Green 
   usb.begin();
    
  
   encoder1.UseInterrupts();
   encoder2.UseInterrupts();
    
   colorWipe(strip.Color(0, 0, 0), 10); // black
}

void sendEncoderReport(byte name, int8_t data){
    // Clear USB Buffer
    clearBuffer();
    // prepare report
    buf[0] = name;    // Rotary
    buf[1] = data < 0 ? 0x0 : 0x01;  // 0 = Left, 1 = Right
    buf[2] = data < 0 ? 0xFF - (data - 1) : data;    // Count
    // send report
    if(is_connected)
      usb.send(buf,MSGSIZE);
}
void sendButtonReport(byte name, uint32_t data){
    // data should be pressingTime
    // Clear USB Buffer
    clearBuffer();
    // prepare report
    buf[0] = name;
    buf[1] = data == 0 ? 0x0 : 0x01;  // 0 = KeyDown, 1 = KeyUp
    buf[2] = (data & 0xFF00) >> 8;    // Pressed Time
    buf[3] = data & 0xFF;             // "
    // send report
    if(is_connected)
      usb.send(buf,MSGSIZE);
}

void loop() {


  
  // USB Report Request
  if (usb.getOutput(buf)) {

    byte c = buf[0];
    byte l = buf[1];
    
    switch(c){
      case 0x01: // get protocol version;
        buf[1] = 0x01;  // Length
        buf[2] = 0x01;  // Version
        is_connected = true;
        break;
      
    }
    
    buf[8] = 0x13;
    buf[9] = 0x37;
    buf[10] = c;
    buf[11] = l;
    buf[11] = buf[2];
    // Answer Request
    usb.send(buf,MSGSIZE);
  }

  int8_t pos1 = encoder1.HasChanged();
  int8_t btn1 = encoder1.GetButtonPressed();
  int8_t pos2 = encoder2.HasChanged();
  int8_t btn2 = encoder2.GetButtonPressed();

  if(pos1){
    count += pos1;
    if(count > NUM_LEDS)
      count = 1;
    else if(count < 1)
      count = NUM_LEDS;
    showValue(strip.Color(255,0,0), count);
    sendEncoderReport(0xD1,pos1);
  }
  if(btn1){
    sendButtonReport(0xB1,encoder1.GetButtonPressedTime());
  }
  
  if(pos2){
    count += pos2;
    if(count > NUM_LEDS)
      count = 1;
    else if(count < 1)
      count = NUM_LEDS;
    showValue(strip.Color(0,255,0), count);
    sendEncoderReport(0xD2,pos2);
    
  }
  if(btn2){
    sendButtonReport(0xB2,encoder2.GetButtonPressedTime());
  }

  delay(5);
}
