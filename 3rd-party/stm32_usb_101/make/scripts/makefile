EXAMPLES = $(wildcard ../usb*) $(wildcard ../others*)

all:
	for x in $(EXAMPLES); do \
	  make -C $$x; \
	done

clean-all:
	for x in $(EXAMPLES); do \
	  make -C $$x clean; \
	done; \
	rm -rf ../lib

clean:
	for x in $(EXAMPLES); do \
	  make -C $$x clean; \
	done

.PHONY : all clean-all clean
