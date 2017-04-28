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
 * @file mixer_parameters.cpp
 *
 * Object owning all global mixer parameters.
 * Initialized from mixer parameters data structure.
 */

#include <stdlib.h>
#include <string.h>
#include <cstdio>

#include "mixer_parameters.h"

//#define debug(fmt, args...)	do { } while(0)
#define debug(fmt, args...)	do { printf("[mixer_parameters] " fmt "\n", ##args); } while(0)

/****************************************************************************/

MixerParameters::MixerParameters()
	: _param_count(0)
	, _value_count(0)
	, _param_values(nullptr)
#if !defined(MIXER_REMOTE)
	, _metadata(nullptr)
#endif //!MIXER_REMOTE
{
}


MixerParameters::~MixerParameters()
{
	reset();
}

void
MixerParameters::reset()
{
	if (_param_values != nullptr) {
		free(_param_values);
	}

	if (_metadata != nullptr) {
		free(_metadata);
	}
}

int
MixerParameters::setParamsSize(int param_count, int value_count)
{
	//Only allow setting of size once
	if (_param_values != nullptr) {
		return -1;
	}

	debug("Setting parameters size to count:%d values:%d", param_count, value_count);

	_param_count = param_count;
	_value_count =  value_count;

	// Create parameter values in heap
	int param_data_size = _value_count * sizeof(mixer_register_val_u);

	debug("Allocating parameter data area of %d bytes", param_data_size);
	_param_values = (mixer_register_val_u *) malloc(param_data_size);

	if (_param_values == nullptr) {
		return -1;
	}

	memset(_param_values, 0, param_data_size);

	// If not a remote mxer then add a container for parameter metadata
#if !defined(MIXER_REMOTE)
	param_data_size = _param_count * sizeof(mixer_parameter_metadata_s);
	_metadata = (mixer_parameter_metadata_s *) malloc(param_data_size);

	if (_metadata == nullptr) {
		return -1;
	}

	memset(_metadata, 0, param_data_size);
#endif //MIXER_REMOTE

	return 0;
}

int
MixerParameters::setValues(mixer_param_values_s *values)
{
	if ((values->value_index + values->value_count) > _value_count) {
		return -1;
	}

	memcpy(_param_values, values->values, values->value_count * sizeof(mixer_register_val_u));
	return values->value_count;
}

int
MixerParameters::setParamMetaData(mixer_parameter_metadata_s *metadata)
{
#if !defined(MIXER_REMOTE)

	if (_metadata == nullptr) {
		return -1;
	}

	if (metadata->param_index < _param_count) {
		_metadata[metadata->param_index] = *metadata;
	}

#endif  //MIXER_REMOTE
	return 0;
}
