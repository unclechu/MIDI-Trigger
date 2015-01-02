/**
	connect_port.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef CONNECT_PORT_H
#define CONNECT_PORT_H

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/ext/atom/atom.h"

#include "../types.h"

void connect_port(LV2_Handle instance, uint32_t port, void* data);

#endif // CONNECT_PORT_H
