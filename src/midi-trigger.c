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
		case buffer:
			plugin->channels.buffer = (float *)data;
			break;
		case gap:
			plugin->channels.gap = (float *)data;
			break;
	}
}

int a = 0;

static void run(LV2_Handle instance, uint32_t n_samples){
	const Plugin *plugin = (const Plugin *)instance;

	const uint32_t capacity = plugin->channels.output_midi->atom.size;

	lv2_atom_sequence_clear(plugin->channels.output_midi);
	plugin->channels.output_midi->atom.type = plugin->uris.atom_Sequence;

	for (int i=0; i<n_samples; ++i) {
		LV2_Atom_Event* event = (LV2_Atom_Event *)malloc(sizeof(LV2_Atom_Event));

		event->time.frames = i;
		event->body.type = plugin->uris.midi_Event;
		event->body.size = 3;

		uint8_t* msg = (uint8_t *)LV2_ATOM_BODY(&event->body);

		// test note hardcode
		if (a == 0) {
			msg[0] = LV2_MIDI_MSG_NOTE_ON;
			a = 1;
		} else {
			msg[0] = LV2_MIDI_MSG_NOTE_OFF;
			a = 0;
		}
		msg[1] = 70;
		msg[2] = 80;

		lv2_atom_sequence_append_event(
			plugin->channels.output_midi, capacity, event);

		free(event);
	}
}

static void cleanup ( LV2_Handle instance ) {
	free( instance );
}
