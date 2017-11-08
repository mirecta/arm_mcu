#include "SdFatFs.h"
 
#ifndef _UTIL_H_
#define _UTIL_H_





#define CONFIG_FILE "device.conf"
#define CONFIG_DIR "/conf"
#define CONFIG CONFIG_DIR "/" CONFIG_FILE



struct Config_t {
      char  ssid[20];
      char  passwd[20];
      bool ap;     
};


int getlen(const char* buffer);
bool streq(const char* buff1, const char* buff2);
/**
 * @brief find delimiter replace it with \0 and sets starts of separate strings to strs
 * 
 * return number of valid splitted string characters 
 */

int split(char* buffer, char** strs, const char delimiter, int maxparts = 2);

/**
 * @brief count from end to start and replace last whitespace with \0
 *         count from start to end and return pointer to first valid char
 */

char* strip(char* buffer);


 int readConfig(SdFatFs& fat);




#endif //_UTIL_H_
