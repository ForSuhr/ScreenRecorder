#pragma warning(disable: 4996)
#include "UtilsWrapper.h"
#include <string>
#include <QApplication>
#include "libavcodec/avcodec.h"
#include <QDateTime>


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
	// locale, config
	string locale = "en-US";
	string module_config_path = "../config";

	// start initialization
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

	// 
	//if (!SetupOutputMode()) {
	//	return false;
	//}

	// finish initialization
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

bool UtilsWrapper::SearchSource(REC_TYPE rec_type, REC_OBJ& m_RecObj)
{
	if (rec_type == REC_MONITOR) {
		captureSource = obs_source_create("monitor_capture", "Desktop Capture", NULL, nullptr);

	}
	else if (rec_type == REC_WINDOW) {
		captureSource = obs_source_create("window_capture", "Window Capture", NULL, nullptr);
	}
	else {
		return false;
	}

	if (captureSource)
	{
		obs_scene_atomic_update(scene, AddSource, captureSource);
	}
	else
	{
		return false;
	}

	setting = obs_data_create();
	obs_data_t* sourceSetting = obs_source_get_settings(captureSource);
	obs_data_apply(setting, sourceSetting);
	obs_data_release(sourceSetting);

	properties = obs_source_properties(captureSource);
	property = obs_properties_first(properties);

	while (property)
	{
		const char* name = obs_property_name(property);
		if (strcmp(name, "monitor") ^ strcmp(name, "window")) {
			size_t count = obs_property_list_item_count(property);
			const char* string = nullptr;

			for (size_t i = 0; i < count; i++) {
				if (rec_type == REC_MONITOR) {
					const char* item_name = obs_property_list_item_name(property, i);
					string = item_name;
				}
				else if (rec_type == REC_WINDOW) {
					string = obs_property_list_item_string(property, i);
				}
				else {
					return false;
				}

				m_RecObj.push_back(string);
			}
		}
		else {
			return false;
		}

		obs_property_next(&property);
	}

	return true;
}

bool UtilsWrapper::SetupOutputMode()
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

bool UtilsWrapper::SetSource(REC_TYPE rec_type, const char* rec_obj)
{

	if (rec_obj)
	{
		if (rec_type == REC_MONITOR) {
			obs_data_set_string(setting, "monitor", rec_obj);
		}
		else if (rec_type == REC_WINDOW) {
			obs_data_set_string(setting, "window", rec_obj);
		}
		obs_source_update(captureSource, setting);
	}
	else
	{
		obs_data_release(setting);
		return false;
	}

	obs_data_release(setting);

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
