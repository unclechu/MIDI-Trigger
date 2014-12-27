/**
	midi-trigger.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include <stdio.h>
#include <stdlib.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"

#define URI "https://github.com/metachronica/audio-dsp-midi-trigger"

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)


typedef enum {
	input_audio = 0,
	output_midi = 1,
	input_gain = 2,
	buffer = 3,
	gap = 4
} PortIndex;


typedef struct {
	const float* input_audio;
	float* output_midi;
	const float* gap;
	const float* buffer;
	const float* input_gain;
} Channels;


static LV2_Handle instantiate (
	const LV2_Descriptor *descriptor,
	double rate,
	const char *bundle_path,
	const LV2_Feature* const* features
);

static void connect_port(LV2_Handle instance, uint32_t port, void* data);
static void run(LV2_Handle instance, uint32_t n_samples);
static void cleanup(LV2_Handle instance);
