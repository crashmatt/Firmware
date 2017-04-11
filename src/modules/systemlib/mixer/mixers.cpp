/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mixer.cpp
 *
 * Generic mixer library.
 */

#include <px4_config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>
#include <systemlib/err.h>

#include "mixers.h"

Mixer *MixerFactory::factory(mixer_base_header_s *mixdata)
{
	if (mixdata == nullptr) {
		return nullptr;
	}

	switch (mixdata->mixer_type) {
	case MIXER_TYPES_ADD : {
			mixer_data_operator_s *opdata = (mixer_data_operator_s *) malloc(sizeof(mixer_data_operator_s));
			memcpy(opdata, mixdata, sizeof(mixer_data_operator_s));
			return new MixerAdd((mixer_data_operator_s *) opdata);
			break;
		}

	case MIXER_TYPES_COPY : {
			mixer_data_operator_s *opdata = (mixer_data_operator_s *) malloc(sizeof(mixer_data_operator_s));
			std::memcpy(opdata, mixdata, sizeof(mixer_data_operator_s));
			return new MixerCopy((mixer_data_operator_s *) opdata);
			break;
		}

	case MIXER_TYPES_MULTIPLY : {
			mixer_data_operator_s *opdata = (mixer_data_operator_s *) malloc(sizeof(mixer_data_operator_s));
			memcpy(opdata, mixdata, sizeof(mixer_data_operator_s));
			return new MixerMultiply((mixer_data_operator_s *) opdata);
			break;
		}

	case MIXER_TYPES_NONE :
	default:
		return nullptr;
		break;
	}
}


int
MixerFactory::from_buffer(MixerGroup *group, uint8_t *mixbuff, int bufflen)
{
	mixer_base_header_s *mixdata;
	Mixer *new_mixer;

	int remaining = bufflen;

	while (remaining > 0) {
		mixdata = (mixer_base_header_s *) &mixbuff[bufflen - remaining];

		if (mixdata->data_size == 0) {
			return remaining;
		}

		if (mixdata->mixer_type == MIXER_TYPES_NONE) {
			return remaining;
		}

		new_mixer = factory(mixdata);

		if (new_mixer == nullptr) {
			return remaining;
		}

		group->append_mixer(new_mixer);

		remaining -= mixdata->data_size;
	}

	return remaining;
}
