#! /usr/bin/python3
from gpiozero import LED
import gpiozero.pins.rpigpio
import time
import sys

def close(self): pass
gpiozero.pins.rpigpio.RPiGPIOPin.close = close

ledB = LED(17, initial_value=None, pin_factory=gpiozero.pins.rpigpio.RPiGPIOFactory())
ledG = LED(27, initial_value=None, pin_factory=gpiozero.pins.rpigpio.RPiGPIOFactory())
ledR = LED(22, initial_value=None, pin_factory=gpiozero.pins.rpigpio.RPiGPIOFactory())

if len(sys.argv) != 4:
    print( 'setLed <red> <green> <blue>' )
    exit(1)

if sys.argv[1] == '0':
   ledR.on()
else:
   ledR.off()

if sys.argv[2] == '0':
   ledG.on()
else:
   ledG.off()

if sys.argv[3] == '0':
   ledB.on()
else:
   ledB.off()
