#pragma once

#include "obs.h"
#include "obs.hpp"
#include <string>
using namespace std;

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
	bool CreateSource();
	bool CreateOutputMode();
	bool SetupFFmpeg();

private:
	OBSOutput output;
	obs_source_t* fadeTransition = nullptr;
	obs_scene_t* scene = nullptr;
	obs_source_t* captureSource = nullptr;
	obs_properties_t* properties = nullptr;
	obs_data_t* setting = nullptr;
	obs_property_t* property = nullptr;
	OBSEncoder aacTrack[MAX_AUDIO_MIXES];
	string aacEncoderID[MAX_AUDIO_MIXES];
};

