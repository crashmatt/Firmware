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

#include "mixer.h"

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer] " fmt "\n", ##args); } while(0)

Mixer::Mixer(mixer_base_header_s *mixdata)
	: _mixdata(mixdata)
{
}

/****************************************************************************/

MixerOperator::MixerOperator(mixer_data_operator_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
	_mixdata->base_type = MIXER_BASE_TYPE_OPERATOR;
}

MixerOperator::~MixerOperator()
{
	if (_mixdata != nullptr) {
		free((mixer_data_operator_s *) _mixdata);
	}
}

/****************************************************************************/

MixerFunction::MixerFunction(mixer_data_function_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
	_mixdata->base_type = MIXER_BASE_TYPE_FUNCTION;
}

MixerFunction::~MixerFunction()
{
}


/****************************************************************************/


MixerObject::MixerObject(mixer_data_object_s *mixdata)
	: Mixer((mixer_base_header_s *) mixdata)
{
	_mixdata->base_type = MIXER_BASE_TYPE_FUNCTION;
}
