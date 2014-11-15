GFXSRC +=   $(GFXLIB)/src/ginput/ginput.c \
			$(GFXLIB)/src/ginput/mouse.c \
			$(GFXLIB)/src/ginput/keyboard.c \
			$(GFXLIB)/src/ginput/toggle.c \
			$(GFXLIB)/src/ginput/dial.c


include $(GFXLIB)/drivers/ginput/touch/ADS7843/ginput_lld.mk
GFXVPATH += $(GFXLIB)/drivers/ginput/touch/ADS7843/

