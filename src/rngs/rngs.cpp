#include "chugin.h"

#include "rings/dsp/part.h"
#include "rings/dsp/string_synth_part.h"
#include "rings/dsp/strummer.h"

CK_DLL_CTOR(rings_ctor);
CK_DLL_DTOR(rings_dtor);

CK_DLL_MFUN(rings_setPolyphony);
CK_DLL_MFUN(rings_getPolyphony);
CK_DLL_MFUN(rings_setModel);
CK_DLL_MFUN(rings_getModel);
CK_DLL_MFUN(rings_setFrequency);
CK_DLL_MFUN(rings_getFrequency);
CK_DLL_MFUN(rings_setStructure);
CK_DLL_MFUN(rings_getStructure);
CK_DLL_MFUN(rings_setBrightness);
CK_DLL_MFUN(rings_getBrightness);
CK_DLL_MFUN(rings_setDamping);
CK_DLL_MFUN(rings_getDamping);
CK_DLL_MFUN(rings_setPosition);
CK_DLL_MFUN(rings_getPosition);
CK_DLL_MFUN(rings_setEasterEgg);
CK_DLL_MFUN(rings_getEasterEgg);
CK_DLL_MFUN(rings_strum);
CK_DLL_MFUN(rings_setInternalExciter);
CK_DLL_MFUN(rings_getInternalExciter);

CK_DLL_TICKF(rings_tick);

t_CKINT rings_data_offset = 0;

struct RingsData {
	rings::Part part;
	rings::Patch patch;
	rings::PerformanceState performance_state;
	rings::StringSynthPart string_synth;
	rings::Strummer strummer;
	uint16_t* reverb_buffer;
	bool easter_egg;
	float in_level;
};

CK_DLL_QUERY(Rings) {
	QUERY->setname(QUERY, "Rings");

	QUERY->begin_class(QUERY, "Rings", "UGen");

	QUERY->add_ctor(QUERY, rings_ctor);
	QUERY->add_dtor(QUERY, rings_dtor);

	QUERY->add_ugen_funcf(QUERY, rings_tick, NULL, 2, 2);

	QUERY->add_mfun(QUERY, rings_setPolyphony, "void", "polyphony");
	QUERY->add_arg(QUERY, "int", "arg");
	QUERY->add_mfun(QUERY, rings_getPolyphony, "int", "polyphony");

	QUERY->add_mfun(QUERY, rings_setModel, "void", "model");
	QUERY->add_arg(QUERY, "int", "arg");
	QUERY->add_mfun(QUERY, rings_getModel, "int", "model");

	QUERY->add_mfun(QUERY, rings_setFrequency, "void", "frequency");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, rings_getFrequency, "float", "frequency");

	QUERY->add_mfun(QUERY, rings_setStructure, "void", "structure");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, rings_getStructure, "float", "structure");

	QUERY->add_mfun(QUERY, rings_setBrightness, "void", "brightness");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, rings_getBrightness, "float", "brightness");

	QUERY->add_mfun(QUERY, rings_setDamping, "void", "damping");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, rings_getDamping, "float", "damping");

	QUERY->add_mfun(QUERY, rings_setPosition, "void", "position");
	QUERY->add_arg(QUERY, "float", "arg");
	QUERY->add_mfun(QUERY, rings_getPosition, "float", "position");

	QUERY->add_mfun(QUERY, rings_setEasterEgg, "void", "easterEgg");
	QUERY->add_arg(QUERY, "int", "arg");
	QUERY->add_mfun(QUERY, rings_getEasterEgg, "int", "easterEgg");

	QUERY->add_mfun(QUERY, rings_setInternalExciter, "void", "internalExciter");
	QUERY->add_arg(QUERY, "int", "arg");
	QUERY->add_mfun(QUERY, rings_getInternalExciter, "int", "internalExciter");

	QUERY->add_mfun(QUERY, rings_strum, "void", "strum");

	rings_data_offset = QUERY->add_mvar(QUERY, "int", "@rings_data", false);

	QUERY->end_class(QUERY);

	return TRUE;
}

CK_DLL_CTOR(rings_ctor) {
	RingsData* x = new RingsData;
	OBJ_MEMBER_INT(SELF, rings_data_offset) = (t_CKINT)x;

	x->reverb_buffer = new uint16_t[32768]();

	x->part.Init(x->reverb_buffer);
	x->string_synth.Init(x->reverb_buffer);
	x->strummer.Init(0.01, 44100.0 / 24);

	x->part.set_polyphony(1);
	x->part.set_model(rings::RESONATOR_MODEL_MODAL);
	x->string_synth.set_polyphony(1);
	x->string_synth.set_fx(rings::FX_REVERB);

	x->easter_egg = false;
	x->in_level = 0.0f;

	x->patch.structure = 0.5f;
	x->patch.brightness = 0.5f;
	x->patch.damping = 0.5f;
	x->patch.position = 0.5f;

	x->performance_state.note = 48.0f;
	x->performance_state.tonic = 48.0f;
	x->performance_state.fm = 0.0f;
	x->performance_state.internal_exciter = true;
	x->performance_state.internal_strum = false;
	x->performance_state.internal_note = false;
	x->performance_state.strum = false;
	x->performance_state.chord = 0;
}

CK_DLL_DTOR(rings_dtor) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	delete[] x->reverb_buffer;
	delete x;
	OBJ_MEMBER_INT(SELF, rings_data_offset) = 0;
}

CK_DLL_TICKF(rings_tick) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);

	float* input = new float[nframes];
	float* output = new float[nframes * 2];

	for (int i = 0; i < nframes; i++) {
		input[i] = (in[i * 2] + in[i * 2 + 1]) * 0.5f;
	}

	if (x->easter_egg) {
		x->strummer.Process(NULL, nframes, &x->performance_state);
		x->string_synth.Process(x->performance_state, x->patch, input, output, output + nframes,
		                        nframes);
	} else {
		x->strummer.Process(input, nframes, &x->performance_state);
		x->part.Process(x->performance_state, x->patch, input, output, output + nframes, nframes);
	}

	for (int i = 0; i < nframes; i++) {
		out[i * 2] = output[i];
		out[i * 2 + 1] = output[i + nframes];
	}

	delete[] input;
	delete[] output;

	return TRUE;
}

CK_DLL_MFUN(rings_setPolyphony) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->part.set_polyphony(GET_NEXT_INT(ARGS));
	x->string_synth.set_polyphony(GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(rings_setModel) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	int model = GET_NEXT_INT(ARGS);
	x->part.set_model(static_cast<rings::ResonatorModel>(model));
	x->string_synth.set_fx(static_cast<rings::FxType>(model));
}

CK_DLL_MFUN(rings_setFrequency) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->performance_state.note = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(rings_setStructure) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->patch.structure = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(rings_setBrightness) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->patch.brightness = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(rings_setDamping) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->patch.damping = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(rings_setPosition) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->patch.position = GET_NEXT_FLOAT(ARGS);
}

CK_DLL_MFUN(rings_setEasterEgg) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->easter_egg = (GET_NEXT_INT(ARGS) != 0);
}

CK_DLL_MFUN(rings_strum) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->performance_state.strum = true;
}

CK_DLL_MFUN(rings_setInternalExciter) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	x->performance_state.internal_exciter = (GET_NEXT_INT(ARGS) != 0);
}

CK_DLL_MFUN(rings_getPolyphony) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_int = x->part.polyphony();
}

CK_DLL_MFUN(rings_getModel) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_int = static_cast<int>(x->part.model());
}

CK_DLL_MFUN(rings_getFrequency) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_float = x->performance_state.note;
}

CK_DLL_MFUN(rings_getStructure) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_float = x->patch.structure;
}

CK_DLL_MFUN(rings_getBrightness) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_float = x->patch.brightness;
}

CK_DLL_MFUN(rings_getDamping) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_float = x->patch.damping;
}

CK_DLL_MFUN(rings_getPosition) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_float = x->patch.position;
}

CK_DLL_MFUN(rings_getEasterEgg) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_int = x->easter_egg ? 1 : 0;
}

CK_DLL_MFUN(rings_getInternalExciter) {
	RingsData* x = (RingsData*)OBJ_MEMBER_INT(SELF, rings_data_offset);
	RETURN->v_int = x->performance_state.internal_exciter ? 1 : 0;
}
