
# Build
## First
```
git clone https://github.com/mirecta/arm_mcu.git
git submodule update --init --recursive
```

## Update
```
git pull --recurse-submodules
```

## Make
```
cd arm_mcu/ixperta-hid/libopencm3
make
cd ../src
make
```

# Hardware
Some STM32F103 with USB [STM32MINI](https://lnb-konvertory.heureka.sk/inverto-black-pro-twin-monoblok-23mm-lnb-4_3/#) or [BluePill](https://wiki.stm32duino.com/index.php?title=Blue_Pill)  

# Pins
* C14 - A
* C15 - B
* A0  - PgUp
* A1  - PgDn
* A2  - Left
* A3  - Right
* A4  - Up
* A5  - Down
* A6  - Enter
* A7  - Esc 


# Schematics
![SCH](https://raw.githubusercontent.com/mirecta/arm_mcu/master/ixpera-hid/doc/image.png)

2019 Miroslav Talasek
