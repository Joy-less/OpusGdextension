// This file is adapted from https://github.com/godotengine/godot/blob/master/editor/audio/audio_stream_editor_plugin.cpp

#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/rendering_server.hpp"
#include "godot_cpp/classes/v_box_container.hpp"
#include "godot_cpp/classes/h_box_container.hpp"
#include "godot_cpp/classes/font.hpp"
#include "godot_cpp/classes/theme.hpp"
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/editor_settings.hpp"

#include "editor_inspector_plugin_audio_stream_opus.h"
#include "audio_stream_opus.h"

using namespace godot;

namespace opus_gdextension {

Ref<Texture2D> EditorInspectorPluginAudioStreamOpusEditor::_editor_get_theme_icon(const StringName &p_icon_name) {
	EditorInterface *editor_interface = EditorInterface::get_singleton();
	if (!editor_interface) {
		return Ref<Texture2D>();
	}
	Ref<Theme> editor_theme = editor_interface->get_editor_theme();
	if (editor_theme.is_null()) {
		return Ref<Texture2D>();
	}
	return editor_theme->get_icon(p_icon_name, SNAME_EditorIcons);
}

float EditorInspectorPluginAudioStreamOpusEditor::_editor_get_scale() {
	EditorInterface *editor_interface = EditorInterface::get_singleton();
	if (!editor_interface) {
		return 1.0;
	}
	return editor_interface->get_editor_scale();
}

void EditorInspectorPluginAudioStreamOpusEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			Ref<Font> font = get_theme_font(SNAME_status_source, SNAME_EditorFonts);

			_current_label->add_theme_font_override(SNAME_font, font);
			_duration_label->add_theme_font_override(SNAME_font, font);

			_play_button->set_button_icon(_editor_get_theme_icon(SNAME_MainPlay));
			_stop_button->set_button_icon(_editor_get_theme_icon(SNAME_Stop));

			set_color(get_theme_color(SNAME_dark_color_1, SNAME_Editor));
		} break;
		case NOTIFICATION_PROCESS: {
			_current = _player->get_playback_position();
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (!is_visible_in_tree()) {
				_stop();
			}
		} break;
		default: {
		} break;
	}
}

void EditorInspectorPluginAudioStreamOpusEditor::_stream_changed() {
	if (!is_visible()) {
		return;
	}
	queue_redraw();
}

void EditorInspectorPluginAudioStreamOpusEditor::_play() {
	if (_player->is_playing()) {
		_pausing = true;
		_player->stop();
		_play_button->set_button_icon(_editor_get_theme_icon(SNAME_MainPlay));
		set_process(false);
	} else {
		_pausing = false;
		_player->play(_current);
		_play_button->set_button_icon(_editor_get_theme_icon(SNAME_Pause));
		set_process(true);
	}
}

void EditorInspectorPluginAudioStreamOpusEditor::_stop() {
	_player->stop();
	_play_button->set_button_icon(_editor_get_theme_icon(SNAME_MainPlay));
	_current = 0;
	set_process(false);
}

void EditorInspectorPluginAudioStreamOpusEditor::_on_finished() {
	_play_button->set_button_icon(_editor_get_theme_icon(SNAME_MainPlay));
	if (!_pausing) {
		_current = 0;
	} else {
		_pausing = false;
	}
	set_process(false);
}

void EditorInspectorPluginAudioStreamOpusEditor::set_stream(const Ref<AudioStream> &p_stream) {
	if (stream.is_valid()) {
		stream->disconnect(SNAME_changed, callable_mp(this, &EditorInspectorPluginAudioStreamOpusEditor::_stream_changed));
	}

	stream = p_stream;
	if (stream.is_null()) {
		hide();
		return;
	}
	stream->connect(SNAME_changed, callable_mp(this, &EditorInspectorPluginAudioStreamOpusEditor::_stream_changed));

	_player->set_stream(stream);
	_current = 0;

	const double length = stream->get_length();

	_duration_label->set_text(String::num(length, 2).pad_decimals(2) + "s");

	queue_redraw();
}

EditorInspectorPluginAudioStreamOpusEditor::EditorInspectorPluginAudioStreamOpusEditor() {
	set_custom_minimum_size(Size2(1, 30) * _editor_get_scale());

	_player = memnew(AudioStreamPlayer);
	_player->connect(SNAME_finished, callable_mp(this, &EditorInspectorPluginAudioStreamOpusEditor::_on_finished));
	add_child(_player);

	VBoxContainer *vbox = memnew(VBoxContainer);
	vbox->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(vbox);

	HBoxContainer *hbox = memnew(HBoxContainer);
	hbox->add_theme_constant_override("separation", 0);
	vbox->add_child(hbox);

	_play_button = memnew(Button);
	hbox->add_child(_play_button);
	_play_button->set_flat(true);
	_play_button->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	_play_button->connect(SNAME_pressed, callable_mp(this, &EditorInspectorPluginAudioStreamOpusEditor::_play));
	_play_button->set_accessibility_name("Play");

	_stop_button = memnew(Button);
	hbox->add_child(_stop_button);
	_stop_button->set_flat(true);
	_stop_button->set_focus_mode(Control::FOCUS_ACCESSIBILITY);
	_stop_button->connect(SNAME_pressed, callable_mp(this, &EditorInspectorPluginAudioStreamOpusEditor::_stop));
	_stop_button->set_accessibility_name("Stop");

	_current_label = memnew(Label);
	_current_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	_current_label->set_h_size_flags(SIZE_EXPAND_FILL);
	_current_label->set_modulate(Color(1, 1, 1, 0.5));
	hbox->add_child(_current_label);

	_duration_label = memnew(Label);
	hbox->add_child(_duration_label);
}

void EditorInspectorPluginAudioStreamOpus::_parse_begin(Object *p_object) {
	AudioStream *stream = Object::cast_to<AudioStream>(p_object);

	EditorInspectorPluginAudioStreamOpusEditor *editor = memnew(EditorInspectorPluginAudioStreamOpusEditor);
	editor->set_stream(Ref<AudioStream>(stream));
	add_custom_control(editor);
}

bool EditorInspectorPluginAudioStreamOpus::_can_handle(Object *p_object) const {
	return Object::cast_to<AudioStreamOpus>(p_object) != nullptr;
}

}
