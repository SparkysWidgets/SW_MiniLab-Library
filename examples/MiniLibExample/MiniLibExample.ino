/////////////////////////////////////////////////////////////////////////////
/*!

MCPMiniLib library Basic usage example file
MinipH, MinieC library driver example usage by Ryan Edwards, Sparkys Widgets

This library tries to make interfacing with Mini-line of hardware much more uniform and easier.
See below for how to use the basic methods more updates will add mroe examples!

I invest a lot of time and resources providing open source hardware, software, and tutorials
Please help support my efforts by purchasing products from www.sparkyswidgets.com, donating some time
on documentation or you can even donate some BitCoin to 1NwPNsf6t5vpph6AYY5bg361PSppPSSgDn

*/
/////////////////////////////////////////////////////////////////////////////


#include <Wire.h>
#include <SW_MiniLib.h>
#include <MCP3221.h> //we get a executable and linking error (.elf) if we dont inlcude here as well hmmm


uint8_t pHi2cAddress = 0x4D;		// MCP3221 A5 in Dec 77 A0 = 72 A7 = 79)
uint8_t eCi2cAddress = 0x48;
//byte altI2CAddress = 0x0C	// A0 = x48, A1 = x49, A2 = x4A, A3 = x4B, 
							// A4 = x4C, A5 = x4D, A6 = x4E, A7 = x4F
//This is for our ADC testing variables in mV ie 4948mV or 4.948v
int I2CadcVRef = 2998; //Measured millivolts of voltage input to ADC (can measure VCC to ground on MinipH for example)
int eCadcVRef = 4989;	//Not sure how much this matter but for accurate readings mine as well measure the VDD input to the ADC

//		I2C address of PHADC, Voltage Ref of PHADC, GAIN of PH Probe Gain Stage, RollingAverage?, 10reference or 4?
MINIPH miniph(pHi2cAddress, I2CadcVRef, DEFAULTOPAMPGAIN, true, false);

//	I2C address of ECADC, Voltage Ref of ECADC, value of RGain, AC voltage of oscillator output, KCell of probe, RollingAverage?
MINIEC miniec(eCi2cAddress, eCadcVRef, FIVEKHALFPOT, LOWOSCVOUT, DEFAULTKCELL, true);

void setup()
{
	//The library assume you will initialize the wire library mine as well do this here
	Wire.begin();
	//initialize and run anything that needs to be performed proir to the main loop executing for the first time
	Serial.begin(9600);
	Serial.println("MINIPHEC Library Usage Example");

}

void loop()
{
	int pHtemp = miniph.calcRollingAVG();
	int eCtemp = miniec.calcRollingAVG();

	Serial.print("CalcpH results : ");
	Serial.print(miniph.calcpH(pHtemp));
	Serial.print(" CalcMillis results : ");
	Serial.print(miniph.calcMillivolts(pHtemp));
	Serial.print(" RollingAvg results : ");
	Serial.println(pHtemp);
	
	Serial.print("CalceC results : ");
	Serial.print(miniec.calceC(eCtemp));
	Serial.print(" CalcMillis results : ");
	Serial.print(miniec.calcMillivolts(eCtemp));
	Serial.print(" RollingAvg results : ");
	Serial.println(eCtemp);

	delay(1000);
}



