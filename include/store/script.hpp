/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2026 Universal-Team
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

#ifndef _UNIVERSAL_UPDATER_SCRIPT_HPP
#define _UNIVERSAL_UPDATER_SCRIPT_HPP

#include "rapidjson/document.h"
#include <3ds/types.h>
#include <vector>

class Action {
public:
	enum class Type {
		Copy,
		DeleteFile,
		DownloadFile,
		DownloadRelease,
		Exit,
		ExtractFile,
		InstallCia,
		Mkdir,
		Move,
		PromptMessage,
		Rmdir,
		Skip,
		Error
	};

private:
	Type vType;

	std::string Input;
	std::string Output;

	std::string Extra;
	bool IncludePrereleases;
	int Count;

public:
	// Exit
	Action(Type type) : vType(type) {}

	// DeleteFile, InstallCia, Mkdir, Rmdir
	Action(Type type, const std::string &file)
		: vType(type), Input(file) {}

	// Skip
	Action(Type type, int count)
		: vType(type), Count(count) {}

	// Copy, DownloadFile, Move
	Action(Type type, const std::string &in, const std::string &out)
		: vType(type), Input(in), Output(out) {}

	// PromptMessage
	Action(Type type, const std::string &message, const std::string &name, int count)
		: vType(type), Input(message), Extra(name), Count(count) {}

	// ExtractFile
	Action(Type type, const std::string &in, const std::string &out, const std::string &file)
		: vType(type), Input(in), Output(out), Extra(file) {}

	// DownloadRelease
	Action(Type type, const std::string &in, const std::string &out, const std::string &repo, bool prereleases)
		: vType(type), Input(in), Output(out), Extra(repo), IncludePrereleases(prereleases) {}

	Type GetType() const { return this->vType; }

	const std::string &GetInput() const { return this->Input; }
	const std::string &GetOutput() const { return this->Output; }
	const std::string &GetExtra() const { return this->Extra; }
	bool GetPrereleases() const { return this->IncludePrereleases; }
	int GetCount() const { return this->Count; }
};

class Script {
	std::vector<Action> Actions;
	std::string Name;
	std::string Size;

	bool Installed;
	bool Git = false;
	bool Prerelease = false;

public:
	Script(const std::string &name, const rapidjson::Value &json, bool installed, const std::string &size = "", const std::string &type = "");

	const std::string &GetName() const { return this->Name; }
	const std::string &GetSize() const { return this->Size; }
	const Action &GetAction(int i) const { return this->Actions[i]; }
	const std::vector<Action> &GetActions() const { return this->Actions; }

	bool IsInstalled() const { return this->Installed; }
	bool IsGit() const { return this->Git; }
	bool IsPrerelease() const { return this->Prerelease; }

	void SetInstalled(bool v) { this->Installed = v; }
};

#endif
