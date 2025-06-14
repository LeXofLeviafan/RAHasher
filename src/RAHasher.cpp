// RAHasher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Git.h"
#include "Util.h"

#include <rcheevos/include/rc_hash.h>

#include <memory>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
 #define WIN32_LEAN_AND_MEAN
 #include <Windows.h>
 #ifndef strcasecmp
   #define strcasecmp _stricmp
 #endif
#elif defined(__unix__)
 #include <glob.h>
 #include <sys/stat.h>
#else
 #include <dirent.h>
 #include <fnmatch.h>
 #include <sys/stat.h>
#endif

#ifdef HAVE_CHD
void rc_hash_init_chd_cdreader(); /* in HashCHD.cpp */
#endif

void   initHash3DS(const std::string& systemDir); /* in Hash3DS.cpp */

static const char* _NINTENDO = "Nintendo";
static const char* _SONY     = "Sony";
static const char* _ATARI    = "Atari";
static const char* _SEGA     = "Sega";
static const char* _NEC      = "NEC";
static const char* _SNK      = "SNK";
static const char* _OTHERS   = "Others";

typedef struct console_t {
  const uint32_t id;
  const char* key;
  const char* group;
  const char* name;
} console_t;

static const console_t CONSOLES[] = {
  {RC_CONSOLE_GAMEBOY,                   "GB",      _NINTENDO, "Game Boy"},
  {RC_CONSOLE_GAMEBOY_ADVANCE,           "GBA",     _NINTENDO, "Game Boy Advance"},
  {RC_CONSOLE_GAMEBOY_COLOR,             "GBC",     _NINTENDO, "Game Boy Color"},
  {RC_CONSOLE_NINTENDO,                  "NES",     _NINTENDO, "NES/Famicom"},
  {RC_CONSOLE_SUPER_NINTENDO,            "SNES",    _NINTENDO, "SNES/Super Famicom"},
  {RC_CONSOLE_NINTENDO_64,               "N64",     _NINTENDO, "Nintendo 64"},
  {RC_CONSOLE_GAMECUBE,                  "GC",      _NINTENDO, "GameCube"},
  {RC_CONSOLE_NINTENDO_DS,               "DS",      _NINTENDO, "Nintendo DS"},
  {RC_CONSOLE_NINTENDO_DSI,              "DSi",     _NINTENDO, "Nintendo DSi"},
  {RC_CONSOLE_POKEMON_MINI,              "MINI",    _NINTENDO, "Pokemon Mini"},
  {RC_CONSOLE_VIRTUAL_BOY,               "VB",      _NINTENDO, "Virtual Boy"},
  {RC_CONSOLE_GAME_AND_WATCH,            "G&W",     NULL,       "Game & Watch"},
  {RC_CONSOLE_NINTENDO_3DS,              "3DS",     NULL,       "Nintendo 3DS"},
  {RC_CONSOLE_WII,                       "Wii",     NULL,       "Nintendo Wii"},
  {RC_CONSOLE_WII_U,                     "WiiU",    NULL,       "Nintendo Wii U"},

  {RC_CONSOLE_PLAYSTATION,               "PS1",     _SONY, "PlayStation"},
  {RC_CONSOLE_PLAYSTATION_2,             "PS2",     _SONY, "PlayStation 2"},
  {RC_CONSOLE_PSP,                       "PSP",     _SONY, "PlayStation Portable"},

  {RC_CONSOLE_ATARI_2600,                "2600",    _ATARI, "Atari 2600"},
  {RC_CONSOLE_ATARI_7800,                "7800",    _ATARI, "Atari 7800"},
  {RC_CONSOLE_ATARI_JAGUAR,              "JAG",     _ATARI, "Atari Jaguar"},
  {RC_CONSOLE_ATARI_JAGUAR_CD,           "JCD",     _ATARI, "Atari Jaguar CD"},
  {RC_CONSOLE_ATARI_LYNX,                "Lynx",    _ATARI, "Atari Lynx"},
  {RC_CONSOLE_ATARI_5200,                "5200",    NULL,    "Atari 5200"},
  {RC_CONSOLE_ATARI_ST,                  "AST",     NULL,    "Atari ST"},

  {RC_CONSOLE_SG1000,                    "SG1K",    _SEGA, "SG-1000"},
  {RC_CONSOLE_MASTER_SYSTEM,             "SMS",     _SEGA, "Master System"},
  {RC_CONSOLE_GAME_GEAR,                 "GG",      _SEGA, "Game Gear"},
  {RC_CONSOLE_MEGA_DRIVE,                "MD",      _SEGA, "Genesis/Mega Drive"},
  {RC_CONSOLE_SEGA_CD,                   "SCD",     _SEGA, "Sega CD"},
  {RC_CONSOLE_SEGA_32X,                  "32X",     _SEGA, "32X"},
  {RC_CONSOLE_SATURN,                    "SAT",     _SEGA, "Saturn"},
  {RC_CONSOLE_DREAMCAST,                 "DC",      _SEGA, "Dreamcast"},
  {RC_CONSOLE_PICO,                      "Pico",    NULL,   "Sega Pico"},

  {RC_CONSOLE_PC_ENGINE,                 "PCE",     _NEC, "PC Engine/TurboGrafx-16"},
  {RC_CONSOLE_PC_ENGINE_CD,              "PCCD",    _NEC, "PC Engine CD/TurboGrafx-CD"},
  {RC_CONSOLE_PC8800,                    "80/88",   _NEC, "PC-8000/8800"},
  {RC_CONSOLE_PCFX,                      "PC-FX",   _NEC, "PC-FX"},
  {RC_CONSOLE_PC6000,                    "PC-6000", NULL,  "PC-6000"},
  {RC_CONSOLE_PC9800,                    "9800",    NULL,  "PC-9800"},

  {RC_CONSOLE_NEO_GEO_CD,                "NGCD",    _SNK, "Neo Geo CD"},
  {RC_CONSOLE_NEOGEO_POCKET,             "NGP",     _SNK, "Neo Geo Pocket"},

  {RC_CONSOLE_3DO,                       "3DO",     _OTHERS, "3DO Interactive Multiplayer"},
  {RC_CONSOLE_AMSTRAD_PC,                "CPC",     _OTHERS, "Amstrad CPC"},
  {RC_CONSOLE_APPLE_II,                  "A2",      _OTHERS, "Apple II"},
  {RC_CONSOLE_ARCADE,                    "ARC",     _OTHERS, "Arcade"},
  {RC_CONSOLE_ARCADIA_2001,              "A2001",   _OTHERS, "Arcadia 2001"},
  {RC_CONSOLE_ARDUBOY,                   "ARD",     _OTHERS, "Arduboy"},
  {RC_CONSOLE_COLECOVISION,              "CV",      _OTHERS, "ColecoVision"},
  {RC_CONSOLE_ELEKTOR_TV_GAMES_COMPUTER, "ELEK",    _OTHERS, "Elektor TV Games Computer"},
  {RC_CONSOLE_FAIRCHILD_CHANNEL_F,       "CHF",     _OTHERS, "Fairchild Channel F"},
  {RC_CONSOLE_INTELLIVISION,             "INTV",    _OTHERS, "Intellivision"},
  {RC_CONSOLE_INTERTON_VC_4000,          "VC4000",  _OTHERS, "Interton VC 4000"},
  {RC_CONSOLE_MAGNAVOX_ODYSSEY2,         "MO2",     _OTHERS, "Magnavox Odyssey 2"},
  {RC_CONSOLE_MEGADUCK,                  "DUCK",    _OTHERS, "Mega Duck"},
  {RC_CONSOLE_MSX,                       "MSX",     _OTHERS, "MSX"},
  //{RC_CONSOLE_STANDALONE,                "EXE",     _OTHERS, "Standalone"},  /* >90, not usable */
  {RC_CONSOLE_UZEBOX,                    "UZE",     _OTHERS, "Uzebox"},
  {RC_CONSOLE_VECTREX,                   "VECT",    _OTHERS, "Vectrex"},
  {RC_CONSOLE_SUPERVISION,               "WSV",     _OTHERS, "Watara Supervision"},
  {RC_CONSOLE_WASM4,                     "WASM4",   _OTHERS, "WASM-4"},
  {RC_CONSOLE_WONDERSWAN,                "WS",      _OTHERS, "WonderSwan"},
  {RC_CONSOLE_AMIGA,                     "Amiga",   NULL,     "Amiga"},
  {RC_CONSOLE_CASSETTEVISION,            "ECV",     NULL,     "Cassette Vision"},
  {RC_CONSOLE_SUPER_CASSETTEVISION,      "ESCV",    NULL,     "Super Cassette Vision"},
  {RC_CONSOLE_COMMODORE_64,              "C64",     NULL,     "Commodore 64"},
  {RC_CONSOLE_FM_TOWNS,                  "FMTowns", NULL,     "FM Towns"},
  {RC_CONSOLE_NOKIA_NGAGE,               "N-Gage",  NULL,     "Nokia N-Gage"},
  {RC_CONSOLE_ORIC,                      "Oric",    NULL,     "Oric"},
  {RC_CONSOLE_CDI,                       "CD-i",    NULL,     "Philips CD-i"},
  {RC_CONSOLE_SHARPX1,                   "X1",      NULL,     "Sharp X1"},
  {RC_CONSOLE_X68K,                      "X68K",    NULL,     "Sharp X68000"},
  {RC_CONSOLE_THOMSONTO8,                "TO8",     NULL,     "Thomson TO8"},
  {RC_CONSOLE_TI83,                      "TI83",    NULL,     "TI-83"},
  {RC_CONSOLE_TIC80,                     "TIC-80",  NULL,     "TIC-80"},
  {RC_CONSOLE_VIC20,                     "VIC-20",  NULL,     "VIC-20"},
  {RC_CONSOLE_ZEEBO,                     "Zeebo",   NULL,     "Zeebo"},
  {RC_CONSOLE_ZX81,                      "ZX81",    NULL,     "ZX81"},
  {RC_CONSOLE_ZX_SPECTRUM,               "ZXS",     NULL,     "ZX Spectrum"},
  {RC_CONSOLE_MS_DOS,                    "DOS",     NULL,     "DOS"},
  {RC_CONSOLE_XBOX,                      "Xbox",    NULL,     "Xbox"},
};
static const long CONSOLES_NUM = sizeof(CONSOLES) / sizeof(console_t);

static int32_t find_console_id (const char* key) {
  for (int i = 0;  i < CONSOLES_NUM;  i++)
    if ((CONSOLES[i].group != NULL) && (strcasecmp(key, CONSOLES[i].key) == 0))
      return CONSOLES[i].id;
  return atoi(key);  // if no matching key found, falling back to original behaviour
}

static void usage(const char* appname)
{
  printf("RAHasher %s\n====================\n", git::getReleaseVersion());

  printf("Usage: %s [-v] [-s systempath] system filepath...\n", util::fileName(appname).c_str());
  printf("\n");
  printf("  -v             (optional) enables verbose messages for debugging\n");
  printf("  -s systempath  (optional) specifies where supplementary files are stored (typically a path to RetroArch/system)\n");
  printf("  system         specifies the system key or id associated to the game (which hash algorithm to use)\n");
  printf("  filepath       specifies the path to the game file (file may include wildcards, path may not)\n");

  printf("\n");
  printf(" ID Key     Group    Name\n");
  printf(" -- ------- -------- ---------------------------\n");
  const char* last_group = NULL;
  for (int i = 0;  i < CONSOLES_NUM;  i++) {
#ifdef HIDE_UNSUPPORTED_CONSOLES
    if (CONSOLES[i].group == NULL)
      continue;
#endif
    if ((CONSOLES[i].group != NULL) && (last_group != NULL) && (last_group != CONSOLES[i].group))
      printf("\n");
    printf(" %2d %-7s %-8s %s\n", CONSOLES[i].id, CONSOLES[i].key,
           (CONSOLES[i].group == NULL ? "" : CONSOLES[i].group), CONSOLES[i].name);
    if (CONSOLES[i].group != NULL)
      last_group = CONSOLES[i].group;
  }
  printf("\nFor a single file, console ID can be specified as '?' (to attempt guessing by extension)\n");
#ifndef HIDE_UNSUPPORTED_CONSOLES
  printf("Warning: consoles with a 'blank' group are currently not supported by RA!\n");
#endif
}

class StdErrLogger : public Logger
{
public:
  void log(enum retro_log_level level, const char* line, size_t length) override
  {
    // ignore non-errors unless they're coming from the hashing code
    if (level != RETRO_LOG_ERROR && strncmp(line, "[HASH]", 6) != 0)
      return;

    // don't print the category
    while (*line && *line != ']')
    {
      ++line;
      --length;
    }
    while (*line == ']' || *line == ' ')
    {
      ++line;
      --length;
    }

    ::fwrite(line, length, 1, stderr);
    ::fprintf(stderr, "\n");
  }
};

static std::unique_ptr<StdErrLogger> logger;

static void rhash_log(const char* message)
{
  printf("%s\n", message);
}

static void rhash_log_error(const char* message)
{
  fprintf(stderr, "%s\n", message);
}

static void* rhash_file_open(const char* path)
{
  return util::openFile(logger.get(), path, "rb");
}

#define RC_CONSOLE_MAX 90

static int process_file(int consoleId, const std::string& file)
{
  char hash[33];
  int result = 1;

  std::string filePath = util::fullPath(file);
  std::string ext = util::extension(file);

  if (consoleId != RC_CONSOLE_ARCADE && consoleId <= RC_CONSOLE_MAX && ext.length() == 4 &&
      tolower(ext[1]) == 'z' && tolower(ext[2]) == 'i' && tolower(ext[3]) == 'p')
  {
    std::string unzippedFilename;
    size_t size;
    void* data = util::loadZippedFile(logger.get(), filePath, &size, unzippedFilename);
    if (data)
    {
      if (rc_hash_generate_from_buffer(hash, consoleId, (uint8_t*)data, size))
        printf("%s", hash);

      free(data);
    }
  }
  else
  {
    /* register a custom file_open handler for unicode support. use the default implementation for the other methods */
    struct rc_hash_filereader filereader;
    memset(&filereader, 0, sizeof(filereader));
    filereader.open = rhash_file_open;
    rc_hash_init_custom_filereader(&filereader);

    if (ext.length() == 4 && tolower(ext[1]) == 'c' && tolower(ext[2]) == 'h' && tolower(ext[3]) == 'd')
    {
#ifdef HAVE_CHD
      rc_hash_init_chd_cdreader();
#else
      printf("CHD not supported without HAVE_CHD compile flag");
      return 0;
#endif
    }
    else
    {
      rc_hash_init_default_cdreader();
    }

    if (consoleId > RC_CONSOLE_MAX)
    {
      rc_hash_iterator iterator;
      rc_hash_initialize_iterator(&iterator, filePath.c_str(), NULL, 0);
      while (rc_hash_iterate(hash, &iterator))
        printf("%s", hash);
      rc_hash_destroy_iterator(&iterator);
    }
    else
    {
      if (rc_hash_generate_from_file(hash, consoleId, filePath.c_str()))
        printf("%s", hash);
    }
  }

  return result;
}

static int process_iterated_file(int console_id, const std::string& file)
{
  int result = process_file(console_id, file);
  if (!result)
    printf("????????????????????????????????");

  printf(" %s\n", util::fileNameWithExtension(file).c_str());
  return result;
}

static int process_files(int consoleId, const std::string& pattern)
{
  int count = 0;

#ifdef _WIN32
  std::string path = util::directory(pattern);
  WIN32_FIND_DATAA fileData;
  HANDLE hFind;

  if (path == pattern) /* no backslash found. scan is in current directory */
    path = ".";

  hFind = FindFirstFileA(pattern.c_str(), &fileData);
  if (hFind != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        const std::string filePath = path + "\\" + fileData.cFileName;
        count += process_iterated_file(consoleId, filePath);
      }
    } while (FindNextFileA(hFind, &fileData));

    FindClose(hFind);
  }
#elif defined(__unix__)
  glob_t globResult;
  memset(&globResult, 0, sizeof(globResult));

  if (glob(pattern.c_str(), GLOB_TILDE, NULL, &globResult) == 0)
  {
    struct stat filebuf;
    size_t i;
    for (i = 0; i < globResult.gl_pathc; ++i)
    {
      if (stat(globResult.gl_pathv[i], &filebuf) == 0 && !S_ISDIR(filebuf.st_mode))
        count += process_iterated_file(consoleId, globResult.gl_pathv[i]);
    }
  }

  globfree(&globResult);
#else
  const std::string filePattern = util::fileNameWithExtension(pattern);
  char resolved_path[PATH_MAX];
  std::string path = util::directory(pattern);
  DIR* dirp;

  realpath(path.c_str(), resolved_path);
  path = resolved_path;

  dirp = opendir(path.c_str());
  if (dirp)
  {
    struct stat filebuf;
    struct dirent *dp;

    while ((dp = readdir(dirp)))
    {
      if (fnmatch(filePattern.c_str(), dp->d_name, 0) == 0)
      {
        if (stat(dp->d_name, &filebuf) == 0 && !S_ISDIR(filebuf.st_mode))
        {
          const std::string filePath = path + "/" + dp->d_name;
          count += process_iterated_file(consoleId, filePath);
        }
      }
    }
  }
#endif

  if (count == 0)
    printf("No matches found\n");

  return count;
}

int main(int argc, char* argv[])
{
  int consoleId = 0;
  int singleFile = 1;
  std::string systemDirectory = ".";

  int argi = 1;

  while (argi < argc && argv[argi][0] == '-')
  {
    if (strcmp(argv[argi], "-v") == 0)
    {
      rc_hash_init_verbose_message_callback(rhash_log);
      ++argi;
    }
    else if (strcmp(argv[argi], "-s") == 0)
    {
      systemDirectory = argv[++argi];
      ++argi;
    }
    else
    {
      usage(argv[0]);
      return 1;
    }
  }

  if (argi + 2 > argc)
  {
    usage(argv[0]);
    return 1;
  }

  const char* consoleKey = argv[argi++];
  consoleId = (strcmp(consoleKey, "?") == 0 ? 1+RC_CONSOLE_MAX : find_console_id(consoleKey));
  if (consoleId == 0)
  {
    usage(argv[0]);
    return 1;
  }

  logger.reset(new StdErrLogger);
  rc_hash_init_error_message_callback(rhash_log_error);

  if (consoleId == RC_CONSOLE_NINTENDO_3DS)
    initHash3DS(systemDirectory);

  if (argi + 1 < argc)
  {
    if (consoleId > RC_CONSOLE_MAX)
    {
      printf("Specific console must be specified when processing multiple files\n");
      return 0;
    }

    singleFile = 0;
  }
  else
  {
    std::string file = argv[argi];
    if (file.find('*') != std::string::npos || file.find('?') != std::string::npos)
    {
      if (consoleId > RC_CONSOLE_MAX)
      {
        printf("Specific console must be specified when using wildcards\n");
        return 0;
      }

      singleFile = 0;
    }
  }

  if (!singleFile)
  {
    /* verbose logging not allowed when processing multiple files */
    rc_hash_init_verbose_message_callback(NULL);
  }

  while (argi < argc)
  {
    std::string file = argv[argi++];

    if (file.find('*') != std::string::npos || file.find('?') != std::string::npos)
    {
      if (!process_files(consoleId, file))
        return 0;
    }
    else
    {
      int result = process_file(consoleId, file);

      if (singleFile)
	printf("\n");
      else
        printf(" %s\n", util::fileNameWithExtension(file).c_str());

      if (!result)
        return result;
    }
  }

  return 1;
}
