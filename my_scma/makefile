#
# file:      makefile
# author:    F. Zaraket
# created:   feb 12, 2012
#-------------------------------------
# description: 
#    In this file we have macros that set necessary directives needed
#    to build code in a directory.
#-------------------------------------
# Kindly add below your modification stamps in the following format.
# Month Day,Year    Name    comment for modification
#
# Feb 12,2010    fadi   initial creation
#
#
#
all: sc ma

spec: sc
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./z3/lib
	./sc

adequate: ma
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./z3/lib
	./ma

APP_VERSION=0.1

BASE:=$(shell pwd | xargs basename)
#echobase:
#	@echo $(BASE)

BIN_DIR=./bin
CFLAGS=-std=c99
CPPSOURCES=${wildcard *.cpp}
CPP_FILTER_SOURCES=${filter-out moc_%.cpp %Lexer.cpp %Parser.cpp,$(CPPSOURCES)}

CSOURCES=${wildcard *.c}
C_FILTER_SOURCES=${filter-out %Lexer.c %Parser.c,$(CSOURCES)}

CCSOURCES=${wildcard *.C}
CXXSOURCES=${wildcard *.cxx}

#mohammad
# call with make D='-DDEBUG -g' for debug mode
MYFLAGS= -Wall -Wextra -std=c99 -pedantic -Werror -Wno-pragmas
# D=-DDEBUG -g
GCC=gcc ${D} ${MYFLAGS}
ROBDD_DIR=robdd
CCSOURCES_ROBDD=$(subst robdd/,,${wildcard robdd/*.c})

ANTLR_DIR=antlr
ANTLR_LIB=$(ANTLR_DIR)/libantlr3c-3.2/.libs/libantlr3c.a
ANTLR_LIB_LFLAG=-L$(ANTLR_DIR)/libantlr3c-3.2/.libs/ -lantlr3c
ANTLR_JAR=$(ANTLR_DIR)/antlr-3.2.jar

#original source code
ANTLR_GRAMMARS=${wildcard *.g}

#code that antler will generate
ANTLR_LEX_SOURCES=$(patsubst %.g,%Lexer.cpp, $(ANTLR_GRAMMARS))
ANTLR_PRS_SOURCES=$(patsubst %.g,%Parser.cpp, $(ANTLR_GRAMMARS))
ANTLR_SOURCES=$(ANTLR_LEX_SOURCES) $(ANTLR_PRS_SOURCES)
#ANTLR_DEPENDENCIES=$(ANTLR_SOURCES:.cpp=.d)
ANTLR_DEPENDENCIES=$(patsubst %.cpp,$(BIN_DIR)/%.d,$(ANTLR_SOURCES))

ANTLR_INCLUDES=-I$(ANTLR_DIR)/libantlr3c-3.2 -I$(ANTLR_DIR)/libantlr3c-3.2/include

echo:
	@echo $(CPP_FILTER_SOURCES)
	@echo $(C_FILTER_SOURCES)
	@echo $(BASEOBJECTS)
	@echo $(ANTLR_DEPENDENCIES)

#SOURCE_DEPENDENCIES=$(CPP_FILTER_SOURCES:.cpp=.d) \
                    $(CCSOURCES:.C=.d) \
                    $(CXXSOURCES:.cxx=.d) \
                    $(C_FILTER_SOURCES:.c=.d) \
                    $(ANTLR_DEPENDENCIES) \
                    $(QTMOC_DEPENDENCIES) \
                    $(QTUIC_DEPENDENCIES)

#mohammad
SOURCE_DEPENDENCIES=$(patsubst %.cpp,$(BIN_DIR)/%.d,$(CPP_FILTER_SOURCES)) \
                    $(patsubst %.C,$(BIN_DIR)/%.d,$(CCSOURCES))  \
					$(patsubst %.c,$(BIN_DIR)/%.d,$(CCSOURCES_ROBDD))  \
                    $(patsubst %.cxx,$(BIN_DIR)/%.d,$(CXXSOURCES)) \
                    $(patsubst %.c,$(BIN_DIR)/%.d,$(C_FILTER_SOURCES)) \
                    $(ANTLR_DEPENDENCIES) \
                    $(QTMOC_DEPENDENCIES) \
                    $(QTUIC_DEPENDENCIES)

ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
include $(SOURCE_DEPENDENCIES)
endif

ifneq ($(findstring clean,$(MAKECMDGOALS)),notiming)
	TIME=-DTIME_ON
else
	TIME=
endif

#antlrMacrosMakefile and qtmocMacrosMakefile can also contribute    
#echodep:
#	@echo $(SOURCE_DEPENDENCIES)

BASEOBJECTS=$(SOURCE_DEPENDENCIES:.d=.o)
#echoobj:
#	@echo $(BASEOBJECTS)

ifneq ($(findstring opt,$(MAKECMDGOALS)),opt)
    OPT_DEBUG=-g
else
    OPT_DEBUG=-O3
endif
CXXFLAGS+=$(OPT_DEBUG) $(TIME)
CFLAGS+=$(OPT_DEBUG) $(TIME)
LDDFLAGS+=$(OPT_DEBUG) $(TIME)

#the following is for experimental unordered_set
#CXXFLAGS+=-std=c++0x

#the following is for experimental future library
CXXFLAGS+=-std=c++11

ifneq ($(findstring 32,$(MAKECMDGOALS)),32)
    BITS=-m64
else
    BITS=-m32
endif
CXXFLAGS+=$(BITS)
CFLAGS+=$(BITS)
LDDFLAGS+=$(BITS)

CXXFLAGS+= -I . -I z3/include $(ANTLR_INCLUDES)
CXXFLAGS+= -isystem $(ROBDD_DIR)
#echoflags:
#	@echo $(CXXFLAGS)
#	@echo $(CFLAGS)

# this is placed here to allow testing echo targets above
#
all: $(TARGET)


sc: $(BASEOBJECTS)
#	$(CXX) -o sc $(BASEOBJECTS) -L z3/lib/ -lz3 $(ANTLR_LIB_LFLAG)
	$(CXX) -o sc $(BASEOBJECTS) -L z3/lib/ -lz3 ./antlr/libantlr3c-3.2/.libs/libantlr3c.a -lrt

ma: sc
	@ln -fs sc ma

#rules to extract dependency files
$(BIN_DIR)/%.d: %.cpp
	@echo "CXX CXXFLAGS -M $< > $@"
	@$(CXX) $(CXXFLAGS) -M $< > $@

$(BIN_DIR)/%.d: %.cxx
	@echo "CXX CXXFLAGS -M $< > $@"
	@$(CXX) $(CXXFLAGS) -M $< > $@

$(BIN_DIR)/%.d: %.C
	@echo "CXX CXXFLAGS -M $< > $@"
	@$(CC) $(CFLAGS) -M $< > $@

$(BIN_DIR)/%.d: %.c
	@echo "CC CFLAGS -M $< > $@"
	@$(CC) $(CFLAGS) -M $< > $@

#mohammad
$(BIN_DIR)/%.d: robdd/%.c
	@echo "CC CFLAGS -M $< > $@"
	@$(GCC) $(MYFLAGS) -M $< > $@

#rules to make object files from c/cplusplus code
$(BIN_DIR)/%.o: %.cpp $(BIN_DIR)/%.d
	@echo "CXX CXXFLAGS -c -o $@ $<"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_DIR)/%.o: %.cxx $(BIN_DIR)/%.d
	@echo "CXX CXXFLAGS -c -o $@ $<"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_DIR)/%.o: %.C $(BIN_DIR)/%.d
	@echo "CXX CXXFLAGS -c -o $@ $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BIN_DIR)/%Lexer.o: %Lexer.c $(BIN_DIR)/%Lexer.d
	@echo "CC Lexer CFLAGS -c -o $@ $<"
	@$(CC) $(CFLAGS) -Wno-unused -c -o $@ $<

$(BIN_DIR)/%Parser.o: %Parser.c $(BIN_DIR)/%Parser.d
	@echo "CC Parser CFLAGS -c -o $@ $<"
	@$(CC) $(CFLAGS) -Wno-unused -Wno-pointer-sign -Wno-missing-braces -c -o $@ $<

$(BIN_DIR)/%.o: %.c $(BIN_DIR)/%.d
	@echo "CC CFLAGS -c -o $@ $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

#mohammad
$(BIN_DIR)/%.o: robdd/%.c $(BIN_DIR)/%.d
	@echo "CC CFLAGS -c -o $@ $<"
	@$(GCC) $(MYFLAGS) -c -o $@ $<

clean: cleanaux
	@rm -rf $(TARGET)

cleanaux:
	@rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/*.d *.so *.a
	@rm -rf moc_*.cpp *Lexer.cpp *Parser.cpp *Lexer.h *Parser.h *Lexer.c *Parser.c *.tokens

#empty rules to allow parametrized make
opt: all

64: all

32: all

cyg: all

#rules to make C code from antler grammar
%Lexer.cpp %Parser.cpp: %.g
	@echo "java -jar $(ANTLR_JAR) $<"
	@java -jar $(ANTLR_JAR) $<
	mv $*Lexer.c $*Lexer.cpp
	mv $*Parser.c $*Parser.cpp

#rules to make CPP code from Qt Q_OBJECT files
moc_%.cpp: %.h
	@echo "MOC $< -o $@"
	@$(QTMOC_BIN) $(QT_MOCFLAGS) $< -o $@

moc_%.moc: %.cpp
	@echo "MOC $< -o $@"
	@$(QTMOC_BIN) $(QT_MOCFLAGS) -i $< -o $@

ui_%.h: %.ui
	@echo "UIC $< -o $@"
	@$(QTUIC_BIN) $(QT_UICFLAGS) $< -o $@
