#include <string>
#include "json.hpp"


std::string get(nlohmann::json json, const std::string &key, const std::string &key2) {
	if(!json.contains(key))	return "MISSING: " + key;
	if(!json.at(key).is_object())	return "NOT OBJECT: " + key;

	if(!json.at(key).contains(key2))	return "MISSING: " + key + "." + key2;
	if(!json.at(key).at(key2).is_string())	return "NOT STRING: " + key + "." + key2;

	return json.at(key).at(key2).get_ref<const std::string&>();
}

void parse(std::string fileName) {
	FILE* file = fopen(fileName.c_str(), "rt");
	if(!file) {
		printf("File not found\n");
		fclose(file);
		return;
	}
	nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
	fclose(file);

	std::string title = get(json, "info", "title");
	std::string description = get(json, "info", "description");

	printf("%s\n", title.c_str());
	printf("%s\n", description.c_str());
	printf("------------------------------------------------------------------------------------------\n");

	printf("Options:\n");
	for(auto it = json.begin();it != json.end(); it++) {
		if(it.key() != "info") {
			printf("> %s\n", it.key().c_str());
		}
	}
	printf("------------------------------------------------------------------------------------------\n");

	char choise[16];
	while(!json.contains(choise)) {
		printf("Choose an option: ");
		scanf("%s", choise);
	}

	printf("Parsing %s...\n", choise);
	for(int i=0;i<json.at(choise).size();i++) {
		std::string type = json.at(choise).at(i).at("type");
		printf("------------------------------------------------------------------------------------------\n");
		printf("Running: %s\n", type.c_str());
		

		if(type == "downloadFile") {
			std::string file = json.at(choise).at(i).at("file");
			std::string output = json.at(choise).at(i).at("output");
			printf("Downloading %s to %s\n", file.c_str(), output.c_str());
		} else if(type == "downloadRelease") {
			std::string file = json.at(choise).at(i).at("file");
			std::string repo = json.at(choise).at(i).at("repo");
			std::string output = json.at(choise).at(i).at("output");
			printf("Downloading %s from %s to %s\n", file.c_str(), repo.c_str(), output.c_str());
		} else if(type == "installCia") {
			std::string file = json.at(choise).at(i).at("file");
			printf("Installing %s\n", file.c_str());
		} else if(type == "deleteFile") {
			std::string file = json.at(choise).at(i).at("file");
			printf("Deleting %s\n", file.c_str());
		}
	}
}