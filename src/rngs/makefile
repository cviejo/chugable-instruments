CHUGIN_NAME=Rngs

mi=../../mutable-intruments/eurorack

CXX_MODULES=rngs.cpp

CC_MODULES=$(mi)/stmlib/utils/random.cc \
	$(mi)/stmlib/dsp/units.cc \
	$(mi)/rings/dsp/fm_voice.cc \
	$(mi)/rings/dsp/part.cc \
	$(mi)/rings/dsp/string_synth_part.cc \
	$(mi)/rings/dsp/str.cc \
	$(mi)/rings/dsp/resonator.cc \
	$(mi)/rings/resources.cc

FLAGS+=-I$(mi) \
	-I$(mi)/stmlib \
	-I$(mi)/rings

include ../common/makefile
