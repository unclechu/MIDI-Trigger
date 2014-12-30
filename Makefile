NAME = midi-trigger
BUNDLE = $(NAME).lv2
LIBS = `pkg-config --cflags --libs lv2` -lm
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

$(NAME).so: rms.o
	$(CXX) $(CXX_FLAGS) -shared \
		src/$(NAME).c rms.o $(LIBS) \
		-o $(NAME).so $(DEBUG_C_FLAGS) $(C_FLAGS)

rms.o:
	$(CXX) $(CXX_FLAGS) src/rms.c -c -o rms.o

clean:
	rm -rf $(BUNDLE) $(NAME).so *.o
