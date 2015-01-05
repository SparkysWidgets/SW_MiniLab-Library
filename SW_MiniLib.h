/////////////////////////////////////////////////////////////////////////////
/*!

  SW_MiniLib.h
  MinipH and MinieC Library by Ryan Edwards, Sparkys Widgets
  
  Use this library with Sparkys Widgets MinipH, MinieC, and will expand to cover other boards using similar hardware

  I invest a lot of time and resources providing open source hardware, software, and tutorials 
  Please help support my efforts by purchasing products from www.sparkyswidgets.com, donating some time 
  on documentation or you can even donate some BitCoin to 1NwPNsf6t5vpph6AYY5bg361PSppPSSgDn

*/
/////////////////////////////////////////////////////////////////////////////

#ifndef _SW_MINILIB_H
#define _SW_MINILIB_H


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
	
//Looks for MCP3221 library in Libraries make sure its there
#include <../MCP3221/MCP3221.h>
#include <avr/eeprom.h>

//Our EEPROM magic number for error/reset check
#define PHWRITE_CHECK 0x1234
#define ECWRITE_CHECK 0x1234
#define DEFAULTOPAMPGAIN 5.25f
#define DEFAULTRGAIN 1000.0f
#define FIVEKHALFPOT 2470.0f
#define DEFAULTOSCVOUT 236.6f
#define LOWOSCVOUT 121.0f
#define DEFAULTKCELL 1.0f

//EEPROM Parameters Structures, maybe these can be condensed down to a single struct?
struct pHParameters_T
{
	uint8_t WriteCheck;
	int pH7Cal, pH4Cal, pH10Cal;
	float pHStep;
};

struct eCParameters_T
{
	uint8_t WriteCheck;
	int eCLow, eCHigh;
	float eCStep;
};

class MINIPH : public MCP3221
{
 public:
	MINIPH(uint8_t adcAddress, int adcVRef, float opAmpGain, bool isRollingAVG, bool ispH10Cal);

	void calibratepH7(int calnum);
	void calibratepH4(int calnum);
	void calibratepH10(int calnum);
	void calcpHSlope ();
	float calcpH(int raw);
	float tempAdjustpH(float pHtoAdjust, float temp);
	
	void updateopAmpGain(float opAmpGain);

	void writeParamsToEEPROM();

 private:
	float _opAmpGain;
	bool _isRollingAVG, _ispH10Cal;
 
	pHParameters_T _pHParams;

	void reset_pHParams(void);
};

class MINIEC : public MCP3221
{
public:
	MINIEC(uint8_t adcAddress, int adcVRef, int RGain, float oscVout, float kCell, bool isRollingAVG);

	void calibrateeCLow(int calnum);
	void calibrateeCHigh(int calnum);
	void calceCSlope();
	float calceC(int raw);
	float tempAdjusteC(float pHtoAdjust, float temp);

	void updateRGain(int RGain);
	void updateoscVout(float oscVout);
	void updatekCell(float kCell);

	void writeParamsToEEPROM();

private:
	float _oscVout, _kCell;
	int _RGain;
	bool _isRollingAVG;

	eCParameters_T _eCParams;

	void reset_eCParams(void);
};

#endif