#pragma once

#ifndef FF_INCLUDED_detection_os_os
#define FF_INCLUDED_detection_os_os

#include "fastfetch.h"

typedef struct FFOSResult
{
    FFstrbuf name;
    FFstrbuf prettyName;
    FFstrbuf id;
    FFstrbuf idLike;
    FFstrbuf variant;
    FFstrbuf variantID;
    FFstrbuf version;
    FFstrbuf versionID;
    FFstrbuf codename;
    FFstrbuf buildID;
    FFstrbuf systemName;
    FFstrbuf architecture;
} FFOSResult;

const FFOSResult* ffDetectOS(const FFinstance* instance);

#endif
