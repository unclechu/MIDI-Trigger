/**
	run.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "run.h"

static inline void gen_midi_event(
	Plugin* plugin,
	uint32_t *capacity,
	LV2_Midi_Message_Type type,
	uint32_t frame,
	float rms_dB
) {
	// TODO try to allocate memory only once and use it for all events
	LV2_Atom_Event* event = (LV2_Atom_Event *)malloc(sizeof(LV2_Atom_Event));

	event->time.frames = frame;
	event->body.type = plugin->uris.midi_Event;
	event->body.size = 3;

	uint8_t* msg = (uint8_t *)LV2_ATOM_BODY(&event->body);

	msg[0] = type;
	msg[1] = floor(*(plugin->channels.midi_note));

	if (type == LV2_MIDI_MSG_NOTE_OFF) {
		msg[0] = 0;
	} else {
		const float rms_min = *(plugin->channels.threshold);
		const float rms_max = *(plugin->channels.limit_rms_velocity_ceiling);
		const float vel_min = *(plugin->channels.velocity_floor);
		const float vel_max = *(plugin->channels.velocity_ceiling);

		// limiting
		if (rms_dB > rms_max) rms_dB = rms_max;

		const float rms_len = LEN(rms_min, rms_max);
		const float vel_len = LEN(vel_min, vel_max);
		const float rms_by_len = rms_dB - rms_min;

		if (rms_len <= 0 || vel_len <= 0) {
			msg[2] = 0;
		} else {
			msg[2] = (uint8_t)(((rms_by_len * vel_len) / rms_len) + vel_min);
		}
	}

	lv2_atom_sequence_append_event(
		plugin->channels.output_midi, *capacity, event);

	free(event);
}

static inline void prepare_midi_port(
	Plugin* plugin, bool* already_prepared, uint32_t* capacity
) {
	if (*already_prepared) return;
	else *already_prepared = true;

	*capacity = plugin->channels.output_midi->atom.size;

	lv2_atom_sequence_clear(plugin->channels.output_midi);
	plugin->channels.output_midi->atom.type = plugin->uris.atom_Sequence;
}

void run(LV2_Handle instance, uint32_t n_samples) {
	Plugin *plugin = (Plugin *)instance;

	bool already_prepared = false;
	uint32_t capacity;

	if (plugin->next_buf_note_on) {
		prepare_midi_port(plugin, &already_prepared, &capacity);
		gen_midi_event(
			plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, 0, plugin->next_buf_rms_dB);
		plugin->next_buf_note_on = false;
	}

	// TODO optimize
	uint32_t detector_buf_size =
		SAMPLES_IN_MS( *(plugin->channels.detector_buffer), plugin->SR );
	uint32_t gap_size =
		SAMPLES_IN_MS( *(plugin->channels.detector_gap), plugin->SR );

	// TODO store last midi note and after change send note off

	// if detector buffer size is changed then reset all counters
	if (plugin->detector_buf_size != detector_buf_size) {
		plugin->detector_counter = 0;
		plugin->detector_gap_counter = 0;
		plugin->detector_buf_size = detector_buf_size;
		plugin->is_gap_active = false;
	}

	for (uint32_t i=0; i<n_samples; i++) {
		// waiting for gap if gap is active
		// TODO skip gap rest samples for i (optimization)
		if (plugin->is_gap_active) {
			// if waiting is finished
			if (plugin->detector_gap_counter >= gap_size) {
				plugin->is_gap_active = false;
				plugin->detector_gap_counter = 0;
				plugin->detector_counter = 0;
			// if still waiting
			} else {
				plugin->detector_gap_counter++;
				continue;
			}
		}

		// storing samples to detector buffer
		// if detector buffer not yet filled
		if (plugin->detector_counter < plugin->detector_buf_size) {
			plugin->detector_buffer[plugin->detector_counter] =
				plugin->channels.input_audio[i]
				* DB_CO(*(plugin->channels.input_gain));
			plugin->detector_counter++;
			continue;
		}

		// if we have fully filled detector buffer

		// calculate RMS in dB by detector buffer
		// TODO support calculating in own thread
		float rms_dB =
			CO_DB(rms(plugin->detector_buffer, plugin->detector_buf_size));

		// if RMS has enough loud peak to send MIDI note
		if (rms_dB >= *(plugin->channels.threshold)) {

			prepare_midi_port(plugin, &already_prepared, &capacity);

			gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_OFF, i, 0.0f);

			uint32_t frame = i + 1;
			if (frame >= n_samples) {
				plugin->next_buf_rms_dB = rms_dB;
				plugin->next_buf_note_on = true;
			} else {
				gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, frame, rms_dB);
			}

			// enable waiting for gap
			plugin->is_gap_active = true;
		} else {
			plugin->detector_counter = 0;
		}
	}
}
