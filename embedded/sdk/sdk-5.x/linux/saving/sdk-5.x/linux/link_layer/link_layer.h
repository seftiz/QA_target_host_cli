#ifndef _APPLICATIONS_LL_LL_H
#define _APPLICATIONS_LL_LL_H

#include <atlk/rdev.h>

/* Initialize Link Layer device */
int
ll_init(const char *dev_name);

/* Get Link Layer device */
rdev_desc_t *
ll_get(void);

#endif /* _APPLICATIONS_LL_LL_H */