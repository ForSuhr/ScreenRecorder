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
	bool SetupVideo();
	bool SetupScene();

private:
	OBSOutput output;
	obs_source_t* fadeTransition = nullptr;
	obs_scene_t* scene = nullptr;
};

