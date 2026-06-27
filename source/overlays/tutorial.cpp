#include "tutorial.hpp"
#include "storeUtils.hpp"
#include <citro2d.h>

static bool drawTutorial = true;
static u32 keyMask = KEYS_ALL;
static int tutorialPage = 0;

static int fadeLevelTop = 0;
static int fadeLevelBottom = 0;
static int fadeTargetTop = 0;
static int fadeTargetBottom = 0;
static int blinkTimer = 0;

extern bool touching(touchPosition touch, Structs::ButtonPos button);

u32 Tutorial::GetKeyMask(void) { return keyMask; }

u32 textColor(int fadeLevel, bool blink = false) {
	if (blink && blinkTimer < 30) fadeLevel = 0;
	return (UIThemes->TextColor() & 0x00FFFFFF) | ((fadeLevel * 255 / 180) << 24);
}

void Tutorial::DrawTop() {
	if (!drawTutorial) return;

	Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadeLevelTop));

	if(tutorialPage >= 10 && tutorialPage < 1000) {
		std::string str = ((tutorialPage % 10) == 0) ? "\uE000 Continue" : "\uE001 Back \uE000 Continue";
		Gui::DrawString(380, 200, 0.7f, textColor(fadeLevelTop), str, 0, 0, font, C2D_AlignRight);
	} else if(tutorialPage == 4) {
		Gui::DrawString(380, 200, 0.7f, textColor(fadeLevelTop), "\uE001 Back \uE002 Exit", 0, 0, font, C2D_AlignRight);
	}

	switch(tutorialPage) {
		case 0:
			Gui::DrawStringCentered(0, 50, 1.0f, textColor(fadeLevelTop), "Welcome to Universal-Updater!", 370, 0, font);
			Gui::DrawStringCentered(0, 100, 0.7f, textColor(fadeLevelTop), "This is a quick tutorial on how to use it.", 370, 0, font, C2D_WordWrap);
			break;

		case 2:
		case 3:
			Gui::DrawString(75, 70, 1.0f, textColor(fadeLevelBottom, true), "←", 0, 0, font);
			break;

		case 4:
			Gui::DrawStringCentered(0, 10, 0.7f, textColor(fadeLevelTop), "The bottom screen has six different tabs each with different uses. Click on the one you'd like to learn about next!\n\nWhen you're done, press \uE002 to close the tutorial.", 380, 0, font, C2D_WordWrap);
			break;

		case 10:
			Gui::DrawStringCentered(0, 10, 0.6f, textColor(fadeLevelTop), "This first one is the information page.\n\nWhen on this page you can use the direction buttons to browse the apps (give it a try!) and you'll get all the basic info on the bottom screen.", 380, 0, font, C2D_WordWrap);
			break;

		case 11:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelTop), "There are four buttons along the bottom of the info page. This first one lets you mark apps with a few different tags (★♥♦♣♠) which can be used for filtering.\n\nFor example, you could mark your favorite apps with a star or a heart, or you could make a list of apps you want to check out later.\n\nYou can then select which tagged apps you want to show from the search & filters tab.", 380, 0, font, C2D_WordWrap);
			break;

		case 12:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelTop), "Next we've got screenshots, this'll pull up a few screenshots of the app so you can get an idea what it'll be like before you download it. Not all apps have screenshots as someone actually has to make them.", 380, 0, font, C2D_WordWrap);
			break;

		case 13:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelTop), "The next one opens up the latest release notes for the app, it's always a good idea to give those a look over when updating!\n\nThe developers may have important notes if there were any major changes, and it's always nice to know what you're getting out of the update anyways.", 380, 0, font, C2D_WordWrap);

			break;

		case 14:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelTop), "Finally we have a link to the app's wiki. Some homebrew can do a lot and the developers may have a wiki or other documentation to help you figure everything out.\n\nA QR code will be displayed that you can scan on your phone to quickly pull it up or you can also attempt to load the wiki in the 3DS's web browser. Sadly the 3DS browser is rather on the older side these days so it doesn't always work.", 380, 0, font, C2D_WordWrap);
			break;

		case 100:
			Gui::DrawStringCentered(0, 50, 1.0f, textColor(fadeLevelTop), "Have fun using Universal-Updater!", 370, 0, font);
			Gui::DrawStringCentered(0, 100, 0.7f, textColor(fadeLevelTop), "If you ever need to access the tutorial again, click this button on the settings tab.", 370, 0, font, C2D_WordWrap);
	}
}

void Tutorial::DrawBottom() {
	if (!drawTutorial) return;

	Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(0, 0, 0, fadeLevelBottom));

	if(tutorialPage < 10) {
		Gui::DrawString(300, 170, 0.7f, textColor(fadeLevelBottom), "\uE000 Continue", 0, 0, font, C2D_AlignRight);
		if (tutorialPage != 0) Gui::DrawString(300, 200, 0.7f, textColor(fadeLevelBottom), "\uE001 Back", 0, 0, font, C2D_AlignRight);
	}

	switch(tutorialPage) {
		case 0:
			Gui::DrawString(50, 170, 0.7f, textColor(fadeLevelBottom), "\uE003 Language", 0, 0, font);
			Gui::DrawString(50, 200, 0.7f, textColor(fadeLevelBottom), "\uE002 Skip", 0, 0, font);
			break;

		case 1:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelBottom), "Up on the top screen is a grid or list of everything in the current UniStore.\n\nUniStores are Universal-Updater's form of repositories. The default that you can see right now is Universal-DB, it contains most homebrew apps made for the 3DS!\n\nYou can find others in settings or even make your own!", 300, 0, font, C2D_WordWrap);
			break;

		case 2:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelBottom), "There are a couple of icons used to indicate the status of apps.\n\nThis SD card icon is shown when Universal-Updater has detected that you have this app installed.", 300, 0, font, C2D_WordWrap);
			break;

		case 3:
			Gui::DrawStringCentered(0, 10, 0.5f, textColor(fadeLevelBottom), "This up arrow in a green circle is shown when there's a new update available!\n\nBoth of these can be used as filters in the search menu for easily finding new apps, apps in need of updates, etc.", 300, 0, font, C2D_WordWrap);
			break;

		case 4:
			for(int i = 0; i < 6; i++) {
				Gui::DrawString(45, 5 + i * 40, 1.0f, textColor(fadeLevelTop, true), "←", 0, 0, font);
			}
			break;

		case 11:
			Gui::DrawString(46, 185, 1.0f, textColor(fadeLevelTop, true), "↓", 0, 0, font);
			break;

		case 12:
			Gui::DrawString(76, 185, 1.0f, textColor(fadeLevelTop, true), "↓", 0, 0, font);
			break;

		case 13:
			Gui::DrawString(106, 185, 1.0f, textColor(fadeLevelTop, true), "↓", 0, 0, font);
			break;

		case 14:
			Gui::DrawString(136, 185, 1.0f, textColor(fadeLevelTop, true), "↓", 0, 0, font);
			break;

		case 100:
			Gui::DrawString(260, -2, 1.0f, textColor(fadeLevelTop, true), "→", 0, 0, font);
	}
}

void Tutorial::Logic(u32 hDown, touchPosition &touch, MainScreen &ms) {
	if (!drawTutorial) return;

	blinkTimer++;
	if (blinkTimer > 60) blinkTimer = 0;

	if (fadeLevelTop < fadeTargetTop - 4) fadeLevelTop += 4;
	else if (fadeLevelTop > fadeTargetTop + 4) fadeLevelTop -= 4;
	else fadeLevelTop = fadeTargetTop;

	if (fadeLevelBottom < fadeTargetBottom - 4) fadeLevelBottom += 4;
	else if (fadeLevelBottom > fadeTargetBottom + 4) fadeLevelBottom -= 4;
	else fadeLevelBottom = fadeTargetBottom;

	switch (tutorialPage) {
		case 0:
			fadeTargetTop = 180;
			fadeTargetBottom = 180;
			keyMask = KEYS_NONE;
			ms.SwitchPage(0, 0);

			if (hDown & KEY_A) {
				tutorialPage++;
			} else if (hDown & KEY_Y) {
				tutorialPage = 1000;
			} else if (hDown & KEY_X) {
				tutorialPage = 100;
			}
			break;

		case 1:
			keyMask = KEYS_NONE;
			ms.SwitchPage(0, 0);
			StoreUtils::RefreshInstalledApps(StoreUtils::entries[0]->GetTitle());
			fadeTargetTop = 0;
			fadeTargetBottom = 180;
			goto base;

		case 2:
			StoreUtils::entries[0]->SetInstalled(true);
			StoreUtils::entries[0]->SetUpdateAvl(false);
			goto base;

		case 3:
			keyMask = KEYS_NONE;
			fadeTargetTop = 0;
			fadeTargetBottom = 180;
			StoreUtils::entries[0]->SetUpdateAvl(true);
			goto base;

		case 4:
			StoreUtils::RefreshInstalledApps(StoreUtils::entries[0]->GetTitle());
			fadeTargetTop = 180;
			fadeTargetBottom = 0;
			if (hDown & KEY_TOUCH) {
				for (int i = 0; i < 6; i++) {
					if(touching(touch, {0, i * 40, 40, 40})) {
						tutorialPage = (i + 1) * 10;
					}
				}
			} else if (hDown & KEY_X) {
				tutorialPage = 100;
			} else if (hDown & KEY_B) {
				tutorialPage--;
			}
			break;

		case 10:
			keyMask = KEYS_DPAD;
			goto base;

		case 11:
			keyMask = KEYS_NONE;
			for(size_t i = 0; i < StoreUtils::entries.size(); i++) {
				const StoreEntry &entry = *StoreUtils::entries[i];
				if(!entry.GetScreenshots().empty() && !entry.GetReleaseNotes().empty() && !entry.GetWiki().empty()) {
					StoreUtils::store->SetEntry(i);
					StoreUtils::store->SetScreenIndx(i / 5);
					StoreUtils::store->SetBox(i % 5);
					break;
				}
			}
			goto base;

		case 15:
			tutorialPage = 4;
			goto base;

		case 100:
			ms.SwitchPage(5, 0);
			fadeTargetBottom = 0;
			if (hDown & KEY_A) tutorialPage = 1001;
			break;

		case 1000:
			keyMask = KEY_A | KEYS_DPAD;
			ms.SwitchPage(5, 4); // Settings -> Language
			fadeTargetBottom = 0;
			if (hDown & (KEY_A | KEY_B)) {
				tutorialPage = hDown & KEY_A ? 1 : 0;
			}
			break;

		case 1001:
			ms.SwitchPage(0, 0);
			keyMask = KEYS_ALL;
			fadeTargetTop = 0;
			fadeTargetBottom = 0;
			if (fadeTargetTop == fadeLevelTop) drawTutorial = false;
			break;

		base:
		default:
			if (hDown & KEY_A) {
				tutorialPage++;
			} else if (hDown & KEY_B && (tutorialPage % 10) != 0) {
				tutorialPage--;
			}

			break;

	}
}