
import re
import time

import logging
logger = logging.getLogger('console')

#
# expect
#
class expect:
    def __init__(self, console, patterns, timeout):
        vt100_pos = '\x1B[6n'
        vt100_max = '\x1B[999C'
        timeoutCountdown = timeout
        self.all = ""
        self.match = -1
        for i in range( timeout, -1, -1):
            if console.in_waiting != 0:
                data = console.read(console.in_waiting)
                self.all += data.decode(encoding='utf-8', errors="ignore")
# for linenoise VT100 commands (ESP32)
                if self.all.find(vt100_pos) != -1:
                    print("Hit pos ddf0");
                    console.write(b'\x1B[8:1R\0')
                    self.all = self.all.replace(vt100_pos,'')

                if self.all.find(vt100_max) != -1:
                    print("Hit max ddf0");
                    console.write(b'\x1B[8:80R\0')
                    self.all = self.all.replace(vt100_max,'')
#end of linenoice
            for i in range(len(patterns)):
                x =  re.search(patterns[i], self.all)
                if x:
                    self.match = i
                    self.text = x.group()
                    return
            if console.in_waiting == 0:
               time.sleep(1)
        self.text = "<timeout "+str(timeout)+" seconds>"
#        logger.debug(str(self.all))
