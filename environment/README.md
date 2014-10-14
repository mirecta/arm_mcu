Environment
===========

First of all unpack arm toolchain to your location

```
cd arm_toolchain_loc
tar xvf  gcc-arm-none-eabi-4_8-2014q4-20141009-linux.tar.bz2
cd gcc-arm-none-eabi-4_8-2014q4
mv * ../
cd ../
rmdir gcc-arm-none-eabi-4_8-2014q4
```

Now setup linux enviroment path to toolchain 
edit file arm_tools.sh
```
vi arm_tools.sh

change path to arm_toolchain_loc
```
and move file into /etc/profile.d (as root of course :))
```
mv arm_toolchain.sh /etc/profile.d
```

next unpack stm32f4 cube libs into stm32f4_loc
```
cd stm32f4_loc
tar xvf STM32Cube_FW_F4_V1.3.0.tar.bz2
```

For every project
==================

Copy startup code into your project (it depends which mcu u have in my case startup_stm32f411xe.s) 
```
cp stm32f4_loc/STM32Cube_FW_F4_V1.3.0/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f411xe.s
```




