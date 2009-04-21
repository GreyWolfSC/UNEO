#ifndef _CFG_H_
#define _CFG_H_

#include <gctypes.h>
#include "disc.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int ENTRIES_PER_PAGE;
extern int MAX_CHARACTERS;
extern int CONSOLE_XCOORD;
extern int CONSOLE_YCOORD;
extern int CONSOLE_WIDTH;
extern int CONSOLE_HEIGHT;
extern int CONSOLE_BG_COLOR;
extern int CONSOLE_FG_COLOR;
extern int COVER_XCOORD;
extern int COVER_YCOORD;

#define CFG_HOME_REBOOT 0
#define CFG_HOME_EXIT   1

#define CFG_LAYOUT_ORIG      0
#define CFG_LAYOUT_ORIG_12   1
#define CFG_LAYOUT_SMALL     2
#define CFG_LAYOUT_MEDIUM    3
#define CFG_LAYOUT_LARGE     4 // nixx
#define CFG_LAYOUT_LARGE_2   5 // usptactical
#define CFG_LAYOUT_ULTIMATE1 6 // Ultimate1: (WiiShizza)
#define CFG_LAYOUT_ULTIMATE2 7 // Ultimate2: (jservs7 / hungyip84)
#define CFG_LAYOUT_ULTIMATE_W 8 // Ultimate widescreen: (jservs7 / hungyip84)

#define CFG_VIDEO_SYS   0  // system default
#define CFG_VIDEO_DEFAULT  1
#define CFG_VIDEO_GAME  1  // game default
#define CFG_VIDEO_PATCH 2  // patch mode
#define CFG_VIDEO_PAL50	3  // force PAL
#define CFG_VIDEO_PAL60	4  // force PAL60
#define CFG_VIDEO_NTSC	5  // force NTSC
#define CFG_VIDEO_COUNT 6

#define CFG_LANG_CONSOLE  0
#define CFG_LANG_JAPANESE 1
#define CFG_LANG_ENGLISH  2
#define CFG_LANG_GERMAN   3
#define CFG_LANG_FRENCH   4
#define CFG_LANG_SPANISH  5
#define CFG_LANG_ITALIAN  6
#define CFG_LANG_DUTCH    7
#define CFG_LANG_S_CHINESE 8
#define CFG_LANG_T_CHINESE 9
#define CFG_LANG_KOREAN   10
#define CFG_LANG_COUNT    11

extern char *cfg_path;
//extern char *cfg_images_path;

struct CFG
{
//	char *background;
//	short layout;
//	short covers;
//	short simple;
//	short video;
//	short language;
//	short ocarina;
//	short vipatch;
//	short home;
//	short download;
//	short installdownload;
//	short hidesettingmenu;
//	short savesettings;
	short widescreen;
	short parentalcontrol;
	char covers_path[100];
	char theme_path[100];
};

extern struct CFG CFG;
//extern u8 ocarinaChoice;
//extern u8 videoChoice;
//extern u8 languageChoice;
//extern u8 viChoice;

struct Game_CFG
{
	u8 id[8];
	u8 video;
	u8 language;
	u8 ocarina;
	u8 vipatch;
};


void CFG_Default();
void CFG_Load(int argc, char **argv);
struct Game_CFG* CFG_get_game_opt(u8 *id);
bool CFG_save_game_opt(u8 *id, u8 video, u8 language, u8 ocarina, u8 vipatch);
bool CFG_forget_game_opt(u8 *id);

char *get_title(struct discHdr *header);
u8 get_block(struct discHdr *header);

void CFG_Cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
