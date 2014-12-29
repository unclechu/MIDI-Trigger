/**
	uris.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef URIS_H
#define URIS_H

#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

typedef struct {
	LV2_URID atom_Sequence;
	LV2_URID midi_Event;
} PluginURIs;

inline void map_plugin_uris(LV2_URID_Map* mapinst, PluginURIs* uris) {
	uris->atom_Sequence = mapinst->map(mapinst->handle, LV2_ATOM__Sequence);
	uris->midi_Event = mapinst->map(mapinst->handle, LV2_MIDI__MidiEvent);
}

#endif // URIS_H