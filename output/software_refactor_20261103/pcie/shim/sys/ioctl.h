#ifndef TEST_IOCTL_H
#define TEST_IOCTL_H
#define _IOWR(type, nr, value_type) (0xC0000000UL | (sizeof(value_type) << 16) | ((type) << 8) | (nr))
int ioctl(int fd, unsigned long request, void *data);
#endif
