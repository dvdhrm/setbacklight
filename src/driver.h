/*
 * setbacklight - driver api
 * Written 2011 by David Herrmann <dh.herrmann@googlemail.com>
 * Dedicated to the Public Domain
 */

/*
 * Brightness Regulation API
 * This driver controls LCD backlight brightness regulation. It uses
 * internally "setpci -s bus reg=<val>", though, the binary path, the
 * busname and the registername can be changed.
 *
 * First you create a new context:
 *   setbl_init()
 * which returns 0 on success. The first argument is a pointer to an
 * unused setbl structure. The 3 other arguments are binary-path, busname
 * and registername.
 *
 * The context can be destroyed at any time with:
 *   setbl_deinit()
 *
 * With:
 *   setbl_get()
 * you can retrieve the current brightness value. The brightness value is
 * mapped from 0-255.
 * With:
 *   setbl_set()
 * you can set the brightness value.
 *
 *   setbl_init_N210()
 * This is a wrapper for setbl_init() with the same semantics but passes the
 * standard busname and registernames which are known to work on Samsung N210 netbooks.
 */

#ifndef SETBL_DRIVER_H
#define SETBL_DRIVER_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct setbl {
	char *path;
	char *bus;
	char *set;
	char *setval;
};

extern int setbl_init(struct setbl *ctx, const char *path, const char *bus, const char *reg);
extern void setbl_deinit(struct setbl *ctx);

extern int setbl_get(struct setbl *ctx, uint8_t *brightness);
extern int setbl_set(struct setbl *ctx, uint8_t brightness);

static inline int setbl_init_N210(struct setbl *ctx, const char *path)
{
	return setbl_init(ctx, path, "00:02.0", "F4.B");
}

#endif /* SETBL_DRIVER_H */
