#include "chugin.h"
#include "rings/dsp/onset_detector.h"

CK_DLL_CTOR(onsetdetector_ctor);
CK_DLL_DTOR(onsetdetector_dtor);
CK_DLL_TICKF(onsetdetector_tick);
CK_DLL_MFUN(onsetdetector_setParams);

t_CKINT offsetEvent = 0;
t_CKINT offsetData = 0;

static CBufferSimple* eventBuffer = NULL;

struct OnsetDetectorData {
	rings::OnsetDetector detector;
	Chuck_Event* event;
	Chuck_VM* vm;
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

	offsetEvent = QUERY->add_mvar(QUERY, "Event", "event", false);
	offsetData = QUERY->add_mvar(QUERY, "int", "@data", false);

	QUERY->end_class(QUERY);

	return TRUE;
}

CK_DLL_CTOR(onsetdetector_ctor) {
	if (eventBuffer == NULL) {
		eventBuffer = eventBuffer = API->vm->create_event_buffer(VM);
	}

	OnsetDetectorData* data = new OnsetDetectorData;
	float srate = API->vm->srate(VM);
	data->detector.Init(8.0f / srate, 160.0f / srate, 1600.0f / srate, srate, 0.005f);
	data->vm = VM;
	OBJ_MEMBER_INT(SELF, offsetData) = (t_CKINT)data;

	Chuck_Type* eventType = API->type->lookup(VM, "Event");
	Chuck_Object* event = API->object->create(SHRED, eventType, true);
	data->event = (Chuck_Event*)event;
	OBJ_MEMBER_INT(SELF, offsetEvent) = (t_CKINT)event;
}

CK_DLL_DTOR(onsetdetector_dtor) {
	OnsetDetectorData* data = (OnsetDetectorData*)OBJ_MEMBER_INT(SELF, offsetData);

	if (data) {
		delete data;
		OBJ_MEMBER_INT(SELF, offsetData) = 0;
	}

	// TODO: cleanup buffer and event
	// can't figure out how to do this properly via chugin.h,
	// destroy_event_buffer is not there and CK_SAFE_ADD_REF is not working.
	// buffer cleanup is not that critical since it's only created once.
}

CK_DLL_MFUN(onsetdetector_setParams) {
	OnsetDetectorData* data = (OnsetDetectorData*)OBJ_MEMBER_INT(SELF, offsetData);

	float low = GET_NEXT_FLOAT(ARGS);
	float lowMid = GET_NEXT_FLOAT(ARGS);
	float midHight = GET_NEXT_FLOAT(ARGS);
	float decimatedSampleRate = GET_NEXT_FLOAT(ARGS);
	float ioiTime = GET_NEXT_FLOAT(ARGS);

	data->detector.Init(low, lowMid, midHight, decimatedSampleRate, ioiTime);
}

CK_DLL_TICKF(onsetdetector_tick) {
	OnsetDetectorData* data = (OnsetDetectorData*)OBJ_MEMBER_INT(SELF, offsetData);

	bool onsetDetected = data->detector.Process(in, nframes);

	if (onsetDetected && data->event) {
		API->vm->queue_event(data->vm, data->event, 1, eventBuffer);
	}

	float value = onsetDetected ? 1.0f : 0.0f;

	for (int i = 0; i < nframes; i++) {
		out[i] = value;
	}

	return TRUE;
}
