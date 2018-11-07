#include "Defines.h"

#include <Utility/GStorage.h>
#include <ROOT_PATH.h>

using namespace CppUtility::Other;
using namespace Define;
using namespace std;

Config * Define::DoConfig() {
	printf("INFO: Try to read config.out\n");
	string rootPath;
	Config * config = new Config;

	rootPath = string(ROOT_PATH);
	printf("INFO: [1] First Try.\n");
	config->Load(rootPath + "/config/config.out");
	if (!config->IsValid()) {
		rootPath = "../../..";
		printf("INFO: [2] Second Try.\n");
		config->Load(rootPath + "/config/config.out");
		if (!config->IsValid()) {
			rootPath = ".";
			printf("INFO: [3] Third Try.\n");
			config->Load(rootPath + "/config.out");
			if (!config->IsValid()) {
				printf(
					"ERROR: Three Try All Fail\n"
					"ERROR: RootPath is not valid.\n"
					"INFO: Please change config/config.out 's value of RootPath and\n"
					"      run exe in correct place( original place or same palce with config.out ).\n");
				exit(1);
			}
		}
	}

	*config->GetStrPtr("RootPath") = rootPath;
	printf("INFO: config.out read success\nINFO: RootPath is %s\n", config->GetStrPtr("RootPath")->c_str());
	GStorage<Config *>::GetInstance()->Register(str_MainCamera, config);
	return config;
}

float Define::lerp(float a, float b, float f){
	return a + f * (b - a);
}