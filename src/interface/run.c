/**
	run.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "run.h"

static inline void prepare_midi_port(
	Plugin* plugin, bool* already_prepared, uint32_t* capacity
) {
	if (*already_prepared) return;
	else *already_prepared = true;

	*capacity = plugin->channels.output_midi->atom.size;

	lv2_atom_sequence_clear(plugin->channels.output_midi);
	plugin->channels.output_midi->atom.type = plugin->uris.atom_Sequence;
}

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
	msg[1] = plugin->last_midi_note;

	if (type == LV2_MIDI_MSG_NOTE_OFF) {
		msg[2] = 0;
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

		if (rms_len <= 0 || vel_len < 0) {
			msg[2] = 0;
		} else {
			float vel = ((rms_by_len * vel_len) / rms_len) + vel_min;
			if (vel < 1) vel = 1;
			else if (vel > 128) vel = 128;
			msg[2] = (uint8_t)(vel) - 1;
		}
	}

	lv2_atom_sequence_append_event(
		plugin->channels.output_midi, *capacity, event);

	free(event);
}

void run(LV2_Handle instance, uint32_t n_samples) {
	Plugin *plugin = (Plugin *)instance;

	bool already_prepared = false;
	uint32_t capacity;

	if (plugin->next_buf_note != LV2_MIDI_MSG_RESET) {
		prepare_midi_port(plugin, &already_prepared, &capacity);
		gen_midi_event(
			plugin, &capacity, plugin->next_buf_note,
			0, plugin->next_buf_rms_dB);
		plugin->next_buf_note = LV2_MIDI_MSG_RESET;
	}

	uint32_t detector_buf_size =
		SAMPLES_IN_MS( *(plugin->channels.detector_buffer), plugin->SR );
	uint32_t gap_size =
		SAMPLES_IN_MS( *(plugin->channels.detector_gap), plugin->SR );
	uint32_t note_off_delay =
		SAMPLES_IN_MS( *(plugin->channels.note_off_delay), plugin->SR );

	// get current midi note control knob value
	uint8_t midi_note = (uint8_t)(*(plugin->channels.midi_note));
	if (midi_note != plugin->last_midi_note) {
		prepare_midi_port(plugin, &already_prepared, &capacity);
		gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_OFF, 0, 0.0f);
		plugin->last_midi_note = midi_note;
	}

	// get current "note-off mode" control knob value
	uint8_t note_off_mode = (uint8_t)(*(plugin->channels.note_off_mode));
	if (note_off_mode < NOTE_OFF_MODE_MIN) note_off_mode = NOTE_OFF_MODE_MIN;
	else if (note_off_mode > NOTE_OFF_MODE_MAX) note_off_mode = NOTE_OFF_MODE_MAX;

	// if detector buffer size is changed then reset all counters
	if (plugin->detector_buf_size != detector_buf_size) {
		plugin->detector_counter = 0;
		plugin->detector_gap_counter = 0;
		plugin->detector_buf_size = detector_buf_size;
		plugin->is_gap_active = false;
	}

	for (uint32_t i=0; i<n_samples; i++) {
		// if we have delayed note-off to send
		if (plugin->note_off_delay_samples != 0) {
			plugin->note_off_delay_samples = note_off_delay;

			if (plugin->note_off_delay_counter >= plugin->note_off_delay_samples) {
				prepare_midi_port(plugin, &already_prepared, &capacity);
				gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_OFF, 0, 0.0f);
				plugin->note_off_delay_counter = 0;
				plugin->note_off_delay_samples = 0;
			} else {
				plugin->note_off_delay_counter++;
			}
		}

		// waiting for gap if gap is active
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
		// TODO support calculating in own thread (optional feature for fun)
		float rms_dB =
			CO_DB(rms(plugin->detector_buffer, plugin->detector_buf_size));

		// if RMS has enough loud peak to send MIDI note
		if (rms_dB >= *(plugin->channels.threshold)) {
			prepare_midi_port(plugin, &already_prepared, &capacity);

			LV2_Midi_Message_Type midi_type_1;
			LV2_Midi_Message_Type midi_type_2;

			switch (note_off_mode) {
				case 1:
					midi_type_1 = LV2_MIDI_MSG_NOTE_OFF;
					midi_type_2 = LV2_MIDI_MSG_NOTE_ON;
					break;
				case 2:
					midi_type_1 = LV2_MIDI_MSG_NOTE_ON;
					midi_type_2 = LV2_MIDI_MSG_NOTE_OFF;
					break;
			}

			switch (note_off_mode) {
				case 1:
				case 2:
					gen_midi_event(plugin, &capacity, midi_type_1, i, rms_dB);
					if (i+1 >= n_samples) {
						// note shifted to next buffer
						plugin->next_buf_rms_dB = rms_dB;
						plugin->next_buf_note = midi_type_2;
					} else {
						// note to next sample in same buffer
						gen_midi_event(plugin, &capacity, midi_type_2, i+1, rms_dB);
					}
					break;

				case 3:
					// if we have delayed note-off to send then send it immediately before note-on
					if (plugin->note_off_delay_samples != 0) {
						gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_OFF, i, rms_dB);

						if (i+1 >= n_samples) {
							// note shifted to next buffer
							plugin->next_buf_rms_dB = rms_dB;
							plugin->next_buf_note = LV2_MIDI_MSG_NOTE_ON;
						} else {
							// note to next sample in same buffer
							gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, i+1, rms_dB);
						}
					} else {
						gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, i, rms_dB);
					}
					plugin->note_off_delay_counter = 0;
					plugin->note_off_delay_samples = note_off_delay;
					break;

				case 4:
					gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, i, rms_dB);
					plugin->note_off_delay_counter = 0;
					plugin->note_off_delay_samples = note_off_delay;
					break;

				case 5:
					gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, i, rms_dB);
					break;
			}

			// enable waiting for gap
			plugin->is_gap_active = true;
		} else {
			plugin->detector_counter = 0;
		}
	}
}
