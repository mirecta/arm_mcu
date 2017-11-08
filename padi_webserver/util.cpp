
#include "util.h"


const uint8_t config0[] = "mode=ap\n";
const uint8_t config1[] = "ssid=padiiap\n";
const uint8_t config2[] = "passwd=password\n";

bool streq(const char* buff1, const char* buff2){
  int len1 = getlen(buff1);
  int len2 = getlen(buff2);
  if (len1 != len2){
    return false;
  }
  for (int i = 0; i < len1; ++i){
    if (buff1[i] != buff2[i]){
      return false;
    }
  }
  return true;
}

int getlen(const char* buffer){
  //obtain len
    int len = 0;
    char c = buffer[len];
    while(c != '\0'){
      ++len;
      c = buffer[len];
    }
    return len;
}


int split(char* buffer, char** strs, const char delimiter, int maxparts){

    //obtain len
    int len = getlen(buffer);
    int split = 0;
    strs[split] = buffer;
    
    for(int i = 0; i < len; ++i){
      if (buffer[i] == delimiter){
        buffer[i] = '\0';
        if( i + 1 != len){
          ++split;
          strs[split] = &buffer[i+1];
          if(split == maxparts - 1)
            break;
        }
      }
    }
    return split + 1;
}

bool space(char c){
  if ((c >= 0x09 && c <=0x0d) || c == 0x20){
    return true;
  }
  return false;
}

char* strip(char* buffer){

  //obtain len
  int len = getlen(buffer);
  int i;
  for ( i = len - 1; i >= 0; --i){
    if (space(buffer[i])){
      buffer[i] = '\0';
    }
    else{
      break;
    }
  }
  len = i + 1;
  for (i = 0; i < len; ++i){
    if (!space(buffer[i])){
        break;
    }
   }
  return &buffer[i];
}

int readConfig(SdFatFs& fat){
  printf(">>>>>>>>\n");
  char buf[128];
  char *parts[2];
  int rd;
  
  int res = 0;
  
  if(fat.isFile(CONFIG) != 1){
    res = fat.mkdir(CONFIG_DIR);
    if (res < 0){
        return res;
    }
    SdFatFile file = fat.open(CONFIG);
    file.write(config0 ,sizeof(config0) - 1);
    file.write(config1 ,sizeof(config1) - 1);
    file.write(config2 ,sizeof(config2) - 1);
    file.close();
  }
  printf("%d \n",res);
  SdFatFile file = fat.open(CONFIG);
  memset(buf, 0, sizeof(buf));
  do{
    rd = file.readline(buf,sizeof(buf));
    //int c = getlen(buf);
    int c = split(buf,parts,'=');
    if (c != 2) continue;
    
    if(streq(strip(parts[0]),"mode")){
      if(streq(strip(parts[1]),"ap")){
        printf("vyrobim ap \n");
      }
    }
    if(streq(strip(parts[0]),"ssid")){
      printf ("ESSID bude:'%s'\n",strip(parts[1]));
    }
    if(streq(strip(parts[0]),"passwd")){
      printf ("heslo bude:'%s'\n",strip(parts[1]));
    }
    /*if (c == 2){
      printf(">>>%s<<< >>>%s<<<<\n", strip(parts[0]),strip(parts[1]));
    }*/
  }while (rd != -1);
  file.close();
  return res;
}




