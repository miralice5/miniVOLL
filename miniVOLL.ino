/* miniVOLL is firmware software for Arduino miniVOLL device
 * Author: Chris Czoba  krzysiek@biotronika.pl
 *
 * For more see: biotronics.eu
 *
 * Calibrate by pressing button in the pen electrode or short the electrodes when power device on
 *
 */

#include <Arduino.h>      // For eclipse IDE only
#include <EEPROM.h>


#define SOFT_VER "2019-02-16"
#define HRDW_VER "NANO 1.0"

#define analogInPin	A3	// Analog input form optocoupler
#define buzzerPin	4	// For signal buzzer


#define oneBitEqivalentVoltage 3.2258 // 1023bits = 3.3V => 1bit = 3.2258mV
#define maximumInputThresholdVoltage 3000 // Is used for calibration purpose. e.g.: 3000mV => 3.3V - 0.30V

#define minimumSignalThresholdVolatge 1171 // Threshold & 0%

#define eeAddress 0

int inputVoltage = 0;
boolean started = false;
int outputValue = 0;

int maximumInputVoltage = maximumInputThresholdVoltage;

void startGraph();
void beep(int millis);
void calibrate();

void setup() {
	pinMode(buzzerPin, OUTPUT);
	analogReference(INTERNAL); //Change to 3.3V External Arduino NANO reference

	Serial.begin(115000);

	beep(100);



	if (analogRead(analogInPin)*oneBitEqivalentVoltage > maximumInputVoltage) {
		//New calibration
		calibrate();
	} else {

		//Set 100% voltage value
		EEPROM.get(eeAddress, maximumInputVoltage);
	}

	Serial.println(100);
	Serial.println(0);
}

void loop() {

  inputVoltage = analogRead(analogInPin)*oneBitEqivalentVoltage;

  if (inputVoltage>minimumSignalThresholdVolatge && !started){
    started = true;
    startGraph();
  }

  if (inputVoltage<=minimumSignalThresholdVolatge){
    started=false;
    delay(100);
  }
  if (started) {
    outputValue = map(inputVoltage, minimumSignalThresholdVolatge , maximumInputVoltage, 0 , 100);
    Serial.println(outputValue);
  }

  delay(10);
}

void startGraph(){

    Serial.println(100);
    for (int i=0; i<100; i++) Serial.println(0);

}

void beep(int millis){

	digitalWrite(buzzerPin,HIGH);
	delay(millis);
	digitalWrite(buzzerPin,LOW);

}

void calibrate(){
// Calibrate device to 100% with shorted electrodes

	inputVoltage = analogRead(analogInPin)*oneBitEqivalentVoltage;

	for (int i=0;i<100;i++){
		inputVoltage = (3*inputVoltage + analogRead(analogInPin)*oneBitEqivalentVoltage)/4;
	}

	EEPROM.put(eeAddress, inputVoltage);
	maximumInputVoltage = inputVoltage;
	beep(400);

	inputVoltage =0;

}
