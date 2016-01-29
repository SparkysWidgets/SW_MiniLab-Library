/////////////////////////////////////////////////////////////////////////////
/*!

  SW_MiniLib.h
  MinipH and MinieC Library by Ryan Edwards, Sparkys Widgets
  
  Use this library with Sparkys Widgets MinipH, MinieC, and will expand to cover other boards using similar hardware

  I invest a lot of time and resources providing open source hardware, software, and tutorials 
  Please help support my efforts by purchasing products from www.sparkyswidgets.com, donating some time 
  on documentation or you can even donate some BitCoin to 1NwPNsf6t5vpph6AYY5bg361PSppPSSgDn

  4/2/2015 Initial changes to the constructor to allow for selection of Pre or Post MiniPh V4.0
  4.0 hardware implements probe Biasing and eliminates the need for the charge pump and its additional
  components(let alone eliminates the extra noise created by the switching noise.
  special thanks to Margaret Johnson of bitknitting.wordpress.com for her contributions to improving
  MinipH!

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
#define ECWRITE_CHECK 0x1235
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
	int eCLow, eCRefLow;
	long eCHigh, eCRefHigh;
	float eCStep;
};

class MINIPH : public MCP3221
{
 public:
	MINIPH(uint8_t adcAddress, int adcVRef, float opAmpGain, bool isRollingAVG, bool ispH10Cal, bool isProbeBiased);

	void calibratepH7(int calnum);
	void calibratepH4(int calnum);
	void calibratepH10(int calnum);
	void calcpHSlope ();
	float getpHSlope();
	float calcpH(int raw);
	float mappH(int raw);
	float tempAdjustpH(float pHtoAdjust, float temp);
	
	void updateopAmpGain(float opAmpGain);

	void writeParamsToEEPROM();

	//Yeah, yeah this really should be private but I dont want to make a getter, setter for each params
	// and I need them exposed to let users know what the stored pH values are should they need it. FIXME for future!
	pHParameters_T _pHParams;
	void reset_pHParams(void);

 private:
	float _opAmpGain;
	bool _isRollingAVG, _ispH10Cal,
		//Here we added a check to see if we are using the New MinipH design or another design which implements Probe Bias
		//Generally one creates a virtual ground at half Supply(or in this case 1/2 adcVRef), on MinipH V4.0 we have no gain on probe
		//Meaning you should beable to use this with orp probes as well but they may clip!
		_isProbeBiased;
};

class MINIEC : public MCP3221
{
public:
	MINIEC(uint8_t adcAddress, int adcVRef, int RGain, float oscVout, float kCell, bool isRollingAVG);

	void calibrateeCLow(int calnum);
	void calibrateeCHigh(int calnum);
	void calibrateeCLow(int calnum, long eCRefLow);
	void calibrateeCHigh(int calnum, long eCRefHigh);
	void calceCSlope();
	float geteCSlope();
	float calceC(int raw);
	float calceC(int raw, long eCRefLow, long eCRefHigh);
	float mapeC(int raw);
	float mapeC(int raw, long eCRefLow, long eCRefHigh);
	float tempAdjusteC(float pHtoAdjust, float temp);

	void updateRGain(int RGain);
	void updateoscVout(float oscVout);
	void updatekCell(float kCell);

	void writeParamsToEEPROM();

	//Yeah, yeah this really should be private but I dont want to make a getter, setter for each params
	// and I need them exposed to let users know what the stored eC values are should they need it. FIXME for future!
	eCParameters_T _eCParams;	
	void reset_eCParams(void);

private:
	float _oscVout, _kCell;
	int _RGain;
	bool _isRollingAVG;	
};

#endif