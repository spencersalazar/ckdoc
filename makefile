

CFLAGS+= -D__MACOSX_CORE__
CFLAGSDEPEND+= -D__MACOSX_CORE__

FRAMEWORKS+= CoreAudio CoreMIDI CoreFoundation IOKit Carbon AppKit Foundation
LDFLAGS+= $(addprefix -framework ,$(FRAMEWORKS)) -lstdc++ -lm

CC=clang
CXX=clang++
LD=clang++

C_FILES=
C_OBJS=$(addsuffix $(basename C_FILES))

CXX_FILES=ckdoc.cpp
CXX_OBJS=$(addsuffix $(basename CXX_FILES))

CHUCK_SRC_DIR=../chuck/trunk/src

CHUCK_CSRCS= $(CHUCK_SRC_DIR)/util_math.c $(CHUCK_SRC_DIR)/util_network.c \
    $(CHUCK_SRC_DIR)/util_raw.c $(CHUCK_SRC_DIR)/util_xforms.c \
    $(CHUCK_SRC_DIR)/util_sndfile.c

CHUCK_CXXSRCS= $(CHUCK_SRC_DIR)/chuck_absyn.cpp $(CHUCK_SRC_DIR)/chuck_parse.cpp \
	$(CHUCK_SRC_DIR)/chuck_errmsg.cpp $(CHUCK_SRC_DIR)/chuck_frame.cpp \
	$(CHUCK_SRC_DIR)/chuck_symbol.cpp $(CHUCK_SRC_DIR)/chuck_table.cpp \
	$(CHUCK_SRC_DIR)/chuck_utils.cpp $(CHUCK_SRC_DIR)/chuck_vm.cpp \
	$(CHUCK_SRC_DIR)/chuck_instr.cpp $(CHUCK_SRC_DIR)/chuck_scan.cpp \
	$(CHUCK_SRC_DIR)/chuck_type.cpp $(CHUCK_SRC_DIR)/chuck_emit.cpp \
	$(CHUCK_SRC_DIR)/chuck_compile.cpp $(CHUCK_SRC_DIR)/chuck_dl.cpp \
	$(CHUCK_SRC_DIR)/chuck_oo.cpp $(CHUCK_SRC_DIR)/chuck_lang.cpp \
	$(CHUCK_SRC_DIR)/chuck_ugen.cpp $(CHUCK_SRC_DIR)/chuck_globals.cpp \
	$(CHUCK_SRC_DIR)/chuck_otf.cpp $(CHUCK_SRC_DIR)/chuck_stats.cpp \
	$(CHUCK_SRC_DIR)/chuck_bbq.cpp $(CHUCK_SRC_DIR)/chuck_shell.cpp \
	$(CHUCK_SRC_DIR)/chuck_console.cpp $(CHUCK_SRC_DIR)/digiio_rtaudio.cpp \
	$(CHUCK_SRC_DIR)/hidio_sdl.cpp \
	$(CHUCK_SRC_DIR)/midiio_rtmidi.cpp $(CHUCK_SRC_DIR)/RtAudio/RtAudio.cpp \
	$(CHUCK_SRC_DIR)/rtmidi.cpp $(CHUCK_SRC_DIR)/ugen_osc.cpp \
	$(CHUCK_SRC_DIR)/ugen_filter.cpp $(CHUCK_SRC_DIR)/ugen_stk.cpp \
	$(CHUCK_SRC_DIR)/ugen_xxx.cpp \
	$(CHUCK_SRC_DIR)/uana_extract.cpp $(CHUCK_SRC_DIR)/uana_xform.cpp \
	$(CHUCK_SRC_DIR)/ulib_machine.cpp $(CHUCK_SRC_DIR)/ulib_math.cpp \
	$(CHUCK_SRC_DIR)/ulib_std.cpp $(CHUCK_SRC_DIR)/ulib_opsc.cpp \
	$(CHUCK_SRC_DIR)/util_buffers.cpp $(CHUCK_SRC_DIR)/util_console.cpp \
	$(CHUCK_SRC_DIR)/util_string.cpp \
	$(CHUCK_SRC_DIR)/util_thread.cpp \
	$(CHUCK_SRC_DIR)/util_opsc.cpp $(CHUCK_SRC_DIR)/util_hid.cpp

CHUCK_COBJS=$(CHUCK_CSRCS:.c=.o)
CHUCK_CXXOBJS=$(CHUCK_CXXSRCS:.cpp=.o)
CHUCK_OBJS+=$(CHUCK_COBJS) $(CHUCK_CXXOBJS) \
    $(CHUCK_SRC_DIR)/chuck.tab.o $(CHUCK_SRC_DIR)/chuck.yy.o

OBJS=$(COBJS) $(CXXOBJS)

-include $(OBJS:.o=.d)

ckdoc: $(OBJS) $(CHUCK_OBJS)
	$(LD) -o ckdoc $(OBJS) $(CHUCK_OBJS) $(LDFLAGS)

$(CHUCK_OBJS): chuck

.PHONY: chuck
chuck:
	CFLAGS=__ALTER_ENTRY_POINT__ make -C $(CHUCK_SRC_DIR)

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CC) -MM $(CFLAGSDEPEND) $< > $*.d

$(CXXOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $(ARCHOPTS) -c $< -o $@
	@$(CXX) -MM $(CFLAGSDEPEND) $< > $*.d

clean: 
	@rm -f ckdoc *.o *.d $(OBJS) $(patsubst %.o,%.d,$(OBJS)) *~

