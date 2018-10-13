

FATFS wih sdhc via SPI 
================

Makefile using Black Magic Probe 


first make

```
ENABLE_SEMIHOSTING=1 make
```
then flash

```
BMP_PORT=/dev/ttyACM0 make flash
```

and debug for view semihosting printf

```
BMP_PORT=/dev/ttyACM0 make debug
```
now in console u can see your printf output :D

list main dir in this case

good luck



