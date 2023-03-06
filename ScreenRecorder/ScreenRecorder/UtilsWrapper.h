#pragma once

#include "obs.h"
#include "obs.hpp"

class UtilsWrapper
{
public:
	UtilsWrapper();
	~UtilsWrapper();

	bool InitUtils();
	bool LoadPlugins();
	bool SetupAudio();
	int SetupVideo();

};

