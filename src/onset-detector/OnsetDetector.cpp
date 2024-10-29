#include "chugin.h"

#include "rings/dsp/onset_detector.h"

CK_DLL_CTOR(onsetdetector_ctor);
CK_DLL_DTOR(onsetdetector_dtor);
CK_DLL_TICKF(onsetdetector_tick);

CK_DLL_MFUN(onsetdetector_setParams);

t_CKINT dataOffset = 0;

struct OnsetDetectorData {
	rings::OnsetDetector detector;
};

CK_DLL_QUERY(OnsetDetector) {
	QUERY->setname(QUERY, "OnsetDetector");

	QUERY->begin_class(QUERY, "OnsetDetector", "UGen");

	QUERY->add_ctor(QUERY, onsetdetector_ctor);
	QUERY->add_dtor(QUERY, onsetdetector_dtor);

	QUERY->add_ugen_funcf(QUERY, onsetdetector_tick, NULL, 1, 2);

	QUERY->add_mfun(QUERY, onsetdetector_setParams, "void", "setParams");
	QUERY->add_arg(QUERY, "float", "low");
	QUERY->add_arg(QUERY, "float", "lowMid");
	QUERY->add_arg(QUERY, "float", "midHigh");
	QUERY->add_arg(QUERY, "float", "decimatedSR");
	QUERY->add_arg(QUERY, "float", "ioiTime");

	dataOffset = QUERY->add_mvar(QUERY, "int", "@od_data", false);

	QUERY->end_class(QUERY);

	return TRUE;
}

CK_DLL_CTOR(onsetdetector_ctor) {
	OBJ_MEMBER_INT(SELF, dataOffset) = 0;

	OnsetDetectorData* data = new OnsetDetectorData;

	float srate = API->vm->srate(VM);

	data->detector.Init(8.0f / srate, 160.0f / srate, 1600.0f / srate, srate, 0.005f);

	OBJ_MEMBER_INT(SELF, dataOffset) = (t_CKINT)data;
}

CK_DLL_DTOR(onsetdetector_dtor) {
	OnsetDetectorData* data = (OnsetDetectorData*)OBJ_MEMBER_INT(SELF, dataOffset);

	if (data) {
		delete data;
		OBJ_MEMBER_INT(SELF, dataOffset) = 0;
	}
}

CK_DLL_MFUN(onsetdetector_setParams) {
	OnsetDetectorData* data = (OnsetDetectorData*)OBJ_MEMBER_INT(SELF, dataOffset);

	float low = GET_NEXT_FLOAT(ARGS);
	float lowMid = GET_NEXT_FLOAT(ARGS);
	float midHight = GET_NEXT_FLOAT(ARGS);
	float decimatedSampleRate = GET_NEXT_FLOAT(ARGS);
	float ioiTime = GET_NEXT_FLOAT(ARGS);

	data->detector.Init(low, lowMid, midHight, decimatedSampleRate, ioiTime);
}

CK_DLL_TICKF(onsetdetector_tick) {
	OnsetDetectorData* data = (OnsetDetectorData*)OBJ_MEMBER_INT(SELF, dataOffset);

	bool onsetDetected = data->detector.Process(in, nframes);

	for (int i = 0; i < nframes; i++) {
		out[i] = onsetDetected ? 1.0f : 0.0f;
	}

	return TRUE;
}
