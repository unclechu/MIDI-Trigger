/**
	uris.c

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#include "uris.h"

void map_plugin_uris(LV2_URID_Map* mapinst, PluginURIs* uris) {
	uris->atom_Sequence = mapinst->map(mapinst->handle, LV2_ATOM__Sequence);
	uris->midi_Event = mapinst->map(mapinst->handle, LV2_MIDI__MidiEvent);
}
