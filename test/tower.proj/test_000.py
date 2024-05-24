#! /usr/bin/python3
from sys import platform

import sys
sys.path.append('..')

import logging
logger = logging.getLogger('console')

import ddf_expect
import ddf_uart

import os
import re

from serial.tools import list_ports

#
# get Kit Number
#
def getKitNumber( console ):
    console.write(b'version\r\n')
    x = ddf_expect.expect( console, ["kit number: ([0-9]+)"], 3)

    if x.match == 0:
       infoRegex = re.compile(r'(\d+)')
       info = infoRegex.search(x.text)
    return(info.group(1))

#
# m a i n
#
logging.basicConfig(level=logging.DEBUG)
prompt = "2DKITS>"

ports = list(list_ports.comports())
for p in ports:
#    logger.debug(p.device)

    console = ddf_uart.connect( p.device, prompt )
    kitNo = getKitNumber( console )
    if kitNo == sys.argv[1]:
        print(p.device)
        console.close()
        exit(0)
    console.close()

exit(1)
