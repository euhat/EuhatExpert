#include "macHdd.h"
#include <common/OpCommon.h>
#include <common/WhCommon.h>

#define INI_SECTION_LOCAL "local"

int isMacHddChanged(DbOpIni &ini, const char *mac)
{
    string macsOld = ini.readStr(INI_SECTION_LOCAL, "mac");

    if (macsOld.empty())
    {
        ini.write(INI_SECTION_LOCAL, "mac", mac);
        return 0;
    }

    if (macsOld != mac)
    {
        ini.write(INI_SECTION_LOCAL, "mac", mac);
        return 1;
    }
    return 0;
}