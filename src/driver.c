/*
 * setbacklight - driver api
 * Written 2011 by David Herrmann <dh.herrmann@googlemail.com>
 * Dedicated to the Public Domain
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "driver.h"

int setbl_init(struct setbl *ctx, const char *path, const char *bus, const char *reg)
{
	size_t len;

	if (!*path || !*bus || !*reg)
		return -1;

	ctx->path = strdup(path);
	if (!ctx->path)
		return -1;

	ctx->bus = strdup(bus);
	if (!ctx->bus) {
		free(ctx->path);
		return -1;
	}

	len = strlen(reg);
	ctx->set = malloc(len + 4);
	if (!ctx->set) {
		free(ctx->bus);
		free(ctx->path);
		return -1;
	}
	sprintf(ctx->set, "%s=00", reg);
	ctx->setval = &ctx->set[len];
	return 0;
}

void setbl_deinit(struct setbl *ctx)
{
	free(ctx->set);
	free(ctx->bus);
	free(ctx->path);
}

static int cmd_exec(const char *path, const char *bus, const char *set, int writefd)
{
	pid_t child_pid;
	int child_ret;

	child_ret = -1;
	child_pid = fork();
	if (child_pid == 0) {
		if (writefd >= 0) {
			close(1);
			if (-1 == dup2(writefd, 1))
				exit(-2);
		}
		execl(path, "setpci", "-s", bus, set, (char*)0);
		exit(-1);
	}
	else if (child_pid > 0) {
		while (child_pid != waitpid(child_pid, &child_ret, 0) && errno == EINTR)
			/* empty */ ;
	}

	close(writefd);
	return child_ret;
}

int setbl_get(struct setbl *ctx, uint8_t *brightness)
{
	int pair[2];
	char buf[3];

	if (0 != pipe(pair))
		return -1;

	ctx->setval[0] = 0;
	if (cmd_exec(ctx->path, ctx->bus, ctx->set, pair[1]) != 0) {
		close(pair[0]);
		return -2;
	}

	if (2 != read(pair[0], buf, 2)) {
		close(pair[0]);
		return -3;
	}
	close(pair[0]);

	buf[sizeof(buf) - 1] = 0;
	*brightness = strtoul(buf, NULL, 16);
	return 0;
}

int setbl_set(struct setbl *ctx, uint8_t brightness)
{
	snprintf(ctx->setval, 4, "=%02x", brightness);
	return cmd_exec(ctx->path, ctx->bus, ctx->set, -1);
}
