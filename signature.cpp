#include <string.h>
#include <stdlib.h>
#include "signature.h"

#ifdef WIN32
#include <windows.h>
#include <TlHelp32.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#include <link.h>
#endif

void *get_func(void *addr, const char *func){
#ifdef WIN32
	return GetProcAddress((HMODULE)addr, func);
#else
	void *result = NULL;
	Dl_info info;
	if(dladdr(addr, &info)){
		void *handle = dlopen(info.dli_fname, RTLD_NOW);
		if(handle){
			result = dlsym(handle, func);
			dlclose(handle);
		}
	}
	return result;
#endif
}

#ifndef WIN32
typedef struct{
	const char *name;
	mem_info *base;
} v_data;

static int callback(struct dl_phdr_info *info, size_t size, void *data){
	v_data *d = (v_data *)data;
	if(!info->dlpi_name || !strstr(info->dlpi_name, d->name)) return 0;
	d->base->addr = (void *)info->dlpi_addr;
	d->base->len = info->dlpi_phdr[0].p_filesz; // p_type=1 p_offset=0
	return 1;
}
#endif

static bool find_base(const char *name, mem_info *base){
#ifdef WIN32
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	if(hModuleSnap==INVALID_HANDLE_VALUE) return false;
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	while(Module32Next(hModuleSnap, &me32)){ // srcds
		if(!strcmp(me32.szModule, name)){
			base->addr = me32.modBaseAddr;
			base->len = me32.modBaseSize;
			CloseHandle(hModuleSnap);
			return true;
		}
	}
	CloseHandle(hModuleSnap);
#else
	v_data vdata = {name, base};
	if(dl_iterate_phdr(callback, &vdata)) return true;
#endif
	return false;
}

void find_base_from_list(const char *name[], mem_info *base){
	base->addr = NULL;
	base->len = 0;
	if(!name) return;
	int i = 0;
	while(name[i] && !find_base(name[i], base)) i++;
}
