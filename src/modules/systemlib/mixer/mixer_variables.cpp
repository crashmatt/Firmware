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

#include "mixer_variables.h"

/****************************************************************************/

MixerVariables::MixerVariables()
	: _var_count(0)
	, _variables(nullptr);
{
}


MixerVariables::~MixerVariables()
{
	if (_variables != nullptr) {
		free(_variables);
	}
}

int
MixerVariables::set(uint16_t variable_count)
{
	//Only allow setting of size once
	if (_variables != nullptr) {
		return -1;
	}

	int param_data_size = param_sizes.parameter_value_count * sizeof(mixer_register_val_u);

	_param_values = (mixer_register_val_u *)  malloc(param_data_size);

	if (_param_values == nullptr) {
		return -1;
	}

	memset(_param_values, 0, param_data_size);
	_params = param_sizes;
	return param_sizes.parameter_value_count;
}
