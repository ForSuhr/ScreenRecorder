#include "UtilsWrapper.h"
#include <string>
using namespace std;

UtilsWrapper::UtilsWrapper()
{
}

UtilsWrapper::~UtilsWrapper()
{
}

bool UtilsWrapper::InitUtils()
{
	string locale = "en-US";
	string module_config_path = "../config";
	if (!obs_startup(locale.c_str(), module_config_path.c_str(), NULL)) {
		return false;
	}
	else {
		return true;
	}
}
