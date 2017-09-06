Environment
===========

First of all unpack [arm toolchain](https://launchpad.net/gcc-arm-embedded) to your location

```
cd <arm_toolchain_loc>
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

change path to <arm_toolchain_loc>
```
and move file into /etc/profile.d (as root of course :))
```
sudo mv arm_tools.sh /etc/profile.d
source /etc/profile.d/arm_tools.sh
```

next unpack stm32f4 cube libs into stm32f4_loc
```
cd <stm32f4_loc>
tar xvf STM32Cube_FW_F4_V1.3.0.tar.bz2
```

For every project (without HAL libraries, only low level)
---------------------------------------------------------

Copy startup code into your project (it depends which mcu u have in my case startup_stm32f411xe.s) 
```
cp <stm32f4_loc>/STM32Cube_FW_F4_V1.3.0/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f411xe.s ./
```
This startup code define 
* reset handler
* copy data part of flash to ram 
* cleanup bss sector in ram 
* define all interrupts halndlers
* call pll setup function
* call main


Copy linker script into your projects (it depends which mcu u have in my case  STM32F411RE_FLASH.ld)
```
cp <stm32f4_loc>/STM32Cube_FW_F4_V1.3.0/Projects/STM32F411RE-Nucleo/Templates/TrueSTUDIO/STM32F4xx-Nucleo/STM32F411RE_FLASH.ld ./
```
This linker script tells to linker vdere is data and program section , where is reset handler.

[ST-Link](https://github.com/texane/stlink)
------------------------------------------
Build and install
```
tar xvf stlink.tar.bz2 
cd stlink
./autogen.sh
./configure
make 
sudo make install
```
Add stlink to udev 
```
sudo cp  49-stlinkv*.rules /etc/udev/rules.d/
sudo /etc/init.d/udev reload
```
Test 
```
st-flash read dump.bin  0x8000000 256

```
if not work  u can try add sudo , if sudo works, there are sam problem with udev :)

GDB

```
st-util
```
then run debugger 
```
arm-none-eabi-gdb --eval-command="target remote localhost:4242" build/main.elf
```

etc.
```
load - flash
br .... - add breakpoint file:line or *address
cont - continue
.....
s, si,n - step, step instruction, next

p args - print args
info registers - print registers
list - list source
disassemble - disassemble memory
x address - pint address cell 

```

Unlock 
```
openocd -f interface/stlink-v2.cfg -f target/stm32f1x_stlink.cfg -c "init" -c "halt" -c "stm32f1x unlock 0" -c "shutdown"
```




Prepare VIM (optional)
----------------------
Edit ctags.sh and set your path to stm cube, then run it
```
./ctags.sh
```
Add this lines to your .vimrc
```
autocmd FileType c setlocal tags+=~/.vim/tags/arm
autocmd FileType cpp setlocal tags+=~/.vim/tags/arm
```

good luck

PS: u must read stm32f4_developer_datasheet.pdf :D
