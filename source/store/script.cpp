#include "script.hpp"

using namespace rapidjson;

Script::Script(const std::string &name, const rapidjson::Value &json, bool installed, const std::string &size, const std::string &type)
	: Name(name), Size(size), Installed(installed) {
	if (type == "git" || type == "nightly") this->Git = true;
	else if (type == "prerelease") this->Prerelease = true;

	if (!json.IsArray()) return;
	for (const Value &item : json.GetArray()) {
		// All items must be objects with types
		if (!item.IsObject() || !item.HasMember("type") || !item["type"].IsString()) {
			this->Actions.clear();
			return;
		}

		Action::Type type = Action::Type::Error;
		const std::string &typeStr = item["type"].GetString();
		if (typeStr == "copy") type = Action::Type::Copy;
		else if (typeStr == "deleteFile") type = Action::Type::DeleteFile;
		else if (typeStr == "downloadFile") type = Action::Type::DownloadFile;
		else if (typeStr == "downloadRelease") type = Action::Type::DownloadRelease;
		else if (typeStr == "exit") type = Action::Type::Exit;
		else if (typeStr == "extractFile") type = Action::Type::ExtractFile;
		else if (typeStr == "installCia") type = Action::Type::InstallCia;
		else if (typeStr == "mkdir") type = Action::Type::Mkdir;
		else if (typeStr == "move") type = Action::Type::Move;
		else if (typeStr == "promptMessage" || typeStr == "promptMsg") type = Action::Type::PromptMessage;
		else if (typeStr == "rmdir") type = Action::Type::Rmdir;
		else if (typeStr == "skip") type = Action::Type::Skip;

		if (type == Action::Type::Error) {
			this->Actions.clear();
			return;
		}

		switch (type) {
			case Action::Type::Copy: {
				if (item.HasMember("source") && item["source"].IsString()
					&& item.HasMember("destination") && item["destination"].IsString()) {
					this->Actions.emplace_back(type, item["source"].GetString(), item["destination"].GetString());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::DeleteFile:
			case Action::Type::InstallCia: {
				if (item.HasMember("file") && item["file"].IsString()) {
					this->Actions.emplace_back(type, item["file"].GetString());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::DownloadFile: {
				if (item.HasMember("file") && item["file"].IsString()
					&& item.HasMember("output") && item["output"].IsString()) {
					this->Actions.emplace_back(type, item["file"].GetString(), item["output"].GetString());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::DownloadRelease: {
				if (item.HasMember("file") && item["file"].IsString()
					&& item.HasMember("output") && item["output"].IsString()
					&& item.HasMember("repo") && item["repo"].IsString()) {
					bool prereleases = (item.HasMember("includePrereleases") && item["includePrereleases"].IsTrue());
					this->Actions.emplace_back(type, item["file"].GetString(), item["output"].GetString(), item["repo"].GetString(), prereleases);
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::Exit: {
				this->Actions.emplace_back(type);
				break;
			}

			case Action::Type::ExtractFile: {
				if (item.HasMember("input") && item["input"].IsString()
					&& item.HasMember("output") && item["output"].IsString()
					&& item.HasMember("file") && item["file"].IsString()) {
					this->Actions.emplace_back(type, item["input"].GetString(), item["output"].GetString(), item["file"].GetString());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::Mkdir:
			case Action::Type::Rmdir: {
				if (item.HasMember("directory") && item["directory"].IsString()) {
					this->Actions.emplace_back(type, item["directory"].GetString());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::Move: {
				if (item.HasMember("old") && item["old"].IsString()
					&& item.HasMember("new") && item["new"].IsString()) {
					this->Actions.emplace_back(type, item["old"].GetString(), item["new"].GetString());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::PromptMessage: {
				if (item.HasMember("message") && item["message"].IsString()) {
					int count = -1;
					std::string name;
					if (item.HasMember("count") && item["count"].IsInt()) count = item["count"].GetInt();
					if (item.HasMember("name") && item["name"].GetString()) name = item["name"].GetString();
					this->Actions.emplace_back(type, item["message"].GetString(), name, count);
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::Skip: {
				if (item.HasMember("count") && item["count"].IsInt()) {
					this->Actions.emplace_back(type, item["count"].GetInt());
				} else {
					this->Actions.clear();
					return;
				}
				break;
			}

			case Action::Type::Error: {
				this->Actions.clear();
				return;
			}
		}
	}
}
