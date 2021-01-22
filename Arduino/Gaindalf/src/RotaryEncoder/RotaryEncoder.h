/*
  RotaryEncoder.h - Library for interfacing rotary encoders with STM32duino
  Created by datenpirat, January 8, 2019.
  Released into the public domain.
*/
#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"

class RotaryEncoder
{
  public:
    RotaryEncoder(int PIN1, int PIN2, int PINBTN);
    void UseInterrupts();
    int8_t Read();

    int8_t GetEncoderValue();
    int8_t HasChanged();

    uint8_t GetButtonPressed();
    uint8_t GetButtonIsPressing();
    uint32_t GetButtonPressedTime();

  private:
    typedef struct {
      int8_t pin;
      volatile int8_t *val;
      volatile uint32_t *tick;
    } IRQHandlerParameters;

    typedef struct {
      int8_t pin;
      volatile uint32_t *lastRising = 0, *riseTime = 0, *fallTime = 0, *dblClickTime = 0;
      uint8_t *buttonPressed = 0, *buttonIsPressing = 0;
    } BtnIRQHandlerParameters;


    int8_t _pin1;
    int8_t _pin2;
    int8_t _pinBtn;

    uint8_t useInterrupts = 0;
    uint8_t prevNextCode = 0;
    uint16_t store=0;
    static void IRQPIN1(void* p);

    static void IRQPIN2(void* p);
    static void IRQBTN(void* p);
    volatile int8_t encoderPos = 0;
    int8_t lastReportedPos = 1;
    uint8_t rotating;
    uint32_t lastTick;

    IRQHandlerParameters ihp1;
    IRQHandlerParameters ihp2;
    BtnIRQHandlerParameters btnhp;

    volatile uint32_t lastRising = 0, riseTime = 0, fallTime = 0, dblClickTime = 0;
    uint8_t buttonPressed = 0, buttonIsPressing = 0;

};

#endif