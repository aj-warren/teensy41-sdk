#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <teensy/usb_serial.h>

extern char _heap_start;
extern char _heap_end;

static char *heap_current;

void *_sbrk(ptrdiff_t increment) {
  char *previous = heap_current;
  char *next;

  if (previous == NULL) {
    previous = &_heap_start;
  }

  if (increment < 0 ||
      increment > (ptrdiff_t)(&_heap_end - previous)) {
    errno = ENOMEM;
    return (void *)-1;
  }

  next = previous + increment;
  if (next > &_heap_end) {
    errno = ENOMEM;
    return (void *)-1;
  }

  heap_current = next;
  return previous;
}

int _write(int file, const void *buffer, size_t count) {
  (void)file;
  return usb_serial_write(buffer, (uint32_t)count);
}

int _read(int file, void *buffer, size_t count) {
  (void)file;
  return usb_serial_read(buffer, (uint32_t)count);
}
