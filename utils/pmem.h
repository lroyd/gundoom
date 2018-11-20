#ifndef __PMEM_H__
#define __PMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "gki.h"

/*------------------------------------------------------------------*/
/* Standard C Library functions */
/*------------------------------------------------------------------*/
#define BtMemCpy memcpy
#define BtMemMove memmove
#define BtStrCpy strcpy
#define BtStrNCpy strncpy
#define BtStrCat strcat
#define BtStrNCat strncat
#define BtMemCmp(s1, s2, n) ((int32) memcmp((s1), (s2), (n)))
#define BtStrCmp(s1, s2) ((int32) strcmp((s1), (s2)))
#define BtStrNCmp(s1, s2, n) ((int32) strncmp((s1), (s2), (n)))
#define BtStrChr strchr
#define BtStrStr strstr
#define BtMemSet memset
#define BtStrLen strlen


#define BtPmemAlloc     GKI_getbuf
#define BtPmemZalloc(s) (BtMemSet(GKI_getbuf(s), 0x00, (s)))

#define BtPmemFree      GKI_freebuf

#define pnew(t) ((t *) (BtPmemAlloc(sizeof(t))))
#define zpnew(t) ((t *) (BtPmemZalloc(sizeof(t))))


#ifdef __cplusplus
}
#endif

#endif

