#!/usr/bin/python



def hslToRgb( h, s, l):

    region = 0
    remainder = 0
    p = 0
    q = 0 
    t = 0

    if s == 0:
    
        r = l;
        g = l;
        b = l;
        return (r,g,b)
            

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (l * (255 - s)) >> 8;
    q = (l * (255 - ((s * remainder) >> 8))) >> 8;
    t = (l * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    if region == 0:
        r = l; g = t; b = p;

    elif region == 1:
        r = q; g = l; b = p;

    elif region == 2:
        r = p; g = l; b = t;

    elif region == 3:
        r = p; g = q; b = l;

    elif region == 4:
        r = t; g = p; b = l;

    else:
        r = l; g = p; b = q;

    #endif

    return (r,g,b)


for i in range (0,256,16):

    color = hslToRgb(i,255,255)
    print ' <div id=hue%03d class="colorsquare" style="background-color:#%02x%02x%02x;"></div>' % (i,color[0],color[1],color[2])


for i in range (0,256,16):
    print '$("#hue%03d").click({param1:"%d", param2: "255"},setColor);' % (i,i)

