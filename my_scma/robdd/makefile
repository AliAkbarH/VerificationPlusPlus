all: robdd

spec: robdd
	./robdd


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
D=-D_ROBDD_H_
GCC=gcc ${D} ${MYFLAGS}
ROBDD_DIR=robdd
CCSOURCES_ROBDD=${wildcard *.c}
CXXSOURCES_ROBDD=${wildcard *.cpp}


#mohammad
SOURCE_DEPENDENCIES=$(patsubst %.c,$(BIN_DIR)/%.d,$(CCSOURCES_ROBDD))  \
					$(patsubst %.cpp,$(BIN_DIR)/%.d,$(CXXSOURCES_ROBDD))  \
    

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

#echoflags:
#	@echo $(CXXFLAGS)
#	@echo $(CFLAGS)

# this is placed here to allow testing echo targets above
#
all: $(TARGET)


robdd: $(BASEOBJECTS)
#	$(CXX) -o sc $(BASEOBJECTS) -L z3/lib/ -lz3 $(ANTLR_LIB_LFLAG)
	$(CXX) -o robdd $(BASEOBJECTS) 


#rules to extract dependency files

#mohammad
$(BIN_DIR)/%.d: %.c
	@echo "CC CFLAGS -M $< > $@"
	@$(GCC) $(MYFLAGS) -M $< > $@
#mohammad
$(BIN_DIR)/%.d: %.cpp
	@echo "CXX CXXFLAGS -M $< > $@"
	@$(CXX) $(CXXFLAGS) -M $< > $@

#rules to make object files from c/cplusplus code


#mohammad
$(BIN_DIR)/%.o: %.c $(BIN_DIR)/%.d
	@echo "CC CFLAGS -c -o $@ $<"
	@$(GCC) $(MYFLAGS) -c -o $@ $<
#mohammad
$(BIN_DIR)/%.o: %.cpp $(BIN_DIR)/%.d
	@echo "CXX CXXFLAGS -c -o $@ $<"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

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
