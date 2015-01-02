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
#include "rms.h"

#define PLUGIN_URI "https://github.com/metachronica/audio-dsp-midi-trigger"

#define DETECTOR_BUFFER_MAX 200 // ms
#define DETECTOR_GAP_MAX 2000 // ms

/** Define a macro for converting a gain in dB to a coefficient */
#define DB_CO(g) ((g) > -90.0f ? powf(10.0f, (g) * 0.05f) : 0.0f)
#define CO_DB(c) (log10f(c) / 0.05f)

#define SAMPLES_IN_MS(MS, SR) (round((SR / 1000.0f) * MS))
#define LEN(MIN, MAX) (MAX - MIN)


typedef enum {
	input_audio = 0,
	output_midi = 1,
	input_gain = 2,
	detector_buffer = 3,
	detector_gap = 4,
	threshold = 5,
	midi_note = 6,
	note_off_mode = 7,
	note_off_delay = 8,
	velocity_floor = 9,
	velocity_ceiling = 10,
	limit_rms_velocity_ceiling = 11
} PortIndex;


typedef struct {
	const float* input_audio;
	LV2_Atom_Sequence* output_midi;
	const float* input_gain;
	const float* detector_buffer;
	const float* detector_gap;
	const float* threshold;
	const float* midi_note;
	const float* velocity_floor;
	const float* velocity_ceiling;
	const float* limit_rms_velocity_ceiling;

	/**
	 * 1 - Send NOTE-OFF immediately before NOTE-ON;
	 * 2 - Send NOTE-OFF immediately after NOTE-ON;
	 * 3 - Send NOTE-OFF after "note-off delay" or early if it interrupted by
	 *     other NOTE-ON;
	 * 4 - Send NOTE-OFF after "note-off delay" only if it not interrupted by
	 *     other NOTE-ON;
	 * 5 - Don't send NOTE-OFF... NEVER!!!
	 */
	const float* note_off_mode;

	/** Only for 3 or 4 modes of "note off mode" */
	const float* note_off_delay;
} Channels;


typedef struct {
	LV2_URID_Map* map; // features
	PluginURIs uris; // URIs
	Channels channels;
	uint32_t SR;
	float* detector_buffer; // array of samples
	uint32_t detector_buf_size; // size in samples
	uint32_t detector_counter; // samples counter
	uint32_t detector_gap_counter; // samples counter
	bool is_gap_active; // waiting flag, TODO do stuff without this flag

	// note on to next buffer
	bool next_buf_note_on;
	float next_buf_rms_dB;
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
