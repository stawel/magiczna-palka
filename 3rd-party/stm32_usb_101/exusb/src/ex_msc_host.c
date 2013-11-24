#include <board_init.h>
#include <boot.h>
#include <delay.h>
#include <error.h>
#include <file_system.h>
#include <irq.h>
#include <lcd_util.h>
#include <stdio.h>
#include <string.h>
#include <usbh_configure.h>
#include <usbh_lib.h>

#define TIMEOUT  1000

static int ReadTestNumber(char const *filename) {
  FILE *file;
  int testnum;

  file = fopen(filename, "r");
  if (file) {
    if (fscanf(file, "%d", &testnum) == 1) {
      ++testnum;
    }
    else {
      testnum = 1;
      fprintf(stderr, "fscanf %.12s failed\n", filename);
    }
    if (fclose(file) != 0) {
      fprintf(stderr, "fclose %.12s failed\n", filename);
    }
  }
  else {
    testnum = 1;
  }

  return testnum;
}

static void WriteTestNumber(int testnum, char const *filename,
                            char const *mode) {
  FILE *file;

  file = fopen(filename, mode);
  if (file == 0) {
    fprintf(stderr, "fopen %.12s failed\n", filename);
  }
  else {
    if (fprintf(file, "%d\n", testnum) < 0) {
      fprintf(stderr, "fprintf %.12s failed\n", filename);
    }
    if (fclose(file) != 0) {
      fprintf(stderr, "fclose %.12s failed\n", filename);
    }
  }
}

static void BiggerTest(int testnum, char const *filename,
                       char const *mode) {
  static const size_t size[4] = {1024, 1024 + 76, 1024 - 41, 1024 - 35};
  static char buff[1024 + 76];
  FILE *file;
  size_t i, j, s, r;
  char c;

  file = fopen(filename, mode);
  if (file == 0) {
    fprintf(stderr, "fopen %.12s failed\n", filename);
  }
  else {
    for (i = 0; i < 17; ++i) {
      s = size[(i + testnum) & 3];
      c = 5 * i + testnum;
      if (mode[0] == 'w') {
        memset(buff, c, s);
        r = fwrite(buff, 1, s, file);
        if (r != s) {
          fprintf(stderr, "fwrite %.12s failed\n", filename);
          break;
        }
      }
      else if (mode[0] == 'r') {
        r = fread(buff, 1, s, file);
        if (r != s) {
          fprintf(stderr, "fread %.12s failed\n", filename);
          break;
        }
        for (j = 0; j < s; ++j) {
          if (buff[j] != c) {
            fprintf(stderr, "compare %.12s failed\n", filename);
            break;
          }
        }
      }
      putchar('.');
      fflush(stdout);
    }
    putchar('\n');
    if (fclose(file) != 0) {
      fprintf(stderr, "fclose %.12s failed\n", filename);
    }
  }
}

int main(void) {
  int       clk;
  usb_phy_t phy;

  GetBootParams(&clk, 0, &phy);
  AllPinsDisable();
  LEDconfigure();
  RedLEDon();
  IRQprotectionConfigure();
  ErrorResetable(ClockConfigure(clk), 2);
  ErrorResetable(LCDconfigure(), 3);
  ErrorResetable(USBHconfigure(phy), 5);

  for (;;) {
    printf("Waiting for device...\n");
    while (!USBHisDeviceReady())
      Delay(TIMEOUT);
    LCDclear();
    if (FileSystemMount() < 0) {
     fprintf(stderr, "mount failed\n");
    }
    else {
      int n;
      n = ReadTestNumber("testnum.txt");
      printf("Test %d\n", n);
      BiggerTest(n, "testbig", "w");
      WriteTestNumber(n, "testlog.txt", "a");
      BiggerTest(n, "testbig", "r");
      WriteTestNumber(n, "testnum.txt", "w");
      if (FileSystemUmount() < 0)
        fprintf(stderr, "umount failed\n");
      else
        printf("Device can be removed.\n");
    }
    while (USBHisDeviceReady())
      Delay(TIMEOUT);
    LCDclear();
  }
}
