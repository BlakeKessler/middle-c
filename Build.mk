#compiler
COMPILER := clang++ -std=c++23
FLAGS := -g -Wall -Wextra -pedantic -pedantic-errors -ftemplate-backtrace-limit=4 -fdiagnostics-show-category=name -Wno-gcc-compat -Wno-trigraphs
# COMPILER := g++-14 -std=c++23
# FLAGS := -g -Wall -Wextra -pedantic -pedantic-errors -ftemplate-backtrace-limit=4 -Wno-trigraphs

#compilation paths
OUT_PATH := test/bin
TEST_SRC_DIR := test/src

#directories with code to compile
MODULES := mcsl/ app/
INC_DIRS := $(foreach root,$(wildcard $(MODULES)),-I$(root) $(foreach dir,$(wildcard $(root)*/),-I$(dir)header))

#expand source code paths
EXPAND_SRCS = $(foreach source,$(wildcard $^),$(source)/src/*.cpp)

#source code that is always included
FORCE_DIRS := app/_lib/ app/langSpecData/ mcsl/*/
FORCE_INC := $(foreach src,$(wildcard $(FORCE_DIRS)),$(src)src/*.cpp)

#compilation command units
COMPILE_OBJ_CMD = $(COMPILER) $(FLAGS) -c $(EXPAND_SRCS) $(FORCE_INC) $(TEST_SRC_DIR)/Test$@.cpp $(INC_DIRS) && mv ./*.o
COMPILE_OUT_CMD = $(COMPILER) $(FLAGS) $(OUT_PATH)/*.o -o $(OUT_PATH)/Test$@.out

#compile/build commands
CLEAN_CMD = rm -f $(OUT_PATH)/*.o
COMPILE_CMD = $(COMPILE_OBJ_CMD) $(OUT_PATH) && $(COMPILE_OUT_CMD)
BUILD_CMD = $(CLEAN_CMD) && $(COMPILE_CMD) && $(CLEAN_CMD)

#CLEF TARGETS
Lexer		:	app/phases app/structures app/elements app/data
	$(BUILD_CMD)
Parser	:	app/phases app/structures app/elements app/data
	$(BUILD_CMD)

#MIDDLE-C STANDARD LIBRARY TARGETS
StrToNum	:	mcsl/utils mcsl/math
	$(BUILD_CMD)
Benchmark:	mcsl/utils mcsl/math
	$(BUILD_CMD)
UintN		: mcsl/math
	$(BUILD_CMD)
MUL		: mcsl/math
	$(BUILD_CMD)
MCSL			:	mcsl/*/src/*
	rm -f $(OUT_PATH)/*.o										\
	&& $(COMPILER) $(FLAGS) -c $^ -Imcs						\
		$(foreach dir,$(wildcard mcsl/*/),-I$(dir)header)\
	&& mv ./*.o $(OUT_PATH)										\
	&& ar rcsv mcsl/mcs.a $(OUT_PATH)/*.o					\
	&& rm -f $(OUT_PATH)/*.o									\
	|| rm -f ./*.o
