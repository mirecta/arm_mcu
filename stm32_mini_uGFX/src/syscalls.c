#include <sys/time.h>
#include <errno.h>

//device
#include "stm32f10x.h" 

int _gettimeofday( struct timeval *tv, void *tzvp ){
  
   
    tv->tv_usec = 0;
    tv->tv_sec = RTC_GetCounter(); 
   

    return 0;
}




/// implementing syscalls for  newlib
    


    


