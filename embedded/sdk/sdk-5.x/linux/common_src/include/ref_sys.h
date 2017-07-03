#ifndef _REF_SYS_H
#define _REF_SYS_H

#include <atlk/sdk.h>
#include <atlk/dsm.h>

/**
   @file
   This is a reference system initialization.
*/

/**
   Initialize reference system

   @param[in] argc Arguments count of application
   @param[in] argc Arguments variables of application
*/
atlk_rc_t atlk_must_check
ref_sys_init(int argc, char *argv[]);

#endif /* _REF_SYS_H */
