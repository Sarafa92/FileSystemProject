CCOPTS= -Wall -g -std=gnu99 -Wstrict-prototypes
LIBS=
CC=gcc
AR=ar


BINS= simplefs_test

OBJS = simplefs_test.c simplefs.c disk_driver.c bitmap.c

HEADERS=simplefs.h\
			disk_driver.h\
			bitmap.h

%.o:	%.c $(HEADERS)
		$(CC) $(CCOPTS) -c -o $@  $<

.phony: clean all


all:	$(BINS)

simplefs_test: simplefs_test.c $(OBJS)
	$(CC) $(CCOPTS)  -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *~  simplefs_test
