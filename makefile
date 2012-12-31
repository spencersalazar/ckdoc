
CHUCK_SRC_DIR=chuck/src

.PHONY: osx linux-oss linux-jack linux-alsa win32 osx-rl
osx linux-oss linux-jack linux-alsa win32 osx-rl: ckdoc

ifneq ($(CK_TARGET),)
.DEFAULT_GOAL:=$(CK_TARGET)
ifeq ($(MAKECMDGOALS),)
MAKECMDGOALS:=$(.DEFAULT_GOAL)
endif
endif

LEX=flex
YACC=bison
CC=gcc
CXX=gcc
LD=g++

CFLAGS+= -I$(CHUCK_SRC_DIR)

ifneq ($(CHUCK_DEBUG),)
CFLAGS+= -g
else
CFLAGS+= -O3
endif

ifneq (,$(strip $(filter osx bin-dist-osx,$(MAKECMDGOALS))))
include $(CHUCK_SRC_DIR)/makefile.osx
endif

ifneq (,$(strip $(filter linux-oss,$(MAKECMDGOALS))))
include $(CHUCK_SRC_DIR)/makefile.oss
endif

ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS))))
include $(CHUCK_SRC_DIR)/makefile.jack
endif

ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS))))
include $(CHUCK_SRC_DIR)/makefile.alsa
endif

ifneq (,$(strip $(filter win32,$(MAKECMDGOALS))))
include $(CHUCK_SRC_DIR)/makefile.win32
endif

CSRCS+= chuck.tab.c chuck.yy.c util_math.c util_network.c util_raw.c \
util_xforms.c
CXXSRCS+= chuck_absyn.cpp chuck_parse.cpp chuck_errmsg.cpp \
chuck_frame.cpp chuck_symbol.cpp chuck_table.cpp chuck_utils.cpp \
chuck_vm.cpp chuck_instr.cpp chuck_scan.cpp chuck_type.cpp chuck_emit.cpp \
chuck_compile.cpp chuck_dl.cpp chuck_oo.cpp chuck_lang.cpp chuck_ugen.cpp \
chuck_otf.cpp chuck_stats.cpp chuck_bbq.cpp chuck_shell.cpp chuck_console.cpp \
chuck_globals.cpp \
digiio_rtaudio.cpp hidio_sdl.cpp midiio_rtmidi.cpp RtAudio/RtAudio.cpp \
rtmidi.cpp ugen_osc.cpp ugen_filter.cpp ugen_stk.cpp ugen_xxx.cpp \
ulib_machine.cpp ulib_math.cpp ulib_std.cpp ulib_opsc.cpp util_buffers.cpp \
util_console.cpp util_string.cpp util_thread.cpp util_opsc.cpp util_hid.cpp \
uana_xform.cpp uana_extract.cpp

CSRCS:=$(addprefix $(CHUCK_SRC_DIR)/,$(CSRCS))
CXXSRCS:=$(addprefix $(CHUCK_SRC_DIR)/,$(CXXSRCS))

CXXSRCS+=ckdoc.cpp

COBJS=$(CSRCS:.c=.o)
CXXOBJS=$(CXXSRCS:.cpp=.o)
OBJS=$(COBJS) $(CXXOBJS)

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

-include $(OBJS:.o=.d)

ckdoc: $(OBJS)
	$(LD) -o ckdoc $(OBJS) $(LDFLAGS)

chuck.tab.c chuck.tab.h: $(CHUCK_SRC_DIR)/chuck.y
	$(YACC) -dv -b $(CHUCK_SRC_DIR)/chuck $(CHUCK_SRC_DIR)/chuck.y

chuck.yy.c: chuck.lex
	$(LEX) -o$(CHUCK_SRC_DIR)/chuck.yy.c $(CHUCK_SRC_DIR)/chuck.lex

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM -MT "$@" $(CFLAGSDEPEND) $< > $*.d

$(CXXOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
	@$(CXX) -MM -MT "$@" $(CFLAGSDEPEND) $< > $*.d

clean: 
	@rm -f ckdoc *.o *.d $(OBJS) $(patsubst %.o,%.d,$(OBJS)) *~

