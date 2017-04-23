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


#define PICOJSON_NO_EXCEPTIONS 1
#define PICOJSON_FLOAT_PRECISION 10
#include <lib/picojson/picojson.h>

#include "mixer_script_parser.h"
#include "mixer_data_parser.h"
#include "mixer_data.h"

using std::string;

//#define debug(fmt, args...)	do { } while(0)
#define debug(fmt, args...)	do { printf("[mixer_data_parser] " fmt "\n", ##args); } while(0)

#define UNUSED(x) (void)(x)
#define MIXER_SCRIPT_MAX_LINE_LENGTH 120


/****************************************************************************/

namespace
{

class mixer_context : public  picojson::null_parse_context
{
protected:
	enum {
		MIXCONTEXT_NULL = 0,
		MIXCONTEXT_MIXER_CONFIG,
		MIXCONTEXT_GROUPS,
		MIXCONTEXT_PARAMETERS,
		MIXCONTEXT_VARIABLES,
		MIXCONTEXT_MIXERS,
		MIXCONTEXT_MIXER,
	};

	int state_;
	picojson::value *out_;

public:
	picojson::value  parameters;
	picojson::value  variables;
	picojson::value  groups;

	mixer_context(picojson::value *out)
		:  picojson::null_parse_context()
		, state_(MIXCONTEXT_NULL)
		, out_(out)
		, parameters()
		, variables()
		, groups()
	{
	}

	struct dummy_str {
		void push_back(int)
		{
		}
	};

	bool set_null()
	{
		std::cerr << "set null";
		return true;
	}
	bool set_bool(bool b)
	{
		std::cerr << "set bool : ";

		if (b) {
			std::cerr << "true";

		} else {
			std::cerr << "false";
		}

		std::cerr << std::endl;
		return true;
	}
#ifdef PICOJSON_USE_INT64
	bool set_int64(int64_t)
	{
		std::cerr << "set int64" << int64_t << endl;
		return true;
	}
#endif
	bool set_number(double f)
	{
		std::cerr << "set number : " << f << std::endl;
		return true;
	}
	template <typename Iter> bool parse_string(picojson::input<Iter> &in)
	{
		std::cerr << "parse string : ";
		picojson::value out = picojson::value(picojson::string_type, false);

		if (picojson::_parse_string(out.get<std::string>(), in)) {
			std::cerr << out << std::endl;

		} else {
			std::cerr << "string value invalid" << std::endl;
			return false;
		}

		return true;
	}
	bool parse_array_start()
	{
		std::cerr << "array start" << std::endl;

		switch (state_) {
		case MIXCONTEXT_GROUPS: {
				groups = picojson::value(picojson::array_type, false);
				break;
			}

		case MIXCONTEXT_PARAMETERS: {
				parameters = picojson::value(picojson::array_type, false);
				break;
			}

		case MIXCONTEXT_VARIABLES: {
				variables = picojson::value(picojson::array_type, false);
				break;
			}
		}

		return true;
	}

	template <typename Iter> bool parse_array_item(picojson::input<Iter> &in, size_t)
	{
		std::cerr << "array item" << std::endl;

		switch (state_) {
		case MIXCONTEXT_GROUPS: {
				picojson::array &a = groups.get<picojson::array>();
				a.push_back(picojson::value());
				picojson::default_parse_context ctx(&a.back());

				if (!picojson::_parse(ctx, in)) {
					return false;
				}

				return true;
				break;
			}

		case MIXCONTEXT_PARAMETERS: {
				picojson::array &a = parameters.get<picojson::array>();
				a.push_back(picojson::value());
				picojson::default_parse_context ctx(&a.back());

				if (!picojson::_parse(ctx, in)) {
					return false;
				}

				return true;
				break;
			}

		case MIXCONTEXT_VARIABLES: {
				picojson::array &a = variables.get<picojson::array>();
				a.push_back(picojson::value());
				picojson::default_parse_context ctx(&a.back());

				if (!picojson::_parse(ctx, in)) {
					return false;
				}

				return true;
				break;
			}

		default:
			break;
		}

		if (!picojson::_parse(*this, in)) {
			return false;
		}

		return true;
	}

	bool parse_array_stop(size_t)
	{
		std::cerr << "array stop" << std::endl;
		return true;
	}
	bool parse_object_start()
	{
		std::cerr << "object start" << std::endl;
		return true;
	}
	template <typename Iter> bool parse_object_item(picojson::input<Iter> &in, const std::string &key)
	{
		std::cerr << "parse object item : " << key << std::endl;

		if (key == "mixer_config") {
			state_ = MIXCONTEXT_MIXER_CONFIG;

		} else if (key == "groups") {
			state_ = MIXCONTEXT_GROUPS;

		} else if (key == "parameters") {
			state_ = MIXCONTEXT_PARAMETERS;

		} else if (key == "variables") {
			state_ = MIXCONTEXT_VARIABLES;

		} else if (key == "mixers") {
			state_ = MIXCONTEXT_MIXERS;
		}

		return picojson::_parse(*this, in);
	}

private:
	mixer_context(const mixer_context &);
	mixer_context &operator=(const mixer_context &);
};
}       //namespace


/****************************************************************************/


MixerPicoJsonParser::MixerPicoJsonParser(MixerDataParser *data_parser)
	: MixerScriptParser(data_parser)
{
}

int
MixerPicoJsonParser::parse_stream(std::istream *is)
{
	picojson::value out;
	std::string err;

//    picojson::default_parse_context ctx(&out);
//    picojson::_parse(ctx, std::istreambuf_iterator<char>(*is), std::istreambuf_iterator<char>(), &err);

	mixer_context ctx(&out);
	picojson::_parse(ctx, std::istreambuf_iterator<char>(*is), std::istreambuf_iterator<char>(), &err);

	std::cerr << picojson::get_last_error() << std::endl;

	if (!err.empty()) {
		std::cerr << err << std::endl;
		return -1;
	}

	std::cerr << "---- groups ----" << std::endl;
	std::cerr << ctx.groups.serialize(true) << std::endl;

	if (!ctx.groups.is<picojson::array>()) {
		std::cerr << "picojson - error - group is not an array" << std::endl;
		return -1;
	}

	for (picojson::value const &group : ctx.groups.get<picojson::array>()) {
		if (!group.contains("name")) {
			std::cerr << "group does not contain name" << std::endl;
			return -1;
		}

		picojson::value nameval = group.get("name");

		if (!nameval.is<std::string>()) {
			std::cerr << "group name is not a string " << std::endl;
			return -1;
		}

		string str = nameval.get<std::string>();
		std::cerr << "group name is " << str << std::endl;

		if (!group.contains("index")) {
			std::cerr << "group does not contain index" << std::endl;
			return -1;
		}

		picojson::value indexval = group.get("index");

		if (!indexval.is<double>()) {
			std::cerr << "group index is not a value " << std::endl;
			return -1;
		}

		double index = indexval.get<double>();
		std::cerr << "group index is : " << index << std::endl;
	}

	std::cerr << std::endl << "---- parameters ----" << std::endl;
	std::cerr << ctx.parameters.serialize(true) << std::endl;

	if (!ctx.parameters.is<picojson::array>()) {
		std::cerr << "picojson - error - group is not an array" << std::endl;
		return -1;
	}

	int param_count = 0;
	int value_count = 0;

	for (picojson::value const &param : ctx.parameters.get<picojson::array>()) {
		//Param ID
		if (!param.contains("id")) {
			std::cerr << "param does not contain id" << std::endl;
			return -1;
		}

		picojson::value idval = param.get("id");

		if (!idval.is<std::string>()) {
			std::cerr << "param id value is not a string" << std::endl;
			return -1;
		}

		std::cerr << "param id is " << idval.get<std::string>() << std::endl;

		//Param value
		if (!param.contains("value")) {
			std::cerr << "param does not contain value" << std::endl;
			return -1;
		}

		picojson::value paramval = param.get("value");

		if (paramval.is<double>()) {
			double value =  paramval.get<double>();
			std::cerr << "param value is double :" << value << std::endl;
			value_count++;

		} else if (paramval.is<picojson::array>()) {
			auto valarray = paramval.get<picojson::array>();
			value_count += valarray.size();
			std::cerr << "param value is array" << std::endl;

		} else {
			std::cerr << "param value is invalid type" << std::endl;
		}

		param_count++;
	}

	std::cerr << "param_count:" << param_count << "  value_count:" << value_count << std::endl;

	std::cerr << std::endl << "---- variables ----" << std::endl;
	std::cerr << ctx.variables.serialize(true) << std::endl;

	return 0;
}
