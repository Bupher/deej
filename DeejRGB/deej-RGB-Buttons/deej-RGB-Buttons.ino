#include <CorsairLightingProtocol.h>
#include <FastLED.h>
#include <HID-Project.h>
#include <HID-Settings.h>

#define DATA_PIN_CHANNEL_1 15 //Signal pin for strip 1
//#define DATA_PIN_CHANNEL_2 3 //Signal pin for strip 2

int LPC1 = 10; //Defines LEDS per channel
int LPC2 = 10; //Defines LEDS per channel

CRGB ledsChannel1[10]; //Defines max number of LEDS on channel 1
//CRGB ledsChannel2[LPC2]; //Defines max number of LEDS on channel 2

CorsairLightingFirmware firmware = corsairLightingNodePROFirmware();
FastLEDController ledController(true);
CorsairLightingProtocolController cLP(&ledController, &firmware);
CorsairLightingProtocolHID cHID(&cLP);

const int NUM_SLIDERS = 5; //Number of Analog inputs
const int analogInputs[NUM_SLIDERS] = {A3, A2, A1, A0, A10}; //Analog Input pin designation
int analogSliderValues[NUM_SLIDERS];

const int NUM_BUTTONS = 5;
const int digitalInputs[NUM_BUTTONS] = {14,3,4,5,6}; //Digital Input Designation
int buttonState[NUM_BUTTONS];
int oldButtonState[NUM_BUTTONS]={0,0,0,0,0};

void setup() { 
  FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_1, GRB>(ledsChannel1, LPC1); //Adds leds to controll
  //FastLED.addLeds<WS2812B, DATA_PIN_CHANNEL_2, GRB>(ledsChannel2, LPC2);
  ledController.addLEDs(0, ledsChannel1, LPC1);
  //ledController.addLEDs(1, ledsChannel2, LPC2);
  
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT); //Sets input mode for all analog inputs
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(digitalInputs[i], INPUT_PULLUP);//Set input mode for all digital inputs
  }  
  
  Keyboard.begin();
  Consumer.begin();
  Serial.begin(9600);
}

void loop() {
  updateSliderValues();
  sendSliderValues(); // Actually send data (all the time)
  updateCLP(); //Updates Lighting
  buttonActions();
  // printSliderValues(); // For debug
  noDelay(10); //Delays would cause hitching in lighing
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  if(Serial.availableForWrite()>32){ //makes sure serial bus is conencted, lighting will messup if this is ommited, and serial is missing
    Serial.println(builtString); //writes string to serial bus
  }
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}

void noDelay(int wait) { //Instead of delaying, we just loop upateCLP, until atleast the desiered time has past. Not super precise, but good enough.
  long startTime = millis(); //Gets loop start time
  while(millis()-startTime<wait){ //While not enough time has passed
    updateCLP(); //Update lighting
  }
}

void updateCLP(){ //Updates Ligthing
    cHID.update();
    if (ledController.updateLEDs()) {
      FastLED.show();
    }
}

void buttonActions(){ //updates button states and runs functions
  for(int i = 0; i<NUM_BUTTONS; i++){ 
    oldButtonState[i]=buttonState[i]; //updates last button state  
    buttonState[i]=digitalRead(digitalInputs[i]); //gets current button state
  }
  
  if(buttonState[0]==0 && oldButtonState[0] == 1){
    //loopTimer=variableToggle(loopTimer); //debug
    Keyboard.press(KEY_F17);
  }
  else if(buttonState[0]==1 && oldButtonState[0]==0){
    Keyboard.release(KEY_F17);
  }
  
  if(buttonState[1]==0 && oldButtonState[1] == 1){    
    Keyboard.press(KEY_F16);
  }
    else if(buttonState[1]==1 && oldButtonState[1]==0){
    Keyboard.release(KEY_F16);
  }
  
  if(buttonState[2]==0 && oldButtonState[2] == 1){
    Keyboard.press(KEY_F15);
  }
    else if(buttonState[2]==1 && oldButtonState[2]==0){
    Keyboard.release(KEY_F15);
  }
  
  if(buttonState[3]==0 && oldButtonState[3] == 1){
    Keyboard.press(KEY_F14);
  }
  else if(buttonState[3]==1 && oldButtonState[3]==0){
    Keyboard.release(KEY_F14);
  }
  
  if(buttonState[4]==0 && oldButtonState[4] == 1){
    Keyboard.press(KEY_F13);
  }
  else if(buttonState[4]==1 && oldButtonState[4]==0){
    Keyboard.release(KEY_F13);
  }
}
