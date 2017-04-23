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
 * @file mixer_functions.h
 *
 *
 */


#ifndef _SYSTEMLIB_MIXER_FUNCTIONS_H
#define _SYSTEMLIB_MIXER_FUNCTIONS_H value

#include "mixer.h"

/****************************************************************************/

/**
 * Mixer for doing add operations
 */
class __EXPORT MixerMultipoint : public Mixer
{
public:
	MixerMultipoint(mixer_data_multipoint_s *mixdata);
//    MixerMultipoint(mixer_register_ref_s in, mixer_register_ref_s out, mixer_register_ref_s inpts, mixer_register_ref_s outpts, int pts);

	uint16_t		mix(MixerRegisterGroups *reg_groups);
	bool            mixerValid(MixerRegisterGroups *reg_groups);
	uint16_t        getMixerType() {return MIXER_TYPES_MULTIPLY;}
protected:
	uint32_t                _count;
	mixer_register_ref_s    _in;
	mixer_register_ref_s    _in_vals;
	mixer_register_ref_s    _out_vals;
	mixer_register_ref_s    _out;
private:
};


#endif
