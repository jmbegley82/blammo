CC          ?= cc
CXX         ?= c++
MKDIR_P     ?= mkdir -p
EXTRA_LIBS   ?=
EXTRA_CFLAGS ?=
COMMONFLAGS ?= -g
CFLAGS      ?= $(COMMONFLAGS) $(EXTRA_CFLAGS) -std=c99
CXXFLAGS    ?= $(COMMONFLAGS) $(EXTRA_CFLAGS)
CPPFLAGS    ?= -I./src/
LDFLAGS     ?= -lpthread $(EXTRA_LIBS)

BUILD_DIR   ?=./build
SRC_DIR     ?=./src

### no easily-user-serviceable parts below this line

SRCS:=$(shell find $(SRC_DIR) -name *.cpp -or -name *.c)
OBJS:=$(SRCS:%=$(BUILD_DIR)/%.o)
DEPS:=$(OBJS:.o=.d)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/blammo-test: $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

bin:	$(BUILD_DIR)/blammo-test

doc:
	doxygen

all:	bin doc

.PHONY:	clean

clean:
	rm -rf build
	rm -rf doc
