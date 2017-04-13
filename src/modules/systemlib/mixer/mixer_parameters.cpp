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

#include "mixer_parameters.h"

/****************************************************************************/

MixerParameters::MixerParameters()
	: _params( {0, 0})
, _param_values(nullptr)
{
}


MixerParameters::~MixerParameters()
{
	if (_param_values != nullptr) {
		free(_param_values);
	}
}

int
MixerParameters::setParamsSize(mixer_parameters_s param_sizes)
{
	//Only allow setting of size once
	if (_param_values != nullptr) {
		return -1;
	}

	int param_data_size = param_sizes.parameter_value_count * sizeof(mixer_register_val_u);

	_param_values = (mixer_register_val_u *) malloc(param_data_size);

	if (_param_values == nullptr) {
		return -1;
	}

	memset(_param_values, 0, param_data_size);
	_params = param_sizes;
	return param_sizes.parameter_value_count;
}

int
MixerParameters::setValues(mixer_param_values_s *values)
{
	if ((values->value_index + values->value_count) > _params.parameter_value_count) {
		return -1;
	}

	memcpy(_param_values, values->values, values->value_count * sizeof(mixer_register_val_u));
	return values->value_count;
}
