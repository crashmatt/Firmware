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
 * @file mixer.c
 *
 * Mixer utility.
 */

#include <px4_config.h>
#include <px4_posix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#include <systemlib/err.h>
#include <systemlib/mixer/mixer.h>
#include <uORB/topics/actuator_controls.h>

#define MIXER_CONFIG_NO_NSH

#if (defined(MIXER_CONFIGURATION) && !defined(MIXER_CONFIG_NO_NSH))
#include <systemlib/mixer/mixer_parameters.h>
#include <systemlib/mixer/mixer_type_id.h>
static const char *mixer_parameter_table[][MIXER_PARAMETERS_MIXER_TYPE_COUNT] = MIXER_PARAMETER_TABLE;
static const unsigned mixer_parameter_count[MIXER_PARAMETERS_MIXER_TYPE_COUNT] = MIXER_PARAMETER_COUNTS;
#endif //defined(MIXER_CONFIGURATION)

/**
 * Mixer utility for loading mixer files to devices
 *
 * @ingroup apps
 */
extern "C" __EXPORT int mixer_main(int argc, char *argv[]);

static void	usage(const char *reason);
static int	load(const char *devname, const char *fname);

#if (defined(MIXER_CONFIGURATION) && !defined(MIXER_CONFIG_NO_NSH))
static int	save(const char *devname, const char *fname);
static int  mixer_list(const char *devname);
static int  mixer_param_list(const char *devname, int mix_index, int sub_index);
static int  mixer_param_set(const char *devname, int mix_index, int sub_index, int param_index, float value);
static int  mixer_show_config(const char *devname);
#endif //defined(MIXER_CONFIGURATION)

int
mixer_main(int argc, char *argv[])
{
	if (argc < 2) {
		usage("missing command");
		return 1;
	}

	if (!strcmp(argv[1], "load")) {
		if (argc < 4) {
			usage("missing device or filename");
			return 1;
		}

		int ret = load(argv[2], argv[3]);

		if (ret != 0) {
			warnx("failed to load mixer");
			return 1;
		}


#if (defined(MIXER_CONFIGURATION) && !defined(MIXER_CONFIG_NO_NSH))

	}  else if (!strcmp(argv[1], "save")) {
		if (argc < 4) {
			usage("missing device or filename");
			return 1;
		}

		int ret = save(argv[2], argv[3]);

		if (ret != 0) {
			warnx("failed to save mixer");
			return 1;
		}

	} else if (!strcmp(argv[1], "list")) {
		if (argc < 3) {
			usage("missing device");
			return 1;
		}

		int ret = mixer_list(argv[2]);

		if (ret != 0) {
			warnx("failed to list mixers");
			return 1;
		}

	}  else if (!strcmp(argv[1], "params")) {
		if (argc < 4) {
			usage("missing device or mixer index");
			return 1;
		}

		int ret;

		if (argc == 4) {
			ret = mixer_param_list(argv[2], strtoul(argv[3], NULL, 0), 0);

		} else {
			ret = mixer_param_list(argv[2], strtoul(argv[3], NULL, 0), strtoul(argv[4], NULL, 0));
		}


		if (ret != 0) {
			warnx("failed to list parameters");
			return 1;
		}

	} else if (!strcmp(argv[1], "set")) {
		if (argc < 6) {
			usage("missing device, mixer index, parameter index or value");
			return 1;
		}

		int ret;

		if (argc == 6)
			ret = mixer_param_set(argv[2],
					      0,
					      strtoul(argv[3], NULL, 0),
					      strtoul(argv[4], NULL, 0),
					      (float) strtod(argv[5], 0));
		else
			ret = mixer_param_set(argv[2],
					      strtoul(argv[3], NULL, 0),
					      strtoul(argv[4], NULL, 0),
					      strtoul(argv[5], NULL, 0),
					      (float) strtod(argv[6], 0));

		if (ret != 0) {
			warnx("failed to list parameters");
			return 1;
		}

	} else if (!strcmp(argv[1], "config")) {
		if (argc < 2) {
			warnx("missing device: usage 'mixer config <device>'");
			return 1;
		}

		int ret = mixer_show_config(argv[2]);

		if (ret != 0) {
			warnx("failed to show config");
			return 1;
		}

#endif //defined(MIXER_CONFIGURATION)

	} else {
		usage("Unknown command");
		return 1;
	}

	return 0;
}

static void
usage(const char *reason)
{
	if (reason && *reason) {
		PX4_INFO("%s", reason);
	}

	PX4_INFO("usage:");
	PX4_INFO("  mixer load <device> <filename>");
#if (defined(MIXER_CONFIGURATION) && !defined(MIXER_CONFIG_NO_NSH))
	PX4_INFO("  mixer save <device> <filename>");
	PX4_INFO("  mixer list <device>");
	PX4_INFO("  mixer params <device> <mixer_index>");
	PX4_INFO("  mixer params <device> <mixer_index> <sub_index>");
	PX4_INFO("  mixer set <device> <mixer_index> <param_index> <value>");
	PX4_INFO("  mixer set <device> <mixer_index> <sub_index> <param_index> <value>");
	PX4_INFO("  mixer config <device>");
#endif //defined(MIXER_CONFIGURATION)
}

static int
load(const char *devname, const char *fname)
{
	// sleep a while to ensure device has been set up
	usleep(20000);

	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return 1;
	}

	/* reset mixers on the device */
	if (px4_ioctl(dev, MIXERIOCRESET, 0)) {
		warnx("can't reset mixers on %s", devname);
		return 1;
	}

	char buf[2048];

	if (load_mixer_file(fname, &buf[0], sizeof(buf)) < 0) {
		warnx("can't load mixer: %s", fname);
		return 1;
	}

	/* Pass the buffer to the device */
	int ret = px4_ioctl(dev, MIXERIOCLOADBUF, (unsigned long)buf);

	if (ret < 0) {
		warnx("error loading mixers from %s", fname);
		return 1;
	}

	return 0;
}

#if (defined(MIXER_CONFIGURATION) && !defined(MIXER_CONFIG_NO_NSH))
static int
save(const char *devname, const char *fname)
{
	// sleep a while to ensure device has been set up
	usleep(20000);

	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return -1;
	}

	char buf[2048];

	int ret = px4_ioctl(dev, MIXERIOCGETCONFIG, (unsigned long)buf);

	px4_close(dev);

	if (ret != 0) {
		warnx("Could not get mixer config for %s\n", devname);
		return -1;
	}

	/* Create the mixer definition file */
#ifdef __PX4_NUTTX
	int fd = open(fname, O_CREAT | O_WRONLY | O_DSYNC);
#else
	int fd = open(fname, O_CREAT | O_WRONLY | O_DSYNC, PX4_O_MODE_666);
#endif

	if (fd < 0) {
		warnx("not able to create file %s", fname);
		return -1;
	}

	int buflen = strlen(buf);

	/* Write the buffer to the file*/
	ssize_t wr_len = write(fd, buf, strlen(buf));

	if (wr_len != buflen) {
		warnx("not able to fully write to file %s", fname);

	} else {
		PX4_INFO("Wrote mixer %s to file %s\n", devname, fname);
	}

	fsync(fd);
	close(fd);

	return 0;
}

static int  mixer_show_config(const char *devname)
{
	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return 1;
	}

	char buf[2048];

	/* Pass the buffer to the device */
	int ret = px4_ioctl(dev, MIXERIOCGETCONFIG, (unsigned long)buf);

	px4_close(dev);

	if (ret == 0) {
		printf("%s", buf);

	} else {
		warnx("Could not generate mixer config for %s\n", devname);
		return 1;
	}

	return 0;
}


static int
mixer_list(const char *devname)
{
	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return 1;
	}

	int mix_count;
	mixer_type_s type;

	/* Get the mixer count */
	int ret = px4_ioctl(dev, MIXERIOCGETMIXERCOUNT, (unsigned long)&mix_count);

	if (ret != 0) {
		warnx("can't get mixer count for:%s", devname);
		px4_close(dev);
		return 1;
	}

	PX4_INFO("List of mixers:");

	printf("Mixer count : %u \n", mix_count);

	int submixer_count = 0;
	int submixer = 0;

	for (int index = 0; index < mix_count; index++) {
		printf("mixer:%u", index);

		submixer_count = index;
		ret = px4_ioctl(dev, MIXERIOCGETSUBMIXERCOUNT, (unsigned long)&submixer_count);

		if (ret != 0) {
			warnx("can't get submixer count. Failure code %i", ret);
			px4_close(dev);
			return 1;
		}

		printf(" submixers:%u ", submixer_count);

		/* Get the mixer type*/
		type.mix_index = index;
		type.mix_sub_index = 0;
		ret = px4_ioctl(dev, MIXERIOCGETTYPE, (unsigned long)&type);

		if (ret < 0) {
			warnx("can't get mixer type. Failure code %i", ret);
			px4_close(dev);
			return 1;
		}

		printf("type:%u id:%s", type.mix_type, MIXER_TYPE_ID[type.mix_type]);
		printf("\n");

		for (submixer = 1; submixer <= submixer_count; submixer++) {
			/* Get the submixer type*/
			type.mix_index = index;
			type.mix_sub_index = submixer;
			ret = px4_ioctl(dev, MIXERIOCGETTYPE, (unsigned long)&type);

			if (ret < 0) {
				warnx("can't get submixer type");
				px4_close(dev);
				return 1;
			}

			printf("mixer:%u  submixer:%u type:%u id:%s", index, submixer, type.mix_type, MIXER_TYPE_ID[type.mix_type]);
			printf("\n");
		}
	}

	px4_close(dev);
	return 0;
}


static int
mixer_param_list(const char *devname, int mix_index, int sub_index)
{
	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return 1;
	}

	/* Get the mixer or submixer type*/
	mixer_type_s mixer_type;
	mixer_type.mix_index = mix_index;
	mixer_type.mix_sub_index = sub_index;
	int ret = px4_ioctl(dev, MIXERIOCGETTYPE, (unsigned long)&mixer_type);

	px4_close(dev);

	if (ret < 0) {
		warnx("can't get mixer:%s type for mixer %u sub mixer:%u", devname, mix_index, sub_index);
		return 1;
	}

	/* Get the mixer or submixer parameter count*/
	int param_count = mixer_parameter_count[mixer_type.mix_type];

	if (param_count == 0) {
		printf("mixer:%u  parameter list empty\n", mix_index);
		return 1;
	}

	mixer_param_s param;

	for (int index = 0; index < param_count; index++) {
		param.mix_index = mix_index;
		param.mix_sub_index = sub_index;
		param.param_index = index;
		ret = px4_ioctl(dev, MIXERIOCGETPARAM, (unsigned long)&param);

		if (ret < 0) {
			warnx("can't get submixer parameter");
			return 1;
		}

		printf("mixer:%u  sub_mix:%u param:%u id:%s value:%.4f\n", mix_index, sub_index, index,
		       mixer_parameter_table[mixer_type.mix_type][index],
		       (double) param.value);
	}

	return 0;
}


static int
mixer_param_set(const char *devname, int mix_index, int sub_index, int param_index, float value)
{
	mixer_param_s param;

	int dev;

	/* open the device */
	if ((dev = px4_open(devname, 0)) < 0) {
		warnx("can't open %s\n", devname);
		return 1;
	}

	param.mix_index = mix_index;
	param.param_index = param_index;
	param.mix_sub_index = sub_index;
	param.value = value;

	int ret = px4_ioctl(dev, MIXERIOCSETPARAM, (unsigned long)&param);

	px4_close(dev);

	if (ret == 0) {
		printf("mixer:%u sub_mixer:%u param:%u value:%.4f set success\n", param.mix_index, param.mix_sub_index,
		       param.param_index,
		       (double) param.value);
		return 0;

	} else {
		warnx("fail to set mixer parameter");
		return -1;
	}
}
#endif //defined(MIXER_CONFIGURATION)
