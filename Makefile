NAME = midi-trigger
BUILD_DIR = ./build
BUNDLE = $(BUILD_DIR)/$(NAME).lv2
LIBS = -lm $(shell pkg-config --cflags --libs lv2)
CXX = gcc
CXX_FLAGS = -std=c99 -fPIC -DPIC

ifeq ($(DEBUG),Y)
	DEBUG_C_FLAGS = -g -DDEBUG
endif

TTLS = manifest.ttl "$(NAME)".ttl

all: $(BUNDLE)

prepare-build:
	mkdir --parent -- "$(BUILD_DIR)" "$(BUILD_DIR)"/interface

$(BUNDLE): prepare-build $(BUILD_DIR)/$(NAME).so
	mkdir -- "$(BUNDLE)"
	cp -- $(TTLS) "$(BUILD_DIR)"/"$(NAME)".so "$(BUNDLE)"

$(BUILD_DIR)/$(NAME).so: prepare-build $(BUILD_DIR)/rms.o $(BUILD_DIR)/uris.o \
$(BUILD_DIR)/interface/instantiate.o $(BUILD_DIR)/interface/connect_port.o \
$(BUILD_DIR)/interface/run.o $(BUILD_DIR)/interface/cleanup.o
	mkdir --parent -- "$(BUILD_DIR)"/
	$(CXX) $(CXX_FLAGS) -shared \
		src/"$(NAME)".c \
		"$(BUILD_DIR)"/rms.o \
		"$(BUILD_DIR)"/uris.o \
		"$(BUILD_DIR)"/interface/instantiate.o \
		"$(BUILD_DIR)"/interface/connect_port.o \
		"$(BUILD_DIR)"/interface/run.o \
		"$(BUILD_DIR)"/interface/cleanup.o \
		$(LIBS) -o "$(BUILD_DIR)"/"$(NAME)".so $(DEBUG_C_FLAGS) $(C_FLAGS)

$(BUILD_DIR)/uris.o: prepare-build
	$(CXX) $(CXX_FLAGS) src/uris.c -c -o "$(BUILD_DIR)"/uris.o

$(BUILD_DIR)/rms.o: prepare-build
	$(CXX) $(CXX_FLAGS) src/rms.c -c -o "$(BUILD_DIR)"/rms.o

$(BUILD_DIR)/interface/instantiate.o: prepare-build
	$(CXX) $(CXX_FLAGS) src/interface/instantiate.c -c -o "$(BUILD_DIR)"/interface/instantiate.o

$(BUILD_DIR)/interface/connect_port.o: prepare-build
	$(CXX) $(CXX_FLAGS) src/interface/connect_port.c -c -o "$(BUILD_DIR)"/interface/connect_port.o

$(BUILD_DIR)/interface/run.o: prepare-build
	$(CXX) $(CXX_FLAGS) src/interface/run.c -c -o "$(BUILD_DIR)"/interface/run.o

$(BUILD_DIR)/interface/cleanup.o: prepare-build
	$(CXX) $(CXX_FLAGS) src/interface/cleanup.c -c -o "$(BUILD_DIR)"/interface/cleanup.o

clean:
	rm -rf -- "$(BUILD_DIR)"
