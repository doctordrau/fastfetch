#include "fastfetch.h"
#include "terminalfont.h"
#include "detection/terminalshell/terminalshell.h"
#include "common/io.h"

#ifdef FF_HAVE_FREETYPE
    #include "common/library.h"
    #include <ft2build.h>
    #include FT_FREETYPE_H
#endif

#define FF_TERMUX_FONT_PATH FASTFETCH_TARGET_DIR_HOME "/.termux/font.ttf"

const char* detectTermux(const FFinstance* instance, FFTerminalFontResult* terminalFont)
{
    #ifdef FF_HAVE_FREETYPE

    FF_LIBRARY_LOAD(freetype, &instance->config.libfreetype, "dlopen libfreetype"FF_LIBRARY_EXTENSION " failed", "libfreetype"FF_LIBRARY_EXTENSION, 2)
    FF_LIBRARY_LOAD_SYMBOL_MESSAGE(freetype, FT_Init_FreeType);
    FF_LIBRARY_LOAD_SYMBOL_MESSAGE(freetype, FT_New_Face);
    FF_LIBRARY_LOAD_SYMBOL_MESSAGE(freetype, FT_Done_Face);
    FF_LIBRARY_LOAD_SYMBOL_MESSAGE(freetype, FT_Done_FreeType);

    FT_Library library = NULL;
    FT_Face face = NULL;
    const char* error = NULL;

    if(ffFT_Init_FreeType(&library))
    {
        error = "FT_Init_FreeType() failed";
        goto exit;
    }

    if(ffFT_New_Face(library, FF_TERMUX_FONT_PATH, 0, &face ))
    {
        error = "FT_NEW_Face(" FF_TERMUX_FONT_PATH ") failed";
        goto exit;
    }

    ffFontInitCopy(&terminalFont->font, face->family_name);

exit:
    if(face) ffFT_Done_Face(face);
    if(library) ffFT_Done_FreeType(library);
    dlclose(freetype);

    return error;

    #else

    FF_UNUSED(terminalFont);
    ffStrbufSetS(&terminalFont->error, "fastfetch is built without freetype2 support");

    #endif
}

void ffDetectTerminalFontPlatform(const FFinstance* instance, const FFTerminalShellResult* terminalShell, FFTerminalFontResult* terminalFont)
{
    if(ffStrbufCompS(&terminalShell->terminalProcessName, "Termux") != 0)
    {
        ffStrbufSetS(&terminalFont->error, "Unsupported terminal");
        return;
    }

    if(!ffFileExists(FF_TERMUX_FONT_PATH, S_IFREG))
    {
        ffFontInitCopy(&terminalFont->font, "monospace");
        return;
    }

    ffStrbufSetS(&terminalFont->error, detectTermux(instance, terminalFont));
}
