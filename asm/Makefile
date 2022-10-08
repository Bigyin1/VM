CC = g++
DOXYGEN = doxygen

EXECUTABLE = bin


SRCS = 	main.cpp \
		stack.cpp



INCLUDES =

CPPFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall \
	-Wextra -Weffc++ -Waggressive-loop-optimizations \
	-Wc++14-compat -Wmissing-declarations -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Wconditionally-supported \
	-Wconversion -Wctor-dtor-privacy -Wempty-body \
	-Wfloat-equal -Wformat-nonliteral -Wformat-security \
	-Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
	-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual \
	-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls \
	-Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
	-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
	-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum \
	-Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast \
	-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
	-Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector \
	-fcheck-new -fsized-deallocation -fstack-check -fstack-protector \
	-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
	-Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE \
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d


BUILD_DIR = build
DEP_DIR = deps
DOCS_DIR = docs


DEPFILES := $(SRCS:%.cpp=$(DEP_DIR)/%.d)
DEP_DIRS := $(dir $(DEPFILES))
$(shell mkdir -p $(DEP_DIRS))

OBJS := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
OBJ_DIRS := $(dir $(OBJS))
$(shell mkdir -p $(OBJ_DIRS))


.PHONY: all
all: $(BUILD_DIR)/$(EXECUTABLE) $(DOCS_DIR)

.PHONY: bin
bin: $(BUILD_DIR)/$(EXECUTABLE)


$(BUILD_DIR)/$(EXECUTABLE): $(OBJS)
	@$(CC) $(CPPFLAGS) $^ -o $@


$(BUILD_DIR)/%.o: %.cpp $(DEP_DIR)/%.d
	@$(CC) -c -o $@ $< $(CPPFLAGS) $(DEPFLAGS)

$(DEPFILES):

include $(wildcard $(DEPFILES))




$(DOCS_DIR): $(SRCS) $(INCLUDES)
	@OUTPUT_DIRECTORY=$(DOCS_DIR) $(DOXYGEN)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm -r $(DOCS_DIR)
