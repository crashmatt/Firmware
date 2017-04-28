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
#include "mixer_factory.h"
//#include <stdio.h>

#define debug(fmt, args...)	do { } while(0)
//#define debug(fmt, args...)	do { printf("[mixer_data_parser] " fmt "\n", ##args); } while(0)

/****************************************************************************/

MixerDataParser::MixerDataParser(MixerGroup *mix_group)
	: _mix_group(mix_group)
{
}

int
MixerDataParser::parse_buffer(uint8_t *buff, int bufflen)
{
	uint8_t *pos    = buff;
	uint8_t *end    = buff + bufflen - sizeof(mixer_datablock_header_s);
	mixer_datablock_header_s *blk_hdr;
	int remaining = bufflen;

	debug("parse buffer with size %u\n", bufflen);

	while (pos < end) {
		blk_hdr = (mixer_datablock_header_s *) pos;
		debug("parse buffer position %u\n", (pos - buff));

		if (blk_hdr->start == MIXER_DATABLOCK_START) {
			remaining = bufflen - (blk_hdr->data - buff);
			debug("Found datablock start\n");

			if (remaining >= blk_hdr->size) {
				switch (blk_hdr->type) {
				case MIXER_DATABLOCK_MIXER: {
						mixer_from_buffer((mixer_base_header_s *)blk_hdr->data, blk_hdr->size);
						break;
					}

				case MIXER_DATABLOCK_PARAMETERS: {
						debug("Parsing parameters datablock\n");
						_mix_group->getParameters()->setParamsSize(((mixer_parameters_s *) blk_hdr->data)->parameter_count,
								((mixer_parameters_s *) blk_hdr->data)->parameter_value_count);
						break;
					}

				case MIXER_DATABLOCK_PARAMETER_METADATA: {
						debug("Parsing parameter metadata datablock\n");
						_mix_group->getParameters()->setParamMetaData((mixer_parameter_metadata_s *) blk_hdr->data);
						break;
					}

				case MIXER_DATABLOCK_PARAM_VALUES: {
						debug("Parsing parameter values datablock\n");
						_mix_group->getParameters()->setValues((mixer_param_values_s *) blk_hdr->data);
						break;
					}

				case MIXER_DATABLOCK_VARIABLE_COUNT: {
						debug("Parsing variable count datablock\n");
						mixer_variables_s *vardata = (mixer_variables_s *) blk_hdr->data;
						_mix_group->getVariables()->setVariableCount(vardata->variable_count);
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


int
MixerDataParser::mixer_from_buffer(mixer_base_header_s *mixdata, int datasize)
{
	Mixer *mix = MixerFactory::factory(mixdata);

	if (mix == nullptr) {
		return -1;
	}

	_mix_group->append_mixer(mix);
	return 0;
}
