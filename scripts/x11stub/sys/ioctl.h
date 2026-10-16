#ifndef STUB_SYS_IOCTL_H
#define STUB_SYS_IOCTL_H
/* Minimal stand-in for Linux <sys/ioctl.h> + <asm-generic/ioctl.h> used by
 * scripts/check_linux_branch.sh: on Windows there is no sys/ioctl.h, but the
 * project's -D__linux__ compile check needs it (pango_pcie_abi.h issues real
 * ioctl calls on the board).  The _IOC* macro values are copied verbatim from
 * <asm-generic/ioctl.h> so the compiled ABI command numbers match the board
 * exactly; this header is NOT used in board builds. */

#define _IOC_NRBITS     8
#define _IOC_TYPEBITS   8
#define _IOC_SIZEBITS   14
#define _IOC_DIRBITS    2

#define _IOC_NRSHIFT    0
#define _IOC_TYPESHIFT  (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC_NONE       0U
#define _IOC_WRITE      1U
#define _IOC_READ       2U

#define _IOC(dir, type, nr, size)                     \
    (((dir)  << _IOC_DIRSHIFT)  |                     \
     ((type) << _IOC_TYPESHIFT) |                     \
     ((nr)   << _IOC_NRSHIFT)   |                     \
     ((size) << _IOC_SIZESHIFT))

#define _IO(type, nr)        _IOC(_IOC_NONE,  (type), (nr), 0)
#define _IOR(type, nr, size) _IOC(_IOC_READ,  (type), (nr), sizeof(size))
#define _IOW(type, nr, size) _IOC(_IOC_WRITE, (type), (nr), sizeof(size))
#define _IOWR(type, nr, size) _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), sizeof(size))

extern int ioctl(int fd, unsigned long request, ...);

#endif /* STUB_SYS_IOCTL_H */
