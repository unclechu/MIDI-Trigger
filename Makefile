NAME = midi-trigger
BUNDLE = $(NAME).lv2
LIBS = -lm `pkg-config --cflags --libs lv2`
CXX = gcc
CXX_FLAGS = -std=c99 -fPIC -DPIC

ifeq ($(DEBUG),Y)
	DEBUG_C_FLAGS = -g -DDEBUG
endif

TTLS = manifest.ttl $(NAME).ttl

all: clean $(BUNDLE)

$(BUNDLE): $(NAME).so
	mkdir $(BUNDLE)
	cp $(TTLS) $(NAME).so $(BUNDLE)

$(NAME).so: rms.o uris.o interface/instantiate.o interface/connect_port.o interface/run.o interface/cleanup.o
	$(CXX) $(CXX_FLAGS) -shared \
		src/$(NAME).c \
		interface/instantiate.o interface/connect_port.o interface/run.o interface/cleanup.o \
		rms.o uris.o \
		$(LIBS) -o $(NAME).so $(DEBUG_C_FLAGS) $(C_FLAGS)

uris.o:
	$(CXX) $(CXX_FLAGS) src/uris.c -c -o uris.o

rms.o:
	$(CXX) $(CXX_FLAGS) src/rms.c -c -o rms.o

interface/instantiate.o:
	mkdir --parent interface
	$(CXX) $(CXX_FLAGS) src/interface/instantiate.c -c -o interface/instantiate.o

interface/connect_port.o:
	mkdir --parent interface
	$(CXX) $(CXX_FLAGS) src/interface/connect_port.c -c -o interface/connect_port.o

interface/run.o:
	mkdir --parent interface
	$(CXX) $(CXX_FLAGS) src/interface/run.c -c -o interface/run.o

interface/cleanup.o:
	mkdir --parent interface
	$(CXX) $(CXX_FLAGS) src/interface/cleanup.c -c -o interface/cleanup.o

clean:
	rm -rf $(BUNDLE) $(NAME).so **/*.o
