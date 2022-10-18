#ifndef _INCLUDE_SIGNATURE_
#define _INCLUDE_SIGNATURE_

#ifdef WIN32
typedef unsigned int uint;
typedef unsigned long long uint64;
#endif
typedef struct{
	void *addr;
	uint len;
} mem_info;

void *get_func(void *addr, const char *func);
void find_base_from_list(const char *name[], mem_info *base);
#endif //_INCLUDE_SIGNATURE_
