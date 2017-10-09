# CocktailMix
An Arduino-based *Thermomix* for cocktails.

Get instructions on how to make a cocktail on a touch display and measure the correct quantity for ingredients using a digital scale.

## Requirements
##### Required hardware
- Arduino board (e.g. Arduino Nano)
- [Nextion NX3224T024_011R](https://nextion.itead.cc) touch display
- HX711 weighing sensor

##### Required software
- [Arduino IDE](https://www.arduino.cc/en/Main/Software) for programming your board
- [HX711 Arduino library](https://github.com/bogde/HX711)
- [Nextion Arduino library](https://github.com/bborncr/nextion)
- [Nextion Editor](https://nextion.itead.cc/resources/download/nextion-editor/) for editing the screen contents

## Schematic

Wire up your Arduino as shown.

<img src="https://user-images.githubusercontent.com/3229505/31336771-a75f9b7e-acf8-11e7-93cc-3f0503d85961.png" width="400">

The HX711 weighing sensor must be connected to i2c pins, A4 and A5 on the Arduino Nano.

## Getting Started

Flash the *screens.HMI* (located in the Nextion folder) to the display using the Nextion Editor.

Fire up the Arduino IDE and add both Arduino libraries to the IDE by adding them via `Sketch > Include Library > Add .ZIP Library...` and selecting the downloaded ZIP folders.

Now load the *CocktailMix.ino*, check the configuration for the correct pinout and then upload the sketch to your Arduino board.

Once it's up and running, click the cog icon on the start screen of the CocktailMix to tare your scale. You will require an object weighing exactly 1kg. (A package of sugar works nicely, for example.) You should only need to do this once. The result is then stored in EEPROM.

## Known Issues

The `screenMix()` function has not been fully tested yet and it may not work.

The **Arduino Nano** does not have enough memory to cope with the recipes and the required Arrays, so a rewrite to manual steps with if-Statements is necessary to run the CocktailMix on it.

If you continually send commands to the Nextion display in a loop (e.g. `myNextion.setComponentText("instruction", "test value");` on every loop cycle) it will start to return garbled serial messages and thus become unusable (it's then impossible to detect touch events, for example). If your serial window shows output like `DISPLAY: ¿¿¿¿` you will need to check your functions for looped requests to the display. 

## Roadmap

What'd be nice to implement in the future:

- Load recipes from storage, e.g. from an SD card.
- Ability to 'scroll' through recipes on the start screen.
- Download cocktail recipes from an online service with an API (over WiFi) and store them on the SD card.
