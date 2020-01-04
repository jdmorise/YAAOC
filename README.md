# Yet Another AVR OLED Clock

This repo contains C code for an Microchip AVR ATMEGA8A based Alarm Clock. The time is displayed with a SSD1306 OLED display in 4 big characters (HH:MM). The fonts and library are offered in the repository https://github.com/jdmorise/BMH-fonts. It offers two push-puttons to set the clock, AM_PM or 24 hours mode, oscillator calibration and runs from a 4MHz crystal. 

## External connections

1. Alarm Output: Conencted to PortB Pin 0. Pin is high when alarm is enabled. 
2. Buttons: Button 1 and 2 are connected to PortD Pin 0 and 1. The buttons are used with pullups, push of button results in a 0. 
3. OLED: connected to TWI pins SCL and SDA on PortC Pins 4 and 5. 




