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
 * @file mixer_data_parser.cpp
 *
 */

#include <stdlib.h>
#include <string.h>

#include "mixer_data_parser.h"
#include "mixers.h"

/****************************************************************************/

MixerDataParser::MixerDataParser(MixerGroup *mix_group, MixerParameters *mix_params, MixerRegisterGroups *mix_regs,
				 MixerVariables *mix_vars)
	: _mix_group(mix_group)
	, _mix_params(mix_params)
	, _mix_regs(mix_regs)
	, _mix_vars(mix_vars)
{
}

int
MixerDataParser::parse_buffer(uint8_t *buff, int bufflen)
{
	uint8_t *pos    = 0;
	mixer_datablock_header_s *blk_hdr;
	int remaining;

	while (pos < (buff - bufflen - sizeof(mixer_datablock_header_s))) {
		blk_hdr = (mixer_datablock_header_s *) pos;

		if (blk_hdr->start == MIXER_DATABLOCK_START) {
			remaining = bufflen - (blk_hdr->data - buff);

			if (remaining >= blk_hdr->size) {
				switch (blk_hdr->type) {
				case MIXER_DATABLOCK_MIXER: {
						_mix_group->from_buffer((uint8_t *) &blk_hdr->data, blk_hdr->size);
						// No check on remaining here.  Just assume everything was consumed
						break;
					}

				case MIXER_DATABLOCK_PARAMETERS: {
#if !defined(MIXER_REMOTE)
						_mix_params->setParamsSize((mixer_parameters_s *) blk_hdr->data, true);
#else
						_mix_params->setParamsSize((mixer_parameters_s *) blk_hdr->data, false);
#endif //MIXER_REMOTE
						break;
					}

				case MIXER_DATABLOCK_PARAMETER_METADATA: {
//                    mixer_parameter_metadata_s
						break;
					}

				case MIXER_DATABLOCK_PARAM_VALUES: {
						_mix_params->setValues((mixer_param_values_s *) blk_hdr->data);
						break;
					}

				case MIXER_DATABLOCK_VARIABLE_COUNT: {
						mixer_variables_s *vardata = (mixer_variables_s *) blk_hdr->data;
						_mix_vars->setVariableCount(vardata->variable_count);
						break;
					}

				default:
					break;
				}

				//Jump over the block regardless of correct parsing or not.
				pos = blk_hdr->data + blk_hdr->size;
			}

		} else {
			pos++;
		}
	}

	return bufflen - (buff - pos);
}
