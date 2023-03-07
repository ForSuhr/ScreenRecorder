#include "UtilsWrapper.h"
#include <string>
#include <QApplication>

#ifdef __APPLE__
#define INPUT_AUDIO_SOURCE "coreaudio_input_capture"
#define OUTPUT_AUDIO_SOURCE "coreaudio_output_capture"
#elif _WIN32
#define INPUT_AUDIO_SOURCE "wasapi_input_capture"
#define OUTPUT_AUDIO_SOURCE "wasapi_output_capture"
#else
#define INPUT_AUDIO_SOURCE "pulse_input_capture"
#define OUTPUT_AUDIO_SOURCE "pulse_output_capture"
#endif

using namespace std;
constexpr auto DL_D3D11 = "libobs-d3d11.dll";
constexpr auto DL_OPENGL = "libobs-opengl.dll";
constexpr auto FPS = 60;
constexpr auto WIDTH = 1920;
constexpr auto HEIGHT = 1080;

enum SOURCE_CHANNELS {
	SOURCE_CHANNEL_TRANSITION,
	SOURCE_CHANNEL_AUDIO_OUTPUT,
	SOURCE_CHANNEL_AUDIO_INPUT,
};

static void ResetAudioDevice(const char* sourceId, const char* deviceId,
	const char* deviceDesc, int channel)
{
	bool disable = deviceId && strcmp(deviceId, "disabled") == 0;
	obs_source_t* source;
	obs_data_t* settings;

	source = obs_get_output_source(channel);
	if (source) {
		if (disable) {
			obs_set_output_source(channel, nullptr);
		}
		else {
			settings = obs_source_get_settings(source);
			const char* oldId =
				obs_data_get_string(settings, "device_id");
			if (strcmp(oldId, deviceId) != 0) {
				obs_data_set_string(settings, "device_id",
					deviceId);
				obs_source_update(source, settings);
			}
			obs_data_release(settings);
		}
		
		obs_source_release(source);

	}
	else if (!disable) {
		settings = obs_data_create();
		obs_data_set_string(settings, "device_id", deviceId);
		source = obs_source_create(sourceId, deviceDesc, settings,
			nullptr);
		obs_data_release(settings);

		obs_set_output_source(channel, source);
		obs_source_release(source);
	}
}

static inline bool HasAudioDevices(const char* source_id)
{
	const char* output_id = source_id;
	obs_properties_t* props = obs_get_source_properties(output_id);
	size_t count = 0;

	if (!props)
		return false;

	obs_property_t* devices = obs_properties_get(props, "device_id");
	if (devices)
		count = obs_property_list_item_count(devices);

	obs_properties_destroy(props);

	return count != 0;
}

UtilsWrapper::UtilsWrapper()
{
}

UtilsWrapper::~UtilsWrapper()
{
}

bool UtilsWrapper::InitUtils()
{
	// start initialization
	string locale = "en-US";
	string module_config_path = "../config";
	if (!obs_initialized()) {
		if (!obs_startup(locale.c_str(), module_config_path.c_str(), NULL)) {
			return false;
		}
	}
	
	// load plugins
	if (!LoadPlugins()) {
		return false;
	}

	// setup audio
	if (!SetupAudio()) {
		return false;
	}	
	
	// setup video
	if (!SetupVideo()) {
		return false;
	}

	// setup scene
	if (!SetupScene()) {
		return false;
	}

	// end initialization
	return true;
}

bool UtilsWrapper::LoadPlugins() {
	string app_path = qApp->applicationDirPath().toStdString();
	string plugin_path = app_path + "/obs-plugins/32bit";
	string data_path = app_path + "/data/obs-plugins/%module%";
	obs_add_module_path(plugin_path.c_str(), data_path.c_str());
	obs_load_all_modules();
	return true;
}

bool UtilsWrapper::SetupAudio() {
	struct obs_audio_info audio_info;
	audio_info.samples_per_sec = 48000;
	audio_info.speakers = SPEAKERS_STEREO;
	return obs_reset_audio(&audio_info);
}

bool UtilsWrapper::SetupVideo() {
	struct obs_video_info video_info;
	video_info.graphics_module = DL_D3D11;
	video_info.fps_num = FPS;
	video_info.fps_den = 1;
	video_info.base_width = WIDTH;
	video_info.base_height = HEIGHT;
	video_info.output_width = WIDTH;
	video_info.output_height = HEIGHT;
	video_info.output_format = VIDEO_FORMAT_RGBA;
	video_info.adapter = 0;
	video_info.gpu_conversion = true;
	video_info.colorspace = VIDEO_CS_DEFAULT;
	video_info.range = VIDEO_RANGE_DEFAULT;
	video_info.scale_type = OBS_SCALE_DISABLE;
	if (obs_reset_video(&video_info) != OBS_VIDEO_SUCCESS) {
		return false;
	}
	return true;
}

bool UtilsWrapper::SetupScene()
{
	obs_set_output_source(SOURCE_CHANNEL_TRANSITION, nullptr);
	obs_set_output_source(SOURCE_CHANNEL_AUDIO_OUTPUT, nullptr);
	obs_set_output_source(SOURCE_CHANNEL_AUDIO_INPUT, nullptr);

	size_t idx = 0;
	const char* id;

	while (obs_enum_transition_types(idx++, &id)) {
		const char* name = obs_source_get_display_name(id);

		if (!obs_is_source_configurable(id)) {
			obs_source_t* tr = obs_source_create_private(id, name, NULL);

			if (strcmp(id, "fade_transition") == 0)
				fadeTransition = tr;
		}
	}

	if (!fadeTransition) {
		return false;
	}
	
	obs_set_output_source(SOURCE_CHANNEL_TRANSITION, fadeTransition);
	obs_source_release(fadeTransition);

	scene = obs_scene_create("DefaultScene");

	if (!scene) {
		return false;
	}

	obs_source_t* source = obs_get_output_source(SOURCE_CHANNEL_TRANSITION);
	obs_transition_set(source, obs_scene_get_source(scene));
	obs_source_release(source);

	bool hasDesktopAudio = HasAudioDevices(OUTPUT_AUDIO_SOURCE);
	bool hasInputAudio = HasAudioDevices(INPUT_AUDIO_SOURCE);

	if (hasDesktopAudio)
		ResetAudioDevice(OUTPUT_AUDIO_SOURCE, "default", "Desktop Audio", SOURCE_CHANNEL_AUDIO_OUTPUT);
	if (hasInputAudio)
		ResetAudioDevice(INPUT_AUDIO_SOURCE, "default", "Mic/Aux", SOURCE_CHANNEL_AUDIO_INPUT);

	return true;
}

bool UtilsWrapper::StartRec()
{
	if (!obs_output_start(output)) {
		return false;
	};
	return true;
}

bool UtilsWrapper::StopRec()
{
	obs_output_stop(output);
	return true;
}
