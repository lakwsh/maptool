#include "maptool.h"
#include <fstream>
#ifdef WIN32
const char *mat_dll = "left4dead2\\bin\\matchmaking_ds.dll";
const char *path = "left4dead2\\addons\\maplist.txt";
#else
const char *mat_dll = "left4dead2/bin/matchmaking_ds_srv.so";
const char *path = "left4dead2/addons/maplist.txt";
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
			std::ofstream fs;
			fs.open(path, std::ios::out|std::ios::trunc);
			KeyValues *map = mis->GetFirstSubKey();
			while(map){
				KeyValues *modes = map->FindKey("modes");
				if(modes && !map->GetInt("BuiltIn")){
					fs<<"<"<<map->GetName()<<"> "<<map->GetString("DisplayTitle")<<std::endl;
					uint64 id = map->GetUint64("workshopid");
					if(id) fs<<"url: https://steamcommunity.com/workshop/filedetails/?id="<<id<<std::endl;
					else fs<<"url: "<<map->GetString("Website")<<std::endl;
					for(unsigned i = 0; i<sizeof(mlist)/sizeof(mlist[0]); i++){
						KeyValues *mode = modes->FindKey(mlist[i]);
						if(!mode) continue;
						fs<<"["<<mode->GetName()<<"]"<<std::endl;
						KeyValues *chap = mode->GetFirstSubKey();
						for(int j = 1; chap && !chap->IsEmpty("Map"); j++){
							fs.width(2);
							fs<<j<<": "<<chap->GetString("Map")<<std::endl;
							fs<<"name: "<<chap->GetString("DisplayName")<<std::endl;
							chap = chap->GetNextKey();
						}
					}
					fs<<std::endl;
				}
				map = map->GetNextKey();
			}
			fs.close();
			Msg("[maptool] done.\n");
		}
	}
	return false;
}
