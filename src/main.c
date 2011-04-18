/*
 * setbacklight - user interface
 * Written 2011 by David Herrmann <dh.herrmann@googlemail.com>
 * Dedicated to the Public Domain
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver.h"

static bool parse_args(int argc, char **argv, bool *abs, long *val, bool *get)
{
	long tmp;

	*abs = true;
	*val = 0xff;
	*get = false;

	if (argc > 1) {
		if (0 == strcmp(argv[1], "get")) {
			*get = true;
		}
		else if (argv[1][0] == '%') {
			if (argv[1][1] == '+' || argv[1][1] == '-')
				*abs = false;
			tmp = strtol(&argv[1][1], NULL, 10);
			if (*abs) {
				if (tmp < 0)
					tmp = 0;
				else if (tmp > 100)
					tmp = 100;
			}
			else {
				if (tmp < -100)
					tmp = -100;
				else if (tmp > 100)
					tmp = 100;
			}
			*val = tmp * 0xff / 100;
		}
		else {
			if (argv[1][0] == '+' || argv[1][0] == '-')
				*abs = false;
			tmp = strtol(&argv[1][0], NULL, 16);
			if (*abs) {
				if (tmp < 0)
					tmp = 0;
				else if (tmp > 0xff)
					tmp = 0xff;
			}
			else {
				if (tmp < -0xff)
					tmp = -0xff;
				else if (tmp > 0xff)
					tmp = 0xff;
			}
			*val = tmp;
		}
		return true;
	}
	else {
		fprintf(stderr, "Usage: setbacklight [%%][#val]\n");
		fprintf(stderr, "       setbacklight [%%][+-][#val]\n");
		fprintf(stderr, "       setbacklight get\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "Written by David Herrmann, 2011\n");
		fprintf(stderr, "Dedicated to the Public Domain\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "This frontend has only support for Samsung N210 netbook.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "If [#val] is prefixed with a '%%' then the value is clamped\n");
		fprintf(stderr, "to the range 0-100 and interpreted as decimal percentage value.\n");
		fprintf(stderr, "Otherwise, [#val] is a hexadecimal value which is clamped to the\n");
		fprintf(stderr, "range 0 to ff.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "If the value is prefixed with '+' or '-', then the current\n");
		fprintf(stderr, "brightness is modified with the specified value, otherwise\n");
		fprintf(stderr, "the current brightness value is overwritten.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "If the first parameter is 'get', then the application reads the\n");
		fprintf(stderr, "current brightness value, prints it to stdout and returns.\n");
		fprintf(stderr, "Otherwise, the current brightness level is changed.\n");
		fprintf(stderr, "The application returns 0 on success.\n");
		return false;
	}
}

int main(int argc, char **argv)
{
	struct setbl ctx;
	long change;
	bool abs, get;
	uint8_t val;

	if (!parse_args(argc, argv, &abs, &change, &get))
		return EXIT_FAILURE;

	if (0 != setbl_init_N210(&ctx, "/usr/sbin/setpci")) {
		fprintf(stderr, "Cannot initialize backlight driver\n");
		return EXIT_FAILURE;
	}

	if (get) {
		if (0 != setbl_get(&ctx, &val)) {
			fprintf(stderr, "Cannot read current brightness level\n");
			setbl_deinit(&ctx);
			return EXIT_FAILURE;
		}
		fprintf(stdout, "%02x\n", val);
		setbl_deinit(&ctx);
		return EXIT_SUCCESS;
	}

	if (!abs) {
		if (0 != setbl_get(&ctx, &val)) {
			fprintf(stderr, "Cannot read current brightness level\n");
			setbl_deinit(&ctx);
			return EXIT_FAILURE;
		}
		change = val + change;
		if (change < 0)
			change = 0;
		else if (change > 0xff)
			change = 0xff;
	}

	val = change;
	if (0 != setbl_set(&ctx, val)) {
		fprintf(stderr, "Cannot set brightness level\n");
		setbl_deinit(&ctx);
		return EXIT_FAILURE;
	}

	setbl_deinit(&ctx);
	return EXIT_SUCCESS;
}
