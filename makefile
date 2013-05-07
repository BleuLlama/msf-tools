# Makefile for msf-dump
#

TARG := msf-dump

SRCS := \
	msf-dump.c

OBJS := $(SRCS:%.c=%.o)


all: $(TARG)

%.o: %.c
	@echo $(CC) $<
	@$(CC) $(CFLAGS) $(DEFS) $(INCS) -c -o $@ $<

msfdump: $(OBJS)
	@echo Link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@

clean:
	@echo Remove build files
	@-rm -f $(OBJS) $(TARG)

test: $(TARG)
	./$(TARG) /Volumes/IC_RECORDER/Voice/
