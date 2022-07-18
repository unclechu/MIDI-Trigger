/**
	types.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/unclechu/MIDI-Trigger/blob/master/LICENSE
*/

#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <stdbool.h>

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

#include "uris.h"

#define DETECTOR_BUFFER_MAX 200 // ms
#define NOTE_OFF_MODE_MIN 1
#define NOTE_OFF_MODE_MAX 5

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

	uint8_t last_midi_note;
	uint8_t last_note_off_mode;

	// note to next buffer (for send note after last sample in buffer)
	LV2_Midi_Message_Type next_buf_note;
	float next_buf_rms_dB;

	// delayed note off
	uint32_t note_off_delay_counter;
	uint32_t note_off_delay_samples; // zero means that now we have no delayed note-off to send
} Plugin;

#endif // TYPES_H
