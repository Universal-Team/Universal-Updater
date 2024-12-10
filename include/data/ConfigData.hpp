// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (c) 2019-2024 Universal-Team

#ifndef _UNIVERSAL_UPDATER_CONFIG_DATA_HPP
#define _UNIVERSAL_UPDATER_CONFIG_DATA_HPP

#include "JSON.hpp"
#include <string>


class ConfigData {
public:
	ConfigData() { this->Load(); };
	void Load();
	void Initialize();
	void Sav();

	/* Last Store. */
	std::string LastStore() const { return this->vLastStore; };
	void LastStore(const std::string &v) { this->vLastStore = v; if (!this->ChangesMade) this->ChangesMade = true; };

	/* Using Top List. */
	bool List() const { return this->vList; };
	void List(bool v) { this->vList = v; if (!this->ChangesMade) this->ChangesMade = true; };

	/* Auto update on boot. */
	bool AutoUpdate() const { return this->vAutoUpdate; };
	void AutoUpdate(bool v) { this->vAutoUpdate = v; if (!this->ChangesMade) this->ChangesMade = true; };

	/* Fetching old metadata. */
	bool MetaData() const { return this->vMetaData; };
	void MetaData(bool v) { this->vMetaData = v; if (!this->ChangesMade) this->ChangesMade = true; };
private:
	template <class T>
	T Get(const std::string &Key, const T IfNotFound) {
		if (this->CFG.is_discarded() || !this->CFG.contains(Key)) return IfNotFound;

		return this->CFG.at(Key).get_ref<const T &>();
	};

	template <class T>
	void Set(const std::string &Key, const T Data) {
		if (!this->CFG.is_discarded()) this->CFG[Key] = Data;
	};

	/* Returns the language code of the system language. */
	std::string SysLang(void) const;

	/* All variables for the config here. */
	std::string vLastStore = "universal-db.unistore";
	bool vList = false, vAutoUpdate = true, vMetaData = true;

	bool ChangesMade = false;
	nlohmann::json CFG = nullptr;
};

#endif