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
 * @file mixer_json_parser.cpp
 *
 */

/*
 * Enable or disable code which demonstrates the behavior change in Xcode 7 / Clang 3.7,
 * introduced by DR1467 and described here: https://github.com/dropbox/json11/issues/86
 * Defaults to off since it doesn't appear the standards committee is likely to act
 * on this, so it needs to be considered normal behavior.
 */
#ifndef JSON11_ENABLE_DR1467_CANARY
#define JSON11_ENABLE_DR1467_CANARY 0
#endif


//#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <stdint.h>

#include <cstdio>
#include <cstring>

//#include "systemlib/uthash/utarray.h"

#include "json11.hpp"

#include "mixer_script_parser.h"
#include "mixer_data_parser.h"
#include "mixer_data.h"

using namespace json11;
using std::string;


//#define debug(fmt, args...)	do { } while(0)
#define debug(fmt, args...)	do { printf("[mixer_data_parser] " fmt "\n", ##args); } while(0)

#define UNUSED(x) (void)(x)



/****************************************************************************/

MixerJson11Parser::MixerJson11Parser(MixerDataParser *data_parser)
	: MixerScriptParser(data_parser)
{
}


int
MixerJson11Parser::parse_buff(char *buff, int len)
{
	UNUSED(len);
	uint8_t mixdata[120];

	if (_data_parser == nullptr) {
		std::cout << "no data parser set" << std::endl;
		return -1;
	}

	mixer_datablock_header_s *blk_hdr = (mixer_datablock_header_s *) &mixdata;
	blk_hdr->start = MIXER_DATABLOCK_START;

	string err;
	const auto json = Json::parse(buff, err);
	std::cout <<  "error - " << err << "\n";

	int param_count = 0;
	int value_count = 0;
	int group_count = 0;;

	if (!json["mixer_config"].is_object()) {
		std::cout << "not a json mixer configuration file" << std::endl;
		return -1;
	}

	// Count the groups
	if (json["mixer_config"]["groups"].is_array()) {
		group_count = json["mixer_config"]["groups"].array_items().size();

	} else {
		std::cout << "groups not an array" << std::endl;
		return -1;
	}

	// Count the parameters
	if (json["mixer_config"]["parameters"].is_array()) {
		for (auto &k : json["mixer_config"]["parameters"].array_items()) {
			std::cout << "    - " << k.dump() << "\n";

			if (k.is_object()) {
				if (k["value"].is_number()) {
					value_count++;

				} else if (k["value"].is_array()) {
					value_count += k["value"].array_items().size();
				}

			} else {
				std::cout << "parameter item not an object" << std::endl;
				return -1;
			}

			param_count++;
		}

		blk_hdr->type = MIXER_DATABLOCK_PARAMETERS;
		blk_hdr->size = sizeof(mixer_parameters_s);
		mixer_parameters_s *params = (mixer_parameters_s *) blk_hdr->data;
		params->parameter_count = param_count;
		params->parameter_value_count = value_count;
		_data_parser->parse_buffer(mixdata, sizeof(mixer_datablock_header_s) + blk_hdr->size);

	} else {
		std::cout << "parameters not an array" << std::endl;
		return -1;
	}

	std::cout  <<  "group_count:" << group_count
		   <<  "  para_count:" << param_count
		   << "   value_count:" << value_count << "\n";

	return 0;
}
