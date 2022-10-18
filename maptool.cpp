#include "maptool.h"
#ifdef WIN32
const char *mat_dll[] = {"matchmaking_ds.dll", "matchmaking.dll", 0};
#else
const char *mat_dll[] = {"matchmaking_ds_srv.so", 0};
#endif

maptool g_maptool;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(maptool, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_maptool);

const char *mlist[] = {"coop", "realism", "versus", "survival", "scavenge"};

bool maptool::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory){
	mem_info base = {NULL, 0};
	find_base_from_list(mat_dll, &base);
	auto inf = (void *(*)(const char *, int *))get_func(base.addr, "CreateInterface");
	if(inf){
		uint **match = (uint **)inf("IMATCHEXT_L4D_INTERFACE_001", NULL);
		if(match){
			KeyValues *mis = ((KeyValues *(*)(void *))match[0][0])(match);
			if(mis){
				Msg("[maptool] Dumping all missions...");
				KeyValues *map = mis->GetFirstSubKey();
				while(map){
					KeyValues *modes = map->FindKey("modes");
					if(modes){
						Msg("[%s] %s\n", map->GetName(), map->GetString("DisplayTitle"));
						uint64 id = map->GetUint64("workshopid");
						if(id) Msg("url: http://steamcommunity.com/workshop/filedetails/?id=%llu\n", id);
						else Msg("url: %s\n", map->GetString("Website"));
						for(unsigned i = 0; i<sizeof(mlist)/sizeof(mlist[0]); i++){
							KeyValues *mode = modes->FindKey(mlist[i]);
							if(!mode) continue;
							Msg("mode: %s\n", mode->GetName());
							KeyValues *chap = mode->GetFirstSubKey();
							while(chap && !chap->IsEmpty("Map")){
								Msg("code: %s\n", chap->GetString("Map"));
								Msg("name: %s\n", chap->GetString("DisplayName"));
								chap = chap->GetNextKey();
							}
						}
					}
					map = map->GetNextKey();
					Msg("\n");
				}
				Msg("[maptool] done.");
			}
		}
	}
	return false;
}
