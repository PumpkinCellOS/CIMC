#pragma once

#include <types.h>

#define ENOENT         2      /* No such file or directory */
#define ESRCH          3      /* No such process */
#define EINTR          4      /* Interrupted system call */
#define EIO            5      /* I/O error */
#define ENOEXEC        8      /* Exec format error */
#define EBADF          9      /* Bad file number */
#define EAGAIN        11      /* Try again */
#define ENOMEM        12      /* Out of memory */
#define EFAULT        14      /* Bad address */
#define ENOTBLK       15      /* Block device required */
#define EBUSY         16      /* Device or resource busy */
#define EEXIST        17      /* File exists */
#define ENODEV        19      /* No such device */
#define ENOTDIR       20      /* Not a directory */
#define EISDIR        21      /* Is a directory */
#define EINVAL        22      /* Invalid argument */
#define ENFILE        23      /* File table overflow */
#define EMFILE        24      /* Too many open files */
#define EFBIG         27      /* File too large */
#define ENOSPC        28      /* No space left on device */
#define ESPIPE        29      /* Illegal seek */
#define EDOM          33      /* Math argument out of domain of func */
#define ERANGE        34      /* Math result not representable */
#define ENAMETOOLONG  36      /* File name too long */
#define ENOSYS        38      /* Invalid system call number */
#define ENOTEMPTY     39      /* Directory not empty */

extern errno_t errno;
