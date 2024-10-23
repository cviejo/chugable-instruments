CHUGIN_NAME=CldsP

CXX_MODULES=cldsp.cpp

mi=../../parasites

CC_MODULES=$(mi)/stmlib/utils/random.cc \
	$(mi)/stmlib/dsp/atan.cc \
	$(mi)/stmlib/dsp/units.cc \
	$(mi)/clouds/dsp/correlator.cc \
	$(mi)/clouds/dsp/granular_processor.cc \
	$(mi)/clouds/dsp/mu_law.cc \
	$(mi)/clouds/dsp/pvoc/frame_transformation.cc \
	$(mi)/clouds/dsp/pvoc/phase_vocoder.cc \
	$(mi)/clouds/dsp/pvoc/stft.cc \
	$(mi)/clouds/resources.cc

FLAGS+=-I$(mi) \
	-I$(mi)/stmlib \
	-I$(mi)/clouds

include ../common/makefile
