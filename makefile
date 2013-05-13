# Makefile for msf-dump
#

TARG := msf-dump

SRCS := \
	msf-dump.cpp

OBJS := $(SRCS:%.cpp=%.o)


all: $(TARG)

%.o: %.cpp
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
