#include <auxiliary.h>
#include <board_init.h>
#include <boot.h>
#include <delay.h>
#include <error.h>
#include <irq.h>
#include <lcd_util.h>
#include <stdio.h>
#include <string.h>
#include <usb_endianness.h>
#include <usbh_configure.h>
#include <usbh_error.h>
#include <usbh_hid_core.h>
#include <usbh_hid_req.h>
#include <usbh_lib.h>
#include <usbh_std_req.h>

#define MAX_ENDPOINT_COUNT  2
#define BUFF_LEN            256
#define SHORT_TIMEOUT       1000000
#define LONG_TIMEOUT        10000000

static void HIDexample(void);
static void PrintError(char const *, int, unsigned);
static int ParseConfiguration(usb_configuration_descriptor_t *,
                              usb_interface_descriptor_t *,
                              usb_hid_main_descriptor_t *,
                              usb_endpoint_descriptor_t *,
                              unsigned *, uint8_t const *, uint16_t);
static void LCDrefresh(void);

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
  for (;;)
    HIDexample();
}

void HIDexample() {
  usb_speed_t speed;
  usb_device_descriptor_t dev_desc;
  usb_configuration_descriptor_t cfg_desc;
  usb_interface_descriptor_t if_desc;
  usb_hid_main_descriptor_t hid_desc;
  usb_endpoint_descriptor_t ep_desc[MAX_ENDPOINT_COUNT];
  unsigned ep_count;
  uint16_t len16;
  int res;
  uint8_t dev_addr;
  char temp[BUFF_LEN];

  res = USBHopenDevice(&speed, &dev_addr, &dev_desc, SHORT_TIMEOUT);
  if (res != USBHLIB_SUCCESS) {
    LCDgoto(3, 5);
    LCDwrite(" NO DEVICE ");
    /* PrintError("USBHopenDevice", res, SHORT_TIMEOUT); */
    return;
  }

  LCDclear();
  if (speed == LOW_SPEED)
    LCDwrite("LOW SPEED  ");
  else if (speed == FULL_SPEED)
    LCDwrite("FULL SPEED ");
  else if (speed == HIGH_SPEED)
    LCDwrite("HIGH SPEED ");
  else
    LCDwrite("ERROR SPEED");
  snprintf(temp, BUFF_LEN, "%04X:%04X",
           (unsigned)dev_desc.idVendor,
           (unsigned)dev_desc.idProduct);
  LCDgoto(0, 12);
  LCDwrite(temp);
  snprintf(temp, BUFF_LEN, "%02X %02X %02X",
           (unsigned)dev_desc.bDeviceClass,
           (unsigned)dev_desc.bDeviceSubClass,
           (unsigned)dev_desc.bDeviceProtocol);
  LCDgoto(1, 0);
  LCDwrite(temp);

  memset(&cfg_desc, 0, sizeof(usb_configuration_descriptor_t));
  res = USBHgetConfDescriptor(1, 0, (uint8_t *)&cfg_desc,
                              sizeof(cfg_desc));
  if (res != USBHLIB_SUCCESS) {
    PrintError("USBHgetConfDescriptor 1", res, LONG_TIMEOUT);
    return;
  }

  len16 = min(cfg_desc.wTotalLength, BUFF_LEN);
  res = USBHgetConfDescriptor(1, 0, (uint8_t *)temp, len16);
  if (res != USBHLIB_SUCCESS) {
    PrintError("USBHgetConfDescriptor 2", res, LONG_TIMEOUT);
    return;
  }

  memset(&if_desc, 0, sizeof(usb_interface_descriptor_t));
  memset(&hid_desc, 0, sizeof(usb_hid_main_descriptor_t));
  memset(ep_desc, 0,
         sizeof(usb_endpoint_descriptor_t) * MAX_ENDPOINT_COUNT);
  ep_count = MAX_ENDPOINT_COUNT;
  res = ParseConfiguration(&cfg_desc, &if_desc, &hid_desc, ep_desc,
                           &ep_count, (uint8_t *)temp, len16);
  if (res < 0) {
    PrintError("ParseConfiguration", res, LONG_TIMEOUT);
    return;
  }

  if (cfg_desc.bmAttributes & SELF_POWERED) {
    LCDgoto(1, 10);
    LCDwrite("self");
  }
  snprintf(temp, BUFF_LEN, "%3u mA",
           (unsigned)cfg_desc.bMaxPower << 1);
  LCDgoto(1, 15);
  LCDwrite(temp);
  if (cfg_desc.bmAttributes & REMOTE_WAKEUP) {
    LCDgoto(2, 10);
    LCDwrite("rem. wakeup");
  }
  snprintf(temp, BUFF_LEN, "%02X %02X %02X",
           (unsigned)if_desc.bInterfaceClass,
           (unsigned)if_desc.bInterfaceSubClass,
           (unsigned)if_desc.bInterfaceProtocol);
  LCDgoto(2, 0);
  LCDwrite(temp);
  LCDgoto(3, 0);

  if (dev_desc.iManufacturer) {
    unsigned len32 = BUFF_LEN;
    res = USBHgetStringDescriptorASCII(1, dev_desc.iManufacturer,
                                       temp, &len32);
    if (res != USBHLIB_SUCCESS) {
      PrintError("USBHgetStringDescriptorASCII Manufacturer", res,
                 LONG_TIMEOUT);
      return;
    }

    LCDwriteLenWrap(temp, len32);
    LCDputcharWrap(' ');
  }

  if (dev_desc.iProduct) {
    unsigned len32 = BUFF_LEN;
    res = USBHgetStringDescriptorASCII(1, dev_desc.iProduct,
                                       temp, &len32);
    if (res != USBHLIB_SUCCESS) {
      PrintError("USBHgetStringDescriptorASCII Product", res,
                 LONG_TIMEOUT);
      return;
    }

    LCDwriteLenWrap(temp, len32);
  }

  if (dev_desc.bDeviceClass == 0 &&
      dev_desc.bDeviceSubClass == 0 &&
      dev_desc.bDeviceProtocol == 0 &&
      if_desc.bInterfaceClass == HUMAN_INTERFACE_DEVICE_CLASS &&
      if_desc.bInterfaceSubClass == BOOT_INTERFACE_SUBCLASS) {

    res = USBHsetConfiguration(1, cfg_desc.bConfigurationValue);
    if (res != USBHLIB_SUCCESS) {
      PrintError("USBHsetConfiguration", res, LONG_TIMEOUT);
      return;
    }

    /* Some mouses do not work if we do not read the report
       descriptor, even if we do not use it. */
    len16 = min(hid_desc.wDescriptorLength1, BUFF_LEN);
    res = HIDgetReportDescriptor(1, (uint8_t *)temp, len16);
    if (res != USBHLIB_SUCCESS) {
      PrintError("HIDgetReportDescriptor", res, LONG_TIMEOUT);
      return;
    }

    res = HIDsetBootProtocol(1, if_desc.bInterfaceNumber,
                             HID_BOOT_PROTOCOL);
    if (res != USBHLIB_SUCCESS) {
      PrintError("HIDsetBootProtocol", res, LONG_TIMEOUT);
      return;
    }

    /* Set infinity idle time. This request is optional for mouse. */
    res = HIDsetIdle(1, if_desc.bInterfaceNumber, 0, 0);
    if (res != USBHLIB_SUCCESS &&
        (if_desc.bInterfaceProtocol != MOUSE_PROTOCOL ||
         res != USBHLIB_ERROR_STALL)) {
      PrintError("HIDsetIdle", res, LONG_TIMEOUT);
      return;
    }

    res = HIDsetMachine(speed, dev_addr, &if_desc, &hid_desc,
                        ep_desc, ep_count);
    if (res != USBHLIB_SUCCESS) {
      PrintError("HIDsetMachine", res, LONG_TIMEOUT);
      return;
    }

    while (HIDisDeviceReady()) {
      LCDrefresh();
      Delay(SHORT_TIMEOUT);
    }
  }
  else {
    while (USBHisDeviceReady())
      Delay(SHORT_TIMEOUT);
  }

  LCDclear();
}

void PrintError(char const *msg, int errno, unsigned wait) {
  char temp[BUFF_LEN];

  snprintf(temp, BUFF_LEN, "%s %d ", msg, errno);
  LCDgoto(5, 0);
  LCDwriteWrap(temp);
  Delay(wait);
}

int ParseConfiguration(usb_configuration_descriptor_t *cfg_desc,
                       usb_interface_descriptor_t *if_desc,
                       usb_hid_main_descriptor_t *hid_desc,
                       usb_endpoint_descriptor_t *ep_desc,
                       unsigned *ep_count,
                       uint8_t const *buffer, uint16_t length) {
  unsigned i;

  if (length < sizeof(usb_configuration_descriptor_t))
    return -1;

  /* Prawidłowa kolejność bajtów składowej wTotalLength deskryptora
     konfiguracji jest ustalana w funkcji USBHgetConfDescriptor. */
  memcpy(cfg_desc, buffer, sizeof(usb_configuration_descriptor_t));
  if (cfg_desc->bDescriptorType != CONFIGURATION_DESCRIPTOR)
    return -2;

  buffer += cfg_desc->bLength;
  length -= cfg_desc->bLength;

  if (length < sizeof(usb_interface_descriptor_t))
    return -3;

  memcpy(if_desc, buffer, sizeof(usb_interface_descriptor_t));
  if (if_desc->bDescriptorType != INTERFACE_DESCRIPTOR)
    return -4;

  if (if_desc->bInterfaceClass != HUMAN_INTERFACE_DEVICE_CLASS)
    return 0;

  buffer += if_desc->bLength;
  length -= if_desc->bLength;

  if (length < sizeof(usb_hid_main_descriptor_t))
    return -5;

  memcpy(hid_desc, buffer, sizeof(usb_hid_main_descriptor_t));
  if (hid_desc->bDescriptorType != HID_MAIN_DESCRIPTOR)
    return -6;

  hid_desc->bcdHID = USBTOHS(hid_desc->bcdHID);
  hid_desc->wDescriptorLength1 = USBTOHS(hid_desc->wDescriptorLength1);

  buffer += hid_desc->bLength;
  length -= hid_desc->bLength;

  *ep_count = min(*ep_count, if_desc->bNumEndpoints);
  if (length < *ep_count * sizeof(usb_endpoint_descriptor_t))
    return -7;

  for (i = 0; i < *ep_count; ++i) {
    memcpy(&ep_desc[i], buffer, sizeof(usb_endpoint_descriptor_t));
    ep_desc[i].wMaxPacketSize = HTOUSBS(ep_desc[i].wMaxPacketSize);
    if (ep_desc[i].bDescriptorType != ENDPOINT_DESCRIPTOR)
      return -8;
    buffer += ep_desc[i].bLength;
  }

  return 0;
}

#define KEYBOARD_SCAN_CODES_COUNT  102

static const char key[KEYBOARD_SCAN_CODES_COUNT][6] = {
  "",  "",  "",  "",  "A", "B", "C", "D", "E", "F", /*   0 -   9 */
  "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", /*  10 -  19 */
  "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /*  20 -  29 */
  "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", /*  30 -  39 */
  "Enter", "Esc", "Bksp", "Tab", "Space", /*  40 -  44 */
  "-", "=", "{", "}", "|", /*  45 -  49 */
  "50", ":", "\"", "~", "<", /*  50 -  54 */
  ">", "?", "Caps", "F1", "F2", /*  55 -  59 */
  "F3", "F4", "F5", "F6", "F7", /*  60 -  64 */
  "F8", "F9", "F10", "F11", "F12", /*  65 -  69 */
  "PrtSc", "ScrLk", "Pause", "Ins", "Home", /*  70 -  74 */
  "PgUp", "Del", "End", "PgDn", "Right", /*  75 -  79 */
  "Left", "Down", "Up", "NumLk", "N/", /*  80 -  84 */
  "N*", "N-", "N+", "NE", "N1", /*  85 -  89 */
  "N2", "N3", "N4", "N5", "N6", /*  90 -  94 */
  "N7", "N8", "N9", "N0", "N.", /*  95 -  99 */
  "100", "Menu" /* 100 - 101 */
};

void LCDrefresh() {
  char temp[BUFF_LEN];
  int  i;

  if (new_mouse_data) {
    new_mouse_data = 0;
    LCDgoto(6, 0);
    if (mouse_buttons & MOUSE_LEFT_BUTTON)
      LCDwrite(" LEFT ");
    else
      LCDwrite("      ");
    if (mouse_buttons & MOUSE_MIDDLE_BUTTON)
      LCDwrite(" MIDDLE ");
    else
      LCDwrite("        ");
    if (mouse_buttons & MOUSE_RIGHT_BUTTON)
      LCDwrite(" RIGHT ");
    else
      LCDwrite("       ");
    snprintf(temp, BUFF_LEN, "%10d %10d", mouse_x, mouse_y);
    LCDgoto(7, 0);
    LCDwrite(temp);
  }
  else if (new_keyboard_data) {
    new_keyboard_data = 0;
    LCDgoto(5, 0);
    if (keyboard_modifiers & KEYBOARD_LEFT_SHIFT)
      LCDwrite("Shift ");
    else
      LCDwrite("      ");
    if (keyboard_modifiers & KEYBOARD_LEFT_GUI)
      LCDwrite("GUI ");
    else
      LCDwrite("    ");
    if (keyboard_modifiers & KEYBOARD_RIGHT_GUI)
      LCDwrite(" GUI");
    else
      LCDwrite("    ");
    if (keyboard_modifiers & KEYBOARD_RIGHT_SHIFT)
      LCDwrite(" Shift");
    else
      LCDwrite("      ");
    LCDgoto(6, 0);
    if (keyboard_modifiers & KEYBOARD_LEFT_CTRL)
      LCDwrite("Ctrl  ");
    else
      LCDwrite("      ");
    if (keyboard_modifiers & KEYBOARD_LEFT_ALT)
      LCDwrite("Alt ");
    else
      LCDwrite("    ");
    if (keyboard_modifiers & KEYBOARD_RIGHT_ALT)
      LCDwrite(" Alt");
    else
      LCDwrite("    ");
    if (keyboard_modifiers & KEYBOARD_RIGHT_CTRL)
      LCDwrite("  Ctrl");
    else
      LCDwrite("      ");
    LCDgoto(7, 0);
    for (i = 0; i < KEYBOARD_MAX_PRESSED_KEYS; ++i) {
      if (keyboard_scan_code[i] < KEYBOARD_SCAN_CODES_COUNT) {
        LCDwrite(key[keyboard_scan_code[i]]);
        LCDputchar(' ');
      }
      else { /* It should never happen for boot protocol. */
        LCDwrite("0x");
        LCDwriteHex(2, keyboard_scan_code[i]);
        LCDputchar(' ');
      }
    }
    LCDwrite("                     ");
  }
}
