
import logging
logger = logging.getLogger('console')

try:
    import serial
except:
    logger.debug("Error can not find serial: py -3 -m pip install pyserial")
    exit(1)

import ddf_expect

#from serial.tools import list_ports
#
# list uart ports
#
#def listPorts():
#    port = list(list_ports.comports())
#    for p in port:
#        logger.debug(p.device)



#
# connect
#
class connect(serial.Serial):
    def __init__(self, port, prompt):
#        logger.debug('connecting to '+port)
        try:
            super().__init__(port, 115200, timeout=1, xonxoff=False, rtscts=False)
            self.reset_output_buffer()
            self.reset_input_buffer()
        except:
            logger.debug("Serial Port open failed")
            logger.debug(Exception)
            exit(1)

# forcing rts and dtr to false is junk from past debugging google research
#        self.rts = False
#        self.dtr = False

        self.write(b'\r\n')
        x = ddf_expect.expect(self, [prompt], 30)
        if x.match != 0:
            logger.debug("command prompt not found")
#        else:
#            logger.debug(": connection verified")
