#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val & 0x00ff;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = val>>8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (value==NULL) return 1;
  uint32_t byte;
  byte = (uint32_t) *value;
  sys_inb(port, &byte);
  *value = (uint8_t) byte;
  return 0;
}
