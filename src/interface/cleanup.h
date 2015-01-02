/**
	cleanup.h

	@author Viacheslav Lotsmanov
	@author Andrew Fatkulin
	@license GNU/GPLv3 https://github.com/metachronica/audio-dsp-midi-trigger/blob/master/LICENSE
*/

#ifndef CLEANUP_H
#define CLEANUP_H

#include <stdlib.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#include "../types.h"

void cleanup(LV2_Handle instance);

#endif // CLEANUP_H
