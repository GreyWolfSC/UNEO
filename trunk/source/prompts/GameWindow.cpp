#include <unistd.h>
#include "GameWindow.hpp"
#include "usbloader/disc.h"
#include "usbloader/wbfs.h"
#include "usbloader/GameList.h"
#include "usbloader/GameBooter.hpp"
#include "usbloader/AlternateDOLOffsets.h"
#include "themes/CTheme.h"
#include "FileOperations/fileops.h"
#include "settings/menus/GameSettingsMenu.hpp"
#include "settings/CSettings.h"
#include "settings/CGameSettings.h"
#include "settings/CGameStatistics.h"
#include "settings/GameTitles.h"
#include "prompts/PromptWindows.h"
#include "prompts/gameinfo.h"
#include "language/gettext.h"
#include "system/IosLoader.h"
#include "menu/menus.h"
#include "menu/WDMMenu.hpp"
#include "banner/OpeningBNR.hpp"
#include "utils/ShowError.h"

#define NONE		0
#define LEFT		1
#define RIGHT		2
#define IN			3
#define OUT			4

extern u8 mountMethod;
extern struct discHdr *dvdheader;

GameWindow::GameWindow(int Selected)
	: GuiWindow(472, 320)
{
	returnVal = -1;
	gameSelected = Selected;
	gameSound = NULL;
	diskImgData = NULL;
	diskImgData2 = NULL;
	reducedVol = false;
	SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	SetPosition(0, -10);

	dialogBox = Resources::GetImageData(Settings.widescreen ? "wdialogue_box_startgame.png" : "dialogue_box_startgame.png");
	btnOutline = Resources::GetImageData("button_dialogue_box.png");
	imgFavorite = Resources::GetImageData("favorite.png");
	imgNotFavorite = Resources::GetImageData("not_favorite.png");
	imgLeft = Resources::GetImageData("startgame_arrow_left.png");
	imgRight = Resources::GetImageData("startgame_arrow_right.png");

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB = new GuiTrigger;
	trigB->SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);
	trigL = new GuiTrigger;
	trigL->SetButtonOnlyTrigger(-1, WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT, PAD_BUTTON_LEFT);
	trigR = new GuiTrigger;
	trigR->SetButtonOnlyTrigger(-1, WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT, PAD_BUTTON_RIGHT);
	trigPlus = new GuiTrigger;
	trigPlus->SetButtonOnlyTrigger(-1, WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS, 0);
	trigMinus = new GuiTrigger;
	trigMinus->SetButtonOnlyTrigger(-1, WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS, 0);

	dialogBoxImg = new GuiImage(dialogBox);

	nameBtnTT = new GuiTooltip(tr( "Rename Game on WBFS" ));
	if (Settings.wsprompt) nameBtnTT->SetWidescreen(Settings.widescreen);
	nameTxt = new GuiText("", 22, thColor("r=0 g=0 b=0 a=255 - game window name text color"));
	if (Settings.wsprompt) nameTxt->SetWidescreen(Settings.widescreen);
	nameTxt->SetMaxWidth(350, SCROLL_HORIZONTAL);
	nameBtn = new GuiButton(120, 50);
	nameBtn->SetLabel(nameTxt);
	nameBtn->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	nameBtn->SetPosition(0, -122);
	nameBtn->SetSoundOver(btnSoundOver);
	nameBtn->SetSoundClick(btnSoundClick2);

	if (Settings.godmode == 1 && !mountMethod)
	{
		nameBtn->SetToolTip(nameBtnTT, 24, -30, ALIGN_LEFT);
		nameBtn->SetTrigger(trigA);
		nameBtn->SetEffectGrow();
	}

	sizeTxt = new GuiText((char*) NULL, 22, thColor("r=0 g=0 b=0 a=255 - game window size text color"));
	sizeTxt->SetAlignment(ALIGN_CENTER, ALIGN_TOP);
	sizeTxt->SetPosition(135, 70);

	diskImg = new GuiDiskCover;
	diskImg->SetWidescreen(Settings.widescreen);
	diskImg->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	diskImg->SetAngle(0);
	diskImg2 = new GuiDiskCover;
	diskImg2->SetWidescreen(Settings.widescreen);
	diskImg2->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	diskImg2->SetPosition(0, -20);
	diskImg2->SetAngle(0);
	diskImg2->SetBeta(180);

	playcntTxt = new GuiText((char*) NULL, 18, thColor("r=0 g=0 b=0 a=255 - game window playcount text color"));
	playcntTxt->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	playcntTxt->SetPosition(-115, 45);

	gameBtn = new GuiButton(160, 160);
	gameBtn->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	gameBtn->SetPosition(0, -20);
	gameBtn->SetImage(diskImg);
	gameBtn->SetSoundOver(btnSoundOver);
	gameBtn->SetSoundClick(btnSoundClick2);
	gameBtn->SetTrigger(trigA);
	gameBtn->SetState(STATE_SELECTED);

	backBtnTxt = new GuiText(tr( "Back" ), 22, thColor("r=0 g=0 b=0 a=255 - prompt windows button text color"));
	backBtnImg = new GuiImage(btnOutline);
	if (Settings.wsprompt)
	{
		backBtnTxt->SetWidescreen(Settings.widescreen);
		backBtnImg->SetWidescreen(Settings.widescreen);
	}
	backBtn = new GuiButton(backBtnImg, backBtnImg, 1, 5, 0, 0, trigA, btnSoundOver, btnSoundClick2, 1);
	backBtn->SetLabel(backBtnTxt);
	backBtn->SetTrigger(trigB);
	backBtn->SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	backBtn->SetPosition(0, -40);

	settingsBtnTxt = new GuiText(tr( "Settings" ), 22, thColor("r=0 g=0 b=0 a=255 - prompt windows button text color"));
	settingsBtnImg = new GuiImage(btnOutline);
	if (Settings.wsprompt)
	{
		settingsBtnTxt->SetWidescreen(Settings.widescreen);
		settingsBtnImg->SetWidescreen(Settings.widescreen);
	}
	settingsBtn = new GuiButton(settingsBtnImg, settingsBtnImg, 0, 4, 50, -40, trigA, btnSoundOver, btnSoundClick2, 1);
	settingsBtn->SetLabel(settingsBtnTxt);

	int xPos = -198;
	for(int i = 0; i < FAVORITE_STARS; ++i)
	{
		FavoriteBtnImg[i] = new GuiImage;
		FavoriteBtnImg[i]->SetWidescreen(Settings.widescreen);
		FavoriteBtn[i] = new GuiButton(imgFavorite->GetWidth(), imgFavorite->GetHeight());
		FavoriteBtn[i]->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
		FavoriteBtn[i]->SetPosition(xPos, -60);
		FavoriteBtn[i]->SetImage(FavoriteBtnImg[i]);
		FavoriteBtn[i]->SetSoundOver(btnSoundOver);
		FavoriteBtn[i]->SetSoundClick(btnSoundClick2);
		FavoriteBtn[i]->SetTrigger(trigA);
		FavoriteBtn[i]->SetEffectGrow();

		xPos += 27;
	}

	btnLeftImg = new GuiImage(imgLeft);
	if (Settings.wsprompt) btnLeftImg->SetWidescreen(Settings.widescreen);
	btnLeft = new GuiButton(btnLeftImg, btnLeftImg, 0, 5, 20, 0, trigA, btnSoundOver, btnSoundClick2, 1);
	btnLeft->SetTrigger(trigL);
	btnLeft->SetTrigger(trigMinus);

	btnRightImg = new GuiImage(imgRight);
	if (Settings.wsprompt) btnRightImg->SetWidescreen(Settings.widescreen);
	btnRight = new GuiButton(btnRightImg, btnRightImg, 1, 5, -20, 0, trigA, btnSoundOver, btnSoundClick2, 1);
	btnRight->SetTrigger(trigR);
	btnRight->SetTrigger(trigPlus);

	detailsBtnTxt = new GuiText(tr( "Details" ), 22, thColor("r=0 g=0 b=0 a=255 - game window details button text color"));
	detailsBtnOverTxt = new GuiText(tr( "Details" ), 22, thColor("r=30 g=30 b=240 a=255 - game window details button over text color"));
	detailsBtn = new GuiButton(detailsBtnTxt->GetTextWidth(), 25);
	detailsBtn->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	detailsBtn->SetPosition(120, 45);
	detailsBtn->SetLabel(detailsBtnTxt);
	detailsBtn->SetLabelOver(detailsBtnOverTxt);
	detailsBtn->SetTrigger(trigA);
	detailsBtn->SetEffectGrow();

	Append(dialogBoxImg);
	if (Settings.ShowPlayCount) Append(playcntTxt);
	Append(backBtn);
	Append(detailsBtn);
	Append(nameBtn);
	Append(sizeTxt);
	if (!mountMethod)//stuff we don't show if it is a DVD mounted
	{
		Append(btnLeft);
		Append(btnRight);
		for(int i = 0; i < FAVORITE_STARS; ++i)
			Append(FavoriteBtn[i]);
	}
	//check if unlocked
	if (mountMethod != 2 && (Settings.godmode || !(Settings.ParentalBlocks & BLOCK_GAME_SETTINGS)))
	{
		backBtn->SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
		backBtn->SetPosition(-50, -40);
		Append(settingsBtn);
	}

	Append(diskImg2);
	Append(gameBtn); //! Appending the disc on top of all

	ChangeGame(NONE);
	diskImg->SetImage(diskImgData);

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
}

GameWindow::~GameWindow()
{
	StopEffect();
	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	ResumeGui();

	while(parentElement && this->GetEffect() > 0) usleep(100);

	HaltGui();

	if(parentElement)
		((GuiWindow * ) parentElement)->Remove(this);

	RemoveAll();

	delete trigA;
	delete trigB;
	delete trigL;
	delete trigR;
	delete trigPlus;
	delete trigMinus;

	delete diskImgData;
	delete diskImgData2;
	delete dialogBox;
	delete btnOutline;
	delete imgFavorite;
	delete imgNotFavorite;
	delete imgLeft;
	delete imgRight;

	delete diskImg;
	delete diskImg2;

	delete dialogBoxImg;
	delete backBtnImg;
	delete settingsBtnImg;
	delete btnLeftImg;
	delete btnRightImg;

	delete nameBtnTT;

	delete sizeTxt;
	delete playcntTxt;
	delete nameTxt;
	delete backBtnTxt;
	delete settingsBtnTxt;
	delete detailsBtnTxt;
	delete detailsBtnOverTxt;

	delete nameBtn;
	delete gameBtn;
	delete backBtn;
	delete settingsBtn;
	delete btnLeft;
	delete btnRight;
	delete detailsBtn;

	for(int i = 0; i < FAVORITE_STARS; ++i)
	{
		delete FavoriteBtnImg[i];
		delete FavoriteBtn[i];
	}

	if(gameSound) gameSound->Stop();
	delete gameSound;
	bgMusic->SetVolume(Settings.volume);

	ResumeGui();
}

void GameWindow::LoadGameSound(const struct discHdr * header)
{
	if (Settings.gamesoundvolume == 0)
		return;

	if (gameSound)
	{
		gameSound->Stop();
		delete gameSound;
		gameSound = NULL;
	}

	u32 gameSoundDataLen;
	const u8 *gameSoundData = NULL;

	if(header->tid != 0)
		gameSoundData = BNRInstance::Instance()->GetBannerSound(header->tid, &gameSoundDataLen,
																(header->type == TYPE_GAME_EMUNANDCHAN) ? Settings.NandEmuChanPath : "");
	else
		gameSoundData = BNRInstance::Instance()->GetBannerSound(header->id, &gameSoundDataLen);

	if (gameSoundData)
	{
		gameSound = new GuiSound(gameSoundData, gameSoundDataLen, Settings.gamesoundvolume, true);
		bgMusic->SetVolume(0);
		reducedVol = true;
		if (Settings.gamesound == 2)
			gameSound->SetLoop(1);
		gameSound->Play();
	}
}

void GameWindow::LoadDiscImage(const u8 * id)
{
	HaltGui();
	delete diskImgData2;
	diskImgData2 = diskImgData;
	diskImgData = NULL;

	char imgPath[150];
	char IDFull[7];
	char ID3[4];
	char ID4[5];
	snprintf(IDFull, sizeof(IDFull), "%s", (char*) id);
	snprintf(ID3, sizeof(ID3), "%s", IDFull);
	snprintf(ID4, sizeof(ID4), "%s", IDFull);

	snprintf(imgPath, sizeof(imgPath), "%s%s.png", Settings.disc_path, IDFull); //changed to current full id
	diskImgData = new GuiImageData(imgPath);

	if (!diskImgData->GetImage())
	{
		delete diskImgData;
		snprintf(imgPath, sizeof(imgPath), "%s%s.png", Settings.disc_path, ID3); //changed to current id
		diskImgData = new GuiImageData(imgPath);
	}
	if (!diskImgData->GetImage())
	{
		delete diskImgData;
		snprintf(imgPath, sizeof(imgPath), "%s%s.png", Settings.disc_path, ID4); //changed to current id
		diskImgData = new GuiImageData(imgPath);
	}
	if (!diskImgData->GetImage())
	{
		delete diskImgData;
		diskImgData = Resources::GetImageData("nodisc.png");
	}
}

void GameWindow::SetWindowEffect(int direction, int in_out)
{
	if(direction == LEFT && Settings.xflip == XFLIP_DISK3D)
	{
		if(in_out == IN)
		{
			diskImg->SetImage(diskImgData);
			diskImg->SetBeta(90);
			diskImg->SetBetaRotateEffect(-90, 15);
			diskImg2->SetImage(diskImgData2);
			diskImg2->SetBeta(270);
			diskImg2->SetBetaRotateEffect(-90, 15);
			sizeTxt->SetEffect(EFFECT_FADE, 17);
			nameTxt->SetEffect(EFFECT_FADE, 17);
		}
		else
		{
			diskImg->SetImage(diskImgData2);
			diskImg->SetBeta(0);
			diskImg->SetBetaRotateEffect(90, 15);
			diskImg2->SetImage(diskImgData);
			diskImg2->SetAngle(diskImg->GetAngle());
			diskImg2->SetBeta(180);
			diskImg2->SetBetaRotateEffect(90, 15);
			sizeTxt->SetEffect(EFFECT_FADE, -17);
			nameTxt->SetEffect(EFFECT_FADE, -17);
		}
	}
	else if(direction == RIGHT && Settings.xflip == XFLIP_DISK3D)
	{
		if(in_out == IN)
		{
			diskImg->SetImage(diskImgData);
			diskImg->SetBeta(270);
			diskImg->SetBetaRotateEffect(90, 15);
			diskImg2->SetImage(diskImgData2);
			diskImg2->SetBeta(90);
			diskImg2->SetBetaRotateEffect(90, 15);
			sizeTxt->SetEffect(EFFECT_FADE, 17);
			nameTxt->SetEffect(EFFECT_FADE, 17);

		}
		else
		{
			diskImg->SetImage(diskImgData2);
			diskImg->SetBeta(0);
			diskImg->SetBetaRotateEffect(-90, 15);
			diskImg2->SetImage(diskImgData);
			diskImg2->SetAngle(diskImg->GetAngle());
			diskImg2->SetBeta(180);
			diskImg2->SetBetaRotateEffect(-90, 15);
			sizeTxt->SetEffect(EFFECT_FADE, -17);
			nameTxt->SetEffect(EFFECT_FADE, -17);
		}
	}
	else if(direction == LEFT)
	{
		if(in_out == IN)
			SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_IN, 50);
		else
			SetEffect(EFFECT_SLIDE_LEFT | EFFECT_SLIDE_OUT, 50);
	}
	else if(direction == RIGHT)
	{
		if(in_out == IN)
			SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_IN, 50);
		else
			SetEffect(EFFECT_SLIDE_RIGHT | EFFECT_SLIDE_OUT, 50);
	}

	ResumeGui();
	while(parentElement && (this->GetEffect() > 0 ||
		  nameTxt->GetEffect() > 0 || diskImg->GetBetaRotateEffect()))
	{
		usleep(100);
	}
}

void GameWindow::ChangeGame(int EffectDirection)
{
	struct discHdr * header = (mountMethod ? dvdheader : gameList[gameSelected]);
	LoadGameSound(header);
	LoadDiscImage(header->id);
	SetWindowEffect(EffectDirection, OUT);

	HaltGui();

	if (header->tid != 0)
	{
		if(header->type == TYPE_GAME_NANDCHAN)
			sizeTxt->SetTextf(tr("Real Nand"));
		else if(header->type == TYPE_GAME_EMUNANDCHAN)
			sizeTxt->SetTextf(tr("Emulated Nand"));

	}
	else if (!mountMethod)
	{
		float size = 0.0f;
		WBFS_GameSize(header->id, &size);
		sizeTxt->SetTextf("%.2fGB", size); //set size text;
	}

	diskImg->SetImage(diskImgData);
	nameTxt->SetText(GameTitles.GetTitle(header));
	playcntTxt->SetTextf("%s: %i", tr( "Play Count" ), GameStatistics.GetPlayCount(header));

	int favoritevar = GameStatistics.GetFavoriteRank(header->id);
	for(int i = 0; i < FAVORITE_STARS; ++i)
		FavoriteBtnImg[i]->SetImage(favoritevar >= i+1 ? imgFavorite : imgNotFavorite);

	EffectDirection = EffectDirection == LEFT ? RIGHT : EffectDirection == RIGHT ? LEFT : NONE;
	SetWindowEffect(EffectDirection, IN);
}

void GameWindow::Hide(void)
{
	GuiWindow *parentWindow = (GuiWindow *) parentElement;
	this->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	while(parentWindow && this->GetEffect() > 0) usleep(100);
	if(parentWindow) parentWindow->Remove(this);
}

void GameWindow::Show(void)
{
	this->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
	GuiWindow *parentWindow = (GuiWindow *) parentElement;
	if(parentWindow)
	{
		parentWindow->SetState(STATE_DISABLED);
		parentWindow->Append(this);
	}
}

int GameWindow::Run()
{
	int choice = -1;

	while(choice == -1)
	{
		usleep(1000);

		if (shutdown) //for power button
			Sys_Shutdown();
		else if (reset) //for reset button
			Sys_Reboot();

		choice = MainLoop();
	}

	return choice;
}

int GameWindow::MainLoop()
{
	diskImg->SetSpin(gameBtn->GetState() == STATE_SELECTED);
	diskImg2->SetSpin(gameBtn->GetState() == STATE_SELECTED);

	if (gameBtn->GetState() == STATE_CLICKED)
	{
		// Hide the window
		Hide();

		// If this function was left then the game start was canceled
		GameWindow::BootGame(mountMethod ? dvdheader : gameList[gameSelected]);

		// Show the window again
		Show();
	}

	else if (backBtn->GetState() == STATE_CLICKED) //back
	{
		mainWindow->SetState(STATE_DEFAULT);
		returnVal = 0;
	}

	else if(settingsBtn->GetState() == STATE_CLICKED) //settings
	{
		settingsBtn->ResetState();
		// Hide the window
		Hide();

		wiilight(0);
		int settret = GameSettingsMenu::Execute(browserMenu, mountMethod ? dvdheader : gameList[gameSelected]);

		// Show the window again or return to browser on uninstall
		if (settret == MENU_DISCLIST)
			returnVal = 1;
		else
			Show();

	}

	else if (nameBtn->GetState() == STATE_CLICKED) //rename
	{
		// Hide the window
		Hide();

		struct discHdr *header = mountMethod ? dvdheader : gameList[gameSelected];

		//enter new game title
		char entered[60];
		snprintf(entered, sizeof(entered), "%s", GameTitles.GetTitle(header));
		int result = OnScreenKeyboard(entered, 60, 0);
		if (result == 1)
		{
			WBFS_RenameGame(header->id, entered);
			GameTitles.SetGameTitle(header->id, entered);
			wString oldFilter(gameList.GetCurrentFilter());
			gameList.ReadGameList();
			gameList.FilterList(oldFilter.c_str());
			if(browserMenu) browserMenu->ReloadBrowser();
		}
		// Show the window again
		Show();

		nameBtn->ResetState();
	}

	else if (btnRight->GetState() == STATE_CLICKED) //next game
	{
		if(Settings.xflip == XFLIP_YES)
		{
			gameSelected = (gameSelected - 1 + gameList.size()) % gameList.size();
			ChangeGame(LEFT);
		}
		else if(Settings.xflip == XFLIP_SYSMENU)
		{
			gameSelected = (gameSelected + 1) % gameList.size();
			ChangeGame(LEFT);
		}
		else if(Settings.xflip == XFLIP_WTF)
		{
			gameSelected = (gameSelected - 1 + gameList.size()) % gameList.size();
			ChangeGame(RIGHT);
		}
		else
		{
			gameSelected = (gameSelected + 1) % gameList.size();
			ChangeGame(RIGHT);
		}

		btnRight->ResetState();
	}

	else if (btnLeft->GetState() == STATE_CLICKED) //previous game
	{
		if(Settings.xflip == XFLIP_YES)
		{
			gameSelected = (gameSelected + 1) % gameList.size();
			ChangeGame(RIGHT);
		}
		else if(Settings.xflip == XFLIP_SYSMENU)
		{
			gameSelected = (gameSelected - 1 + gameList.size()) % gameList.size();
			ChangeGame(RIGHT);
		}
		else if(Settings.xflip == XFLIP_WTF)
		{
			gameSelected = (gameSelected + 1) % gameList.size();
			ChangeGame(LEFT);
		}
		else
		{
			gameSelected = (gameSelected - 1 + gameList.size()) % gameList.size();
			ChangeGame(LEFT);
		}

		btnLeft->ResetState();
	}
	else if(detailsBtn->GetState() == STATE_CLICKED)
	{
		diskImg->SetState(STATE_DISABLED);
		showGameInfo(gameSelected);
		mainWindow->SetState(STATE_DISABLED);
		this->SetState(STATE_DEFAULT);
		diskImg->SetState(STATE_DEFAULT);
		detailsBtn->ResetState();
	}

	if (reducedVol)
	{
		if (gameSound)
		{
			if (Settings.gamesound == 1 && !gameSound->IsPlaying())
			{
				bgMusic->SetVolume(Settings.volume);
				reducedVol = false;
			}
		}
		else
		{
			bgMusic->SetVolume(Settings.volume);
			reducedVol = false;
		}
	}

	for(int i = 0; i < FAVORITE_STARS; ++i)
	{
		if(FavoriteBtn[i]->GetState() == STATE_CLICKED)
		{
			struct discHdr * header = (mountMethod ? dvdheader : gameList[gameSelected]);
			int FavoriteRank = (i+1 == GameStatistics.GetFavoriteRank(header->id)) ? 0 : i+1; // Press the current rank to reset the rank

			GameStatistics.SetFavoriteRank(header->id, FavoriteRank);
			GameStatistics.Save();
			for(int j = 0; j < FAVORITE_STARS; ++j)
				FavoriteBtnImg[j]->SetImage(FavoriteRank >= j+1 ? imgFavorite : imgNotFavorite);

			FavoriteBtn[i]->ResetState();
		}
	}

	return returnVal;
}


void GameWindow::BootGame(struct discHdr *header)
{
	wiilight(0);

	GameCFG* game_cfg = GameSettings.GetGameCFG(header->id);

	char IDfull[7];
	snprintf(IDfull, sizeof(IDfull), "%s", (char *) header->id);

	int gameIOS = game_cfg->ios == INHERIT ? Settings.cios : game_cfg->ios;

	if (game_cfg->loadalternatedol == 2)
	{
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.dol", Settings.dolpath, IDfull);
		if (CheckFile(filepath) == false)
		{
			sprintf(filepath, "%s %s", filepath, tr( "does not exist!" ));
			if(!WindowPrompt(tr( "Error" ), filepath, tr( "Continue" ), tr( "Cancel")))
				return;
		}
	}
	else if(game_cfg->loadalternatedol == 3 && WDMMenu::Show(header) == 0)
	{
		// Canceled
		return;
	}
	else if(game_cfg->loadalternatedol == 4)
	{
		if(!IosLoader::IsD2X(gameIOS))
			defaultDolPrompt((char *) header->id);
	}

	if (game_cfg->ocarina == ON || (game_cfg->ocarina == INHERIT && Settings.ocarina == ON))
	{
		char filepath[200];
		snprintf(filepath, sizeof(filepath), "%s%s.gct", Settings.Cheatcodespath, IDfull);
		if (CheckFile(filepath) == false)
		{
			sprintf(filepath, "%s %s", filepath, tr( "does not exist!  Loading game without cheats." ));
			if(!WindowPrompt(tr( "Error" ), filepath, tr( "Continue" ), tr( "Cancel")))
				return;
		}
	}

	if(header->type == TYPE_GAME_EMUNANDCHAN)
	{
		if(!IosLoader::IsD2X(gameIOS))
		{
			ShowError(tr("Launching emulated nand channels only works on d2x cIOS! Change game IOS to a d2x cIOS first."));
			return;
		}
	}

	GameStatistics.SetPlayCount(header->id, GameStatistics.GetPlayCount(header->id)+1);
	GameStatistics.Save();

	//Just calling that shuts down everything and starts game
	GameBooter::BootGame(header);
}