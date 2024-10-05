/* LED_Functions.ino
  Author : Carson Brown
  This file contains the functions related to the LED strip.

  Each function clears all of the leds, creates the color for the state, then applies it to the pair of leds required by the instructions.
  ledStrip.clear() sets all leds to RGB : #000000
  ledStrip.Color() creats a bit packed number which the Adafruit library can read as a color from values of 0-255 of red, green, and blue.
  ledStrip.fill() takes a color, the starting led, and the number of leds to fill, and paints those leds to that color
  ledStrip.show() moves the led instructions from the ESP-32 to the led strip.

  There are 8 leds in the strip, and they are 0 indexed.
*/


void ledPatternForwards() { //The color green to the first two LEDs
  ledStrip.clear();
  ledStrip.fill(ledStrip.Color(0, 68, 0), 0, 2);
  ledStrip.show();
}

void ledPatternStop() { //The color red to the last two leds
  ledStrip.clear();
  ledStrip.fill(ledStrip.Color(68, 0, 0), 6, 2);
  ledStrip.show();
}

void ledPatternBackwards() { //The color yellow to the center two leds
  ledStrip.clear();
  ledStrip.fill(ledStrip.Color(34, 34, 0), 3, 2);
  ledStrip.show();
}