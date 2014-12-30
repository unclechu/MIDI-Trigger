NAME = midi-trigger
BUNDLE = $(NAME).lv2
LIBS = `pkg-config --cflags --libs lv2`

ifeq ($(DEBUG),Y)
	DEBUG_C_FLAGS = -g -DDEBUG
endif

TTLS = manifest.ttl $(NAME).ttl

$(BUNDLE): clean $(NAME).so
	mkdir $(BUNDLE)
	cp $(TTLS) $(NAME).so $(BUNDLE)

$(NAME).so:
	gcc -std=c99 -shared -fPIC -DPIC src/$(NAME).c $(LIBS) -o $(NAME).so $(DEBUG_C_FLAGS)

clean:
	rm -rf $(BUNDLE) $(NAME).so
