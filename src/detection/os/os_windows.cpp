extern "C" {
#include "os.h"
}
#include "util/windows/wmi.hpp"

extern "C"
void ffDetectOSImpl(FFOSResult* os, const FFinstance* instance)
{
    ffStrbufInit(&os->name);
    ffStrbufInit(&os->prettyName);
    ffStrbufInit(&os->id);
    ffStrbufInit(&os->idLike);
    ffStrbufInit(&os->variant);
    ffStrbufInit(&os->variantID);
    ffStrbufInit(&os->version);
    ffStrbufInit(&os->versionID);
    ffStrbufInit(&os->codename);
    ffStrbufInit(&os->buildID);
    ffStrbufInit(&os->systemName);
    ffStrbufInit(&os->architecture);

    FFWmiQuery query(L"SELECT Caption, Version, BuildNumber, OSArchitecture FROM Win32_OperatingSystem");
    if(!query)
        return;

    if(FFWmiRecord record = query.next())
    {
        record.getString(L"Caption", &os->variant);
        if(ffStrbufStartsWithS(&os->variant, "Microsoft Windows "))
        {
            ffStrbufAppendS(&os->name, "Microsoft Windows");
            ffStrbufAppendS(&os->prettyName, "Windows");

            ffStrbufSubstrAfter(&os->variant, strlen("Microsoft Windows ") - 1);

            if(ffStrbufStartsWithS(&os->variant, "Server "))
            {
                ffStrbufAppendS(&os->name, " Server");
                ffStrbufAppendS(&os->prettyName, " Server");
                ffStrbufSubstrAfter(&os->variant, strlen(" Server") - 1);
            }

            uint32_t index = ffStrbufFirstIndexC(&os->variant, ' ');
            ffStrbufAppendNS(&os->version, index, os->variant.chars);
            ffStrbufSubstrAfter(&os->variant, index);

            // Windows Server 20xx Rx
            if(ffStrbufEndsWithC(&os->prettyName, 'r'))
            {
                if(os->variant.chars[0] == 'R' &&
                isdigit(os->variant.chars[1]) &&
                (os->variant.chars[2] == '\0' || os->variant.chars[2] == ' '))
                {
                    ffStrbufAppendF(&os->version, " R%c", os->variant.chars[1]);
                    ffStrbufSubstrAfter(&os->variant, strlen("Rx ") - 1);
                }
            }
        }
        else
        {
            // Unknown Windows name, please report this
            ffStrbufAppend(&os->name, &os->variant);
            ffStrbufClear(&os->variant);
        }

        ffStrbufAppendF(&os->id, "%*s %*s", os->prettyName.length, os->prettyName.chars, os->version.length, os->version.chars);

        record.getString(L"BuildNumber", &os->buildID);
        record.getString(L"OSArchitecture", &os->architecture);

        ffStrbufSetS(&os->systemName, instance->state.utsname.sysname);
    }
}
