/**********************************
  
  CocktailMix v1

***********************************

  Leonardo Re
  
  mail@leonardo.re
  https://leonardo.re

***********************************/

#include <SoftwareSerial.h>
#include <EEPROM.h>

// https://github.com/bborncr/nextion
#include <Nextion.h>

// https://github.com/bogde/HX711
#include <HX711.h>


// --------------------------------- //
// Load variables                    //
// --------------------------------- //

// Nextion TX to pin 2 and RX to pin 3 of Arduino
SoftwareSerial nextion(2, 3);
Nextion myNextion(nextion, 9600);
HX711 scale;

String curPage = "start";
String curCocktail = "";
byte i = 0;
byte previous_percentage = 0;
byte percentage = 0;
byte instruction = 0;
int scaleinit = 0;
struct st_t { long lo; byte by; double db; };
float scale_set;
byte buzzer = 9;

byte steps = 0;
byte cocktailloop = 0;

byte stepType[]={0};
byte stepTypeMeasurement[]={0};
String stepInstruction[]={""};

/*
 * 
 * NANO SETTINGS:
 * nextion(2, 3);
 * buzzer = 9;
 * 
 * NANO PINS:
 * D3 - Gelb Nextion
 * D2 - Blau Nextion
 * GND - Schwarz Nextion
 * 
 * D9 - Rot Pieper
 * 
 * A4 - Weiß Waage
 * A5 - Grau Waage
 * +5V - Lila Waage
 */


// --------------------------------- //
// Setup                             //
// --------------------------------- //

void setup() {
  Serial.begin(9600);
  Serial.println("**********************");
  Serial.println("*   CocktailMix v1   *");
  Serial.println("**********************");
  Serial.println("");
  Serial.print("Initialising ...");

  pinMode(buzzer, OUTPUT);

  myNextion.init();
  myNextion.sendCommand("page start");
  myNextion.setComponentText("instruction", "Select a drink.");

  // HX711.DOUT   - pin #A4
  // HX711.PD_SCK - pin #A5
  scale.begin(A4, A5);
  
  // check if scale calibration on device, otherwise load default
  if (EEPROM_readAnything(0, scale_set)) { 
    scale.set_scale(scale_set);
  } else {
    // default: 2112.66
    scale.set_scale(2112.66);
  }
  scale.tare();
  scaleinit = scale.read_average(5);
  
  Serial.println(" done!");
  Serial.println("");
}



// --------------------------------- //
// Main loop                         //
// --------------------------------- //

void loop() {

  if (curPage == "start") {
    screenHome();
  } else if (curPage == "calibrate") {
    screenCalibrate();
  } else if (curPage == "tare") {
    screenTare();
  } else if (curPage == "mix") {
    screenMix();
  } else if (curPage == "done") {
    screenDone();
  }

}


// --------------------------------- //
// Display / screen functions        //
// --------------------------------- //


void screenDone() {

  if (i == 0) {
    myNextion.setComponentText("instruction", "Enjoy your "+curCocktail+"!");
    i = 1;
  }

  delay(200);
  i++;

  if (i >= 25) {
    curCocktail = "";
    myNextion.sendCommand("page start");
    curPage = "start";
    i = 0;
  }
  
  String input = listenDisplay();
  if (input.startsWith("65 4 ")) {
    curCocktail = "";
    myNextion.sendCommand("page start");
    curPage = "start";
    i = 0;
  }
}




void screenMix() {
  // if we land here by error, reset
  if(curCocktail == "") { curPage = "start"; }

  String input = listenDisplay();
  if (input == "65 3 5 0 ffff ffff ffff") {
    // abort & return home
    Serial.println("Cancelling - back to start.");
    curCocktail = "";
    myNextion.sendCommand("page start");
    curPage = "start";
    i = 0;
  }


  // Glas: 250ml (~250g)
  if (steps == 0) {
    if (curCocktail == "Mojito") {
      // load recipe
      
      // Step types:
      // 0 = instruction only (with OK button)
      // 1 = measurement
  
      // Step type measurements in ml (= gram)
      
      steps = 6;
      byte stepType[]={0,0,0,0,1,1};
      byte stepTypeMeasurement[]={0,0,0,0,50,60};
      String stepInstruction[]={
        "Divide a lime into eights and add it."
        "Add 2 teaspoons of brown sugar.",
        "Add 8 leaves of mint. Pestle the contents.",
        "Fill glass with crushed ice.",
        "Add 5cl white rum.",
        "Add 6cl soda."
      };
    }
  
    if (curCocktail == "Long Island Ice Tea") {
      // load recipe
      
      steps = 9;
      byte stepType[]={0,1,1,1,1,1,1,0,0};
      byte stepTypeMeasurement[]={0,20,20,10,20,10,20,0,0};
      String stepInstruction[]={
        "Add ice cubes."
        "Add 2cl brown rum.",
        "Add 2cl gin.",
        "Add 1cl lime juice.",
        "Add 2cl vodka.",
        "Add 1cl orange liqueur.",
        "Add 2cl orange juice.",
        "Add a pinch of sugar and shake well.",
        "Top up with coke."
      };
    }

  }
  
  makeCurrentCocktail();
  
}


void screenTare() {
  // if we land here by error, reset
  if(curCocktail == "") { curPage = "start"; }

  if (i == 0) {
    myNextion.setComponentText("instruction", "Place your glass on the scale.");
    myNextion.sendCommand("vis okay,1");
    i = 1;
  }

  /* EXPERIMENT: DETECT GLASS AUTOMAGICALLY 
  int scalecurrent = scale.read_average(5);
  Serial.print("Init: ");
  Serial.print(scaleinit);
  Serial.print(" - Current: ");
  Serial.println(scalecurrent);
  if (scalecurrent > (scaleinit+200) || scalecurrent < (scaleinit+200)) {
    delay(5000);
    // Automatic glass detection
    scale.tare();
    myNextion.sendCommand("page mix");
    i = 0;
    curPage = "mix";
  }
  */

  String input = listenDisplay();
  if (input == "65 2 5 0 ffff ffff ffff") {
    // abort & return home
    Serial.println("Cancelling - back to start.");
    curCocktail = "";
    myNextion.sendCommand("page start");
    curPage = "start";
    
  } else if (input == "65 2 4 0 ffff ffff ffff") {

    Serial.println("OK pressed. Setting scale now.");
    scale.tare();
    myNextion.sendCommand("page mix");
    i = 0;
    curPage = "mix";
    
  }

}



void screenCalibrate() {

  if (i == 0) {
    myNextion.sendCommand("vis okay,0");
    myNextion.setComponentText("instruction", "Please wait, measuring");
    
    Serial.println("READING SCALE");
    Serial.print("read: \t\t");
    Serial.println(scale.read());      // print a raw reading from the ADC
  
    Serial.print("read average: \t\t");
    Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC
    
    Serial.print("get value: \t\t");
    Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)
  
    scale.set_scale();
    delay(1000);
    scale.tare();
  
    myNextion.setComponentText("instruction", "Place 1kg on the scale and press ok.");
    myNextion.sendCommand("vis okay,1");
    i = 1;
  }

  String input = listenDisplay();
  if (input == "65 2 5 0 ffff ffff ffff") {
    // abort & return home
    Serial.println("Cancelling - back to start.");
    curCocktail = "";
    myNextion.sendCommand("page start");
    curPage = "start";
    i = 0;
    
  } else if (input == "65 2 4 0 ffff ffff ffff") {

    Serial.println("OK pressed. Calibrating ...");

    float reading = scale.get_units(10);
   
    Serial.print("get units: \t\t");
    Serial.println(reading);

    float result = reading/1000;
    Serial.print("DIVIDING ");
    Serial.print(reading);
    Serial.print(" / 1000 = ");
    Serial.println(result);

    // save calibration
    EEPROM_writeAnything(0, result);
    scale.set_scale(result);
    scale.tare();

    Serial.println("..................................");
    Serial.println("After setting up the scale:");

    Serial.print("read: \t\t");
    Serial.println(scale.read());                 // print a raw reading from the ADC
  
    Serial.print("read average: \t\t");
    Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC
  
    Serial.print("get value: \t\t");
    Serial.println(scale.get_value(5));    // print the average of 5 readings from the ADC minus the tare weight, set with tare()
  
    Serial.print("get units: \t\t");
    Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
              // by the SCALE parameter set with set_scale
              
    Serial.println("..................................");
    
  }

}


void screenHome() {
  
  // Mojito:      65 1 3 0 ffff ffff ffff
  // Long Island Ice Tea:  65 1 4 0 ffff ffff ffff

  // @TODO: BUTTONS DEFINIEREN! z.B. Long Island Ice Tea

  String input = listenDisplay();
  if (input == "65 1 3 0 ffff ffff ffff") {

    Serial.println("--> Mojito it is!");
    curCocktail = "Mojito";
    myNextion.sendCommand("page tare");
    curPage = "tare";
    
  } else if (input == "65 1 4 0 ffff ffff ffff") {
    
    Serial.println("--> Long Island Ice Tea it is!");
    curCocktail = "Long Island Ice Tea";
    myNextion.sendCommand("page tare");
    curPage = "tare";
    
  } else if (input == "65 1 6 0 ffff ffff ffff") {
    
    Serial.println("SETTINGS OPENED");
    myNextion.sendCommand("page tare");
    curPage = "calibrate";
    
  }
  
}


// --------------------------------- //
// Cocktail-instruction loop         //
// --------------------------------- //

void makeCurrentCocktail() {

/*
  cocktailloop;


      steps = 6;
      byte stepType[6]={0,0,0,0,1,1};
      byte stepTypeMeasurement[6]={0,0,0,0,50,60};
      String stepInstruction[6]={
        "Divide a lime into eights and add it."
        "Add 2 teaspoons of brown sugar.",
        "Add 8 leaves of mint. Pestle the contents.",
        "Fill glass with crushed ice.",
        "Add 5cl white rum.",
        "Add 6cl soda."
      };

*/

  if (cocktailloop > steps+1) {
    
    // all steps executed, ending instructions
    
    Serial.println("--- END INSTRUCTIONS ---");
    myNextion.sendCommand("page done");
    instruction = 0;
    percentage = 0;
    previous_percentage = 0;
    curPage = "done";
    cocktailloop = 0;
    i = 0;
    steps = 0;
    byte stepType[]={0};
    byte stepTypeMeasurement[]={0};
    String stepInstruction[]={""};
    sendTone("done");
    
  } else {
    
    // we're in the making of something great ;)

    // check if we need to refresh the screen
    if (instruction <= cocktailloop) {
      Serial.println("--- NEW INSTRUCTIONS ---");
      Serial.print("Current loop: ");
      Serial.println(cocktailloop);
      Serial.print("Total steps: ");
      Serial.println(steps);
      
      if (stepType[cocktailloop] == 1) {    
        Serial.println("Type: measurement");
        myNextion.sendCommand("page mix");
        myNextion.setComponentText("instruction", stepInstruction[cocktailloop]);
        myNextion.setComponentValue("status", 0);
      } else {
        Serial.println("Type: instruction only");
        myNextion.sendCommand("page tare");
        myNextion.setComponentText("instruction", stepInstruction[cocktailloop]);     
      }
      instruction++;
    }

    // we're waiting for the correct weight
    if (stepType[cocktailloop] == 1) {

      float weight = scale.get_units(1);
      previous_percentage = percentage;
      percentage = weight/stepTypeMeasurement[cocktailloop]*100;
      if (percentage < 0) percentage = 0;
      if (percentage > 100) percentage = 100;
      if (previous_percentage != percentage) {
        myNextion.setComponentValue("status", percentage);
      }
  
      Serial.print("Weight: ");
      Serial.println(weight);
  
      if (weight >= stepTypeMeasurement[cocktailloop]) {
        sendTone("buzz");
        scale.tare();
        weight = 0;
        cocktailloop++;
      }
      
    } else {
    // we're waiting for a button press

      String input = listenDisplay();
      if (input == "65 2 4 0 ffff ffff ffff") {
        sendTone("buzz");
        cocktailloop++;
      }
      
    }

    
  }
  
  
}



// --------------------------------- //
// General functions                 //
// --------------------------------- //

String listenDisplay() {
  String message = myNextion.listen();
  if(message != ""){
    Serial.println("DISPLAY: "+message);
    return message;
  }
}


void sendTone(String melody) {
  if (melody == "buzz") {
    tone(buzzer, 1000);
    delay(150);
    noTone(buzzer);
    delay(150);
    tone(buzzer, 1000);
    delay(150);
    noTone(buzzer);
  } else if (melody == "done") {
    tone(buzzer, 1000);
    delay(250);
    tone(buzzer, 1500);
    delay(250);
    tone(buzzer, 2000);
    delay(250);
    noTone(buzzer);
  }
}


// --------------------------------- //
// EEPROM                            //
// --------------------------------- //

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
   const byte* p = (const byte*)(const void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
   return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
   byte* p = (byte*)(void*)&value;
   int i;
   for (i = 0; i < sizeof(value); i++)
       *p++ = EEPROM.read(ee++);
   return i;
}

