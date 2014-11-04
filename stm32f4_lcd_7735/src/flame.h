#include <stdint.h>

#ifndef _FLAME_H_
#define _FLAME_H_

#define FLAME_WIDTH   160
#define FLAME_HEIGHT  80
#define LAST_LINE (FLAME_WIDTH * FLAME_HEIGHT - FLAME_WIDTH)



class Flame
{

    public: 
        Flame();
        void compute(uint8_t power, bool randomize = true);
        const uint8_t* data(void) {return _data;}
        int size(void) const {return FLAME_WIDTH * FLAME_HEIGHT;}
        static uint16_t getColor(uint8_t heat);


   private:
        uint8_t _data[FLAME_WIDTH * FLAME_HEIGHT];

};


#endif


