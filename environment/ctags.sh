#!/bin/bash

CUBE_PATH=/home/miro/agct/STM32Cube_FW_F4_V1.3.0
mkdir -p ~/.vim/tags

ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++ -f ~/.vim/tags/arm ${CUBE_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++ -a -f ~/.vim/tags/arm ${CUBE_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc/
ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++ -a -f ~/.vim/tags/arm  ${CUBE_PATH}/Drivers/CMSIS/Include

