#include <iostream>

#include "chugin.h"

#include "clouds/dsp/granular_processor.h"

CK_DLL_CTOR(clds_ctor);
CK_DLL_DTOR(clds_dtor);
CK_DLL_MFUN(clds_setPosition);
CK_DLL_MFUN(clds_getPosition);
CK_DLL_MFUN(clds_setSize);
CK_DLL_MFUN(clds_getSize);
CK_DLL_MFUN(clds_setPitch);
CK_DLL_MFUN(clds_getPitch);
CK_DLL_MFUN(clds_setDensity);
CK_DLL_MFUN(clds_getDensity);
CK_DLL_MFUN(clds_setTexture);
CK_DLL_MFUN(clds_getTexture);
CK_DLL_MFUN(clds_setBlend);
CK_DLL_MFUN(clds_getBlend);
CK_DLL_MFUN(clds_setSpread);
CK_DLL_MFUN(clds_getSpread);
CK_DLL_MFUN(clds_setFeedback);
CK_DLL_MFUN(clds_getFeedback);
CK_DLL_MFUN(clds_setReverb);
CK_DLL_MFUN(clds_getReverb);
CK_DLL_MFUN(clds_setFreeze);
CK_DLL_MFUN(clds_getFreeze);
CK_DLL_MFUN(clds_setMode);
CK_DLL_MFUN(clds_getMode);

CK_DLL_TICKF(clds_tick);

t_CKINT clds_data_offset = 0;

struct CldsData {
	clouds::GranularProcessor* processor;
	uint8_t* block_mem;
	uint8_t* block_ccm;
	clouds::PlaybackMode playback;
	int quality;
	bool freeze;
	int blendMode;
};

CK_DLL_QUERY(Clds) {
	QUERY->setname(QUERY, "Clds");

	QUERY->begin_class(QUERY, "Clds", "UGen");

	QUERY->add_ctor(QUERY, clds_ctor);
	QUERY->add_dtor(QUERY, clds_dtor);

	QUERY->add_ugen_funcf(QUERY, clds_tick, NULL, 1, 2);

	QUERY->add_mfun(QUERY, clds_setPosition, "float", "position");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getPosition, "float", "position");

	QUERY->add_mfun(QUERY, clds_setSize, "float", "size");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getSize, "float", "size");

	QUERY->add_mfun(QUERY, clds_setPitch, "float", "pitch");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getPitch, "float", "pitch");

	QUERY->add_mfun(QUERY, clds_setDensity, "float", "density");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getDensity, "float", "density");

	QUERY->add_mfun(QUERY, clds_setTexture, "float", "texture");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getTexture, "float", "texture");

	QUERY->add_mfun(QUERY, clds_setBlend, "float", "blend");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getBlend, "float", "blend");

	QUERY->add_mfun(QUERY, clds_setSpread, "float", "spread");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getSpread, "float", "spread");

	QUERY->add_mfun(QUERY, clds_setFeedback, "float", "feedback");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getFeedback, "float", "feedback");

	QUERY->add_mfun(QUERY, clds_setReverb, "float", "reverb");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, clds_getReverb, "float", "reverb");

	QUERY->add_mfun(QUERY, clds_setFreeze, "int", "freeze");
	QUERY->add_arg(QUERY, "int", "arg");
	QUERY->add_mfun(QUERY, clds_getFreeze, "int", "freeze");

	QUERY->add_mfun(QUERY, clds_setMode, "int", "mode");
	QUERY->add_arg(QUERY, "int", "arg");
	QUERY->add_mfun(QUERY, clds_getMode, "int", "mode");

	clds_data_offset = QUERY->add_mvar(QUERY, "int", "@clouds_data", false);

	QUERY->end_class(QUERY);

	return TRUE;
}

CK_DLL_CTOR(clds_ctor) {
	CldsData* x = new CldsData;
	OBJ_MEMBER_INT(SELF, clds_data_offset) = (t_CKINT)x;

	const int memLen = 118784;
	const int ccmLen = 65536 - 128;

	x->block_mem = new uint8_t[memLen]();
	x->block_ccm = new uint8_t[ccmLen]();
	x->processor = new clouds::GranularProcessor();
	x->freeze = false;
	x->blendMode = 0;
	x->playback = clouds::PLAYBACK_MODE_GRANULAR;
	x->quality = 0;

	memset(x->processor, 0, sizeof(*x->processor));

	x->processor->Init(x->block_mem, memLen, x->block_ccm, ccmLen);

	x->processor->set_bypass(false);
	x->processor->set_low_fidelity(false);
	x->processor->set_num_channels(2);
	x->processor->set_playback_mode(x->playback);
	x->processor->set_silence(false);

	x->processor->mutable_parameters()->density = 1.0f;
	x->processor->mutable_parameters()->dry_wet = 1.0f;
	x->processor->mutable_parameters()->feedback = 0.0f;
	x->processor->mutable_parameters()->freeze = false;
	x->processor->mutable_parameters()->pitch = -12.0f;
	x->processor->mutable_parameters()->position = 0.5f;
	x->processor->mutable_parameters()->reverb = 0.0f;
	x->processor->mutable_parameters()->size = 0.5f;
	x->processor->mutable_parameters()->stereo_spread = 0.5f;
	x->processor->mutable_parameters()->texture = 0.5f;
}

CK_DLL_DTOR(clds_dtor) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	delete x->processor;
	delete[] x->block_mem;
	delete[] x->block_ccm;
	delete x;
	OBJ_MEMBER_INT(SELF, clds_data_offset) = 0;
}

CK_DLL_TICKF(clds_tick) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);

	clouds::ShortFrame input[nframes];
	clouds::ShortFrame output[nframes];

	for (int i = 0; i < nframes; i++) {
		input[i].l = input[i].r = (short)(in[i] * 32767.0f);
	}

	x->processor->Prepare();
	x->processor->Process(input, output, nframes);

	for (int i = 0; i < nframes; i++) {
		out[i * 2] = output[i].l / 32768.0f;
		out[i * 2 + 1] = output[i].r / 32768.0f;
	}

	return TRUE;
}

CK_DLL_MFUN(clds_setPosition) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->position = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setSize) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->size = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setPitch) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->pitch = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setDensity) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->density = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setTexture) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->texture = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setBlend) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->dry_wet = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setSpread) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->stereo_spread = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setFeedback) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->feedback = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setReverb) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	float value = GET_NEXT_FLOAT(ARGS);
	x->processor->mutable_parameters()->reverb = value;
	RETURN->v_float = value;
}

CK_DLL_MFUN(clds_setFreeze) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	int value = GET_NEXT_INT(ARGS);
	x->freeze = (value != 0);
	x->processor->mutable_parameters()->freeze = x->freeze;
	RETURN->v_int = value;
}

CK_DLL_MFUN(clds_setMode) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	int value = GET_NEXT_INT(ARGS);
	x->playback = static_cast<clouds::PlaybackMode>(value % clouds::PLAYBACK_MODE_LAST);
	x->processor->set_playback_mode(x->playback);
	RETURN->v_int = value;
}

CK_DLL_MFUN(clds_getPosition) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->position;
}

CK_DLL_MFUN(clds_getSize) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->size;
}

CK_DLL_MFUN(clds_getPitch) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->pitch;
}

CK_DLL_MFUN(clds_getDensity) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->density;
}

CK_DLL_MFUN(clds_getTexture) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->texture;
}

CK_DLL_MFUN(clds_getBlend) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->dry_wet;
}

CK_DLL_MFUN(clds_getSpread) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->stereo_spread;
}

CK_DLL_MFUN(clds_getFeedback) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->feedback;
}

CK_DLL_MFUN(clds_getReverb) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_float = x->processor->mutable_parameters()->reverb;
}

CK_DLL_MFUN(clds_getFreeze) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_int = x->freeze ? 1 : 0;
}

CK_DLL_MFUN(clds_getMode) {
	CldsData* x = (CldsData*)OBJ_MEMBER_INT(SELF, clds_data_offset);
	RETURN->v_int = static_cast<int>(x->playback);
}
