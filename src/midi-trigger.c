/**
	midi-trigger.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "midi-trigger.h"


static const LV2_Descriptor descriptor = {
	PLUGIN_URI,
	instantiate,
	connect_port,
	NULL,
	run,
	NULL,
	cleanup
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor ( uint32_t index ) {
	switch (index) {
	case 0:
		return &descriptor;
	default:
		return NULL;
	}
}

static LV2_Handle instantiate (
	const LV2_Descriptor *descriptor,
	double rate,
	const char *bundle_path,
	const LV2_Feature* const* features
) {
	Plugin *plugin = (Plugin *)malloc(sizeof(Plugin));
	if (!plugin) return NULL;

	plugin->map = NULL;
	plugin->detector_buf_size = 0;
	plugin->detector_counter = 0;
	plugin->detector_gap_counter = 0;
	plugin->is_gap_active = false;

	// get host features
	for (int i=0; features && features[i]; ++i) {
		if (strcmp(features[i]->URI, LV2_URID__map) == 0) {
			plugin->map = (LV2_URID_Map *)features[i]->data;
		}
	}
	if (!plugin->map) {
		fprintf(stderr, "Missing feature \"urid:map\"\n");
		free(plugin);
		return NULL;
	}

	// mapping URIs
	map_plugin_uris(plugin->map, &plugin->uris);

	plugin->SR = rate; // store samplerate

	plugin->detector_buffer = (float*)malloc(
		SAMPLES_IN_MS(DETECTOR_BUFFER_MAX, plugin->SR) * sizeof(float) );

	return (LV2_Handle)plugin;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	Plugin *plugin = (Plugin* )instance;

	switch((PortIndex)port) {
		case input_audio:
			plugin->channels.input_audio = (const float *)data;
			break;
		case output_midi:
			plugin->channels.output_midi = (LV2_Atom_Sequence *)data;
			break;
		case input_gain:
			plugin->channels.input_gain = (float *)data;
			break;
		case detector_buffer:
			plugin->channels.detector_buffer = (float *)data;
			break;
		case detector_gap:
			plugin->channels.detector_gap = (float *)data;
			break;
		case threshold:
			plugin->channels.threshold = (float *)data;
			break;
		case midi_note:
			plugin->channels.midi_note = (float *)data;
			break;
		case velocity_floor:
			plugin->channels.velocity_floor = (float *)data;
			break;
		case velocity_ceiling:
			plugin->channels.velocity_ceiling = (float *)data;
			break;
		case limit_rms_velocity_ceiling:
			plugin->channels.limit_rms_velocity_ceiling = (float *)data;
			break;
	}
}

static inline void gen_midi_event(
	Plugin* plugin,
	uint32_t *capacity,
	LV2_Midi_Message_Type type,
	uint32_t frame,
	float rms_dB
) {
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

static void run(LV2_Handle instance, uint32_t n_samples) {
	Plugin *plugin = (Plugin *)instance;

	bool already_prepared = false;
	uint32_t capacity;

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
				plugin->channels.input_audio[i];
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

			// temporarly test solution
			uint32_t frame = i;
			if (frame <= 0) frame = 1;
			prepare_midi_port(plugin, &already_prepared, &capacity);
			gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_OFF, frame-1, 0.0f);
			gen_midi_event(plugin, &capacity, LV2_MIDI_MSG_NOTE_ON, frame, rms_dB);

			// enable waiting for gap
			plugin->is_gap_active = true;
		} else {
			plugin->detector_counter = 0;
		}
	}
}

static void cleanup(LV2_Handle instance) {
	const Plugin *plugin = (const Plugin *)instance;
	free(plugin->detector_buffer);
	free(instance);
}
