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

//#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

//#include "systemlib/uthash/utarray.h"

extern "C" {
#include "tinybson.h"
//#include <systemlib/bson/tinybson.h>
}

#include "mixer_json_parser.h"
#include "mixer_data_parser.h"


//#define debug(fmt, args...)	do { } while(0)
#define debug(fmt, args...)	do { printf("[mixer_data_parser] " fmt "\n", ##args); } while(0)

#define UNUSED(x) (void)(x)
#define MIXER_SCRIPT_MAX_LINE_LENGTH 120

/****************************************************************************/



struct mixer_load_state {
	bool mark_saved;
};

static int
mixer_json_parse_callback(bson_decoder_t decoder, void *priv, bson_node_t node)
{
//    float f;
//    int32_t i;
//    void *v = NULL;
	void *tmp = NULL;
	int result = -1;
	struct mixer_load_state *state = (struct mixer_load_state *)priv;
	UNUSED(state);

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	if (node->type == BSON_EOO) {
		debug("end of mixer file");
		result = -1;
		goto out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:

	if (tmp != NULL) {
		free(tmp);
	}

	return result;
}



/****************************************************************************/


MixerJsonParser::MixerJsonParser(MixerDataParser *data_parser)
	: _data_parser(nullptr)
{
}


int
MixerJsonParser::parse(int fd)
{
	UNUSED(fd);

//    struct param_wbuf_s *s = NULL;

	struct bson_decoder_s decoder;
	int result = -1;
	struct mixer_load_state state;

//    uint8_t buffer[2048];
//    size_t buf_size = 2048;

//    if (bson_decoder_init_buf(&decoder, &buffer, buf_size, mixer_json_parse_callback, &state)) {
//        debug("decoder init failed");
//        goto out;

//    }

	if (bson_decoder_init_file(&decoder, fd, mixer_json_parse_callback, &state) != 0) {
		debug("decoder init failed");
		goto out;
	}

	state.mark_saved = false;

	do {
		result = bson_decoder_next(&decoder);

	} while (result > 0);

out:

	if (result < 0) {
		debug("BSON error decoding parameters");
	}

	return result;
}




//    char    *startln  = script;
//    char    *endln  = script;

//    while(*endln != 0){
//        if(*endln == '\n'){
////            _parse_line(startln);
//            startln = endln + 1;
//        }
//        endln++;
//    }
//    UNUSED(startln);
