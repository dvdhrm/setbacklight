#
# setbacklight - makefile
# Written 2011 by David Herrmann <dh.herrmann@googlemail.com>
# Dedicated to the Public Domain
#

.PHONY: all clean install uninstall

all:
	gcc -Wall -O2 -o setbacklight src/main.c src/driver.c

clean:
	rm -f setbacklight

install:
	install -m 4755 setbacklight /usr/bin

uninstall:
	rm /usr/bin/setbacklight
