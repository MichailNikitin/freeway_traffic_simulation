# Generated Makefile. При изменении этого файла вручную удалите данную строку

TARGET = release

CSOURCES = $(wildcard *.c)
CPPSOURCES = $(filter-out %_unittest.cpp %_featuretest.cpp,$(wildcard *.cpp))

SMSOURCES = $(wildcard statemachine*.uxf)
SMGENERH = $(patsubst %.uxf,$(TARGET)/%.hpp,$(SMSOURCES))
SMGENERCPP = $(patsubst %.uxf,$(TARGET)/%.cpp,$(SMSOURCES))
SMGENER =  $(SMGENERH) $(SMGENERCPP)


CAIOSOURCES = $(wildcard *.caio)
CAIOGENERCPP = $(patsubst %.caio,$(TARGET)/%.cpp,$(CAIOSOURCES)) $(patsubst %.caio,$(TARGET)/%.lex.cpp,$(CAIOSOURCES)) $(patsubst %.caio,$(TARGET)/%.grm.cpp,$(CAIOSOURCES))
CAIOGENERH = $(patsubst %.caio,$(TARGET)/%.hpp,$(CAIOSOURCES))
CAIOGENER =  $(CAIOGENERH) $(CAIOGENERCPP)

LEXSOURCE = $(firstword $(wildcard *.lex))
GRMSOURCE = $(firstword $(wildcard *.grm))

ifneq (,$(LEXSOURCE))
LEXGENER = $(TARGET)/lex.yy.cpp
else
LEXGENER =
endif

ifneq (,$(GRMSOURCE))
GRMGENER = $(TARGET)/y.tab.cpp
GRMHEADER = $(TARGET)/y.tab.hpp
else
GRMGENER =
GRMHEADER =
endif

CHEADERS = $(wildcard *.h)
CPPHEADERS = $(wildcard *.hpp)
ifeq ($(TARGET),debug)
CCFLAGS = -g -O0  
else
CCFLAGS = -O1  
endif
GCC = gcc -std=c11
GPP = g++ -std=c++20
LFLAGS = -Wl,--stack,83886080 -static
CPPLIBS =  conio2 nuklear bgi gdi32 msimg32 comdlg32 uuid oleaut32 ole32 reflex
CLIBS =  conio2 nuklear gdi32 msimg32
ifneq (,$(strip $(CPPSOURCES) $(LEXSOURCE) $(GRMSOURCE) $(CAIOSOURCES) $(SMSOURCES)))
CC = $(GPP)
LIBS = $(CPPLIBS)
OBJS = $(sort $(patsubst %.c,$(TARGET)/%.o,$(CSOURCES)) $(patsubst %.cpp,$(TARGET)/%.o,$(subst $(TARGET)/,,$(CPPSOURCES) $(LEXGENER) $(GRMGENER) $(CAIOGENERCPP) $(SMGENERCPP))))
else
CC = $(GCC)
LIBS = $(CLIBS)
OBJS = $(sort $(patsubst %.c,$(TARGET)/%.o,$(CSOURCES)))
endif

CCCS = -finput-charset=utf-8 -fexec-charset=windows-1251
CCCW = -finput-charset=windows-1251 -fexec-charset=windows-1251
HS = $(sort $(CHEADERS) $(GRMHEADER) $(CAIOGENERH) $(SMGENERH))
INIT="C:\Program Files\MinIDE\SciTE/../lib/initansicp.o"

.PHONY : all clean

all: $(TARGET)/_all_.exe

$(TARGET)/_all_.exe $(OBJS) $(GRMGENER) $(GRMHEADER) $(LEXGENER) $(CAIOGENER) $(SMGENER): | $(TARGET)

$(TARGET):
	mkdir $(TARGET)

$(TARGET)/lex.yy.o: | $(LEXGENER)

$(TARGET)/y.tab.o: | $(GRMGENER)

$(LEXGENER): $(LEXSOURCE)
	reflex --flex -o$(LEXGENER) $<

$(GRMGENER) $(GRMHEADER): $(GRMSOURCE)
	bison -Werror=conflicts-sr,conflicts-rr -Wcounterexamples -o$(GRMGENER) -d $<


$(TARGET)/%.cpp $(TARGET)/%.hpp: %.uxf
	smgen -o $(TARGET)/ $<

$(TARGET)/%.cpp $(TARGET)/%.hpp $(TARGET)/%.lex.cpp $(TARGET)/%.grm.cpp: %.caio
	caio -o$(TARGET)/ $<
	ifexist.cmd $(TARGET)/$(patsubst %.caio,%.lex,$<) "reflex --flex -o$(TARGET)/$(patsubst %.caio,%.lex.cpp,$<) $(TARGET)/$(patsubst %.caio,%.lex,$<)" "echo // >$(TARGET)/$(patsubst %.caio,%.lex.cpp,$<)"
	ifexist.cmd $(TARGET)/$(patsubst %.caio,%.grm,$<) "bison -Werror=conflicts-sr,conflicts-rr -Wcounterexamples -o$(TARGET)/$(patsubst %.caio,%.grm.cpp,$<) $(TARGET)/$(patsubst %.caio,%.grm,$<)" "echo // >$(TARGET)/$(patsubst %.caio,%.grm.cpp,$<)"

$(TARGET)/%.o: %.cpp $(CPPHEADERS) $(HS)
	$(GPP) $(CCFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $@ -I. -I$(TARGET) -c $<

$(TARGET)/%.o: $(TARGET)/%.cpp $(CPPHEADERS) $(HS)
	$(GPP) $(CCFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $@ -I. -I$(TARGET) -c $<

$(TARGET)/%.o: %.c $(HS)
	$(GCC) $(CCFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $@ -I. -I$(TARGET) -c $<

$(TARGET)/_all_.exe: $(OBJS) Makefile
	$(CC) $(CCFLAGS) $(LFLAGS) -o $(TARGET)/_all_ $(OBJS) $(INIT) $(addprefix -l,$(LIBS))

$(TARGET)/%.exe: %.lex Makefile
	$(if $(wildcard $(TARGET)),,mkdir $(TARGET))
	reflex --flex --bison -o$(TARGET)/lex.yy.cpp $<
	$(GPP) $(CCFLAGS) $(LFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $(TARGET)/$(patsubst %.lex,%,$<) -I. -I$(TARGET) $(TARGET)/lex.yy.cpp $(INIT) $(addprefix -l,$(CPPLIBS))

$(TARGET)/%.exe: %.grm Makefile
	$(if $(wildcard $(TARGET)),,mkdir $(TARGET))
	bison -Werror=conflicts-sr,conflicts-rr -Wcounterexamples -o$(TARGET)/y.tab.cpp $<
	$(GPP) $(CCFLAGS) $(LFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $(TARGET)/$(patsubst %.grm,%,$<) -I. -I$(TARGET) $(TARGET)/y.tab.cpp $(INIT) $(addprefix -l,$(CPPLIBS))

$(TARGET)/%.exe: %.caio Makefile
	$(if $(wildcard $(TARGET)),,mkdir $(TARGET))
	caio.exe -o$(TARGET)/ $<
	ifexist.cmd $(TARGET)/$(patsubst %.caio,%.lex,$<) "reflex --flex -o$(TARGET)/$(patsubst %.caio,%.lex.cpp,$<) $(TARGET)/$(patsubst %.caio,%.lex,$<)" "echo // >$(TARGET)/$(patsubst %.caio,%.lex.cpp,$<)"
	ifexist.cmd $(TARGET)/$(patsubst %.caio,%.grm,$<) "bison -Werror=conflicts-sr,conflicts-rr -Wcounterexamples -o$(TARGET)/$(patsubst %.caio,%.grm.cpp,$<) $(TARGET)/$(patsubst %.caio,%.grm,$<)" "echo // >$(TARGET)/$(patsubst %.caio,%.grm.cpp,$<)"
	$(GPP) $(CCFLAGS) $(LFLAGS) -o $(TARGET)/$(patsubst %.caio,%,$<) $(TARGET)/$(patsubst %.caio,%.cpp,$<) $(TARGET)/$(patsubst %.caio,%.lex.cpp,$<) $(TARGET)/$(patsubst %.caio,%.grm.cpp,$<) $(INIT) $(addprefix -l,$(CPPLIBS))

$(TARGET)/%.exe: %.c Makefile
	$(if $(wildcard $(TARGET)),,mkdir $(TARGET))
	$(GCC) $(CCFLAGS) $(LFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $(TARGET)/$(patsubst %.c,%,$<) -I. -I$(TARGET) $< $(INIT) $(addprefix -l,$(CLIBS))

$(TARGET)/%.exe: %.cpp Makefile
	$(if $(wildcard $(TARGET)),,mkdir $(TARGET))
	$(GPP) $(CCFLAGS) $(LFLAGS) $(if $(filter uc%,$<),$(CCCS),$(CCCW)) -o $(TARGET)/$(patsubst %.cpp,%,$<) -I. -I$(TARGET) $< $(INIT) $(addprefix -l,$(CPPLIBS))

clean:
	$(if $(wildcard release),rmdir /q /s release)
	$(if $(wildcard debug),rmdir /q /s debug)
	$(if $(wildcard coverage),rmdir /q /s coverage)
	$(if $(wildcard temp),rmdir /q /s temp)
	$(if $(wildcard *.orig),del *.orig)

