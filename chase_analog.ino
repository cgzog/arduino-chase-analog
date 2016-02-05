// chase_analog
//
// chase marquee style sequence using PWM - this provides the ability to transition LEDs on and off
// in a manner that simulates incandescent bulbs turning and off over a period of time and not instantaneously
// like LEDs
//
//
// Built using the 1.6.7 Arduino IDE
//
//
// Copyright (c) 2016, Christian Herzog
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other materials
//    provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used
//    to endorse or promote products derived from this software without specific prior written
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#define LED_OVERLAP     2   // max number active at once - if you choose a number which is close
                            // to the number of LEDs (like 1 or 2 less), you'll get a chase effect
                            // with a travelling off segment rather than on for more of a classic
                            // theatre marquee look
                            //
                            // must be greater than 1!

// the constants below are very high and result in a very slow chase exagerating what is happening
//
// a typical value for LED_ON_TIME might be 100, LED_FADE_TIME of 30, and LED_FADE_STEPS of 4 but
// choose what looks good in your application

#define LED_ON_TIME     500  // amount of time an LED is fully on in milliseconds
#define LED_FADE_TIME   300  // amount of time an LED transitions from full on or off to the other state - milliseconds
#define LED_FADE_STEPS  20   // number of PWM steps between full on or off - independent of the LED_FADE_TIME; fade time
                             // is split into LED_FADE_TIME / LED_FADE_STEPS transitions


#define LED_ON          255 // full PWM on value
#define LED_OFF         0



typedef struct  {
  int port;
} LEDS;

LEDS  leds[] = {
  3,                        // I/O port numbers - needs to be a PWM capable port
  5,                        // different Arduino CPUs have more or fewer PWM
  6,                        // ports so make sure only PWM supported ports are specified
  9,
  10,
  11
};


void setup() {

unsigned int i;

  for (i = 0; i < sizeof(leds) / sizeof(leds[0]) ; i++) {
    pinMode(leds[i].port, OUTPUT);
    analogWrite(leds[i].port, 0);     // make sure each LED starts in off state
  }
}



void loop() {

int activeLeds[LED_OVERLAP];          // stores the indexes into the leds table which are turning on, on, or turning off
int i, steps;

  for (i = 0 ; i < LED_OVERLAP ; i++) {
      activeLeds[i] = i;              // start with the first LED and set the index of the LEDs that would be on
                                      // based on the LED_OVERLAP value (we start with that many LEDs on)
  }

  while (1) {
    
    for (steps = 0 ; steps < LED_FADE_STEPS; steps++) {         // fade first down and last up
      
      analogWrite(leds[activeLeds[0]].port, LED_ON - (LED_ON / LED_FADE_STEPS * i));
      
      analogWrite(leds[activeLeds[LED_OVERLAP-1]].port, LED_ON / LED_FADE_STEPS * steps);

      for (i = 1 ; i < LED_OVERLAP - 1 ; i++) {
        analogWrite(leds[activeLeds[i]].port, LED_ON);          // turn the others on
      }
      
      delay(LED_FADE_TIME / LED_FADE_STEPS);                    // wait for the appropriate amount of time based on the total fade time and number of steps
    }

    analogWrite(leds[activeLeds[0]].port, LED_OFF);             // make sure first is all the way off
    analogWrite(leds[activeLeds[LED_OVERLAP-1]].port, LED_ON);  // make sure last is all the way on

    delay(LED_ON_TIME);                                         // just leave things alone for the ON time

    incLeds(activeLeds, LED_OVERLAP, sizeof(leds) / sizeof(leds[0]));  // "chase" to the next sequence of LEDS
  }
}



void incLeds(int *ledList, int overlap, int total)

{
int i;

  for (i = 0 ; i < overlap ; i++) {           // run through the number of LEDs that can be active at once
    
    if (ledList[i] + 1 < total) {             // would we fall off the end of the list?
      ledList[i]++;                           // if not, increment
    } else {
      ledList[i] = 0;                         // if so, wrap back to 0
    }
  }
}
