Welcome To: Sparkys Widgets MinipH, eC and other Mini-board Driver library
================================


##### Note: This can be used with any AVR/Arduino that needs to interface with hardware that is based on the Mini line of interface boards(MinipH, MinieC, etc...)

Written to help simplify adding in multiple interfaces boards (even the same type I suppose) into a project and help keep things more uniform. This also takes advantage of moving the hardware driver(IE the actually ADC interfacing) portions to its own driver library thus the first steps of interchangeability are in place. One could in theory change to a spi driven ADC for example.

Class Info
-------------------------

- Each sensor type support has its own class
- Feed it address and and measured VREF(I.E measured VDD into IC) for ADC interfacing
- Each sensor inherits from the MCP3221 Library 


Method Info
-------------------------

- Access to MCP3221 Library methods(CalcMillis, RollingAVG, EXPAVG, READADC etc...)
- Calibration methods for 4, 7 and 10 reference solutions
- Calibration methods for eC solutions (a low and high to calc slope)
- temperature compensation methods

Additions, Errata
-------------------------

- Inherent from MCP4017 library to add in digital potentiometer support (will branch lib with ext extension)
- Add more probe support ORP, DO etc...


License Info
-------------------------

<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">SW_MiniLib</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="http://www.sparkyswidgets.com/" property="cc:attributionName" rel="cc:attributionURL">Ryan Edwards, Sparky's Widgets</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.