

SEMIHOSTING TEST
================

Semihosting means u can use printf function via stlink to pc

first make

```
ENABLE_SEMIHOSTING=1 make
```

then run OPENOCD

```
openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg
```

in other console run gdb

```
arm-none-eabi-gdb --eval-command="target remote localhost:3333" --eval-command="mon arm semihosting enable" adc.elf
```
in gdb

```
load
cont
```

now in console with openOCD u can see your printf output :D

good luck



