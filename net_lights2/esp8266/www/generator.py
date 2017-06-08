#!/usr/bin/python

import zlib

fl = open("index.html",'rt')
data_in = fl.read()

fl.close()

data_out = zlib.compress(data_in,9)

print "// before: %d , after: %d " % (len(data_in),len(data_out))


print "const char index_page[] = {"

data = []
i = 1;
for bt in data_out:
    if i % 20 == 0:
        data.append("\n0x%02x" % ord(bt))
    else:
        data.append("0x%02x" % ord(bt))
    i += 1
#endfor

print ','.join(data)
print "};"
