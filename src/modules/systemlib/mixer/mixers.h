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
 * @file mixers.h
 *
 * Collection of all mixer headers
 */

#ifndef _SYSTEMLIB_MIXER_MIXERS_H
#define _SYSTEMLIB_MIXER_MIXERS_H value

#include <px4_config.h>
#include "drivers/drv_mixer.h"

#include "mixer_registers.h"
#include "mixer.h"
#include "mixer_operators.h"

#include "mixer_types.h"


class __EXPORT MixerFactory
{
public:
	/**
	 * Create a new mixer according to the data passed to it
	 *
	 * @param[in]   mixdata     mixer data structure describing the whole mixer
	 * @return                  pointer to new mixer with base class Mixer
	 */
	static Mixer *factory(mixer_base_header_s *mixdata);

	/**
	 * Create new mixer(s) from a data buffer and adds them into a MixerGroup.
	 *
	 * @param[in]   group       MixerGroup object to put parsed mixers in.
	 * @param[in]   mixbuff     mixer data buffer describing all mixers
	 * @param[in]   bufflen     buffer length.
	 * @return                  Remaining buffer length not used.
	 */
	static int from_buffer(MixerGroup *group, uint8_t *mixbuff, int bufflen);
};

#endif  //_SYSTEMLIB_MIXER_MIXERS_H
