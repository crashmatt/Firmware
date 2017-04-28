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
 * @file mixer_parameters.h
 *
 * Descripition of mixer parameters
 */


#ifndef _SYSTEMLIB_MIXER_PARAMETERS_H
#define _SYSTEMLIB_MIXER_PARAMETERS_H value

#include "mixer_data.h"

/**
 * Class containing a reference to a mixer register array and a size for the array
 *
 */
class __EXPORT MixerParameters
{
public:
	MixerParameters();
	~MixerParameters();

	void reset(void);
	int setParamsSize(int param_count, int value_count);
	int setValues(mixer_param_values_s *values);
	int valueCount() {return _value_count;}
	int paramCount() {return _param_count;}
	mixer_register_val_u *paramValues() {return _param_values;}

#if !defined(MIXER_REMOTE)
	int setParamMetaData(mixer_parameter_metadata_s *metadata);
#endif //!MIXER_REMOTE

protected:
	int                    _param_count;
	int                     _value_count;
	mixer_register_val_u   *_param_values;

#if !defined(MIXER_REMOTE)
	mixer_parameter_metadata_s  *_metadata;
#endif
};

#endif
