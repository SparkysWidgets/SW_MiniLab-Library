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

#define MAX_LENGTH 18
#define VERSION 0x0001

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

static String messageIn = "";

void setup()
{
	//The library assume you will initialize the wire library mine as well do this here
	Wire.begin();
	//initialize and run anything that needs to be performed proir to the main loop executing for the first time
	Serial.begin(115200);
	Serial.println("MINIPHEC Library Usage Example");

}

void loop()
{
	int pHtemp = miniph.calcRollingAVG();
	int eCtemp = miniec.calcRollingAVG();

	//Will process any Seiral data coming in for commands this is non blocking and should accumalte messages over multiple loops should they
	//ready in one iteration
	if (Serial.available() > 0)
	{
		processIncomingByte(Serial.read());
	}

	//simple output change it how you want
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

	//Blocking delay please see fade without delay if you want a non-blocking routine here
	delay(1000);
}

void processIncomingByte(const char inByte)
{

	static unsigned int numChars = 0;
	switch (inByte)
	{

	case '\n':   // end of text      
		// terminator reached! process input_line here ...
		numChars = 0;
		processMessage(messageIn);
		break;

	case '\r':   // discard carriage return
		break;

	default:
		// keep adding if not full ... allow for terminating null byte
		if (numChars < (MAX_LENGTH - 1))
			messageIn += inByte;
		numChars++;
		break;

	}  // end of switch

} // end of processIncomingByte

void processMessage(String msg)
{
	Serial.println(msg);
	if (msg.startsWith("C"))
	{
		//we recieved a calibration string lets rip it apart to see what to do
		char whichType, whatCal;
		whichType = msg.charAt(1);
		whatCal = msg.charAt(2);
		String tempStringtoInt = "";

		int nDex = 3;
		//Serial.println(msg);
		while (msg[nDex] != ',' && nDex < MAX_LENGTH)
		{
			//Serial.println(msg[nDex]);
			tempStringtoInt += msg[nDex];
			nDex++;
		}
		int calMapReference = tempStringtoInt.toInt();

		switch (whichType)
		{
			case 'E':				
				//now lets check whichcal we want to run in the eC group
				//We want to calibrate our low
				if (whatCal == 'L')
				{
					miniec.calibrateeCLow(miniec.calcRollingAVG(),(long)tempStringtoInt.toInt());
				}
				else if (whatCal == 'H') //or our high
				{
					miniec.calibrateeCHigh(miniec.calcRollingAVG(), (long)tempStringtoInt.toInt());
				}
			break;

			case 'P':
				if (whatCal == '7')
				{
					miniph.calibratepH7(miniph.calcRollingAVG());
				}
				else if (whatCal == '4')
				{
					miniph.calibratepH4(miniph.calcRollingAVG());
				}
				else if (whatCal == '10')
				{
					miniph.calibratepH10(miniph.calcRollingAVG());
				}
			break;

			default:
				break;

		}

	}
	if (msg.startsWith("R"))
	{
		Serial.print("pH: ");
		Serial.print(miniph.calcpH(miniph.calcRollingAVG()));
		Serial.print(" | ");
		Serial.print("eC: ");
		Serial.println(miniec.mapeC(miniec.calcRollingAVG()));
	}
	if (msg.startsWith("S"))
	{
		//Stop
		//do whatever you want here to stop or what and S command might be
	}

	if (msg.startsWith("I"))
	{
		//Lets read in our parameters and spit out the info! 
		//eeprom_read_block(&params, (void *)0, sizeof(params));
 		sendSerialInfo('I');
	}
	if (msg.startsWith("X"))
	{
		//restore to default settings
		Serial.println("Reseting to default settings");
		miniec.reset_eCParams();
		miniph.reset_pHParams();
	}
	//clear out our message buffer
	messageIn = "";
}

void sendSerialInfo(char charStatusInfo)
{
	
	Serial.print("MiniLib pH and eC Example: Firmware Ver ");
	Serial.println(VERSION);
	Serial.print("pH 7 cal: ");
	Serial.print(miniph._pHParams.pH7Cal);
	Serial.print(" | ");
	Serial.print("pH 4 cal: ");
	Serial.print(miniph._pHParams.pH4Cal);
	Serial.print(" | ");
	Serial.print("pH 10 cal: ");
	Serial.print(miniph._pHParams.pH10Cal);
	Serial.print(" | ");
	Serial.print("pH probe slope: ");
	Serial.println(miniph._pHParams.pHStep);

	Serial.print("eC Low cal: ");
	Serial.print(miniec._eCParams.eCLow);
	Serial.print(" | ");
	Serial.print("eC Low Ref: ");
	Serial.print(miniec._eCParams.eCRefLow);
	Serial.print(" | ");
	Serial.print("eC High cal: ");
	Serial.print(miniec._eCParams.eCHigh);
	Serial.print(" | ");
	Serial.print("eC High Ref: ");
	Serial.println(miniec._eCParams.eCRefHigh);

}
