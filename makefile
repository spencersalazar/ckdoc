
CHUCK_SRC_DIR=chuck/src

.PHONY: osx linux-oss linux-jack linux-alsa win32 osx-rl
osx linux-oss linux-jack linux-alsa win32 osx-rl: ckdoc list_ugens list_classes gen_class_css

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

ifneq (,$(strip $(filter osx bin-dist-osx,$(MAKECMDGOALS) $(CK_TARGET))))
include $(CHUCK_SRC_DIR)/makefile.osx
endif

ifneq (,$(strip $(filter linux-oss,$(MAKECMDGOALS) $(CK_TARGET))))
include $(CHUCK_SRC_DIR)/makefile.oss
endif

ifneq (,$(strip $(filter linux-jack,$(MAKECMDGOALS) $(CK_TARGET))))
include $(CHUCK_SRC_DIR)/makefile.jack
endif

ifneq (,$(strip $(filter linux-alsa,$(MAKECMDGOALS) $(CK_TARGET))))
include $(CHUCK_SRC_DIR)/makefile.alsa
endif

ifneq (,$(strip $(filter win32,$(MAKECMDGOALS) $(CK_TARGET))))
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
CXXSRCS+=ckdoc.cpp list_ugens.cpp list_classes.cpp gen_class_css.cpp

COBJS=$(CSRCS:.c=.o)
CXXOBJS=$(CXXSRCS:.cpp=.o)
LO_COBJS=$(LO_CSRCS:.c=.o)
OBJS=$(COBJS) $(CXXOBJS) $(LO_COBJS)

CKDOC_OBJS=$(filter-out list_ugens.o list_classes.o gen_class_css.o,$(OBJS))
LISTUGENS_OBJS=$(filter-out ckdoc.o list_classes.o gen_class_css.o,$(OBJS))
LISTCLASSES_OBJS=$(filter-out list_ugens.o ckdoc.o gen_class_css.o,$(OBJS))
GENCLASSCSS_OBJS=$(filter-out list_ugens.o ckdoc.o list_classes.o,$(OBJS))

# remove -arch options
CFLAGSDEPEND=$(CFLAGS)

-include $(OBJS:.o=.d)

ckdoc: $(CKDOC_OBJS)
	$(LD) -o ckdoc $(CKDOC_OBJS) $(LDFLAGS)

list_ugens: $(LISTUGENS_OBJS)
	$(LD) -o list_ugens $(LISTUGENS_OBJS) $(LDFLAGS)

list_classes: $(LISTCLASSES_OBJS)
	$(LD) -o list_classes $(LISTCLASSES_OBJS) $(LDFLAGS)

gen_class_css: $(GENCLASSCSS_OBJS)
	$(LD) -o gen_class_css $(GENCLASSCSS_OBJS) $(LDFLAGS)

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

DOC_ROOT=doc

STDLIB_TITLE=Standard Classes and Libraries
STDLIB_CLASSES=Object @array string Event Std Machine Math Shred RegEx
STDLIB_FILE=stdlib.html
STDLIB_INDEX=$(foreach CLASS,$(STDLIB_CLASSES),$(CLASS) $(STDLIB_FILE)\n)

UGEN_TITLE=Basic Unit Generators
UGEN_CLASSES=UGen Gain Step Osc Phasor SinOsc TriOsc SawOsc PulseOsc SqrOsc \
    SndBuf SndBuf2 Noise Impulse HalfRect FullRect \
    UGen_Multi UGen_Stereo Mix2 Pan2 Chubgraph Chugen
UGEN_FILE=ugen.html
UGEN_INDEX=$(foreach CLASS,$(UGEN_CLASSES),$(CLASS) $(UGEN_FILE)\n)

FILTERS_TITLE=Filters
FILTERS_CLASSES=FilterBasic BPF BRF LPF HPF ResonZ BiQuad \
    OnePole TwoPole OneZero TwoZero PoleZero FilterStk
FILTERS_FILE=filter.html
FILTERS_INDEX=$(foreach CLASS,$(FILTERS_CLASSES),$(CLASS) $(FILTERS_FILE)\n)

ADVUGEN_TITLE=Advanced Unit Generators
ADVUGEN_CLASSES=LiSa LiSa10 GenX Gen5 Gen7 Gen9 Gen10 Gen17 CurveTable \
    WarpTable CNoise Dyno
ADVUGEN_FILE=advugen.html
ADVUGEN_INDEX=$(foreach CLASS,$(ADVUGEN_CLASSES),$(CLASS) $(ADVUGEN_FILE)\n)

STK_TITLE=Synthesis Toolkit (STK)
STK_CLASSES=Envelope ADSR Delay DelayA DelayL Echo JCRev NRev PRCRev Chorus \
    Modulate PitShift SubNoise BLT Blit BlitSaw BlitSquare WvIn WaveLoop WvOut \
    StkInstrument BandedWG BlowBotl BlowHole Bowed Brass Clarinet Flute \
    Mandolin ModalBar Moog Saxofony Shakers Sitar StifKarp VoicForm \
    FM BeeThree FMVoices HevyMetl PercFlut Rhodey TubeBell Wurley
STK_FILE=stk.html
STK_INDEX=$(foreach CLASS,$(STK_CLASSES),$(CLASS) $(STK_FILE)\n)

UANA_TITLE=Unit Analyzers
UANA_CLASSES=UAna UAnaBlob Windowing FFT IFFT DCT IDCT Centroid Flux RMS RollOff ZeroX Flip pilF FeatureCollector
UANA_FILE=uana.html
UANA_INDEX=$(foreach CLASS,$(UANA_CLASSES),$(CLASS) $(UANA_FILE)\n)

IO_TITLE=Input / Output
IO_CLASSES=IO FileIO StdOut StdErr OscIn OscOut OscMsg Hid HidMsg SerialIO \
    MidiIn MidiOut MidiMsg MidiFileIn
IO_FILE=io.html
IO_INDEX=$(foreach CLASS,$(IO_CLASSES),$(CLASS) $(IO_FILE)\n)

CHUGINS_TITLE=ChuGins
CHUGINS_CLASSES=ABSaturator AmbPan3 Bitcrusher MagicSine KasFilter FIR \
    Pan4 Pan8 Pan16 PitchTrack GVerb Mesh2D Spectacle Elliptic \
    WinFuncEnv PowerADSR FoldbackSaturator WPDiodeLadder WPKorg35
CHUGINS_FILE=chugins.html
CHUGINS_INDEX=$(foreach CLASS,$(CHUGINS_CLASSES),$(CLASS) $(CHUGINS_FILE)\n)

CHUGL_TITLE=ChuGL
CHUGL_CLASSES=chugl chuglImage Pointer OpenGL curve curveExp
CHUGL_FILE=chugl.html
CHUGL_INDEX=$(foreach CLASS,$(CHUGINS_CLASSES),$(CLASS) $(CHUGINS_FILE)\n)

GROUPS=STDLIB_ UGEN_ FILTERS_ ADVUGEN_ STK_ UANA_ IO_ CHUGINS_
GROUPS_INDEX=$(foreach GROUP,$(GROUPS),--group:"$($(GROUP)TITLE)" --url:$($(GROUP)FILE) $($(GROUP)CLASSES))
GROUPS_CLASSINDEX=$(foreach GROUP,$(GROUPS),$($(GROUP)INDEX))

SINGLE_CLASSES=$(foreach GROUP,$(GROUPS),$($(GROUP)CLASSES))
SINGLE_CLASSINDEX=$(foreach CLASS,$(SINGLE_CLASSES),$(CLASS) $(CLASS).html\n)
SINGLE_ROOT=$(DOC_ROOT)/single

docs: ckdoc index $(DOC_ROOT)/class.css class.index $(DOC_ROOT) $(DOC_ROOT)/ckdoc.css $(GROUPS) single
	./ckdoc --title:All > $(DOC_ROOT)/all.html

chugl-doc: ckdoc class.index $(DOC_ROOT) $(DOC_ROOT)/ckdoc.css $(DOC_ROOT)/class.css
	./ckdoc --title:"$(CHUGL_TITLE)" $(CHUGL_CLASSES) > $(DOC_ROOT)/$(CHUGL_FILE)

# test:
# 	@echo $(SINGLE_CLASSES)

$(DOC_ROOT)/class.css: gen_class_css $(DOC_ROOT)
	./gen_class_css > $(DOC_ROOT)/class.css

$(DOC_ROOT):
	mkdir -p $(DOC_ROOT)

$(SINGLE_ROOT): $(DOC_ROOT)
	mkdir -p $(SINGLE_ROOT)

$(DOC_ROOT)/ckdoc.css: ckdoc.css $(DOC_ROOT)
	cp $< $@

$(GROUPS): $(DOC_ROOT) ckdoc
	./ckdoc --title:"$($@TITLE)" $($@CLASSES) > $(DOC_ROOT)/$($@FILE)

index: gen_index $(DOC_ROOT)
	./gen_index --title:"ChucK Class Library Reference" $(GROUPS_INDEX) \
        > $(DOC_ROOT)/index.html

class.index: makefile
	@echo '$(GROUPS_CLASSINDEX)' > class.index



single: $(SINGLE_CLASSES) $(SINGLE_ROOT)
	./gen_class_css > $(SINGLE_ROOT)/class.css
	cp ckdoc.css $(SINGLE_ROOT)/ckdoc.css

$(SINGLE_CLASSES): $(SINGLE_ROOT) ckdoc $(SINGLE_ROOT)/class.index
	./ckdoc --title:"$@" --no-toc --no-heading --index:$(SINGLE_ROOT)/class.index $@ > $(SINGLE_ROOT)/$@.html

$(SINGLE_ROOT)/class.index: makefile $(SINGLE_ROOT)
	@echo '$(SINGLE_CLASSINDEX)' > $(SINGLE_ROOT)/class.index

upload:
	rsync -rave ssh $(DOC_ROOT)/* ccrma-gate.stanford.edu:Library/Web/ckdoc/

clean-docs:
	@rm -rf class.index $(DOC_ROOT)

clean:
	@rm -rf ckdoc *.o *.d $(OBJS) $(patsubst %.o,%.d,$(OBJS)) *~ class.index $(DOC_ROOT)

