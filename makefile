# Makefile for msf-dump
#

TARGA := msf-autosave
TARGD := msf-dump

SRCSCOMMON := \
	DiskUtils.cpp \
	VoiceFile.cpp

SRCSD := $(SRCSCOMMON) \
	msf-dump.cpp

SRCSA := $(SRCSCOMMON) \
	msf-autosave.cpp

OBJSA := $(SRCSA:%.cpp=%.o)

OBJSD := $(SRCSD:%.cpp=%.o)


CFLAGS += -Wall -pedantic


all: $(TARGA) $(TARGD)

%.o: %.cpp
	@echo $(CXX) $<
	@$(CXX) $(CFLAGS) $(DEFS) $(INCS) -c -o $@ $<

$(TARGD): $(OBJSD)
	@echo Link $@
	@$(CXX) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@

$(TARGA): $(OBJSA)
	@echo Link $@
	@$(CXX) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


clean:
	@echo Remove build files
	@-rm -f $(OBJSA) $(TARGA) $(TARGA).exe
	@-rm -f $(OBJSD) $(TARGD) $(TARGD).exe

testd: $(TARGD)
	./$(TARGD) /Volumes/IC_RECORDER/Voice/

testa: $(TARGA)
	./$(TARGA) saved
