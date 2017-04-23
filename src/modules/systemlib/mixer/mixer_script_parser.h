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
 * @file mixer_script_parser.h
 *
 */


#ifndef _SYSTEMLIB_MIXER_SCRIPT_PARSER_H
#define _SYSTEMLIB_MIXER_SCRIPT_PARSER_H value


class MixerDataParser;
#include <istream>

#ifndef MIXER_SCRIPT_MAX_LINE_LENGTH
#define MIXER_SCRIPT_MAX_LINE_LENGTH 120
#endif

/**
 * Class responsible for parsing mixer script
 *
 */
class __EXPORT MixerScriptParser
{
public:
	MixerScriptParser(MixerDataParser *data_parser = nullptr);

	void setDataParser(MixerDataParser *data_parser) {_data_parser = data_parser;}
	virtual int parse_fd(int fd) {return -1;}
	virtual int parse_buff(char *buff, int len) {return -1;}
	virtual int parse_stream(std::istream *is) {return -1;}

protected:
	MixerDataParser             *_data_parser;
};

/**
 * Class responsible for parsing json mixer script with json11
 *
 */
class __EXPORT MixerJson11Parser : public MixerScriptParser
{
public:
	MixerJson11Parser(MixerDataParser *data_parser = nullptr);

	int parse_buff(char *buff, int len);
};


/**
 * Class responsible for parsing mixer script
 *
 */
class __EXPORT MixerBsonParser : public MixerScriptParser
{
public:
	MixerBsonParser(MixerDataParser *data_parser = nullptr);

	void setDataParser(MixerDataParser *data_parser) {_data_parser = data_parser;}
	int parse_fd(int fd);
};


/**
 * Class responsible for parsing mixer script
 *
 */
class __EXPORT MixerPicoJsonParser : public MixerScriptParser
{
public:
	MixerPicoJsonParser(MixerDataParser *data_parser = nullptr);

	void setDataParser(MixerDataParser *data_parser) {_data_parser = data_parser;}
	int parse_stream(std::istream *is);
};



#endif  //_SYSTEMLIB_MIXER_SCRIPT_PARSER_H
