#include "maptool.h"
#ifdef WIN32
const char *mat_dll = "left4dead2\\bin\\matchmaking_ds.dll";
#else
const char *mat_dll = "left4dead2/bin/matchmaking_ds_srv.so";
#endif

maptool g_maptool;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(maptool, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_maptool);

const char *mlist[] = {"coop", "realism", "versus", "survival", "scavenge"};

bool maptool::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory){
	auto fn = Sys_GetFactory(mat_dll);
	if(!fn) return false;
	uint **match = (uint **)fn("IMATCHEXT_L4D_INTERFACE_001", NULL);
	if(match){
		KeyValues *mis = ((KeyValues *(*)(void *))match[0][0])(match);
		if(mis){
			Msg("[maptool] Dumping all missions...\n");
			KeyValues *map = mis->GetFirstSubKey();
			while(map){
				KeyValues *modes = map->FindKey("modes");
				if(modes && !map->GetInt("BuiltIn")){
					Msg("<%s> %s\n", map->GetName(), map->GetString("DisplayTitle"));
					uint64 id = map->GetUint64("workshopid");
					if(id) Msg("url: https://steamcommunity.com/workshop/filedetails/?id=%llu\n", id);
					else Msg("url: %s\n", map->GetString("Website"));
					for(unsigned i = 0; i<sizeof(mlist)/sizeof(mlist[0]); i++){
						KeyValues *mode = modes->FindKey(mlist[i]);
						if(!mode) continue;
						Msg("[%s]\n", mode->GetName());
						KeyValues *chap = mode->GetFirstSubKey();
						for(int j = 1; chap && !chap->IsEmpty("Map"); j++){
							Msg("%2d: %s\n", j, chap->GetString("Map"));
							Msg("name: %s\n", chap->GetString("DisplayName"));
							chap = chap->GetNextKey();
						}
					}
					Msg("\n");
				}
				map = map->GetNextKey();
			}
			Msg("[maptool] done.\n");
		}
	}
	return false;
}
