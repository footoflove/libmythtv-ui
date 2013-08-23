DIR_LOCAL	= libmythui

#***********************************************************************
# Do not change the following include
#***********************************************************************
include $(_TMROOT)/sde/environment.mk




#-----------------------------------------------------------------------
# Source environment variables
#-----------------------------------------------------------------------
CXX_SOURCES 	=

C_SOURCES	=


#-----------------------------------------------------------------------
# Required components
#-----------------------------------------------------------------------
REQUIRES        = 

#-----------------------------------------------------------------------
# Directory where the 3rdparty includes are stored
#-----------------------------------------------------------------------
DIR_INCLUDE	= 

#-----------------------------------------------------------------------
# local CFLAGS
#-----------------------------------------------------------------------
LOCAL_CFLAGS	=

LOCAL_LDFLAGS   = -L$(GCC_BASE)/lib

#-----------------------------------------------------------------------
# local CPPFLAGS
#-----------------------------------------------------------------------
LOCAL_CXXFLAGS	=


#***********************************************************************
# Do not change this
#***********************************************************************

##include diversity.mk

ROOTFS_TMPLT_DIR   := $(_TMTGTBUILDROOT)/os/oslinux/comps/rootfs/tmplt
INSTALL            := install


#############################################################################
# The version of busybox to build
#############################################################################
BB_VERSION       := 1.14.0

ifeq ($(_TMTGTREL),debug)

else

endif

###########################################################################

#-----------------------------------------------------------------------
# local CFLAGS
#-----------------------------------------------------------------------
QT_CFLAGS_DIRECTFB 	= -I$(_TMTGTBUILDROOT)/comps/generic_apps/usr/include/directfb
QT_LIBS_DIRECTFB        = -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr -ldirectfb -lfusion -ldirect -lpthread
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/systems
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/wm
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/inputdrivers
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/interfaces/IDirectFBFont
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/interfaces/IDirectFBImageProvider
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/interfaces/IDirectFBVideoProvider
QT_LIBS_DIRECTFB        += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib/directfb-1.4-0/gfxdrivers
QT_LIBS_DIRECTFB        += $(QT_DFB_FLAGS)

QT_LIBS_DIRECTFB +=     -udirectfb_cnxtgfx -ldirectfb_fbdev -udirectfb_fbdev -ldirectfb_cnxtgfx -udirectfbwm_default -ldirectfbwm_default -ldirectfb_linux_input
QT_LIBS_DIRECTFB +=     -udirectfb_linux_input_ctor -udirectfb_linux_input_dtor -lidirectfbimageprovider_png -uIDirectFBImageProvider_PNG_ctor
QT_LIBS_DIRECTFB +=     -uIDirectFBImageProvider_PNG_dtor -lpng -lz -lidirectfbimageprovider_jpeg -uIDirectFBImageProvider_JPEG_ctor -uIDirectFBImageProvider_JPEG_dtor
QT_LIBS_DIRECTFB +=     -ljpeg -lidirectfbfont_ft2 -uIDirectFBFont_FT2_ctor -uIDirectFBFont_FT2_dtor -lidirectfbfont_dgiff -uIDirectFBFont_DGIFF_ctor -uIDirectFBFont_DGIFF_dtor
QT_LIBS_DIRECTFB +=     -lidirectfbfont_default -uIDirectFBFont_DEFAULT_ctor -uIDirectFBFont_DEFAULT_dtor -ldirectfb -udirectfb -ldirect -udirect -lpthread -lm -lrt -lc -lfusion
QT_LIBS_DIRECTFB +=     -ljpeg -lfreetype
QT_LIBS_DIRECTFB +=     $(DIRECTFB_LIBS_QT)


ifeq ($(_TMTGTREL),debug)
ifeq ($(DFB_LINKTYPE),dynamic)
DIRECTFB_LIBS_QT += -llnxtmvssUsr -llnxnotifyqUsr -llnxUKAL -llnxplatUsr -llnxcssUsr -llnxpvrUsr 
#we are using _g in above line as we dont have corresponsing .so avaliable for cnxt user space component
else
DIRECTFB_LIBS_QT += -llnxtmvssUsr_g -llnxnotifyqUsr_g -llnxUKAL_g -llnxplatUsr_g -llnxcssUsr_g -llnxpvrUsr_g 
endif
else
DIRECTFB_LIBS_QT += -llnxtmvssUsr -llnxnotifyqUsr -llnxUKAL -llnxplatUsr -llnxcssUsr -llnxpvrUsr 
endif

export  DIRECTFB_LIBS_QT

QT_DFB_FLAGS += -L$(_TMTGTBUILDROOT)/comps/generic_apps/usr/lib -L$(_TMTGTBUILDROOT)/comps/generated/$(_SDE_DIR_LIB_EXT)/$(_SDE_DIVERSITY) -L$(_TMTGTBUILDROOT)/comps/generated/$(_SDE_DIR_LIB_EXT)

export  QT_DFB_FLAGS

##########################################################################
############################################################################
#
# Local targets
#
###########################################################################

export QMAKESPEC=$(_TMTGTBUILDROOT)/comps/generic_apps/qt/tmp/mkspecs/qws/linux-arm-g++

############################################################################
all: libmythui
	$(INSTALL) -d $(SYSTEM_APPFS_DIR)/qt/fonts
	$(INSTALL) -m 644 $(_TMROOT)/sd/apps/huangpu/comps/huangpu_ui/opt/qt.conf  $(SYSTEM_APPFS_DIR)/bin
	$(INSTALL) -m 644 $(_TMROOT)/sd/apps/huangpu/comps/huangpu_ui/opt/wqy-zenhei.ttf $(SYSTEM_APPFS_DIR)/lib/qt/fonts

libmythui_src_dir := ./src
libmythui_dst_dir := $(_TMTGTBUILDROOT)/comps/generic_apps/$(DIR_LOCAL)

libmythui: $(libmythui_dst_dir)/libmythui.so


.PHONY: $(libmythui_dst_dir)/libmythui.so $(mainmenu_dst_dir)/mainmenu $(readkeyv_dst_dir)/readkeyv $(browser_dst_dir)/browser

PP:=$(_TMTGTBUILDROOT)/comps/generic_apps/qt/tmp/bin/pp.sh

$(libmythui_dst_dir)/libmythui.so:
	( if [ ! -d $(libmythui_dst_dir) ]; then mkdir -p $(libmythui_dst_dir); fi )
	$(INSTALL) -d $(SYSTEM_APPFS_DIR)/lib/qt/plugins/kbddrivers
	$(_TMTGTBUILDROOT)/comps/generic_apps/qt/tmp/bin/qmake  $(libmythui_src_dir)/libmythui.pro "OBJECTS_DIR=$(libmythui_dst_dir)" "DESTDIR=$(libmythui_dst_dir)" -o $(libmythui_dst_dir)/Makefile
	$(PP) $(libmythui_dst_dir)/Makefile
	$(MAKE) -C $(libmythui_dst_dir)
	$(INSTALL) -m 755 $(libmythui_dst_dir)/libmythui*.so* $(SYSTEM_APPFS_DIR)/lib/qt/

distclean:
	@echo "Cleanup all qt related stuff"
	$(MAKE) -C $(libmythui_dst_dir) distclean
############################################################################
debug:
	@echo "src dir:" $(glib_src_dir)
	@echo "_TMROOT" $(_TMROOT)
	@echo " Kernel path ==>" $(KSRC)
	@echo "_TMSYSROOT" $(_TMSYSROOT)
	@echo "_TMTGTBUILDROOT" $(_TMTGTBUILDROOT)
	@echo "ROOTFS_TMPLT_DIR" $(ROOTFS_TMPLT_DIR)
	@echo "PATH " $(NXP_BASE_ROOT)/target/output/objs/$(KERNEL_CONFIG)/include


#***********************************************************************
# Do not change the following include
#***********************************************************************
ifneq ($(DIR_CONFIG),_)
include $(DIR_SDE)/$(DIR_CONFIG)/makelib.mk
endif
