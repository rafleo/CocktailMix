# CocktailMix
An Arduino-based *Thermomix* for cocktails.

Get instructions on how to make a cocktail on a touch display and measure the correct quantity for ingredients using a digital scale.

## Requirements
##### Required hardware
- Arduino board (e.g. Arduino Nano)
- [Nextion NX3224T024_011R](https://nextion.itead.cc) touch display
- HX711 digital scale

##### Required software
- [Arduino IDE](https://www.arduino.cc/en/Main/Software) for programming your board
- [HX711 Arduino library](https://github.com/bogde/HX711)
- [Nextion Arduino library](https://github.com/bborncr/nextion)
- [Nextion Editor](https://nextion.itead.cc/resources/download/nextion-editor/) for editing the screen contents

## Getting Started
Flash the *screens.HMI* (located in the Nextion folder) to the display using the Nextion Editor.

Fire up the Arduino IDE and add both Arduino libraries to the IDE by adding them via `Sketch > Include Library > Add .ZIP Library...` and selecting the downloaded ZIP folders.

Now load the *CocktailMix.ino*, check the configuration for the correct pinout and then upload the sketch to your Arduino board.

