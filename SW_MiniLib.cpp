/////////////////////////////////////////////////////////////////////////////
/*!

  SW_MiniLib.cpp
  MinipH and MinieC Library by Ryan Edwards, Sparkys Widgets
  
  Use this library with Sparkys Widgets MinipH, MinieC, and will expand to cover other boards using similar hardware

  I invest a lot of time and resources providing open source hardware, software, and tutorials 
  Please help support my efforts by purchasing products from www.sparkyswidgets.com, donating some time 
  on documentation or you can even donate some BitCoin to 1NwPNsf6t5vpph6AYY5bg361PSppPSSgDn

*/
/////////////////////////////////////////////////////////////////////////////

#include "SW_MiniLib.h"

MINIPH::MINIPH(uint8_t adcAddress, int adcVRef, float opAmpGain, bool isRollingAVG, bool ispH10Cal) : MCP3221(adcAddress, adcVRef)
{
	_isRollingAVG = isRollingAVG;
	_opAmpGain = opAmpGain;
	_ispH10Cal = ispH10Cal;
	//Lets double check to make sure we have some values stored in EEPROM if not load defaults
	eeprom_read_block(&_pHParams, (void *)0, sizeof(_pHParams));
	if (_pHParams.WriteCheck != PHWRITE_CHECK){
		reset_pHParams();
	}
}

void MINIPH::calibratepH7(int calnum)
{
	_pHParams.pH7Cal = calnum;
	calcpHSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	//eeprom_write_block(&_pHParams, (void *)0, sizeof(_pHParams)); 
}

void MINIPH::calibratepH4(int calnum)
{
	_pHParams.pH4Cal = calnum;
	calcpHSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	//eeprom_write_block(&_pHParams, (void *)0, sizeof(_pHParams)); 
}

void MINIPH::calibratepH10(int calnum)
{
	_pHParams.pH10Cal = calnum;
	calcpHSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	//eeprom_write_block(&_pHParams, (void *)0, sizeof(_pHParams)); 
}

void MINIPH::calcpHSlope ()
{
	if(_ispH10Cal)
	{
	//RefVoltage * our deltaRawpH / 12bit steps *mV in V / OP-Amp gain /pH step difference 10-7
		_pHParams.pHStep = ((calcMillivolts(_pHParams.pH10Cal) - calcMillivolts(_pHParams.pH7Cal)) / _opAmpGain) / 3;
	}
	else
	{
	//RefVoltage * our deltaRawpH / 12bit steps *mV in V / OP-Amp gain /pH step difference 7-4
		_pHParams.pHStep = ((calcMillivolts(_pHParams.pH7Cal) - calcMillivolts(_pHParams.pH4Cal)) / _opAmpGain) / 3;
	}
  
}

float MINIPH::getpHSlope()
{
	return _pHParams.pHStep;
}

float MINIPH::calcpH(int raw)
{
	float milliVolts, pH;

	milliVolts = calcMillivolts(raw);
	
	//Grab the neutral(pH7 calibration reading) reading so we can tare it out of the calculations
	//Math broken into steps to make more clear.
	float ph7calMillis = calcMillivolts(_pHParams.pH7Cal);
	float taredMillis = (ph7calMillis - milliVolts)/_opAmpGain;
	pH = 7-(taredMillis/_pHParams.pHStep);
	return pH;
}

float MINIPH::mappH(int raw)
{
	float temp;
	if (_ispH10Cal)
	{
		temp = map(raw, _pHParams.pH7Cal, _pHParams.pH10Cal, 700, 1000);
	}
	else
	{
		temp = map(raw, _pHParams.pH4Cal, _pHParams.pH7Cal, 400, 700);
	}
	temp = constrain(temp, 100, 1400);
	return temp/100;
}

float MINIPH::tempAdjustpH(float pHtoAdjust, float temp)
{

	float adjustedpH, pHDiffFrom7, tempDiffFrom25degC; 
	pHDiffFrom7 = abs(pHtoAdjust-7);
	tempDiffFrom25degC = abs(temp-25);
	adjustedpH = (0.03*pHDiffFrom7)*(tempDiffFrom25degC/10);

	if (pHtoAdjust>7 && temp<25)
		adjustedpH=adjustedpH;
 
	if (pHtoAdjust>7 && temp>25)
	adjustedpH=adjustedpH*-1;

	if (pHtoAdjust<7 && temp>25)
	adjustedpH=adjustedpH;

	if (pHtoAdjust<7 && temp<25)
	adjustedpH=adjustedpH*-1;

	adjustedpH = pHtoAdjust + adjustedpH;
	return adjustedpH;
}

void MINIPH::updateopAmpGain(float opAmpGain)
{
	_opAmpGain = opAmpGain;
}

void MINIPH::writeParamsToEEPROM()
{
	eeprom_write_block(&_pHParams, (void *)0, sizeof(_pHParams));
}

void MINIPH::reset_pHParams(void)
{
	//Restore to default set of parameters!
	_pHParams.WriteCheck = PHWRITE_CHECK;
	_pHParams.pH10Cal = 3096;
	_pHParams.pH7Cal = 2048; //assume ideal probe and amp conditions 1/2 of 4096
	_pHParams.pH4Cal = 1286; //using ideal probe slope we end up this many 12bit units away on the 4 scale
	_pHParams.pHStep = 59.16;//ideal probe slope
	writeParamsToEEPROM();
}

/*
Ec Portion

*/

MINIEC::MINIEC(uint8_t adcAddress, int adcVRef, int RGain, float oscVout, float kCell, bool isRollingAVG) : MCP3221(adcAddress, adcVRef)
{
	_isRollingAVG = isRollingAVG;
	_RGain = RGain;
	_oscVout = oscVout;
	_kCell = kCell;
	//Lets double check to make sure we have some values stored in EEPROM if not load defaults
	eeprom_read_block(&_eCParams, (void *)0, sizeof(_eCParams));
	if (_eCParams.WriteCheck != ECWRITE_CHECK){
		reset_eCParams();
	}
}

void MINIEC::calibrateeCLow(int calnum)
{
	_eCParams.eCLow = calnum;
	calceCSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	eeprom_write_block(&_eCParams, (void *)0, sizeof(_eCParams)); 
}

void MINIEC::calibrateeCHigh(int calnum)
{
	_eCParams.eCHigh = calnum;
	calceCSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	eeprom_write_block(&_eCParams, (void *)0, sizeof(_eCParams)); 
}

void MINIEC::calibrateeCLow(int calnum, long eCRefLow)
{
	_eCParams.eCLow = calnum;
	_eCParams.eCRefLow = eCRefLow;
	calceCSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	eeprom_write_block(&_eCParams, (void *)0, sizeof(_eCParams));
}

void MINIEC::calibrateeCHigh(int calnum, long eCRefHigh)
{
	_eCParams.eCHigh = calnum;
	_eCParams.eCRefHigh = eCRefHigh;
	calceCSlope();
	//write these settings back to eeprom, Lets do these save separately to save EEPROM writes
	eeprom_write_block(&_eCParams, (void *)0, sizeof(_eCParams));
}

void MINIEC::calceCSlope()
{

	//Technically we dont really need to calculate eC this way but we probably should implement this anyway
	_eCParams.eCStep = _eCParams.eCHigh / _eCParams.eCLow;
}

float MINIEC::geteCSlope()
{
	return _eCParams.eCStep;
}

float MINIEC::calceC(int raw, long eCRefLow, long eCRefHigh)
{
	//eCRefLow and eCRefHigh allow users to input the reference range
	//need to refactor to make these calculations with with any range. while it works careful using!
	float milliVolts, eC;

	milliVolts = calcMillivolts(raw);

	//Math broken into steps to make more clear.
	//Frist calc what the gain of the amp stage is op-amp gain (non-inv) = (Vout/Vin)-1 Vin is our oscVout voltage which should be measured with AC RMS preferred
	float ampGain = (milliVolts / _oscVout) - 1.0;
	//Now we can calculate what the Probes resistance is based on the gain of the amp (one part of the gain divider is fixed RGain)
	float RProbe = (_RGain / ampGain);
	//Since we use differently spaced probes to measure the different ranges of eC called KCell we need to account for this
	//.1KCell = .1cm per area distance, 1KCell = 1cm per area distance, and 10KCell = 10cm per area distance. As can be seen the smaller distance(.1Kprobe) would be useful for lower uS solutions
	//Where it would be overloaded in a very high uS solution meant more for a 1 or 10KCell probe. since uS is micro (or 10^-6) we can adjust our probes by shifting the decimal to match the probe
	//With 1KCell matching 10^-6, despite the probes changes on the readings we just effectively scaled them all to match!!:)
	eC = ((1000000) * _kCell) / RProbe;

	return eC;
}

float MINIEC::calceC(int raw)
{
	//eCRefLow and eCRefHigh allow users to input the reference range
	//need to refactor to make these calculations with with any range. while it works careful using!
	float milliVolts, eC;

	milliVolts = calcMillivolts(raw);

	//Math broken into steps to make more clear.
	//Frist calc what the gain of the amp stage is op-amp gain (non-inv) = (Vout/Vin)-1 Vin is our oscVout voltage which should be measured with AC RMS preferred
	float ampGain = (milliVolts / _oscVout) - 1.0;
	//Now we can calculate what the Probes resistance is based on the gain of the amp (one part of the gain divider is fixed RGain)
	float RProbe = (_RGain / ampGain);
	//Since we use differently spaced probes to measure the different ranges of eC called KCell we need to account for this
	//.1KCell = .1cm per area distance, 1KCell = 1cm per area distance, and 10KCell = 10cm per area distance. As can be seen the smaller distance(.1Kprobe) would be useful for lower uS solutions
	//Where it would be overloaded in a very high uS solution meant more for a 1 or 10KCell probe. since uS is micro (or 10^-6) we can adjust our probes by shifting the decimal to match the probe
	//With 1KCell matching 10^-6, despite the probes changes on the readings we just effectively scaled them all to match!!:)
	eC = ((1000000) * _kCell) / RProbe;

	return eC;
}

float MINIEC::mapeC(int raw)
{

	float temp;

	//eCRefLow and eCRefHigh allow users to input the reference range they have, this should make this calculation valid despite KCell and ranges
	//Caveat that they range falls within the standard range of the KCell of the probe used
	temp = map(raw, _eCParams.eCLow, _eCParams.eCHigh, _eCParams.eCRefLow, _eCParams.eCRefHigh);

	return temp;
}

float MINIEC::mapeC(int raw, long eCRefLow, long eCRefHigh)
{

	float temp;

	//eCRefLow and eCRefHigh allow users to input the reference range they have, this should make this calculation valid despite KCell and ranges
	//Caveat that they range falls within the standard range of the KCell of the probe used
	temp = map(raw, _eCParams.eCLow, _eCParams.eCHigh, eCRefLow, eCRefHigh); 

	return temp;
}

float MINIEC::tempAdjusteC(float eCtoAdjust, float temp)
{

	float adjustedeC, tempDiffFrom25degC;
	tempDiffFrom25degC = abs(temp - 25);
	//adjustedeC = (0.03*pHDiffFrom7)*(tempDiffFrom25degC / 10);

	adjustedeC = eCtoAdjust + adjustedeC;
	return adjustedeC;
}

void MINIEC::updateRGain(int RGain)
{
	_RGain = RGain;
}

void MINIEC::updateoscVout(float oscVout)
{
	_oscVout = oscVout;
}

void MINIEC::updatekCell(float kCell)
{
	_kCell = kCell;
}

void MINIEC::writeParamsToEEPROM()
{
	eeprom_write_block(&_eCParams, (void *)0, sizeof(_eCParams));
}

void MINIEC::reset_eCParams(void)
{
	//Restore to default set of parameters!
	_eCParams.WriteCheck = ECWRITE_CHECK;
	_eCParams.eCLow = 220; //assume ideal probe and amp conditions 1/2 of 4096
	_eCParams.eCHigh = 3200; //using ideal probe slope we end up this many 12bit units away on the 4 scale
	_eCParams.eCRefLow = 10000L;
	_eCParams.eCRefHigh = 40000L;
	_eCParams.eCStep = 59.16;//ideal probe slope
	writeParamsToEEPROM();
}