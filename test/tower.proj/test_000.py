#! /usr/bin/python3
from sys import platform

import sys
sys.path.append('..')

import logging
logger = logging.getLogger('console')

import od_expect
import od_uart

from serial.tools import list_ports

#
# get Kit Number
#
def getKitNumber( console ):
    console.write(b'version\r\n')
    x = od_expect.expect( console, ["kit number: [0-9]+"], 3)
    return(x)

#
# m a i n
#
logging.basicConfig(level=logging.DEBUG)
prompt = "2DKITS>"

ports = list(list_ports.comports())
for p in ports:
    logger.debug(p.device)

    console = od_uart.connect( p.device, prompt )
    version = getKitNumber( console )
    if version.match == -1:
        logger.debug("kit number: na "+p.device)
    else:
        logger.debug(version.text+" "+p.device)
    console.close()

exit(1)
