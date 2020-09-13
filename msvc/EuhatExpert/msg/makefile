BINGODEBUG ?= 1
PROGRAM := msg
LDFILES := direct_input8 direct_input5 direct_input_lib

BINGOLIB := common array tree winop strplus fileop dx bit_stream
include $(BINGODIR)/m/head.mak

#LDLIBS := winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib
LDLIBS := kernel32.lib user32.lib dxguid.lib

ifneq ($(BINGOOS),_WIN98)
LDLIBS += psapi.lib
endif

LDOBJS += $(LIB_winop)/$(OUTPATH)/winop.lib \
	  $(LIB_common)/$(OUTPATH)/eu_common.obj

all : $(OUTPATH) $(DLLPROGRAM) copy_to_main_bin

$(DLLPROGRAM) : defmsg.h

copy_to_main_bin : $(DLLPROGRAM)
	-cp $(DLLPROGRAM) ../../$(OUTPATH)

$(DLLPROGRAM) : $(LDOBJS) 

include $(BINGODIR)/m/tail.mak
