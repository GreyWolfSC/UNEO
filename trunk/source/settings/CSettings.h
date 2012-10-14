/****************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#ifndef _CSETTINGS_H_
#define _CSETTINGS_H_

#include <string>
#include <stdio.h>
#include <gctypes.h>
#include <vector>
#include "SettingsEnums.h"
#include "GameCube/DML_Config.h"
#include "GameCube/DEVO_Config.h"

class CSettings
{
	public:
		//!Constructor
		CSettings();
		//!Destructor
		~CSettings();
		//!Set Default Settings
		void SetDefault();
		//!Load Settings
		bool Load();
		//!Save Settings
		bool Save();
		//!Reset Settings
		bool Reset();
		//!Load a languagefile
		//!\param language
		bool LoadLanguage(const char *path, int language = -1);

		//! System settings stuff
		bool widescreen;
		bool PAL50;
		bool NTSC;

		/** Variables **/
		char BootDevice[10];
		char unlockCode[20];
		char db_language[20];
		char returnTo[20];
		char ConfigPath[80];
		char covers_path[100];
		char coversFull_path[100];
		char covers2d_path[100];
		char theme_path[100];
		char theme[100];
		char disc_path[100];
		char titlestxt_path[100];
		char language_path[100];
		char languagefiles_path[100];
		char ogg_path[150];
		char Cheatcodespath[100];
		char TxtCheatcodespath[100];
		char BcaCodepath[100];
		char WipCodepath[100];
		char dolpath[100];
		char update_path[100];
		char homebrewapps_path[100];
		char WDMpath[100];
		char WiinnertagPath[100];
		char NandEmuPath[50];
		char NandEmuChanPath[50];
		char BNRCachePath[50];
		char GameCubePath[100];
		char GameCubeSDPath[100];
		char DEVOLoaderPath[100];
		char CustomBannersURL[100];
		short videomode;
		short language;
		short ocarina;
		short videopatch;
		short sinfo;
		short hddinfo;
		short rumble;
		short xflip;
		short volume;
		short sfxvolume;
		short gamesoundvolume;
		short tooltips;
		short parentalcontrol;
		short cios;
		short quickboot;
		short wsprompt;
		short keyset;
		short GameSort;
		short wiilight;
		short gameDisplay;
		short patchcountrystrings;
		short screensaver;
		short partition;
		short musicloopmode;
		short godmode;
		short error002;
		short titlesOverride; // db_titles
		short gridRows;
		short autonetwork;
		short discart;
		short coversfull;
		short gamesound;
		short marknewtitles;
		short InstallToDir;
		short GameSplit;
		short PlaylogUpdate;
		short ShowFreeSpace;
		short HomeMenu;
		short MultiplePartitions;
		short USBPort;
		short CacheTitles;
		short BlockIOSReload;
		u32 InstallPartitions;
		u32 ParentalBlocks;
		f32 WSFactor;
		f32 FontScaleFactor;
		f32 ClockFontScaleFactor;
		f32 PointerSpeed;
		short Wiinnertag;
		short SelectedGame;
		short GameListOffset;
		short sneekVideoPatch;
		std::vector<u32> EnabledCategories;
		std::vector<u32> RequiredCategories;
		std::vector<u32> ForbiddenCategories;
		u8 EntryIOS;
		short NandEmuMode;
		short NandEmuChanMode;
		short UseSystemFont;
		short Hooktype;
		short WiirdDebugger;
		short WiirdDebuggerPause;
		short ShowPlayCount;
		short RememberUnlock;
		short LoaderMode;
		short SearchMode;
		short GameAspectRatio;
		short UseChanLauncher;
		int AdjustOverscanX;
		int AdjustOverscanY;
		short ForceDiscTitles;
		short TooltipDelay;
		short GameWindowMode;
		short CacheBNRFiles;
		short BannerAnimStart;
		float BannerGridSpeed;
		short BannerZoomDuration;
		float BannerProjectionOffsetX;
		float BannerProjectionOffsetY;
		float BannerProjectionWidth;
		float BannerProjectionHeight;
		float GCBannerScale;
		short GameCubeMode;
		short DMLVideo;
		short DMLProgPatch;
		short DMLNMM;
		short DMLActivityLED;
		short DMLPADHOOK;
		short DMLNoDisc;
		short DMLNoDisc2;
		short DMLWidescreen;
		short DMLJPNPatch;
		short DMLDebug;
		short DEVOMCEmulation;
		short DEVOWidescreen;
		short DEVOActivityLED;
		short GCInstallCompressed;
		short GCInstallAligned;

		// This variable is not saved to the settings file
		bool FirstTimeRun;
	protected:
		bool ValidVersion(FILE * file);
		bool SetSetting(char *name, char *value);
		//!Find the config file in the default paths
		bool FindConfig();

		void ParseLine(char *line);
		void TrimLine(char *dest, char *src, int size);
};

extern CSettings Settings;

#endif
