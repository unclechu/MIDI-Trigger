/**
	cleanup.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/unclechu/MIDI-Trigger/blob/master/LICENSE
*/

#include "cleanup.h"

void cleanup(LV2_Handle instance) {
	const Plugin *plugin = (const Plugin *)instance;
	free(plugin->detector_buffer);
	free(instance);
}
