default: Parser

#directories and files
MODULES := mcsl app frozen

ALL_CODE_DIRS := $(foreach dir, $(MODULES),${shell find -L $(dir) -type d -print})
ALL_SRC_FILES := $(foreach dir, $(ALL_CODE_DIRS), $(wildcard $(dir)/*.cpp))
ALL_HEADER_FILES := $(foreach dir, $(ALL_CODE_DIRS), $(wildcard $(dir)/*.hpp))

ALL_TESTS := $(wildcard test/src/*.cpp)

ALL_AUTO_MAKEFILES := $(ALL_SRC_FILES:%.cpp=_build/%.mk) $(ALL_TESTS:%.cpp=_build/%.mk)
ALL_OBJ_FILES := $(ALL_SRC_FILES:%.cpp=_build/%.o)

#compiler
COMPILER := clang++ -std=c++23
FLAGS := -g -Wall -Wextra -Wshadow-all -pedantic -pedantic-errors -ftemplate-backtrace-limit=4 -fdiagnostics-show-category=name -Wno-gcc-compat -Wno-trigraphs -Wno-pessimizing-move -Wno-dtor-name -Wno-nested-anon-types -Wno-gnu-anonymous-struct -Wno-c23-extensions
# FLAGS := -g -Wall -Wextra -Wshadow-all -pedantic -pedantic-errors -ftemplate-backtrace-limit=4 -fdiagnostics-show-category=name -Wno-gcc-compat -Wno-trigraphs -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls
# COMPILER := g++ -std=c++23
# FLAGS := -g -Wall -Wextra -Wshadow-all -pedantic -pedantic-errors -ftemplate-backtrace-limit=4 -Wno-trigraphs -Wno-attributes -Wno-parentheses -Wno-class-memaccess -Wno-pessimizing-move

#compile commands
COMPILE := $(COMPILER) $(FLAGS) $(addprefix -I, $(MODULES) $(filter %/header, $(ALL_CODE_DIRS)))


#clean
.PHONY: clean
clean:
	rm -rf _build


#set up directory structure
app/embeds/out/%.txt : app/embeds/src/%.c
	xxd -i $< | grep "^ " > $@

.PHONY: setup
setup:
	mkdir -p $(addprefix _build/,$(ALL_CODE_DIRS)) _build/test/src _build/out

#generate prereq makefiles
$(ALL_AUTO_MAKEFILES): _build/%.mk : %.cpp | setup
	(($(COMPILE) -MM $^) | (sed -E 's/([^ ]*)\.o([^u])?/_build\/\1.o _build\/\1.mk\2/') > $@) && echo $@

#include prereq makefiles
-include $(ALL_AUTO_MAKEFILES)

#include unit test makefiles
-include $(wildcard _build/test/*.mk)

#compile object files
$(ALL_OBJ_FILES): _build/%.o : %.cpp | _build/%.mk setup
	($(COMPILE) -c $^ -o $@) && echo $@


.PHONY: objects
objects: $(ALL_OBJ_FILES)

#compile unit test files
.PHONY: Lexer
Lexer: _build/test/src/TestLexer.mk test/src/TestLexer.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestLexer.cpp $(shell find _build -name *.o) -o _build/out/$@.out
.PHONY: Parser
Parser: _build/test/src/TestParser.mk test/src/TestParser.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestParser.cpp $(shell find _build -name *.o) -o _build/out/$@.out
.PHONY: Codegen
Codegen: _build/test/src/TestCodegen.mk test/src/TestCodegen.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestCodegen.cpp $(shell find _build -name *.o) -o _build/out/$@.out

.PHONY: StrToNum
StrToNum: _build/test/src/TestStrToNum.mk test/src/TestStrToNum.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestStrToNum.cpp $(shell find _build -name *.o) -o _build/out/$@.out
.PHONY: Printf
Printf: _build/test/src/TestPrintf.mk test/src/TestPrintf.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestPrintf.cpp $(shell find _build -name *.o) -o _build/out/$@.out
.PHONY: List
List: _build/test/src/TestList.mk test/src/TestList.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestList.cpp $(shell find _build -name *.o) -o _build/out/$@.out
.PHONY: Set
Set: _build/test/src/TestSet.mk test/src/TestSet.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestSet.cpp $(shell find _build -name *.o) -o _build/out/$@.out
.PHONY: Map
Map: _build/test/src/TestMap.mk test/src/TestMap.cpp | $(ALL_OBJ_FILES) setup
	$(COMPILE) test/src/TestMap.cpp $(shell find _build -name *.o) -o _build/out/$@.out
