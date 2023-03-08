#pragma warning(disable: 4996)
#include "UtilsWrapper.h"
#include <string>
#include <QApplication>
#include "libavcodec/avcodec.h"
#include <QDateTime>

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

#define VIDEO_ENCODER_ID           AV_CODEC_ID_H264
#define VIDEO_ENCODER_NAME         "libx264"
#define RECORD_OUTPUT_FORMAT       "mp4"
#define RECORD_OUTPUT_FORMAT_MIME  "video/mp4"
#define AUDIO_BITRATE 128 
#define VIDEO_BITRATE 150
#define OUT_WIDTH  1920
#define OUT_HEIGHT 1080

enum SOURCE_CHANNELS {
	SOURCE_CHANNEL_TRANSITION,
	SOURCE_CHANNEL_AUDIO_OUTPUT,
	SOURCE_CHANNEL_AUDIO_INPUT,
};

static void AddSource(void* _data, obs_scene_t* scene)
{
	obs_source_t* source = (obs_source_t*)_data;
	obs_scene_add(scene, source);
	obs_source_release(source);
}

static bool CreateAACEncoder(OBSEncoder& res, string& id, const char* name, size_t idx)
{
	const char* id_ = "ffmpeg_aac";

	if (id == id_)
		return true;

	id = id_;
	res = obs_audio_encoder_create(id_, name, nullptr, idx, nullptr);

	if (res) {
		obs_encoder_release(res);
		return true;
	}

	return false;
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

static void ResetAudioDevice(const char* sourceId, const char* deviceId, const char* deviceDesc, int channel)
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

	// setup source
	if (!CreateSource()) {
		return false;
	}

	//
	if (!CreateOutputMode()) {
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
	video_info.output_format = VIDEO_FORMAT_I420;
	video_info.adapter = 0;
	video_info.gpu_conversion = true;
	video_info.colorspace = VIDEO_CS_709;
	video_info.range = VIDEO_RANGE_FULL;
	video_info.scale_type = OBS_SCALE_BICUBIC;
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

	scene = obs_scene_create("Default Scene");

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

bool UtilsWrapper::CreateSource()
{
	captureSource = obs_source_create("monitor_capture", "Monitor Capture", NULL, nullptr);

	if (captureSource)
	{
		obs_scene_atomic_update(scene, AddSource, captureSource);
	}
	else
	{
		return false;
	}

	setting = obs_data_create();
	obs_data_t* curSetting = obs_source_get_settings(captureSource);
	obs_data_apply(setting, curSetting);
	obs_data_release(curSetting);

	properties = obs_source_properties(captureSource);
	property = obs_properties_first(properties);

	while (property)
	{
		const char* name = obs_property_name(property);
		if (strcmp(name, "monitor") == 0)
		{
			size_t count = obs_property_list_item_count(property);
			const char* string = nullptr;

			for (size_t i = 0; i < count; i++)
			{
				const char* item_name = obs_property_list_item_name(property, i);
				string = item_name;
				break;
			}

			if (string)
			{
				obs_data_set_string(setting, name, string);
				obs_source_update(captureSource, setting);
				break;
			}
			else
			{
				obs_data_release(setting);
				return false;
			}
		}

		obs_property_next(&property);
	}

	obs_data_release(setting);

	return true;
}

bool UtilsWrapper::CreateOutputMode()
{
	if (!output) {
		output = obs_output_create("ffmpeg_output", "adv_ffmpeg_output", nullptr, nullptr);
	
		if (!output) {
			return false;
		}
	}

	for (int i = 0; i < MAX_AUDIO_MIXES; i++) {
		char name[9];
		sprintf(name, "adv_aac%d", i);

		if (!CreateAACEncoder(aacTrack[i], aacEncoderID[i], name, i)) {
			return false;
		}

		obs_encoder_set_audio(aacTrack[i], obs_get_audio());
	}

	return true;
}

bool UtilsWrapper::SetupFFmpeg()
{
	obs_data_t* settings = obs_data_create();
	QDateTime qdt = QDateTime::currentDateTime();
	string timeStr = qdt.toString("yyyy-MM-ddThh-mm-ss").toStdString();
	string userDir = getenv("USERPROFILE");
	string capturesDir = R"(\Videos\Captures\capture_)";
	string dataFormat = ".mp4";
	string outFileName = userDir + capturesDir + timeStr + dataFormat;

	obs_data_set_string(settings, "url", outFileName.c_str());
	obs_data_set_string(settings, "format_name", RECORD_OUTPUT_FORMAT);
	obs_data_set_string(settings, "format_mime_type", RECORD_OUTPUT_FORMAT_MIME);
	obs_data_set_string(settings, "muxer_settings", "movflags=faststart");
	obs_data_set_int(settings, "gop_size", FPS * 10);
	obs_data_set_string(settings, "video_encoder", VIDEO_ENCODER_NAME);
	obs_data_set_int(settings, "video_encoder_id", VIDEO_ENCODER_ID);

	if (VIDEO_ENCODER_ID == AV_CODEC_ID_H264)
		obs_data_set_string(settings, "video_settings", "profile=main x264-params=crf=22");
	else if (VIDEO_ENCODER_ID == AV_CODEC_ID_FLV1)
		obs_data_set_int(settings, "video_bitrate", VIDEO_BITRATE);

	obs_data_set_int(settings, "audio_bitrate", AUDIO_BITRATE);
	obs_data_set_string(settings, "audio_encoder", "aac");
	obs_data_set_int(settings, "audio_encoder_id", AV_CODEC_ID_AAC);
	obs_data_set_string(settings, "audio_settings", NULL);

	obs_data_set_int(settings, "scale_width", OUT_WIDTH);
	obs_data_set_int(settings, "scale_height", OUT_HEIGHT);

	obs_output_set_mixer(output, 1);
	obs_output_set_media(output, obs_get_video(), obs_get_audio());
	obs_output_update(output, settings);

	obs_data_release(settings);

	return true;
}

bool UtilsWrapper::StartRec()
{
	// setup FFmpeg
	if (!SetupFFmpeg()) {
		return false;
	}

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
