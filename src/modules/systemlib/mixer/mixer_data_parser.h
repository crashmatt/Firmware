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
 * @file mixer.h
 *
 * Structure for holding resgisters which a mixer uses for input and output.
 * Holds structure information on regsiters so a mixer can validate correct registers
 * Supports mixer parameters for reporting register metadata
 */


#ifndef _SYSTEMLIB_MIXER_DATAPARSER_H
#define _SYSTEMLIB_MIXER_DATAPARSER_H value

#include <stdint.h>

class MixerGroup;
class MixerParameters;
class MixerRegisterGroups;
class MixerVariables;

/****************************************************************************/
// Mixer datablock structures

typedef enum {
	MIXER_DATABLOCK_NONE = 0,
	MIXER_DATABLOCK_MIXER,
	MIXER_DATABLOCK_GROUP,
	MIXER_DATABLOCK_GROUP_METADATA,
	MIXER_DATABLOCK_PARAMETERS,
	MIXER_DATABLOCK_PARAM_VALUES,
	MIXER_DATABLOCK_VARIABLE_COUNT,
	MIXER_DATABLOCK_PARAMETER_METADATA,
	MIXER_DATABLOCK_VARIABLE_METADATA,
} mixer_datablocks_e;

#define MIXER_DATABLOCK_START 0x55AA

typedef struct
__attribute__((packed))
{
	uint32_t    start;
	uint16_t    size;   //Size of the block not including the header.
	uint16_t    type;
	uint8_t     data[0];
} mixer_datablock_header_s;

/**
 * Class responsible for I/O parsing of mixers and related data.
 *
 */
class __EXPORT MixerDataParser
{
public:
	MixerDataParser(MixerGroup *mix_group, MixerParameters *mix_params, MixerRegisterGroups *mix_regs,
			MixerVariables *mix_vars);

	int parse_buffer(uint8_t *buff, int bufflen);

protected:
	MixerGroup          *_mix_group;
	MixerParameters     *_mix_params;
	MixerRegisterGroups *_mix_regs;
	MixerVariables      *_mix_vars;
};

#endif  //_SYSTEMLIB_MIXER_DATAPARSER_H
