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
 * @file mixer_factory.cpp
 *
 * Static mixer factory for generating mixer objects from mixer data structures
 */

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mixer_factory.h"
#include "mixers.h"

Mixer *MixerFactory::factory(mixer_base_header_s *mixdata)
{
	if (mixdata == nullptr) {
		return nullptr;
	}

	switch (mixdata->mixer_type) {
	case MIXER_TYPES_ADD : {
			if (mixdata->data_size != sizeof(mixer_data_operator_s)) {
				return nullptr;
			}

			mixer_data_operator_s *opdata = (mixer_data_operator_s *) malloc(sizeof(mixer_data_operator_s));
			memcpy(opdata, mixdata, sizeof(mixer_data_operator_s));
			return new MixerAdd((mixer_data_operator_s *) opdata);
			break;
		}

	case MIXER_TYPES_ADD_CONST : {
			if (mixdata->data_size != sizeof(mixer_data_const_operator_s)) {
				return nullptr;
			}

			mixer_data_const_operator_s *opdata = (mixer_data_const_operator_s *) malloc(sizeof(mixer_data_const_operator_s));
			memcpy(opdata, mixdata, sizeof(mixer_data_const_operator_s));
			return new MixerAddConst((mixer_data_const_operator_s *) opdata);
			break;
		}

	case MIXER_TYPES_COPY : {
			if (mixdata->data_size != sizeof(mixer_data_operator_s)) {
				return nullptr;
			}

			mixer_data_operator_s *opdata = (mixer_data_operator_s *) malloc(sizeof(mixer_data_operator_s));
			memcpy(opdata, mixdata, sizeof(mixer_data_operator_s));
			return new MixerCopy((mixer_data_operator_s *) opdata);
			break;
		}

	case MIXER_TYPES_MULTIPLY : {
			if (mixdata->data_size != sizeof(mixer_data_operator_s)) {
				return nullptr;
			}

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
