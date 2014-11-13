GFXINC +=   $(GFXLIB)
GFXSRC +=	$(GFXLIB)/src/gfx.c


include $(GFXLIB)/src/gos/sys_make.mk
include $(GFXLIB)/src/gqueue/sys_make.mk
include $(GFXLIB)/src/gdisp/sys_make.mk
include $(GFXLIB)/src/gevent/sys_make.mk
include $(GFXLIB)/src/gtimer/sys_make.mk
include $(GFXLIB)/src/gwin/sys_make.mk
include $(GFXLIB)/src/ginput/sys_make.mk
include $(GFXLIB)/src/gadc/sys_make.mk
include $(GFXLIB)/src/gaudio/sys_make.mk
include $(GFXLIB)/src/gmisc/sys_make.mk
include $(GFXLIB)/src/gfile/sys_make.mk

GFXVPATH += $(GFXLIB)/src
GFXVPATH += $(GFXLIB)/src/gos
GFXVPATH += $(GFXLIB)/src/gqueue
GFXVPATH += $(GFXLIB)/src/gdisp
GFXVPATH += $(GFXLIB)/src/gevent
GFXVPATH += $(GFXLIB)/src/gtimer
GFXVPATH += $(GFXLIB)/src/gwin
GFXVPATH += $(GFXLIB)/src/ginput
GFXVPATH += $(GFXLIB)/src/gadc
GFXVPATH += $(GFXLIB)/src/gaudio
GFXVPATH += $(GFXLIB)/src/gmisc
GFXVPATH += $(GFXLIB)/src/gfile
