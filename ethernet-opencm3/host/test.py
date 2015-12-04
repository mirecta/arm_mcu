#!/usr/bin/python

import struct
import sys
import socket


sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.sendto(struct.pack("i", int(sys.argv[1])),("192.168.1.101",1200));
