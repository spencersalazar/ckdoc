
CHUCK_SRC_DIR=chuck/src

.PHONY: osx linux-oss linux-jack linux-alsa win32 osx-rl
osx linux-oss linux-jack linux-alsa win32 osx-rl: ckdoc list_ugens list_classes

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

CFLAGS+= -I$(CHUCK_SRC_DIR) -DCK_DOC

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
	chuck_main.cpp chuck_otf.cpp chuck_stats.cpp chuck_bbq.cpp chuck_shell.cpp \
	chuck_console.cpp chuck_globals.cpp chuck_io.cpp \
    digiio_rtaudio.cpp hidio_sdl.cpp \
	midiio_rtmidi.cpp RtAudio/RtAudio.cpp rtmidi.cpp ugen_osc.cpp ugen_filter.cpp \
	ugen_stk.cpp ugen_xxx.cpp ulib_machine.cpp ulib_math.cpp ulib_std.cpp \
	ulib_opsc.cpp ulib_regex.cpp util_buffers.cpp util_console.cpp \
	util_string.cpp util_thread.cpp util_opsc.cpp util_serial.cpp \
	util_hid.cpp uana_xform.cpp uana_extract.cpp
LO_CSRCS+= lo/address.c lo/blob.c lo/bundle.c lo/message.c lo/method.c \
    lo/pattern_match.c lo/send.c lo/server.c lo/server_thread.c lo/timetag.c

LO_CFLAGS=-DHAVE_CONFIG_H -I.

CFLAGSDEPEND=$(CFLAGS)

ifneq (,$(ARCHS))
ARCHOPTS=$(addprefix -arch ,$(ARCHS))
else
ARCHOPTS=
endif

CSRCS:=$(addprefix $(CHUCK_SRC_DIR)/,$(CSRCS))
CXXSRCS:=$(addprefix $(CHUCK_SRC_DIR)/,$(CXXSRCS))
CXXSRCS:=$(filter-out $(CHUCK_SRC_DIR)/chuck_main.cpp,$(CXXSRCS))
LO_CSRCS:=$(addprefix $(CHUCK_SRC_DIR)/,$(LO_CSRCS))
CXXSRCS+=ckdoc.cpp list_ugens.cpp list_classes.cpp

COBJS=$(CSRCS:.c=.o)
CXXOBJS=$(CXXSRCS:.cpp=.o)
LO_COBJS=$(LO_CSRCS:.c=.o)
OBJS=$(COBJS) $(CXXOBJS) $(LO_COBJS)

CKDOC_OBJS=$(filter-out list_ugens.o list_classes.o,$(OBJS))
LISTUGENS_OBJS=$(filter-out ckdoc.o list_classes.o,$(OBJS))
LISTCLASSES_OBJS=$(filter-out list_ugens.o ckdoc.o,$(OBJS))

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

-include $(OBJS:.o=.d)

ckdoc: $(CKDOC_OBJS)
	$(LD) -o ckdoc $(CKDOC_OBJS) $(LDFLAGS)

list_ugens: $(LISTUGENS_OBJS)
	$(LD) -o list_ugens $(LISTUGENS_OBJS) $(LDFLAGS)

list_classes: $(LISTCLASSES_OBJS)
	$(LD) -o list_classes $(LISTCLASSES_OBJS) $(LDFLAGS)

$(CHUCK_SRC_DIR)/chuck.tab.c $(CHUCK_SRC_DIR)/chuck.tab.h: $(CHUCK_SRC_DIR)/chuck.y
	$(YACC) -dv -b $(CHUCK_SRC_DIR)/chuck $(CHUCK_SRC_DIR)/chuck.y

$(CHUCK_SRC_DIR)/chuck.yy.c: $(CHUCK_SRC_DIR)/chuck.lex
	$(LEX) -o$(CHUCK_SRC_DIR)/chuck.yy.c $(CHUCK_SRC_DIR)/chuck.lex

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) -MM -MT "$@" $(CFLAGSDEPEND) $< > $*.d

$(LO_COBJS): %.o: %.c
	$(CC) $(CFLAGS) $(LO_CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM -MQ "$@" $(CFLAGSDEPEND) $(LO_CFLAGS) $< > $*.d

$(CXXOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
	@$(CXX) -MM -MT "$@" $(CFLAGSDEPEND) $< > $*.d

clean: 
	@rm -f ckdoc *.o *.d $(OBJS) $(patsubst %.o,%.d,$(OBJS)) *~

