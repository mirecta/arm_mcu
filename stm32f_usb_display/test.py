#!/usr/bin/python

import fcntl
import os
import struct

def ioctl(fd, EVIOC, code, return_type, buf = None):
    size = struct.calcsize(return_type)
    if buf == None:
        buf = size*'\x00'
    abs = fcntl.ioctl(fd, EVIOC(code, size), buf)
    return struct.unpack(return_type, abs)


_IOC_WRITE = 1
_IOC_READ = 2

_IOC_NRBITS = 8
_IOC_TYPEBITS = 8
_IOC_SIZEBITS = 14
_IOC_DIRBITS = 2

_IOC_NRSHIFT = 0
_IOC_TYPESHIFT = _IOC_NRSHIFT + _IOC_NRBITS
_IOC_SIZESHIFT = _IOC_TYPESHIFT + _IOC_TYPEBITS
_IOC_DIRSHIFT = _IOC_SIZESHIFT + _IOC_SIZEBITS

#define _IOC(dir,type,nr,size) \
#   (((dir)  << _IOC_DIRSHIFT) | \
#    ((type) << _IOC_TYPESHIFT) | \
#    ((nr)   << _IOC_NRSHIFT) | \
#    ((size) << _IOC_SIZESHIFT))
def _IOC(dir, type, nr, size):
        return ( (dir << _IOC_DIRSHIFT) |
                 (ord(type) << _IOC_TYPESHIFT) |
                 (nr << _IOC_NRSHIFT) |
                 (size << _IOC_SIZESHIFT))

#define _IOR(type,nr,size)  _IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
def _IOR(type,nr,size):
    return _IOC(_IOC_READ, type, nr, size)

#define _IOW(type,nr,size)  _IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))
def _IOW(type,nr,size):
    return _IOC(_IOC_WRITE, type, nr, size)




#define HIDIOCGRAWINFO      _IOR('H', 0x03, struct hidraw_devinfo)
def _HIDIOCGRAWINFO(none, len):
    return _IOR('H', 0x03, len)

def HIDIOCGRAWINFO(fd):
    """ get hidraw device infos """
    type = 'ihh'
    return ioctl(fd, _HIDIOCGRAWINFO, None, type)

#define HIDIOCGRAWNAME(len)     _IOC(_IOC_READ, 'H', 0x04, len)
def _HIDIOCGRAWNAME(none, len):
    return _IOC(_IOC_READ, 'H', 0x04, len)

def HIDIOCGRAWNAME(fd):
    """ get device name """
    type = 1024*'c'
    return "".join(ioctl(fd, _HIDIOCGRAWNAME, None, type)).rstrip('\x00')


def listHidraw():

    """
    """
    files = [ f for f in os.listdir("/dev/") if f.startswith("hidraw") ]
    result = []

    for fl in files:
        fd = os.open("/dev/%s" % fl,  os.O_RDONLY)
        
        name =  HIDIOCGRAWNAME(fd)
        bus,vid,pid = HIDIOCGRAWINFO(fd)
        print "HID %s : %04x:%04x, %s " % (fl,vid,pid & 0xffff,name)
        os.close(fd)
    #endfor


if __name__ == "__main__":
    listHidraw()

