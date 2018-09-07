
	# build, source, include directories, precompiled header
DIR_BUILD = build/
DIR_SRC   = ./
DIR_INC   = ./
PCH       = # headers.hpp

COMPILER = g++ -std=c++11

	# compiler flags
CFLAGS  = -I. -I$(DIR_INC) -Wall -fPIE # -O3

	# linker flags
LDFLAGS  = -pie

	# clean extensions
FILES_CLEAN = *.o

	# output file name
OUTPUT = junior1

	# main dependencies
MAINDEP = $(DIR_BUILD)$(OUTPUT)

ifneq ($(PCH),)
	MAINDEP += $(DIR_INC)$(PCH).gch
endif

	# list of source files
SRC = $(wildcard $(DIR_SRC)*.cpp)

	# list of header files
HDR = $(wildcard $(DIR_INC)*.hpp)

	# list of object files
OBJ = $(patsubst $(DIR_SRC)%,$(DIR_BUILD)obj/%,$(foreach source, $(basename $(SRC)),$(source).o))

	# ignore implicit chain and therefore deletion of object files
.SECONDARY: $(OBJECTS_NORMAL)

	# define phony targets for make commands
.PHONY: all all_init clean run

all: all_init $(MAINDEP)
	@printf "\nCompilation done - No Errors.\n"

all_init:
	@printf "\n#### compiling project\n"
	mkdir -p $(DIR_BUILD)obj/

$(DIR_BUILD)$(OUTPUT): $(OBJ)
	@printf "\n#### linking project\n"
	$(COMPILER) $(OBJ) -o $@ $(LDFLAGS)

$(DIR_INC)%.gch: $(DIR_INC)$(PCH)
	@echo "\n#### compiling precompiled header"
	$(COMPILER) -o $@ $< $(CFLAGS)

$(DIR_BUILD)obj/%.o: $(DIR_SRC)%.cpp $(HDR)
	@printf "\n#### compiling \"$@\"\n"
	$(COMPILER) -o $@ -c $(CFLAGS) $<

run: all
	@printf "\n#### executing $(OUTPUT)\n"
	$(DIR_BUILD)$(OUTPUT) $(ARGS)

clean:
	@printf "\n#### cleaning project\n"
	rm -rf $(DIR_BUILD) $(DIR_INC)$(PCH).gch
