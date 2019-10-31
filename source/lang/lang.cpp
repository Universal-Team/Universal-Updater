#include "lang/lang.hpp"

#include <stdio.h>

nlohmann::json appJson;

std::string Lang::get(const std::string &key) {
	if(!appJson.contains(key)) {
		return "MISSING: " + key;
	}
	return appJson.at(key).get_ref<const std::string&>();
}

std::string langs[] = {"de", "en", "es", "fr", "it", "jp", "lt", "pt"};

void Lang::load(int lang) {
	FILE* values;
	values = fopen(("romfs:/lang/"+langs[lang]+"/app.json").c_str(), "rt");
	if(values)    appJson = nlohmann::json::parse(values, nullptr, false);
	fclose(values);
}