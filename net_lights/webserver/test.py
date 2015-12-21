#!/usr/bin/python

import struct
import sys
import socket

print """
// 0x00 0x00 0x00 - reset
// 0x01 0xMM 0x00 - set mode MM (1 - 4)
// 0x02 0xSS 0x00 - set speed  (0 - 8)
// 0x03 0xHH 0xSS - set color hue HH sat SS
// 0x04 0xLL 0x00 - set max lightness
"""

data = chr(int(sys.argv[1])) + chr(int(sys.argv[2])) + chr(int(sys.argv[3]))

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)


sock.sendto(data,("192.168.1.252",1200));
