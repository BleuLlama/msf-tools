# Makefile for msf-dump
#

TARG := msf-dump

SRCS := \
	DiskUtils.cpp \
	VoiceFile.cpp \
	msf-dump.cpp

OBJS := $(SRCS:%.cpp=%.o)


all: $(TARG)

%.o: %.cpp
	@echo $(CXX) $<
	@$(CXX) $(CFLAGS) $(DEFS) $(INCS) -c -o $@ $<

$(TARG): $(OBJS)
	@echo Link $@
	@$(CXX) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@

clean:
	@echo Remove build files
	@-rm -f $(OBJS) $(TARG) $(TARG).exe

test: $(TARG)
	./$(TARG) /Volumes/IC_RECORDER/Voice/
