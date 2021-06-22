/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 * Copyright (C) 2008-2021 TrinityCore <http://www.trinitycore.org/>
 */

#include "AppenderConsole.h"
#include "LogMessage.h"
#include "SmartEnum.h"
#include "StringConvert.h"
#include "StringFormat.h"
#include "Tokenize.h"
#include "Util.h"
#include <sstream>

#if AC_PLATFORM == AC_PLATFORM_WINDOWS
#include <Windows.h>
#endif

AppenderConsole::AppenderConsole(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags, std::vector<std::string_view> const& args)
    : Appender(id, name, level, flags), _colored(false)
{
    for (uint8 i = 0; i < NUM_ENABLED_LOG_LEVELS; ++i)
        _colors[i] = ColorTypes(NUM_COLOR_TYPES);

    if (3 < args.size())
        InitColors(name, args[3]);
}

void AppenderConsole::InitColors(std::string const& name, std::string_view str)
{
    if (str.empty())
    {
        _colored = false;
        return;
    }

    std::vector<std::string_view> colorStrs = Acore::Tokenize(str, ' ', false);
    if (colorStrs.size() != NUM_ENABLED_LOG_LEVELS)
    {
        throw InvalidAppenderArgsException(Acore::StringFormat("Log::CreateAppenderFromConfig: Invalid color data '%s' for console appender %s (expected %u entries, got %zu)",
            std::string(str).c_str(), name.c_str(), NUM_ENABLED_LOG_LEVELS, colorStrs.size()));
    }

    for (uint8 i = 0; i < NUM_ENABLED_LOG_LEVELS; ++i)
    {
        if (Optional<uint8> color = Acore::StringTo<uint8>(colorStrs[i]); color && EnumUtils::IsValid<ColorTypes>(*color))
            _colors[i] = static_cast<ColorTypes>(*color);
        else
        {
            throw InvalidAppenderArgsException(Acore::StringFormat("Log::CreateAppenderFromConfig: Invalid color '%s' for log level %s on console appender %s",
                std::string(colorStrs[i]).c_str(), EnumUtils::ToTitle(static_cast<LogLevel>(i)), name.c_str()));
        }
    }

    _colored = true;
}

void AppenderConsole::SetColor(bool stdout_stream, ColorTypes color)
{
#if AC_PLATFORM == AC_PLATFORM_WINDOWS
    static WORD WinColorFG[NUM_COLOR_TYPES] =
    {
        0,                                                  // BLACK
        FOREGROUND_RED,                                     // RED
        FOREGROUND_GREEN,                                   // GREEN
        FOREGROUND_RED | FOREGROUND_GREEN,                  // BROWN
        FOREGROUND_BLUE,                                    // BLUE
        FOREGROUND_RED |                    FOREGROUND_BLUE, // MAGENTA
        FOREGROUND_GREEN | FOREGROUND_BLUE,                 // CYAN
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // WHITE
                                                            // YELLOW
        FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY,
                                                            // RED_BOLD
        FOREGROUND_RED |                                      FOREGROUND_INTENSITY,
                                                            // GREEN_BOLD
        FOREGROUND_GREEN |                   FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,             // BLUE_BOLD
                                                            // MAGENTA_BOLD
        FOREGROUND_RED |                    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                                                            // CYAN_BOLD
        FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                                                            // WHITE_BOLD
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
    };

    HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    SetConsoleTextAttribute(hConsole, WinColorFG[color]);
#else
    enum ANSITextAttr
    {
        TA_NORMAL                                = 0,
        TA_BOLD                                  = 1,
        TA_BLINK                                 = 5,
        TA_REVERSE                               = 7
    };

    enum ANSIFgTextAttr
    {
        FG_BLACK                                 = 30,
        FG_RED,
        FG_GREEN,
        FG_BROWN,
        FG_BLUE,
        FG_MAGENTA,
        FG_CYAN,
        FG_WHITE,
        FG_YELLOW
    };

    enum ANSIBgTextAttr
    {
        BG_BLACK                                 = 40,
        BG_RED,
        BG_GREEN,
        BG_BROWN,
        BG_BLUE,
        BG_MAGENTA,
        BG_CYAN,
        BG_WHITE
    };

    static uint8 UnixColorFG[NUM_COLOR_TYPES] =
    {
        FG_BLACK,                                          // BLACK
        FG_RED,                                            // RED
        FG_GREEN,                                          // GREEN
        FG_BROWN,                                          // BROWN
        FG_BLUE,                                           // BLUE
        FG_MAGENTA,                                        // MAGENTA
        FG_CYAN,                                           // CYAN
        FG_WHITE,                                          // WHITE
        FG_YELLOW,                                         // YELLOW
        FG_RED,                                            // LRED
        FG_GREEN,                                          // LGREEN
        FG_BLUE,                                           // LBLUE
        FG_MAGENTA,                                        // LMAGENTA
        FG_CYAN,                                           // LCYAN
        FG_WHITE                                           // LWHITE
    };

    fprintf((stdout_stream? stdout : stderr), "\x1b[%d%sm", UnixColorFG[color], (color >= YELLOW && color < NUM_COLOR_TYPES ? ";1" : ""));
    #endif
}

void AppenderConsole::ResetColor(bool stdout_stream)
{
#if AC_PLATFORM == AC_PLATFORM_WINDOWS
    HANDLE hConsole = GetStdHandle(stdout_stream ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#else
    fprintf((stdout_stream ? stdout : stderr), "\x1b[0m");
#endif
}

void AppenderConsole::_write(LogMessage const* message)
{
    bool stdout_stream = !(message->level == LOG_LEVEL_ERROR || message->level == LOG_LEVEL_FATAL);

    if (_colored)
    {
        uint8 index;

        switch (message->level)
        {
            case LOG_LEVEL_TRACE:
               index = 5;
               break;
            case LOG_LEVEL_DEBUG:
               index = 4;
               break;
            case LOG_LEVEL_INFO:
               index = 3;
               break;
            case LOG_LEVEL_WARN:
               index = 2;
               break;
            case LOG_LEVEL_FATAL:
               index = 0;
               break;
            default:
               index = 1;
               break;
        }

        SetColor(stdout_stream, _colors[index]);
        utf8printf(stdout_stream ? stdout : stderr, "%s%s\n", message->prefix.c_str(), message->text.c_str());
        ResetColor(stdout_stream);
    }
    else
        utf8printf(stdout_stream ? stdout : stderr, "%s%s\n", message->prefix.c_str(), message->text.c_str());
}
