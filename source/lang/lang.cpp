#include "config.hpp"
#include "lang.hpp"

#include <stdio.h>

nlohmann::json appJson;

std::string Lang::get(const std::string &key) {
	if(!appJson.contains(key)) {
		return "MISSING: " + key;
	}
	return appJson.at(key).get_ref<const std::string&>();
}

std::string langs[] = {"br", "de", "en", "es", "fr", "it", "lt", "pt", "ru", "jp"};

void Lang::load(int lang) {
	FILE* values;
	if (Config::LangPath == 1) {
		values = fopen(("sdmc:/3ds/Universal-Updater/app.json"), "rt");
	} else {
		values = fopen(("romfs:/lang/"+langs[lang]+"/app.json").c_str(), "rt");
	}
	if(values)    appJson = nlohmann::json::parse(values, nullptr, false);
	fclose(values);
}