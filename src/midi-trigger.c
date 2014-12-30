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
	}
}

void gen_midi_event(
	Plugin* plugin, LV2_Midi_Message_Type type, uint32_t frame
) {
	const uint32_t capacity = plugin->channels.output_midi->atom.size;

	lv2_atom_sequence_clear(plugin->channels.output_midi);
	plugin->channels.output_midi->atom.type = plugin->uris.atom_Sequence;

	LV2_Atom_Event* event = (LV2_Atom_Event *)malloc(sizeof(LV2_Atom_Event));

	event->time.frames = frame;
	event->body.type = plugin->uris.midi_Event;
	event->body.size = 3;

	uint8_t* msg = (uint8_t *)LV2_ATOM_BODY(&event->body);

	msg[0] = type;
	msg[1] = floor(*(plugin->channels.midi_note));
	msg[2] = 127;

	lv2_atom_sequence_append_event(
		plugin->channels.output_midi, capacity, event);

	free(event);
}

static void run(LV2_Handle instance, uint32_t n_samples) {
	Plugin *plugin = (Plugin *)instance;

	uint32_t detector_buf_size =
		SAMPLES_IN_MS( *(plugin->channels.detector_buffer), plugin->SR );
	uint32_t gap_size =
		SAMPLES_IN_MS( *(plugin->channels.detector_gap), plugin->SR );

	if (plugin->detector_buf_size != detector_buf_size) {
		plugin->detector_counter = 0;
		plugin->detector_gap_counter = 0;
		plugin->detector_buf_size = detector_buf_size;
		plugin->is_gap_active = true;
	}

	for (uint32_t i=0; i<n_samples; i++) {
		// TODO skip gap rest samples for i (optimization)
		if (plugin->is_gap_active) {
			if (plugin->detector_gap_counter >= gap_size) {
				plugin->is_gap_active = false;
				plugin->detector_gap_counter = 0;
				plugin->detector_counter = 0;
			} else {
				plugin->detector_gap_counter++;
				continue;
			}
		}

		// storing samples to detector buffer
		if (plugin->detector_counter < plugin->detector_buf_size) {
			plugin->detector_buffer[plugin->detector_counter] =
				plugin->channels.input_audio[i];
			plugin->detector_counter++;
			continue;
		}

		// if we have fully filled buffer

		float dB =
			CO_DB(rms(plugin->detector_buffer, plugin->detector_buf_size));

		if (dB >= *(plugin->channels.threshold)) {
			uint32_t frame = i;
			if (frame <= 0) frame = 1;

			gen_midi_event(plugin, LV2_MIDI_MSG_NOTE_OFF, frame-1);
			gen_midi_event(plugin, LV2_MIDI_MSG_NOTE_ON, frame);

			// enable waiting
			plugin->is_gap_active = true;
		}
	}
}

static void cleanup(LV2_Handle instance) {
	const Plugin *plugin = (const Plugin *)instance;
	free(plugin->detector_buffer);
	free(instance);
}
