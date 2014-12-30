/**
	midi-trigger.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef MIDI_TRIGGER_H
#define MIDI_TRIGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"

#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/time/time.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"

#include "lv2/lv2plug.in/ns/ext/urid/urid.h"

#include "uris.h"

#define PLUGIN_URI "https://github.com/metachronica/audio-dsp-midi-trigger"

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)


typedef enum {
	input_audio = 0,
	output_midi = 1,
	input_gain = 2,
	buffer = 3,
	gap = 4,
	threshold = 5,
	midi_note = 6,
	velocity_floor = 7,
	velocity_ceiling = 8
} PortIndex;


typedef struct {
	const float* input_audio;
	LV2_Atom_Sequence* output_midi;
	const float* input_gain;
	const float* buffer;
	const float* gap;
	const float* threshold;
	const float* midi_note;
	const float* velocity_floor;
	const float* velocity_ceiling;
} Channels;


typedef struct {
	LV2_URID_Map* map; // features
	PluginURIs uris; // URIs
	Channels channels;
	float** detector_buffer; // array of samples
	uint32_t detector_buf_size; // size in samples
	uint32_t detector_counter; // samples counter
	uint32_t gap_counter; // samples counter
} Plugin;

static LV2_Handle instantiate (
	const LV2_Descriptor *descriptor,
	double rate,
	const char *bundle_path,
	const LV2_Feature* const* features);

static void connect_port(LV2_Handle instance, uint32_t port, void* data);
static void run(LV2_Handle instance, uint32_t n_samples);
static void cleanup(LV2_Handle instance);

#endif // MIDI_TRIGGER_H
