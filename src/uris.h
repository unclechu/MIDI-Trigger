/**
	uris.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef URIS_H
#define URIS_H

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"

typedef struct {
	LV2_URID atom_Sequence;
	LV2_URID midi_Event;
} PluginURIs;

void map_plugin_uris(LV2_URID_Map* mapinst, PluginURIs* uris);

#endif // URIS_H
