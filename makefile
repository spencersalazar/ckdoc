
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


STDLIB_TITLE=Standard Classes and Libraries
STDLIB_CLASSES=Object @array string Event Std Machine Math Shred RegEx
STDLIB_FILE=stdlib.html

UGEN_TITLE=Basic Unit Generators
UGEN_CLASSES=UGen Gain Step Osc Phasor SinOsc TriOsc SawOsc PulseOsc SqrOsc \
    SndBuf SndBuf2 Noise Impulse HalfRect FullRect ZeroX \
    UGen_Multi UGen_Stereo Mix2 Pan2 Chubgraph Chugen
UGEN_FILE=ugen.html

FILTERS_TITLE=Filters
FILTERS_CLASSES=FilterBasic BPF BRF LPF HPF ResonZ BiQuad \
    OnePole TwoPole OneZero TwoZero PoleZero FilterStk
FILTERS_FILE=filter.html

STK_TITLE=Synthesis Toolkit (STK)
STK_CLASSES=Envelope ADSR Delay DelayA DelayL Echo JCRev NRev PRCRev Chorus \
    Modulate PitShift SubNoise Blit BlitSaw BlitSquare WvIn WaveLoop WvOut \
    StkInstrument BandedWG BlowBotl BlowHole Bowed Brass Clarinet Flute \
    Mandolin ModalBar Moog Saxofony Shakers Sitar StifKarp VoicForm \
    FM BeeThree FMVoices HevyMetl PercFlut Rhodey TubeBell Wurley
STK_FILE=stk.html

UANA_TITLE=Unit Analyzers
UANA_CLASSES=UAna UAnaBlob Windowing FFT IFFT DCT IDCT Centroid Flux RMS RollOff
UANA_FILE=uana.html

IO_TITLE=Input / Output
IO_CLASSES=IO FileIO StdOut StdErr OscIn OscOut OscMsg Hid HidMsg SerialIO \
    MidiIn MidiOut MidiMsg MidiFileIn
IO_FILE=io.html

CHUGINS_TITLE=ChuGins
CHUGINS_CLASSES=ABSaturator AmbPan3 Bitcrusher MagicSine KasFilter FIR \
    Pan4 Pan8 Pan16 PitchTrack GVerb Mesh2D Spectacle Elliptic
CHUGINS_FILE=chugins.html

docs: ckdoc index
	./ckdoc --title:"$(STDLIB_TITLE)" $(STDLIB_CLASSES) > $(STDLIB_FILE)
	./ckdoc --title:"$(UGEN_TITLE)" $(UGEN_CLASSES) > $(UGEN_FILE) 
	./ckdoc --title:"$(FILTERS_TITLE)" $(FILTERS_CLASSES) > $(FILTERS_FILE)
	./ckdoc --title:"$(STK_TITLE)" $(STK_CLASSES) > $(STK_FILE)
	./ckdoc --title:"$(UANA_TITLE)" $(UANA_CLASSES) > $(UANA_FILE)
	./ckdoc --title:"$(IO_TITLE)" $(IO_CLASSES) > $(IO_FILE)
	./ckdoc --title:"$(CHUGINS_TITLE)" $(CHUGINS_CLASSES) > $(CHUGINS_FILE)

index:
	./gen_index --title:"ChucK Class Library Reference" \
        --group:"$(STDLIB_TITLE)" --url:$(STDLIB_FILE) $(STDLIB_CLASSES) \
        --group:"$(UGEN_TITLE)" --url:$(UGEN_FILE) --cssclass:ugenname $(UGEN_CLASSES) \
        --group:"$(FILTERS_TITLE)" --url:$(FILTERS_FILE) --cssclass:ugenname $(FILTERS_CLASSES) \
        --group:"$(STK_TITLE)" --url:$(STK_FILE) --cssclass:ugenname $(STK_CLASSES) \
        --group:"$(UANA_TITLE)" --url:$(UANA_FILE) --cssclass:ugenname $(UANA_CLASSES) \
        --group:"$(IO_TITLE)" --url:$(IO_FILE) $(IO_CLASSES) \
        --group:"$(CHUGINS_TITLE)" --url:$(CHUGINS_FILE) --cssclass:ugenname $(CHUGINS_CLASSES) \
        > index.html

clean: 
	@rm -f ckdoc *.o *.d $(OBJS) $(patsubst %.o,%.d,$(OBJS)) *~

