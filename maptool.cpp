#include <fstream>

#include "maptool.h"
#include "matchmaking/l4d2/imatchext_l4d.h"
#include "sigtool.h"

static const char *path = "left4dead2/addons/maplist.txt";

maptool g_maptool;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(maptool, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_maptool);

static const char *mlist[] = {"coop", "realism", "versus", "survival", "scavenge"};

bool maptool::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
    mem_info info = {"matchmaking_ds_srv.so"};
    if (!find_base(&info)) return false;
    auto match = (IMatchExtL4D *)get_sym(info.addr, "_ZL13g_MatchExtL4D");
    if (!match) return false;

    KeyValues *mis = match->GetAllMissions();
    if (!mis) return false;
    Msg("[maptool] Dumping all missions...\n");

    std::ofstream fs;
    fs.open(path, std::ios::out | std::ios::trunc);

    KeyValues *map = mis->GetFirstSubKey();
    while (map) {
        KeyValues *modes = map->FindKey("modes");
        if (modes && !map->GetInt("BuiltIn")) {
            fs << "<" << map->GetName() << "> " << map->GetString("DisplayTitle") << std::endl;
            uint64 id = map->GetUint64("workshopid");
            if (id) fs << "url: https://steamcommunity.com/workshop/filedetails/?id=" << id << std::endl;
            else fs << "url: " << map->GetString("Website") << std::endl;

            for (unsigned i = 0; i < sizeof(mlist) / sizeof(mlist[0]); i++) {
                KeyValues *mode = modes->FindKey(mlist[i]);
                if (!mode) continue;

                fs << "[" << mode->GetName() << "]" << std::endl;
                KeyValues *chap = mode->GetFirstSubKey();
                for (int j = 1; chap && !chap->IsEmpty("Map"); j++) {
                    fs.width(2);
                    fs << j << ": " << chap->GetString("Map") << std::endl;
                    fs << "name: " << chap->GetString("DisplayName") << std::endl;
                    chap = chap->GetNextKey();
                }
            }
            fs << std::endl;
        }
        map = map->GetNextKey();
    }

    fs.close();

    Msg("[maptool] done.\n");
    return false;
}
