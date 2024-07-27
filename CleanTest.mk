OUT_PATH := test/bin
TEST_SRC_DIR := test/src

COMPILER := clang++ -std=c++23
STAT_TESTER:= clang-tidy
FLAGS := -g -Wall -Wextra
MODULES := mcs app

INC_DIRS := $(foreach root,$(wildcard $(MODULES)),-I$(root) $(foreach dir,$(wildcard $(root)/*/),-I$(dir)header))

COMPILE_OBJ_CMD = $(COMPILER) $(FLAGS) -c $^									\
	$(foreach src,$(wildcard app/_lib/src/*.cpp),$(src))					\
	$(foreach src,$(wildcard app/langSpecData/src/*.cpp),$(src))		\
	$(TEST_SRC_DIR)/Test$@.cpp $(INC_DIRS)
COMPILE_OUT_CMD = $(COMPILER) $(OUT_PATH)/*.o $(FLAGS) -o $(OUT_PATH)/Test$@.out
CLEAN_CMD = rm -f $(OUT_PATH)/*.o

COMPILE_CMD = $(COMPILE_OBJ_CMD)		&&		mv ./*.o $(OUT_PATH)		&&		$(COMPILE_OUT_CMD)
BUILD_CMD = $(CLEAN_CMD) && $(COMPILE_CMD) && $(CLEAN_CMD)

#CLEF TARGETS
SrcReader	:	app/srcReader/src/SourceReader.cpp
	$(BUILD_CMD)
Tokenizer	:	app/tokenizer/src/*.cpp app/srcReader/src/*.cpp
	$(BUILD_CMD)
Parser		:	app/parser/src/* app/syntaxTree/src/* app/srcReader/src/* app/tokenizer/src/* mcs/*/src/*
	$(BUILD_CMD)

#MIDDLE-C STANDARD LIBRARY TARGETS
MCS			:	mcs/*/src/*
	rm -f $(OUT_PATH)/*.o										\
	&& $(COMPILER) $(FLAGS) -c $^ -Imcs						\
		$(foreach dir,$(wildcard mcs/*/),-I$(dir)header)\
	&& mv ./*.o $(OUT_PATH)										\
	&& ar rcsv mcs/mcs.a $(OUT_PATH)/*.o					\
	&& rm -f $(OUT_PATH)/*.o									\
	|| rm -f ./*.o