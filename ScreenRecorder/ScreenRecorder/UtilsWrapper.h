#pragma once

#include "obs.h"
#include "obs.hpp"

class UtilsWrapper
{
public:
	UtilsWrapper();
	~UtilsWrapper();

	bool InitUtils();
	bool StartRec();
	bool StopRec();

private:
	bool LoadPlugins();
	bool SetupAudio();
	int SetupVideo();

private:
	OBSOutput output;
};

