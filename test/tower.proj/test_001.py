#! /usr/bin/python3
from sys import platform

import sys
sys.path.append('..')

import logging
logger = logging.getLogger('console')

import ddf_expect
import ddf_uart
import os

#
# get Version
#
def getVersion( console ):
    console.write(b'version\r\n')
#    logger.debug('send version string '+'rts='+str(console.rts)+' dtr='+str(console.dtr));
    x = ddf_expect.expect( console, ["software version: [0-9.]+"], 3)
    logger.debug(str(x.match) + ": " + x.text)
    return(x)

#
# m a i n
#
logging.basicConfig(level=logging.DEBUG)
prompt = "2DKITS>"

port = os.environ['KIT_PORT']

console = ddf_uart.connect( port, prompt )
version = getVersion( console )
if version.match == 0:
    logger.debug('** PASSED')
    console.close()
    exit(0)

logger.debug('** FAILED')
console.close()
exit(1)
