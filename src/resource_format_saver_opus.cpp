#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/resource_saver.hpp"

#include <ogg/ogg.h>
#include <opus/opus.h>

#include "audio_stream_opus.h"
#include "resource_format_saver_opus.h"

using namespace godot;

namespace opus_gdextension {

PackedStringArray ResourceFormatSaverOpus::_get_recognized_extensions(const Ref<Resource> &p_resource) const {
	if (p_resource.is_valid() && p_resource->is_class("AudioStreamOpus")) {
		return { "oggopusstr_gdextension" };
	}
	return {};
}

bool ResourceFormatSaverOpus::_recognize(const Ref<Resource> &p_resource) const {
	return p_resource.is_valid() && p_resource->is_class("AudioStreamOpus");
}

Error ResourceFormatSaverOpus::_save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	// Unfortunately, ResourceFormatSaverBinaryInstance::save is not exposed, so we save manually

	static const uint32_t oggopusstr_gdextension_version = 1;

	Ref<AudioStreamOpus> opus_stream = p_resource;
	if (opus_stream.is_null()) {
		return ERR_INVALID_DATA;
	}

	Ref<FileAccess> save_file_access = FileAccess::open(p_path, FileAccess::WRITE);
	if (save_file_access.is_null()) {
		return ERR_FILE_CANT_OPEN;
	}

	if (!save_file_access->store_32(oggopusstr_gdextension_version)) {
		return ERR_FILE_CANT_WRITE;
	}
	if (!save_file_access->store_8(opus_stream->has_loop() ? 1 : 0)) {
		return ERR_FILE_CANT_WRITE;
	}
	if (!save_file_access->store_double(opus_stream->get_loop_offset())) {
		return ERR_FILE_CANT_WRITE;
	}
	if (!save_file_access->store_double(opus_stream->_get_bpm())) {
		return ERR_FILE_CANT_WRITE;
	}
	if (!save_file_access->store_64((uint64_t)opus_stream->_get_beat_count())) {
		return ERR_FILE_CANT_WRITE;
	}
	if (!save_file_access->store_64((uint64_t)opus_stream->get_bar_beats())) {
		return ERR_FILE_CANT_WRITE;
	}
	PackedByteArray opus_stream_data = opus_stream->get_data();
	if (!save_file_access->store_64((uint64_t)opus_stream_data.size())) {
		return ERR_FILE_CANT_WRITE;
	}
	if (!save_file_access->store_buffer(opus_stream_data)) {
		return ERR_FILE_CANT_WRITE;
	}
	save_file_access->close();
	return OK;
}

}
