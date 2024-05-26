#! /usr/bin/python3
try:
    import serial
except:
    logger.debug("Error can not find serial: py -3 -m pip install pyserial")
    exit()

from serial.tools import list_ports
from sys import platform

import sys
sys.path.append('..')

import logging
logger = logging.getLogger('console')

import ddf_expect
import ddf_uart


#
# setup wifi
#
def setWifi( console ):
    console.write(b'addssid dforsythnet\r\n')
    x = ddf_expect.expect( console, ["Got IP: [0-9.]+"], 30)
    logger.debug(str(x.match) + ": " + x.text)
    return(x)
#
# check wifi
#
def checkWifi( console ):
    console.write(b'winfo\r\n')
    x = ddf_expect.expect( console, ["IP: [0-9.]+","Gateways 0 entries"], 3)
    logger.debug(str(x.match) + ": " + x.text)
    return(x)

#
# m a i n
#
logging.basicConfig(level=logging.DEBUG)
prompt = "2DKITS>"

port = os.environ['KIT_PORT']

console = ddf_uart.connect( port, prompt )
wifi = checkWifi( console )
if wifi.match != 0:
    wifi = setWifi( console )
    wifi = checkWifi( console )
if wifi.match == 0:
    logger.debug('** PASSED')
    console.close()
    exit(0)
else:
    logger.debug('** FAILED')

console.close()
exit(1)
