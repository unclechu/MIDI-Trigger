/**
	midi-trigger.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "midi-trigger.h"


static const LV2_Descriptor descriptor = {
	URI,
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
	Channels *plugin = (Channels *)malloc(sizeof(Channels));
	return (LV2_Handle)plugin;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	Channels *plugin = (Channels* )instance;

	switch((PortIndex)port) {
		case input_audio:
			plugin->input_audio = (const float *)data;
			break;
		case output_midi:
			plugin->output_midi = (float *)data;
			break;
		case input_gain:
			plugin->input_gain = (float *)data;
			break;
		case buffer:
			plugin->buffer = (float *)data;
			break;
		case gap:
			plugin->gap = (float *)data;
			break;
	}
}

static void run(LV2_Handle instance, uint32_t n_samples){
	const Channels *plugin = (const Channels *)instance;
	(plugin->output_midi)[0] = 64;
}

static void cleanup ( LV2_Handle instance ) {
	free( instance );
}
