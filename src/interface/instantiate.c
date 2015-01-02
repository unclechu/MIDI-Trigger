/**
	instantiate.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "instantiate.h"

LV2_Handle instantiate (
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
	plugin->next_buf_note_on = false;

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
