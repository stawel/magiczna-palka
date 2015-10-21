TARGETS = ex_libusb ex_vendor_test
INCLUDES = ../../common/include/usb_def.h
CFLAGS = -Wall -O2 -I../../common/include
LDFLAGS = -lusb

.PHONY : all clean

all : $(TARGETS)

ex_libusb        : ex_libusb.o
ex_libusb.o      : ex_libusb.c
ex_vendor_test   : ex_vendor_test.o
ex_vendor_test.o : $(INCLUDES) ex_vendor_test.c

clean:
	rm -f $(TARGETS) *~ *.bak *.o
