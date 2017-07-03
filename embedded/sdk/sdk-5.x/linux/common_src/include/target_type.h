#ifndef _COMMON_SRC_TARGET_TYPE_H
#define _COMMON_SRC_TARGET_TYPE_H

typedef enum {
  TARGET_TYPE_CRATON2,
  TARGET_TYPE_SECTON,
} target_type_t;

target_type_t
target_type_get(void);

#endif /* _COMMON_SRC_TARGET_TYPE_H */
