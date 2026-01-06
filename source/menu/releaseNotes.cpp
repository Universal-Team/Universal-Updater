/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "animation.hpp"
#include "common.hpp"
#include "download.hpp"
#include "storeUtils.hpp"

std::vector<std::string> wrappedNotes;
touchPosition touches[2]; //Stores last two touch positions
static const Structs::ButtonPos back = { 4, 0, 24, 24 };
extern bool touching(touchPosition touch, Structs::ButtonPos button);

size_t StoreUtils::FindSplitPoint(const std::string &str, const std::vector<std::string> splitters) {
	for (const std::string &splitter : splitters) {
		size_t pos = str.rfind(splitter);
		if (pos != std::string::npos) return pos;
	}

	return std::string::npos;
}

/* Process release notes into lines */
const std::vector<std::string> &StoreUtils::ProcessReleaseNotes(std::string releaseNotes, float wrapWidth, float fontSize) {
	wrappedNotes.clear();

	size_t splitPos = 0;
	do {
		splitPos = releaseNotes.find('\n');
		std::string substr = releaseNotes.substr(0, splitPos);

		Gui::clearTextBufs();
		float width = Gui::GetStringWidth(fontSize, substr, font);

		/* If too long to fit on screen, wrap at spaces, slashes, periods, etc. */
		size_t spacePos;
		while (width > wrapWidth && (spacePos = FindSplitPoint(substr.substr(0, splitPos - 1), {" ", "/", ".", "-", "_", "。", "、", "，"})) != std::string::npos) {
			splitPos = spacePos;
			if (substr[splitPos] != ' ') splitPos++;

			/* Skip to next if UTF-8 multibyte char */
			while ((substr[splitPos] & 0xC0) == 0x80) splitPos++;

			substr = substr.substr(0, splitPos);
			Gui::clearTextBufs();
			width = Gui::GetStringWidth(fontSize, substr, font);
		}

		wrappedNotes.push_back(substr);

		if (splitPos != std::string::npos) {
			if (releaseNotes[splitPos] == ' ' || releaseNotes[splitPos] == '\n') splitPos++;
			releaseNotes = releaseNotes.substr(splitPos);
		}
	} while (splitPos != std::string::npos);

	return wrappedNotes;
}

void StoreUtils::DrawReleaseNotes(const float &scrollOffset, const std::unique_ptr<StoreEntry> &entry) {
	if (entry && StoreUtils::store) {
		Gui::ScreenDraw(Bottom);
		Gui::Draw_Rect(0, 26, 320, 214, UIThemes->BGColor());
		
		float fontHeight = Gui::GetStringHeight(0.5f, "", font);
		for (size_t i = 0; i < wrappedNotes.size(); i++) {
			if (25 + i * fontHeight > scrollOffset && 25 + i * fontHeight < scrollOffset + 240.0f) 
			Gui::DrawString(5, 25 + i * fontHeight - scrollOffset, 0.5f, UIThemes->TextColor(), wrappedNotes[i], 310, 0, font);
		}
		uint32_t accentColor = config->useAccentColor() && !config->changelog() ? entry->GetAccentColor() : 0;

		Gui::Draw_Rect(0, 0, 320, 25, accentColor ? accentColor : UIThemes->EntryBar());
		Gui::Draw_Rect(0, 25, 320, 1, UIThemes->BarOutline());
		Gui::DrawStringCentered(0, 1, 0.7f, accentColor ? WHITE : UIThemes->TextColor(), config->changelog() ? std::string("Universal-Updater ") + C_V : entry->GetTitle(), 310, 0, font);
		
		GFX::DrawIcon(sprites_arrow_idx, back.x, back.y, UIThemes->TextColor());

		Gui::ScreenDraw(Top);
		Gui::Draw_Rect(0, 0, 400, 240, DIM_COLOR); // Darken.

	}
	Animation::QueueEntryDone();
}

/*
	As the name says: Release notes logic.

	float &scrollOffset: The scroll offset for the Release Notes text.
	float &scrollDelta: The scroll delta for the Release Notes text.
	int &storeMode: The store mode to properly return back.
*/
void StoreUtils::ReleaseNotesLogic(float &scrollOffset, float &scrollDelta, int &storeMode) {
	int linesPerScreen = ((240.0f - 25.0f) / Gui::GetStringHeight(0.5f, "", font));
	scrollOffset += scrollDelta;
	if (scrollDelta > 0) {
		scrollDelta = std::max(scrollDelta - 1.0f, 0.0f);
	} else if (scrollDelta < 0) {
		scrollDelta = std::min(scrollDelta + 1.0f, 0.0f);
	}

	if ((int)wrappedNotes.size() > linesPerScreen) {
		//D-Pad
		if (hHeld & KEY_DDOWN && scrollDelta < 10.0f) scrollDelta += 2.0f;
		if (hHeld & KEY_DUP && scrollDelta > -10.0f) scrollDelta -= 2.0f;
		if (hHeld & KEY_DRIGHT && scrollDelta < 20.0f) scrollDelta += 5.0f;
		if (hHeld & KEY_DLEFT && scrollDelta > -20.0f) scrollDelta -= 5.0f;

		//Circle Pad
		circlePosition circlePad;
		hidCircleRead(&circlePad);
		float deltaCircle = -circlePad.dy / 60.0f;
		if (deltaCircle >= 0.5f || deltaCircle <= -0.5f) {
			if (scrollDelta < 10.0f && scrollDelta > -10.0f) scrollDelta += deltaCircle;
		}

		//Touch
		if (hDown & KEY_TOUCH && touch.py > 25) {
			touches[1] = touch;
			scrollDelta = 0.0f;
		}
		if (hHeld & KEY_TOUCH) {
			if (touch.py > 25) {
				scrollOffset -= touch.py - touches[1].py;
			}
			touches[0] = touches[1];
			touches[1] = touch;
		}
		if (hUp & KEY_TOUCH && touches[1].py > 25) {
			scrollDelta = touches[0].py - touches[1].py;
		}

	}

	/* Ensure it doesn't scroll off screen. */
	if (scrollOffset < 0.0f) {
		scrollOffset = 0.0f;
		scrollDelta = 0.0f;
	}
	int maxScroll = wrappedNotes.size() * Gui::GetStringHeight(0.5f, "", font) - (240.0f - 25.0f);
	if (scrollOffset > maxScroll && ((int)wrappedNotes.size() > linesPerScreen)) {
		scrollOffset = maxScroll;
		scrollDelta = 0.0f;
	}

	if (hDown & KEY_B || (hDown & KEY_TOUCH && touching(touch, back))) {
		if (config->changelog()) config->changelog(false);
		scrollOffset = 0.0f;
		scrollDelta = 0.0f;
		storeMode = 0;
	}
}