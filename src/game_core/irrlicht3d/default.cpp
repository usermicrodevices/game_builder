#define USE_TTFONT

enum IMAGE_TYPE{
IMAGE_DEFAULT,
IMAGE_HERO = 1,
IMAGE_WALL = 2,
IMAGE_DOOR = 3,
IMAGE_KEY = 4,
IMAGE_WEAPON = 5,
IMAGE_AMMO = 6,
IMAGE_HEALTH = 7,
IMAGE_ENEMY = 8,
IMAGE_FRIEND = 9,
IMAGE_OBJECT = 10,
IMAGE_TYPE_COUNT = 100
};

#define CAMERA_MAYA -2
#define CAMERA_FPS -3

bool global_exit_from_app = false;
const char* common_codec = "cp1251";

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <map>
using namespace std;

#include <bass.h>

string get_bass_error_description(int error_code)
{
	switch(error_code)
	{
		case BASS_OK:{return string("all is OK");}
		case BASS_ERROR_MEM:{return string("memory error");}
		case BASS_ERROR_FILEOPEN:{return string("can't open the file");}
		case BASS_ERROR_DRIVER:{return string("can't find a free/valid driver");}
		case BASS_ERROR_BUFLOST:{return string("the sample buffer was lost");}
		case BASS_ERROR_HANDLE:{return string("invalid handle");}
		case BASS_ERROR_FORMAT:{return string("unsupported sample format");}
		case BASS_ERROR_POSITION:{return string("invalid position");}
		case BASS_ERROR_INIT:{return string("BASS_Init has not been successfully called");}
		case BASS_ERROR_START:{return string("BASS_Start has not been successfully called");}
		case BASS_ERROR_ALREADY:{return string("already initialized/paused/whatever");}
		case BASS_ERROR_NOCHAN:{return string("can't get a free channel");}
		case BASS_ERROR_ILLTYPE:{return string("an illegal type was specified");}
		case BASS_ERROR_ILLPARAM:{return string("an illegal parameter was specified");}
		case BASS_ERROR_NO3D:{return string("no 3D support");}
		case BASS_ERROR_NOEAX:{return string("no EAX support");}
		case BASS_ERROR_DEVICE:{return string("illegal device number");}
		case BASS_ERROR_NOPLAY:{return string("not playing");}
		case BASS_ERROR_FREQ:{return string("illegal sample rate");}
		case BASS_ERROR_NOTFILE:{return string("the stream is not a file stream");}
		case BASS_ERROR_NOHW:{return string("no hardware voices available");}
		case BASS_ERROR_EMPTY:{return string("the MOD music has no sequence data");}
		case BASS_ERROR_NONET:{return string("no internet connection could be opened");}
		case BASS_ERROR_CREATE:{return string("couldn't create the file");}
		case BASS_ERROR_NOFX:{return string("effects are not available");}
		case BASS_ERROR_NOTAVAIL:{return string("requested data is not available");}
		case BASS_ERROR_DECODE:{return string("the channel is a 'decoding channel'");}
		case BASS_ERROR_DX:{return string("a sufficient DirectX version is not installed");}
		case BASS_ERROR_TIMEOUT:{return string("connection timedout");}
		case BASS_ERROR_FILEFORM:{return string("unsupported file format");}
		case BASS_ERROR_SPEAKER:{return string("unavailable speaker");}
		case BASS_ERROR_VERSION:{return string("invalid BASS version (used by add-ons)");}
		case BASS_ERROR_CODEC:{return string("codec is not available/supported");}
		case BASS_ERROR_ENDED:{return string("the channel/file has ended");}
		case BASS_ERROR_BUSY:{return string("the device is busy");}
		case BASS_ERROR_UNKNOWN:{return string("some other mystery problem");}
		default: break;
	}
	char buf[10];
	_itoa(error_code, buf, 10);
	return string(buf);
}

#include <irrlicht.h>
#ifdef WIN32
#include <windows.h>
#endif
using namespace irr;

#ifdef USE_TTFONT
#include <IGUITTFont.h>
#endif

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "irrlicht.lib")
#pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#endif

#include "map_reader.hpp"

u32 default_color_range[] = {0, 255};
core::stringc music_formats("it, mo3, mod, s3m, xm");

enum GUI_ID{
GUI_ID_QUIT = 0x10000,
GUI_ID_ABOUT = 0x10001,
GUI_ID_LOAD = 0x10002,
GUI_ID_SAVE = 0x10003,
GUI_ID_OPTIONS = 0x10004,
GUI_ID_CAMERA_NONE = 0x10005,
GUI_ID_CAMERA_MAYA = 0x10006,
GUI_ID_CAMERA_FPS = 0x10007,
GUI_ID_LOG = 0x10008,

GUI_ID_WINDOW_OPTIONS = 0x10020,

GUI_ID_SOUND_ON_OFF = 0x10021,
GUI_ID_TEXTURE_FROM_FILE = 0x10022,
GUI_ID_MENU_SOUND_VOLUME = 0x10023,
GUI_ID_WINDOW_SOUND_VOLUME = 0x10024,
GUI_ID_SOUND_VOLUME = 0x10025,

GUI_ID_OPT_BACK_SOUND = 0x10100,
GUI_ID_OPT_FULL_SCREEN = 0x10101,
GUI_ID_OPT_HERO_SCREEN = 0x10102,
GUI_ID_OPT_SAVE = 0x10103,

GUI_ID_DRIVER = 0x10200,
GUI_ID_EDT_SOFTWARE = GUI_ID_DRIVER | video::EDT_SOFTWARE,
GUI_ID_EDT_BURNINGSVIDEO = GUI_ID_DRIVER | video::EDT_BURNINGSVIDEO,
GUI_ID_EDT_DIRECT3D8 = GUI_ID_DRIVER | video::EDT_DIRECT3D8,
GUI_ID_EDT_DIRECT3D9 = GUI_ID_DRIVER | video::EDT_DIRECT3D9,
GUI_ID_EDT_OPENGL = GUI_ID_DRIVER | video::EDT_OPENGL
};

struct application_properties{std::string config_file_name; core::stringw description; std::string music_file; E_DEVICE_TYPE device_type; video::E_DRIVER_TYPE driver_type; u8 bits, zbuffer_bits; void* window_id; ELOG_LEVEL logging_level; u32 display_adapter; bool full_screen, stencil_buffer, vsync, handle_srgb, with_alpha_channel, double_buffer, ignore_input, stereo_buffer, high_precision_fpu, driver_multithreaded, use_performance_timer;};
application_properties app_properties = {std::string("config.ini"), core::stringw(L"Demo from Game Builder"), std::string(""), EIDT_BEST, video::EDT_OPENGL, 16, 16, 0, ELL_DEBUG, 0, false, false, false, false, false, true, false, false, false, false, true};

struct event_sound_struct{u32 index/*index from sounds array*/; core::stringc event; event_sound_struct(u32 i, core::stringc& e){index = i; event = e;};};
struct event_message_struct{core::stringc event; core::stringw message; event_message_struct(core::stringc& e, core::stringw& m){event = e; message = m;};};
struct scene_item
{
	u32 index/*index from images array*/;
	IMAGE_TYPE type;
	core::stringc name, key;
	int goto_level_index;
	f32 scale;
	int x, y, z, power, delay_animation_idle;
	core::array<u32> animation_idle;
	int delay_animation_destroy;
	core::array<u32> animation_destroy;
	int delay_animation_move;
	core::array<u32> animation_move;
	int delay_animation_speak;
	core::array<u32> animation_speak;
	int delay_animation_shot;
	core::array<u32> animation_shot;
	core::array<event_sound_struct> event_sounds;
	core::array<event_message_struct> messages;
	scene_item(u32 v0, IMAGE_TYPE v1, core::stringc v2, core::stringc v3, int v4, f32 v5, int v6, int v7, int v8, int v9, int v10, core::array<u32> v11, int v12, core::array<u32> v13, int v14, core::array<u32> v15, int v16, core::array<u32> v17, int v18, core::array<u32> v19, core::array<event_sound_struct> v20, core::array<event_message_struct> v21):
	index(v0), type(v1), name(v2), key(v3), goto_level_index(v4), scale(v5), x(v6), y(v7), z(v8), power(v9), delay_animation_idle(v10), animation_idle(v11), delay_animation_destroy(v12), animation_destroy(v13), delay_animation_move(v14), animation_move(v15), delay_animation_speak(v16), animation_speak(v17), delay_animation_shot(v18), animation_shot(v19), event_sounds(v20), messages(v21)
	{};
	scene_item(u32 idx, int _x, int _y, int _z, IMAGE_TYPE _type = IMAGE_DEFAULT, core::stringc _name = "image", core::stringc _key = "", int goto_level = -1, f32 _scale = 0.0f, int _power = 0, int delay_anim_idle = 0, core::array<u32> anim_idle = 0, int delay_anim_destroy = 0, core::array<u32> anim_destroy = 0, int delay_anim_move = 0, core::array<u32> anim_move = 0, int delay_anim_speak = 0, core::array<u32> anim_speak = 0, int delay_anim_shot = 0, core::array<u32> anim_shot = 0, core::array<event_sound_struct>& sounds = core::array<event_sound_struct>(), core::array<event_message_struct>& msgs = core::array<event_message_struct>()):
	index(idx), x(_x), y(_y), z(_z), type(_type), name(_name), key(_key), goto_level_index(goto_level), scale(_scale), power(_power), delay_animation_idle(delay_anim_idle), animation_idle(anim_idle), delay_animation_destroy(delay_anim_destroy), animation_destroy(anim_destroy), delay_animation_move(delay_anim_move), animation_move(anim_move), delay_animation_speak(delay_anim_speak), animation_speak(anim_speak), delay_animation_shot(delay_anim_shot), animation_shot(anim_shot), event_sounds(sounds), messages(msgs)
	{};
	scene_item(u32 idx, int _x, int _y, int _z, IMAGE_TYPE _type = IMAGE_DEFAULT, core::array<event_sound_struct>& sounds = core::array<event_sound_struct>(), core::stringc _name = "image", core::stringc _key = "", int goto_level = -1, f32 _scale = 0.0f, int _power = 0, int delay_anim_idle = 0, core::array<u32> anim_idle = 0, int delay_anim_destroy = 0, core::array<u32> anim_destroy = 0, int delay_anim_move = 0, core::array<u32> anim_move = 0, int delay_anim_speak = 0, core::array<u32> anim_speak = 0, int delay_anim_shot = 0, core::array<u32> anim_shot = 0, core::array<event_message_struct>& msgs = core::array<event_message_struct>()):
	index(idx), x(_x), y(_y), z(_z), type(_type), name(_name), key(_key), goto_level_index(goto_level), scale(_scale), power(_power), delay_animation_idle(delay_anim_idle), animation_idle(anim_idle), delay_animation_destroy(delay_anim_destroy), animation_destroy(anim_destroy), delay_animation_move(delay_anim_move), animation_move(anim_move), delay_animation_speak(delay_anim_speak), animation_speak(anim_speak), delay_animation_shot(delay_anim_shot), animation_shot(anim_shot), event_sounds(sounds), messages(msgs)
	{};
	scene_item(u32 idx, int _x, int _z):
	index(idx), type(IMAGE_DEFAULT), name("image"), key(""), goto_level_index(-1), scale(0.0f), x(_x), y(0), z(_z), power(0), delay_animation_idle(0), animation_idle(0), delay_animation_destroy(0), animation_destroy(0), delay_animation_move(0), animation_move(0), delay_animation_speak(0), animation_speak(0), delay_animation_shot(0), animation_shot(0), event_sounds(0), messages(0)
	{};
};

struct level_struct
{
	u32 x, y, z;
	bool use_floor_plane, use_ceiling_plane, use_border_planes;
	list<scene_item> floors, ceilings, objects, lights;
	int index_music;
	level_struct(u32 _x, u32 _y, u32 _z, bool _use_floor_plane, bool _use_ceiling_plane, bool _use_border_planes, list<scene_item> _floors, list<scene_item> _ceilings, list<scene_item> _objects, list<scene_item> _lights, int _index_music = -1):
	x(_x), y(_y), z(_z), use_floor_plane(_use_floor_plane), use_ceiling_plane(_use_ceiling_plane), use_border_planes(_use_border_planes), floors(_floors), ceilings(_ceilings), objects(_objects), lights(_lights), index_music(_index_music)
	{};
};

size_t char_to_wchar(const char* src_buf, wchar_t* dst_buf, size_t src_size = 0)
{
	size_t result = 0;
	if(!src_size)
		src_size = strlen(src_buf) + 1;
	if (src_size > 1)
	{
#ifdef _MSC_VER
		size_t NumOfCharConverted;
		result = mbstowcs_s(&NumOfCharConverted, dst_buf, src_size, src_buf, _TRUNCATE);
#else
		result = mbstowcs(dst_buf, src_buf, MB_CUR_MAX);
#endif
	}
	return result;
}

core::stringc vector3df_to_string(core::vector3d<f32> value)
{
	char* buf = (char*)malloc(sizeof(char) * 255);
	sprintf(buf, "vector3d<f32>(%f, %f, %f)", value.X, value.Y, value.Z);
	core::stringc result(buf);
	free(buf);
	return result;
}

class config_file
{
	bool insert_not_exists, changed;
	string path_file;
	std::map<string, string>* values;

public:
	config_file(string file_path = string("config.cfg"), ios_base::openmode file_mode = ios_base::in | ios_base::out, bool insert_if_not_exists = true)
	{
		changed = false;
		insert_not_exists = insert_if_not_exists;
		path_file = file_path;
		values = new std::map<string, string>();
		fstream file_stream(path_file.c_str(), file_mode);
		if(!file_stream.is_open())
			changed = true;
		else
		{
			string line;
			while(getline(file_stream, line))
			{
				if(line.substr(0,1) == "#" || line.empty())
					continue;
				s32 isym = line.find_first_of('=');
				if(isym > 0)
					(*values)[line.substr(0, isym)] = line.substr(isym + 1);
			}
			file_stream.close();
		}
	}
	void set_insert_not_exists(bool value = true) { insert_not_exists = value; }
	bool get_insert_not_exists() { return insert_not_exists; }
	string get(string key, string default_value = "")
	{
		std::map<string, string>::iterator value = values->find(key);
		if(value == values->end())
		{
			if(insert_not_exists)
				set(key, default_value);
			return default_value;
		}
		else
			return value->second;
	}
	bool get_bool(string key, bool default_value = false)
	{
		std::map<string, string>::iterator value = values->find(key);
		if(value == values->end())
		{
			if(insert_not_exists)
				set_bool(key, default_value);
			return default_value;
		}
		else
			return (value->second[0] != '0');
	}
	int get_int(string key, int default_value = 0)
	{
		std::map<string, string>::iterator value = values->find(key);
		if(value == values->end())
		{
			if(insert_not_exists)
				set_int(key, default_value);
			return default_value;
		}
		else
			return atoi(value->second.c_str());
	}
	float get_float(string key, float default_value = 0.0f)
	{
		std::map<string, string>::iterator value = values->find(key);
		if(value == values->end())
		{
			if(insert_not_exists)
				set_float(key, default_value);
			return default_value;
		}
		else
			return (float)atof(value->second.c_str());
	}
	f32 get_f32(string key, f32 default_value = 0.0f)
	{
		std::map<string, string>::iterator value = values->find(key);
		if(value == values->end())
		{
			if(insert_not_exists)
				set_f32(key, default_value);
			return default_value;
		}
		else
			return (f32)atof(value->second.c_str());
	}
	double get_double(string key, double default_value = 0.0)
	{
		std::map<string, string>::iterator value = values->find(key);
		if(value == values->end())
		{
			if(insert_not_exists)
				set_double(key, default_value);
			return default_value;
		}
		else
			return atof(value->second.c_str());
	}
	void set(string key, string value)
	{
		(*values)[key] = value;
		changed = true;
	}
	void set_bool(string key, bool value)
	{
		(*values)[key] = string(1, (value ? '1' : '0'));
		changed = true;
	}
	void set_int(string key, int value)
	{
		char buf[10];
		_itoa(value, buf, 10);
		(*values)[key] = string(buf);
		changed = true;
	}
	void set_float(string key, float value)
	{
		std::ostringstream buf;
		buf << value;
		(*values)[key] = buf.str();
		changed = true;
	}
	void set_f32(string key, f32 value)
	{
		std::ostringstream buf;
		buf << value;
		(*values)[key] = buf.str();
		changed = true;
	}
	void set_double(string key, double value)
	{
		std::ostringstream buf;
		buf << value;
		(*values)[key] = buf.str();
		changed = true;
	}
	void save()
	{
		if(values->empty())
			cout << endl << "INFO config_file: internal values is empty" << endl;
		else if(changed)
		{
			fstream file_stream(path_file.c_str(), ios_base::out);
			if(!file_stream.is_open())
				cout << endl << "ERROR config_file: for save not open file with name " << path_file << endl;
			else
			{
				std::ostringstream content;
				for(std::map<string, string>::iterator it = values->begin(); it!=values->end(); ++it)
				{
					content << it->first << "=" << it->second << endl;
				}
				file_stream.write(content.str().c_str(), content.tellp());
				file_stream.close();
			}
		}
	}
	~config_file()
	{
		save();
		delete values;
	}
};

class object_door
{
	bool dropped;
	scene::IMeshSceneNode* node;
	core::stringc key;
	core::array<event_sound_struct> sounds;
	s32 level_goto;
	f32 speed_open;

public:
	object_door(scene::IMeshSceneNode* value_node, const core::stringc& value_key, core::array<event_sound_struct>& value_sounds = core::array<event_sound_struct>(), s32 value_level_goto = -1, f32 value_speed_open = 0.2f, bool value_dropped = false):
	node(value_node), key(value_key), sounds(value_sounds), level_goto(value_level_goto), speed_open(value_speed_open), dropped(value_dropped)
	{}
	~object_door(){remove_node();}
	void remove_node()
	{
		if(node)
		{
			node->removeAnimators();
			node->remove();
			if(dropped)
				node->drop();
		}
	}
	scene::IMeshSceneNode* get_node(){return node;}
	void set_node(scene::IMeshSceneNode* n){remove_node(); node = n;}
	core::stringc& get_key(){return key;}
	void set_key(core::stringc& k){key = k;}
	core::array<event_sound_struct>& get_sounds(){return sounds;}
	void set_sounds(core::array<event_sound_struct>& es){sounds = es;}
	int get_sound(const core::stringc& key)
	{
		for(u32 i = 0; i < sounds.size(); ++i)
		{
			if(sounds[i].event == key)
				return sounds[i].index;
		}
		return -1;
	}
	s32 get_level_goto(){return level_goto;}
	void set_level_goto(s32 l){level_goto = l;}
	f32 get_speed_open(){return speed_open;}
	void set_speed_open(f32 s){speed_open = s;}
};

class object_friend
{
	bool dropped;
	scene::IBillboardSceneNode* node;
	core::array<event_sound_struct> sounds;
	core::array<event_message_struct> messages;

public:
	object_friend(scene::IBillboardSceneNode* value_node, core::array<event_message_struct>& value_messages = core::array<event_message_struct>(), core::array<event_sound_struct>& value_sounds = core::array<event_sound_struct>(), bool value_dropped = false):
	node(value_node), messages(value_messages), sounds(value_sounds), dropped(value_dropped)
	{}
	~object_friend(){remove_node();}
	void remove_node()
	{
		if(node)
		{
			node->removeAnimators();
			node->remove();
			if(dropped)
				node->drop();
		}
	}
	scene::IBillboardSceneNode* get_node(){return node;}
	void set_node(scene::IBillboardSceneNode* n){remove_node(); node = n;}
	core::array<event_message_struct>& get_messages(){return messages;}
	void set_messages(core::array<event_message_struct>& m){messages = m;}
	const wchar_t* get_message(const core::stringc& key)
	{
		for(u32 i = 0; i < messages.size(); ++i)
		{
			if(messages[i].event == key)
			{
				return messages[i].message.c_str();
			}
		}
		return 0;
	}
	core::array<event_sound_struct>& get_sounds(){return sounds;}
	void set_sounds(core::array<event_sound_struct>& s){sounds = s;}
	int get_sound(const core::stringc& key)
	{
		for(u32 i = 0; i < sounds.size(); ++i)
		{
			if(sounds[i].event == key)
				return sounds[i].index;
		}
		return -1;
	}
};

bool run_level = false;
scene::IMeshSceneNode* hero_node;
int debug_level, current_level;

class Game
{
	core::array<io::path> sounds, images;
	core::array<level_struct> levels;

	HSTREAM bass_handle_game, bass_handle_level;
	BOOL game_sound_playing, level_sound_playing, event_sound_playing;
	float sound_volume;
	std::string music_file;

	config_file* config;

	bool play_sound, hero_screen_show, shot_enable, fog_enable, fog_pixel, fog_range;
	int time_delay, health, weapon_current, weapon_current_texture, hero_screen_height, hero_screen_line_width;
	u32 menu_sound, menu_sound_volume, font_size, menu_height;
	f32 fog_start, fog_end, fog_density;
	io::path logo_file;
	std::string json_map_file;  // New: JSON map file path

	video::E_FOG_TYPE fog_type;
	video::E_DRIVER_TYPE driver_type;

	core::dimension2d<u32> logo_size;
	core::rect<s32> logo_rect, hero_source_rect, warning_position;
	video::SColor fog_color, hero_screen_color, warning_color;

	video::ITexture *logo, *hero_texture;
	scene::ICameraSceneNode *camera_maya, *camera_fps;
	const scene::IGeometryCreator* i_geometry_creator;
	scene::IMetaTriangleSelector* meta;
	scene::ISceneCollisionManager* collision_manager;

	gui::IGUIContextMenu *menu, *menu_options, *menu_device_type;
	gui::IGUIWindow *main_window, *dialog_help, *dialog_options;
#ifdef USE_TTFONT
	gui::CGUITTFont* font;
#else
	gui::IGUIFont* font;
#endif
	gui::ICursorControl* cursor_control;

	irr::SIrrlichtCreationParameters device_parameters;
	irr::IrrlichtDevice* device;
	video::IVideoDriver* video_driver;

	core::array<scene::IMeshSceneNode*> floors, ceilings, walls, bullets, enemy_bullets, outside_nodes;
	core::array<scene::IBillboardSceneNode*> objects, enemies, keys, weapons;
	core::array<video::ITexture*> hero_weapons, hero_keys;
	core::array<object_door*> doors;
	core::array<object_friend*> friends;

public:

	class UserIEventReceiver: public IEventReceiver
	{
		Game& game;
	public:
		bool KeyIsDown[KEY_KEY_CODES_COUNT];
		UserIEventReceiver(Game& g) : game(g)
		{
			for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
				KeyIsDown[i] = false;
		}
		bool IsKeyDown(EKEY_CODE keyCode)
		{
			return KeyIsDown[keyCode];
		}
		virtual bool OnEvent(const SEvent& event)
		{
			switch(event.EventType)
			{
				case EET_KEY_INPUT_EVENT:
					if(event.KeyInput.Key == KEY_ESCAPE && !event.KeyInput.PressedDown)
					{
						if(game.dialog_options)
						{
							game.close_options_dialog();
						}
						else if(game.get_run_level())
						{
							game.set_run_level(false);
							game.shot_enable = false;
							game.cursor_control->setVisible(true);
							game.clear_level();
							game.main_window_show();
						}
						else
						{
							global_exit_from_app = true;
						}
					}
					else if(event.KeyInput.Key > KEY_HELP && event.KeyInput.Key < KEY_KEY_A)
						game.weapon_current = event.KeyInput.Key - KEY_KEY_0;
					break;
				case EET_MOUSE_INPUT_EVENT:
					switch(event.MouseInput.Event)
					{
						case EMIE_LMOUSE_PRESSED_DOWN:
							game.weapon_current_texture += 1;
							break;
						case EMIE_LMOUSE_LEFT_UP:
							if (game.shot_enable && !game.dialog_help && !game.dialog_options)
							{
								game.weapon_current_texture = 0;
								game.create_bullet();
							}
							break;
					}
					break;
				case EET_GUI_EVENT:
				{
					gui::IGUIElement* caller = event.GUIEvent.Caller;
					s32 caller_id = event.GUIEvent.Caller->getID();
					switch(event.GUIEvent.EventType)
					{
						case gui::EGET_BUTTON_CLICKED:
							switch(caller_id)
							{
								case GUI_ID_LOAD:
									game.load_level();
									break;
								case GUI_ID_ABOUT:
									game.show_about_dialog();
									break;
								case GUI_ID_OPTIONS:
									game.show_options_dialog();
									break;
								case GUI_ID_QUIT:
									KeyIsDown[KEY_ESCAPE] = true;
									global_exit_from_app = true;
									break;
								case GUI_ID_OPT_SAVE:
									game.close_options_dialog();
									break;
								default:
									cout << endl << "... EGET_BUTTON_CLICKED " << caller_id << " ..." << endl;
									break;
							}
							break;
						case gui::EGET_ELEMENT_CLOSED:
							if(game.dialog_options)
								game.close_options_dialog();
							break;
						case gui::EGET_CHECKBOX_CHANGED:
						{
							bool is_checked = ((gui::IGUICheckBox*)caller)->isChecked();
							switch(caller_id)
							{
								case GUI_ID_OPT_BACK_SOUND:
									if(game.get_run_level())
										game.sound_on_off();
									else
										game.play_sound = is_checked;
									break;
								case GUI_ID_OPT_FULL_SCREEN:
									game.device_parameters.Fullscreen = is_checked;
									break;
								case GUI_ID_OPT_HERO_SCREEN:
									game.hero_screen_show = is_checked;
									break;
							}
							break;
						}
						case gui::EGET_MESSAGEBOX_OK:
							game.dialog_help = 0;
							if(game.get_run_level())
							{
								game.cursor_control->setVisible(false);
								game.setActiveCamera(game.camera_fps);
							}
							break;
						case gui::EGET_SCROLL_BAR_CHANGED:
						{
							gui::IGUIScrollBar* scroll_bar = (gui::IGUIScrollBar*)caller;
							if(caller_id == GUI_ID_SOUND_VOLUME)
								game.set_sound_volume(scroll_bar->getPos());
							break;
						}
						case gui::EGET_MENU_ITEM_SELECTED:
						{
							game.dialog_help = 0;
							gui::IGUIContextMenu* menu = (gui::IGUIContextMenu*)caller;
							s32 menu_id = menu->getItemCommandId(menu->getSelectedItem());
							switch(menu_id)
							{
								case GUI_ID_QUIT:
									KeyIsDown[KEY_ESCAPE] = true;
									break;
								case GUI_ID_CAMERA_NONE:
									game.setActiveCameraOff();
									game.cursor_control->setVisible(true);
									break;
								case GUI_ID_CAMERA_MAYA:
									game.setActiveCamera(game.camera_maya);
									game.cursor_control->setVisible(true);
									break;
								case GUI_ID_CAMERA_FPS:
									game.setActiveCamera(game.camera_fps);
									game.cursor_control->setVisible(false);
									break;
								case GUI_ID_EDT_SOFTWARE:
									game.set_device_type(video::EDT_SOFTWARE);
									break;
								case GUI_ID_EDT_OPENGL:
									game.set_device_type(video::EDT_OPENGL);
									break;
								case GUI_ID_EDT_DIRECT3D9:
									game.set_device_type(video::EDT_DIRECT3D9);
									break;
								case GUI_ID_EDT_BURNINGSVIDEO:
									game.set_device_type(video::EDT_BURNINGSVIDEO);
									break;
								case GUI_ID_SOUND_ON_OFF:
									game.sound_on_off();
									break;
								case GUI_ID_MENU_SOUND_VOLUME:
									game.create_sound_volume();
									break;
								case GUI_ID_LOAD:
									game.load_level();
									break;
								case GUI_ID_ABOUT:
									game.show_about_dialog();
									break;
							}
							break;
						}
						default:
							break;
					}
					break;
				}
			}
			return false;
		}
	};

	Game(const std::string& _json_map_file = "project.json", int debug_level = 0)
		: json_map_file(_json_map_file)
	{
		config = new config_file(app_properties.config_file_name);
		font_size = config->get_int("font_size", 24);
		font = 0;
		time_delay = config->get_int("time_delay", 2);
		play_sound = config->get_bool("play_sound", true);
		sound_volume = config->get_f32("sound_volume", 0.3f);
		music_file = config->get("music_file", app_properties.music_file);
		bass_handle_game = 0;
		bass_handle_level = 0;
		game_sound_playing = false;
		level_sound_playing = false;
		device_parameters.DeviceType = (E_DEVICE_TYPE)config->get_int("device_type", app_properties.device_type);
		device_parameters.DriverType = (video::E_DRIVER_TYPE)config->get_int("driver_type", app_properties.driver_type);
		device_parameters.Fullscreen = config->get_bool("full_screen", app_properties.full_screen);
		if(!device_parameters.Fullscreen)
			device_parameters.WindowSize = core::dimension2d<u32>(config->get_int("window_width", 640), config->get_int("window_height", 480));
		device_parameters.Stencilbuffer = config->get_bool("stencil_buffer", app_properties.stencil_buffer);
		device_parameters.Vsync = config->get_bool("vsync", app_properties.vsync);
		device_parameters.Bits = config->get_int("bits", app_properties.bits);
		device_parameters.ZBufferBits = config->get_int("zbuffer_bits", app_properties.zbuffer_bits);
		device_parameters.AntiAlias = video::EAAM_LINE_SMOOTH;
		device_parameters.HandleSRGB = config->get_bool("handle_srgb", app_properties.handle_srgb);
		device_parameters.WithAlphaChannel = config->get_bool("with_alpha_channel", app_properties.with_alpha_channel);
		device_parameters.Doublebuffer = config->get_bool("double_buffer", app_properties.double_buffer);
		device_parameters.IgnoreInput = config->get_bool("ignore_input", app_properties.ignore_input);
		device_parameters.Stereobuffer = config->get_bool("stereo_buffer", app_properties.stereo_buffer);
		device_parameters.HighPrecisionFPU = config->get_bool("high_precision_fpu", app_properties.high_precision_fpu);
		device_parameters.WindowId = (void*)config->get_int("window_id", (int)app_properties.window_id);
		device_parameters.LoggingLevel = (ELOG_LEVEL)config->get_int("logging_level", app_properties.logging_level);
		device_parameters.DisplayAdapter = (u32)config->get_int("display_adapter", app_properties.display_adapter);
		device_parameters.DriverMultithreaded = config->get_bool("driver_multithreaded", app_properties.driver_multithreaded);
		device_parameters.UsePerformanceTimer = config->get_bool("use_performance_timer", app_properties.use_performance_timer);
		device = createDeviceEx(device_parameters);
		dialog_help = 0;
		dialog_options = 0;
		fog_enable = config->get_bool("fog_enable", false);
		fog_color = video::SColor(0,255,255,255);
		fog_type = (video::E_FOG_TYPE)config->get_int("fog_type", video::EFT_FOG_LINEAR);
		fog_start = config->get_f32("fog_start", 500.0f);
		fog_end = config->get_f32("fog_end", 1000.0f);
		fog_density = config->get_f32("fog_density", 0.01f);
		fog_pixel = config->get_bool("fog_pixel", false);
		fog_range = config->get_bool("fog_range", false);
		logo_file = io::path("graphics/irrlicht_logo.png");
		logo = 0;
		current_level = -1;
		empty_objects();
		hero_screen_show = config->get_bool("hero_screen_show", true);
		hero_screen_height = 100;
		hero_screen_line_width = 11;
		hero_screen_color = video::SColor(255, 100, 100, 255);
		hero_node = 0;
		shot_enable = false;
		set_run_level(false);
		weapon_current = -1;
		hero_weapons = 0;
		menu_height = 50;
	}

	core::stringw _(core::stringw value)
	{
		return value;
	}

	int get_type_handle(HSTREAM& handle)
	{
		int type_handle = 0;
		BASS_CHANNELINFO* channel_info = (BASS_CHANNELINFO*)malloc(sizeof(BASS_CHANNELINFO));
		if(BASS_ChannelGetInfo(handle, channel_info))
			type_handle = channel_info->ctype;
		else
			cout << endl << "+++ BASS_ChannelGetInfo error " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
		free(channel_info);
		return type_handle;
	}

	void set_sound_volume(s32 value)
	{
		sound_volume = (float)value / 100.0f;
		BASS_SetVolume(sound_volume);
		config->set_float("sound_volume", sound_volume);
	}

	HSTREAM create_sound_handle(const io::path& file_path, bool looped = false)
	{
		HSTREAM handle;
		if(!device->getFileSystem()->existFile(file_path))
			cout << endl << "+++ sound file " << file_path.c_str() << " not found" << endl;
		else
		{
			core::stringc sound_format("wav");
			s32 dot_idx = file_path.findLast('.') + 1;
			if(dot_idx > 0)
				sound_format = file_path.subString(dot_idx, file_path.size() - dot_idx, true);
			bool is_music = ((music_formats.find(sound_format.c_str()) + 1) > 0);
			DWORD flags = 0;
			if(looped)
				flags |= BASS_SAMPLE_LOOP;
			else
				flags |= BASS_STREAM_AUTOFREE;
			if(is_music)
			{
				flags |= BASS_MUSIC_PRESCAN;
				handle = BASS_MusicLoad(false, file_path.c_str(), 0, 0, flags, 0);
			}
			else
				handle = BASS_StreamCreateFile(false, file_path.c_str(), 0, 0, flags);
			if(!handle)
			{
				if(is_music)
					cout << endl << "+++ BASS_MusicLoad " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
				else
					cout << endl << "+++ BASS_StreamCreateFile " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
				cout << endl << "+++ sound file is " << file_path.c_str() << endl;
			}
		}
		return handle;
	}

	void create_sound_volume()
	{
		gui::IGUIElement* e = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_WINDOW_SOUND_VOLUME, true);
		if(e)
			e->remove();
		gui::IGUIWindow* window = device->getGUIEnvironment()->addWindow(core::rect<s32>(10,45,400,150), false, _("Sound volume window").c_str(), device->getGUIEnvironment()->getRootGUIElement(), GUI_ID_WINDOW_SOUND_VOLUME);
		device->getGUIEnvironment()->addStaticText(_("Select sound volume").c_str(), core::rect<s32>(10,30,380,60), true, false, window);
		gui::IGUIScrollBar* scrollbar = device->getGUIEnvironment()->addScrollBar(true, core::rect<s32>(10,80,380,100), window, GUI_ID_SOUND_VOLUME);
		scrollbar->setPos((int)(sound_volume * 100.0));
	}

	void event_sound_play(u32 index)
	{
		if(sounds.size() > index)
		{
			HSTREAM bass_handle_event = create_sound_handle(sounds[index]);
			if(bass_handle_event)
				event_sound_playing = BASS_ChannelPlay(bass_handle_event, false);
		}
	}

	bool sound_play(HSTREAM& bass_handle, const io::path& file_path = io::path(""))
	{
		bool result = false;
		if(!bass_handle)
		{
			if(device->getFileSystem()->existFile(file_path))
				bass_handle = create_sound_handle(file_path, true);
			else
			{
				cout << endl << "ERROR sound file " << file_path.c_str() << " not found" << endl;
				return result;
			}
		}
		if(bass_handle)
		{
			if(BASS_ChannelPlay(bass_handle, false))
				result = true;
			else
			{
				int code = BASS_ErrorGetCode();
				if(code == BASS_ERROR_START)
				{
					if(!BASS_Start())
						cout << endl << "ERROR BASS_Start " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
					else
					{
						if(BASS_ChannelPlay(bass_handle, false))
							result = true;
					}
				}
				if(!result)
					cout << endl << "ERROR BASS_ChannelPlay " << get_bass_error_description(code) << " | " << file_path.c_str() << endl;
			}
		}
		return result;
	}

	void sound_stop(HSTREAM& bass_handle, bool close_handle = false)
	{
		if(bass_handle)
		{
			if(BASS_ChannelIsActive(bass_handle) == BASS_ACTIVE_PLAYING)
			{
				if(!BASS_ChannelStop(bass_handle))
					cout << endl << "ERROR BASS_ChannelStop " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
			}
			if(close_handle)
			{
				int type_handle = get_type_handle(bass_handle);
				if(type_handle >= BASS_CTYPE_MUSIC_MOD)
				{
					if(!BASS_MusicFree(bass_handle))
						cout << endl << "ERROR BASS_MusicFree " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
					else
						bass_handle = 0;
				}
				else if(type_handle >= BASS_CTYPE_STREAM)
				{
					if(!BASS_StreamFree(bass_handle))
						cout << endl << "ERROR BASS_StreamFree " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
					else
						bass_handle = 0;
				}
			}
		}
	}

	void game_sound_play()
	{
		if(!music_file.empty())
			game_sound_playing = sound_play(bass_handle_game, io::path(music_file.c_str()));
	}

	void game_sound_stop(bool close_handle = false)
	{
		sound_stop(bass_handle_game, close_handle);
	}

	void level_sound_play(const io::path& file_path = io::path(""))
	{
		level_sound_playing = sound_play(bass_handle_level, file_path);
	}

	void level_sound_stop(bool close_handle = false)
	{
		level_sound_playing = false;
		sound_stop(bass_handle_level, close_handle);
		if(!BASS_Pause())
			cout << endl << "ERROR BASS_Pause " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
		if(!BASS_Stop())
			cout << endl << "ERROR BASS_Stop " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
	}

	void sound_on_off()
	{
		if(play_sound)
		{
			if(bass_handle_game && game_sound_playing)
			{
				game_sound_playing = false;
				if(!BASS_ChannelStop(bass_handle_game))
					cout << endl << "ERROR BASS_ChannelStop " << get_bass_error_description(BASS_ErrorGetCode()) << endl;
			}
			if(bass_handle_level && level_sound_playing)
			{
				level_sound_playing = false;
				BASS_ChannelStop(bass_handle_level);
			}
		}
		else
		{
			level_sound_play();
		}
		play_sound = !play_sound;
		if(menu_options)
			menu_options->setItemChecked(menu_sound, play_sound);
		config->set_bool("play_sound", play_sound);
	}

	video::ITexture* get_new_texture(video::ITexture* tex, const io::path& name = "")
	{
		video::IImage* img = video_driver->createImageFromData(tex->getColorFormat(), tex->getSize(), tex->lock(video::ETLM_READ_ONLY));
		tex->unlock();
		video::ITexture* result = video_driver->addTexture(name, img);
		img->drop();
		return result;
	}

	int randrange(int rnd_min = 0, int rnd_max = RAND_MAX)
	{
		return rnd_min+int((rnd_max-rnd_min+1)*rand()/(RAND_MAX + 1.0));
	}
	
	video::ITexture* texture_generator(video::ECOLOR_FORMAT image_format = video::ECF_R8G8B8, const core::dimension2d<u32>& image_size = core::dimension2d<u32>(2, 2), const core::stringc& texture_name = core::stringc("texture_name"), u32 alpha_value = 128, u32 red[] = default_color_range, u32 green[] = default_color_range, u32 blue[] = default_color_range)
	{
		u32 alpha = 0, r = 0, g = 0, b = 0;
		bool blend = false;
		video::IImage* image = device->getVideoDriver()->createImage(image_format, image_size);
		video::ECOLOR_FORMAT color_format = image->getColorFormat();
		if(color_format == video::ECF_A1R5G5B5 || color_format == video::ECF_A8R8G8B8 || color_format == video::ECF_A16B16G16R16F || color_format == video::ECF_A32B32G32R32F)
		{
			alpha = alpha_value;
			blend = true;
		}
		srand((unsigned)time(NULL));
		for(u32 row = 0; row < image_size.Height; ++row)
		{
			for(u32 column = 0; column < image_size.Width; ++column)
			{
				if(red)
					r = randrange(red[0], red[1]);
				if(green)
					g = randrange(green[0], green[1]);
				if(blue)
					b = randrange(blue[0], blue[1]);
				image->setPixel(row, column, video::SColor(alpha, r, g, b), blend);
			}
		}
		return device->getVideoDriver()->addTexture(texture_name, image);
	}

	void create_wall_plane_selector(const core::dimension2d<f32> tileSize, const core::dimension2d<u32>& tileCount=core::dimension2d<u32>(1,1), video::SMaterial* material = 0, const core::dimension2d<f32>& textureRepeatCount = core::dimension2d<f32>(1.f,1.f), const core::vector3d<f32>* pos = 0, const core::vector3d<f32>* rotation = 0)
	{
		scene::IMesh* i_mesh = i_geometry_creator->createPlaneMesh(tileSize, tileCount, material, textureRepeatCount);
		scene::IMeshSceneNode* i_mesh_scene_node = device->getSceneManager()->addMeshSceneNode(i_mesh);
		outside_nodes.push_back(i_mesh_scene_node);
		i_mesh->drop();
		if(pos)
			i_mesh_scene_node->setPosition(*pos);
		if(rotation)
			i_mesh_scene_node->setRotation(*rotation);
		scene::ITriangleSelector* selector = device->getSceneManager()->createTriangleSelector(i_mesh, i_mesh_scene_node);
		i_mesh_scene_node->setTriangleSelector(selector);
		meta->addTriangleSelector(selector);
		selector->drop();
	}

	void create_wall_plane_selector(const core::dimension2d<f32> tileSize, const core::dimension2d<u32>& tileCount, video::SMaterial* material, const core::dimension2d<f32>& textureRepeatCount, const core::vector3d<f32>& pos, const core::vector3d<f32>& rotation)
	{
		create_wall_plane_selector(tileSize, tileCount, material, textureRepeatCount, &pos, &rotation);
	}

	void set_cube_texture(scene::ISceneNode* node, bool color_is_green = false, const core::stringc& texture_name = core::stringc("cube"))
	{
		video::SMaterial material = node->getMaterial(0);
		material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		video::ITexture* texture;
		u32 color_range[] = {100, 255};
		if(color_is_green)
			texture = texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(4, 4), texture_name, 128, 0, color_range, 0);
		else
			texture = texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(4, 4), texture_name, 128, color_range, 0, 0);
		if(texture)
			material.setTexture(0, texture);
		material.BackfaceCulling = false;
		material.FogEnable = fog_enable;
		material.AmbientColor = video::SColor(128, 255, 0, 0);
		material.DiffuseColor = video::SColor(128, 255, 0, 0);
		material.EmissiveColor = video::SColor(128, 255, 0, 0);
		material.SpecularColor = video::SColor(128, 255, 0, 0);
	}

	void show_warning()
	{
		device->getGUIEnvironment()->addMessageBox(_("Warning").c_str(), _("For finish this operation you need restart game!").c_str());
	}

	void set_device_type(video::E_DRIVER_TYPE new_device_type = video::EDT_NULL)
	{
		driver_type = new_device_type;
		for(u32 i = 0; i < menu_device_type->getItemCount(); ++i)
			menu_device_type->setItemChecked(i, ((driver_type | GUI_ID_DRIVER) == (video::E_DRIVER_TYPE)menu_device_type->getItemCommandId(i)));
		config->set_int("driver_type", driver_type);
		show_warning();
	}

	void setActiveCamera(scene::ICameraSceneNode* camera)
	{
		if(device || camera)
		{
			scene::ICameraSceneNode* active = device->getSceneManager()->getActiveCamera();
			if(active)
				active->setInputReceiverEnabled(false);
			camera->setInputReceiverEnabled(true);
			device->getSceneManager()->setActiveCamera(camera);
		}
	}

	void setActiveCameraOff()
	{
		scene::ICameraSceneNode* cam = device->getSceneManager()->getActiveCamera();
		if(cam)
			cam->setInputReceiverEnabled(false);
		cursor_control->setVisible(true);
	}

	void create_font()
	{
#ifdef USE_TTFONT
#if defined(_WIN32)
#define BUF_SIZE 32767
		fschar_t buf[BUF_SIZE];
		GetWindowsDirectory(buf, BUF_SIZE);
		io::path font_file(core::stringw(buf)+"/fonts/arial.ttf");
#else
		io::path font_file("fonts/arial.ttf");
#endif
		if(device->getFileSystem()->existFile(font_file))
		{
			font = gui::CGUITTFont::createTTFont(device->getGUIEnvironment(), font_file, font_size);
			if(font)
			{
				device->getGUIEnvironment()->getSkin()->setFont(font);
				font->drop();
			}
		}
		else
			cout << endl << "ERROR: file not found (" << font_file.c_str() << ")" << endl;
		if(!font)
		{
			cout << endl << "ERROR: CGUITTFont not created !!!" << endl;
			font = (gui::CGUITTFont*)device->getGUIEnvironment()->getBuiltInFont();
		}
#else
		font = device->getGUIEnvironment()->getBuiltInFont();
#endif
	}

	void create_logo()
	{
		if(device->getFileSystem()->existFile(logo_file))
		{
			logo = video_driver->getTexture(logo_file);
			if(logo)
			{
				logo_size = logo->getOriginalSize();
				logo_rect = core::rect<s32>(0, 0, logo_size.Width, logo_size.Height);
			}
		}
	}

	void show_about_dialog()
	{
		device->getGUIEnvironment()->addMessageBox(_("About").c_str(), app_properties.description.c_str());
	}

	void show_help_dialog()
	{
		setActiveCameraOff();
		dialog_help = device->getGUIEnvironment()->addMessageBox(_("Help").c_str(), _("F2 - options; ESC - exit").c_str());
	}

	void show_options_dialog()
	{
		if(!dialog_options)
		{
			dialog_options = device->getGUIEnvironment()->addWindow(core::rect<s32>(5,5,640,480), true, _("Options").c_str(), device->getGUIEnvironment()->getRootGUIElement(), GUI_ID_WINDOW_OPTIONS);
			device->getGUIEnvironment()->addCheckBox(play_sound, core::rect<s32>(10,30,500,60), dialog_options, GUI_ID_OPT_BACK_SOUND, _("Enable background sound").c_str());
			device->getGUIEnvironment()->addCheckBox(device_parameters.Fullscreen, core::rect<s32>(10,80,500,110), dialog_options, GUI_ID_OPT_FULL_SCREEN, _("Enable full screen").c_str());
			device->getGUIEnvironment()->addCheckBox(hero_screen_show, core::rect<s32>(10,130,500,160), dialog_options, GUI_ID_OPT_HERO_SCREEN, _("Enable hero screen").c_str());
			device->getGUIEnvironment()->addStaticText(_("Select sound volume").c_str(), core::rect<s32>(10,180,500,210), false, false, dialog_options);
			gui::IGUIScrollBar* scrollbar = device->getGUIEnvironment()->addScrollBar(true, core::rect<s32>(10,220,500,240), dialog_options, GUI_ID_SOUND_VOLUME);
			scrollbar->setPos((int)(sound_volume * 100.0));
			device->getGUIEnvironment()->addButton(core::rect<s32>(10,440,200,470), dialog_options, GUI_ID_OPT_SAVE, _("Save to file").c_str());
		}
		else
		{
			if(!dialog_options->isVisible())
				dialog_options->setVisible(true);
		}
		cursor_control->setVisible(true);
	}

	void close_options_dialog()
	{
		if(dialog_options)
		{
			dialog_options->remove();
			dialog_options= 0;
			if(get_run_level())
				cursor_control->setVisible(false);
		}
	}

	void create_top_menu()
	{
		menu = device->getGUIEnvironment()->addMenu();
		menu->addItem(_("File").c_str(), -1, true, true);
		menu->addItem(_("View").c_str(), -1, true, true);
		menu->addItem(_("Options").c_str(), -1, true, true);
		menu->addItem(_("Help").c_str(), -1, true, true);

		menu->getSubMenu(0)->addItem(_("Load game").c_str(), GUI_ID_LOAD);
		menu->getSubMenu(0)->addItem(_("Save game").c_str(), GUI_ID_SAVE);
		menu->getSubMenu(0)->addSeparator();
		menu->getSubMenu(0)->addItem(_("Quit").c_str(), GUI_ID_QUIT);

		menu->getSubMenu(1)->addItem(_("Camera off").c_str(), GUI_ID_CAMERA_NONE);
		menu->getSubMenu(1)->addItem(_("Perspective view").c_str(), GUI_ID_CAMERA_MAYA);
		menu->getSubMenu(1)->addItem(_("First Person view").c_str(), GUI_ID_CAMERA_FPS);

		menu_options = menu->getSubMenu(2);
		menu_options->addItem(_("Start/stop log").c_str(), GUI_ID_LOG);
		menu_options->addItem(_("Choose graphics driver").c_str(), GUI_ID_DRIVER, true, true);
		menu_device_type = menu_options->getSubMenu(1);
		menu_device_type->addItem(_("Software").c_str(), GUI_ID_EDT_SOFTWARE, true, false, (driver_type == video::EDT_SOFTWARE));
		menu_device_type->addItem(_("OpenGL").c_str(), GUI_ID_EDT_OPENGL, true, false, (driver_type == video::EDT_OPENGL));
		menu_device_type->addItem(_("DirectX 9").c_str(), GUI_ID_EDT_DIRECT3D9, true, false, (driver_type == video::EDT_DIRECT3D9));
		menu_device_type->addItem(_("Burningsvideo").c_str(), GUI_ID_EDT_BURNINGSVIDEO, true, false, (driver_type == video::EDT_BURNINGSVIDEO));
		menu_sound = menu_options->addItem(_("Sound on/off").c_str(), GUI_ID_SOUND_ON_OFF, true, false, play_sound);
		menu_sound_volume = menu_options->addItem(_("Sound volume").c_str(), GUI_ID_MENU_SOUND_VOLUME);

		gui::IGUIContextMenu* submenu = menu->getSubMenu(3);
		submenu->addItem(_("About").c_str(), GUI_ID_ABOUT);
	}

	void create_main_window(u32 width = 0, u32 height = 0)
	{
		if(!width)
			width = 200;
		if(!height)
			height = font_size * 16;
		u32 x1 = (device_parameters.WindowSize.Width - width) / 2;
		u32 y1 = (device_parameters.WindowSize.Height - height) / 2;
		u32 x2 = device_parameters.WindowSize.Width - x1;
		u32 y2 = device_parameters.WindowSize.Height - y1;
		main_window = device->getGUIEnvironment()->addWindow(core::rect<s32>(x1, y1, x2, y2), false, _("Main menu").c_str());
		if(main_window)
		{
			main_window->getCloseButton()->setVisible(false);
			main_window->setDraggable(false);
			main_window->setDrawTitlebar(false);
			main_window->setTabStop(true);
			u32 x = 10;
			u32 y = font_size;
			u32 w = width - x;
			u32 h = y * 2;
			u32 d = h / 2;
			device->getGUIEnvironment()->addButton(core::rect<s32>(x, y, w, y + h), main_window, GUI_ID_LOAD, _("Start game").c_str(), _("Start new game").c_str())->setTabStop(true);
			y += h + d;
			device->getGUIEnvironment()->addButton(core::rect<s32>(x, y, w, y + h), main_window, GUI_ID_SAVE, _("Save game").c_str(), _("Save game to file").c_str())->setTabStop(true);
			y += h + d;
			device->getGUIEnvironment()->addButton(core::rect<s32>(x, y, w, y + h), main_window, GUI_ID_OPTIONS, _("Options").c_str(), _("Setup game settings").c_str())->setTabStop(true);
			y += h + d;
			device->getGUIEnvironment()->addButton(core::rect<s32>(x, y, w, y + h), main_window, GUI_ID_ABOUT, _("About").c_str(), _("Game description").c_str())->setTabStop(true);
			y += h + d;
			device->getGUIEnvironment()->addButton(core::rect<s32>(x, y, w, y + h), main_window, GUI_ID_QUIT, _("Quit").c_str(), _("Exit from game").c_str())->setTabStop(true);
		}
	}

	void main_window_show()
	{
		game_sound_play();
		if(main_window)
		{
			main_window->setVisible(true);
			device->getGUIEnvironment()->setFocus(main_window);
		}
	}

	void draw_2d_line(const core::position2d<s32>& position_start = core::position2d<s32>(), const core::position2d<s32>& position_end = core::position2d<s32>(), const video::SColor& color = video::SColor(255,255,255,255), s32 width = 0)
	{
		if (width < 2)
			video_driver->draw2DLine(position_start, position_end, color);
		else
		{
			if (width%2)
			{
				s32 count = (width-1)/2;
				for(s32 i = -count; i <= count; ++i)
					video_driver->draw2DLine(core::position2d<s32>(position_start.X+i, position_start.Y+i), core::position2d<s32>(position_end.X+i, position_end.Y+i), color);
			}
			else
			{
				s32 count = width/2;
				for(s32 i = -count; i < count; ++i)
					video_driver->draw2DLine(core::position2d<s32>(position_start.X+i, position_start.Y+i), core::position2d<s32>(position_end.X+i, position_end.Y+i), color);
			}
		}
	}

	bool has_key_in_hero_keys(core::array<video::ITexture*> keys, const core::stringc& key)
	{
		bool result = false;
		for(u32 i = 0; i < keys.size(); ++i)
		{
			if(keys[i]->getName().getPath() == key)
			{
				result = true;
				break;
			}
		}
		return result;
	}

	void delete_doors()
	{
		for(u32 i = 0; i < doors.size(); ++i)
			delete doors[i];
		doors.clear();
	}

	void delete_friends()
	{
		for(u32 i = 0; i < friends.size(); ++i)
			delete friends[i];
		friends.clear();
	}

	void remove_node(scene::ISceneNode* node)
	{
		if(node)
		{
			if(node->getAnimators().size())
				node->removeAnimators();
			node->remove();
		}
	}

	void remove_mesh_nodes(core::array<scene::IMeshSceneNode*>* nodes)
	{
		for(u32 i = 0; i < nodes->size(); ++i)
		{
			remove_node((*nodes)[i]);
		}
		nodes->clear();
	}

	void remove_billboard_nodes(core::array<scene::IBillboardSceneNode*>* nodes)
	{
		for(u32 i = 0; i < nodes->size(); ++i)
			remove_node((*nodes)[i]);
		nodes->clear();
	}

	void remove_nodes(core::array<scene::ISceneNode*>* nodes)
	{
		for(u32 i = 0; i < nodes->size(); ++i)
			remove_node((*nodes)[i]);
		nodes->clear();
	}

	void empty_objects()
	{
		weapon_current_texture = 0;
		remove_billboard_nodes(&weapons);
		remove_billboard_nodes(&enemies);
		remove_billboard_nodes(&keys);
		remove_billboard_nodes(&objects);
		delete_friends();
		remove_mesh_nodes(&enemy_bullets);
		remove_mesh_nodes(&bullets);
		remove_mesh_nodes(&walls);
		remove_mesh_nodes(&ceilings);
		remove_mesh_nodes(&floors);
		remove_mesh_nodes(&outside_nodes);
		delete_doors();
		for(u32 i = 0; i < hero_keys.size(); ++i)
			video_driver->removeTexture(hero_keys[i]);
		hero_keys.clear();
		health = 100;
	}

	void clear_level()
	{
		empty_objects();
		meta->removeAllTriangleSelectors();
		level_sound_stop(true);
	}

	bool get_run_level()
	{
		return run_level;
	}

	void set_run_level(bool value = true)
	{
		run_level = value;
	}

	void load_level_from_json(int level_index)
	{
		LevelData levelData = parseLevelFromJson(json_map_file, level_index);
		
		if (!levelData.has_data()) {
			cout << "ERROR: Failed to load level " << level_index << " from " << json_map_file << endl;
			set_run_level(false);
			shot_enable = false;
			cursor_control->setVisible(true);
			main_window->setVisible(true);
			return;
		}
		
		levels.clear();
		
		list<scene_item> data_floors, data_ceilings, data_objects, data_lights;
		
		for (const auto& [cell_key, cell] : levelData.cells) {
			int world_x = cell.x;
			int world_z = cell.y;
			int world_y = 0;
			
			// Floor tile
			if (cell.floor >= 0) {
				scene_item floor_item(0, world_x, world_z);
				floor_item.type = IMAGE_OBJECT;
				floor_item.index = cell.floor;
				data_floors.push_back(floor_item);
			}
			
			// Wall tile (if present)
			if (cell.wall >= 0) {
				scene_item wall_item(0, world_x, world_z);
				wall_item.type = IMAGE_WALL;
				wall_item.index = cell.wall;
				wall_item.y = levelData.default_side_size;
				data_objects.push_back(wall_item);
			}
			
			// Roof tile
			if (cell.roof >= 0) {
				scene_item roof_item(0, world_x, world_z);
				roof_item.type = IMAGE_OBJECT;
				roof_item.index = cell.roof;
				roof_item.y = levelData.default_side_size * 2;
				data_ceilings.push_back(roof_item);
			}
			
			// Special type handling (hero spawn, etc.)
			if (cell.type == 1) { // Hero spawn point
				scene_item hero_item(0, world_x, world_z);
				hero_item.type = IMAGE_HERO;
				hero_item.index = 0; // Hero texture index
				data_objects.push_back(hero_item);
			}
		}
		
		level_struct new_level(
			levelData.count_x,
			levelData.default_side_size,
			levelData.count_y,
			true,  // use_floor_plane
			true,  // use_ceiling_plane
			true,  // use_border_planes
			data_floors,
			data_ceilings,
			data_objects,
			data_lights,
			-1     // index_music
		);
		
		levels.push_back(new_level);
		
		for (const auto& [tex_id, tex_path] : levelData.textures) {
			// Ensure images array has enough space
			while ((int)images.size() <= tex_id) {
				images.push_back(io::path(""));
			}
			images[tex_id] = io::path(tex_path);
		}
	}

	void load_level(int index = 0)
	{
		clear_level();
		current_level = index;
		
		load_level_from_json(current_level);
		
		if(current_level >= (int)levels.size())
		{
			set_run_level(false);
			shot_enable = false;
			cursor_control->setVisible(true);
			main_window->setVisible(true);
			return;
		}

		cursor_control->setVisible(false);

		video::ITexture* texture = 0;

		io::path file_name("graphics//skydome.jpg");
		if(device->getFileSystem()->existFile(file_name))
			texture = video_driver->getTexture(file_name);
		else
		{
			u32 color_range[] = {150, 200};
			texture = texture_generator(video::ECF_R8G8B8, core::dimension2d<u32>(64, 64), "skydome", 0, color_range, color_range, 0);
		}
		scene::ISceneNode* sky_node = device->getSceneManager()->addSkyDomeSceneNode(texture);

		u32 d = levels[current_level].y / 2;
		core::dimension2d<f32> tileSize((f32)levels[current_level].y, (f32)levels[current_level].y);
		core::dimension2d<u32> tileCount(levels[current_level].x, levels[current_level].z);
		core::dimension2d<f32> textureRepeatCount((f32)levels[current_level].x, (f32)levels[current_level].z);
		video::SMaterial material;
		material.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
		material.FogEnable = fog_enable;
		material.EmissiveColor = video::SColor(128, 255, 255, 255);
		
		if(levels[current_level].use_floor_plane)
		{
			io::path file_name("graphics//stones.jpg");
			if(device->getFileSystem()->existFile(file_name))
				material.setTexture(0, video_driver->getTexture(file_name));
			else
				material.setTexture(0, texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(2, 2), "bottom", 0));
			scene::IMesh* i_mesh_bottom = i_geometry_creator->createPlaneMesh(tileSize, tileCount, &material, textureRepeatCount);
			scene::IMeshSceneNode* i_mesh_scene_node_bottom = device->getSceneManager()->addOctreeSceneNode(i_mesh_bottom);
			outside_nodes.push_back(i_mesh_scene_node_bottom);
			i_mesh_bottom->drop();
			i_mesh_scene_node_bottom->setPosition(core::vector3d<f32>((f32)(levels[current_level].x * d - d), 0.0f, (f32)(levels[current_level].z * d - d)));
			scene::ITriangleSelector* selector_bottom = device->getSceneManager()->createOctreeTriangleSelector(i_mesh_bottom, i_mesh_scene_node_bottom);
			i_mesh_scene_node_bottom->setTriangleSelector(selector_bottom);
			meta->addTriangleSelector(selector_bottom);
			selector_bottom->drop();
		}
		
		if(levels[current_level].use_ceiling_plane)
		{
			io::path file_name("graphics//opengllogo.png");
			if(driver_type == video::EDT_SOFTWARE)
				file_name = io::path("graphics//irrlichtlogo3.png");
			else if(driver_type == video::EDT_BURNINGSVIDEO)
				file_name = io::path("graphics//burninglogo.png");
			else if(driver_type == video::EDT_DIRECT3D8 && driver_type == video::EDT_DIRECT3D9)
				file_name = io::path("graphics//directxlogo.png");
			if(device->getFileSystem()->existFile(file_name))
				material.setTexture(0, video_driver->getTexture(file_name));
			else
				material.setTexture(0, texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(2, 2), "top", 0));
			scene::IMesh* i_mesh_top = i_geometry_creator->createPlaneMesh(tileSize, tileCount, &material, textureRepeatCount);
			scene::IMeshSceneNode* i_mesh_scene_node_top = device->getSceneManager()->addOctreeSceneNode(i_mesh_top);
			outside_nodes.push_back(i_mesh_scene_node_top);
			i_mesh_top->drop();
			i_mesh_scene_node_top->setPosition(core::vector3d<f32>((f32)(levels[current_level].x * d - d), (f32)levels[current_level].y, (f32)(levels[current_level].z * d - d)));
			i_mesh_scene_node_top->setRotation(core::vector3d<f32>(180, 0, 0));
			scene::ITriangleSelector* selector_top = device->getSceneManager()->createOctreeTriangleSelector(i_mesh_top, i_mesh_scene_node_top);
			i_mesh_scene_node_top->setTriangleSelector(selector_top);
			meta->addTriangleSelector(selector_top);
			selector_top->drop();
		}

		if(levels[current_level].use_border_planes)
		{
			core::dimension2d<u32> tileCountX(levels[current_level].x, 1);
			core::dimension2d<f32> textureRepeatCountX((f32)levels[current_level].x, 1.0f);
			core::dimension2d<u32> tileCountZ(levels[current_level].z, 1);
			core::dimension2d<f32> textureRepeatCountZ((f32)levels[current_level].z, 1.0f);
			u32 color_range[] = {250, 255};
			f32 x = (f32)(levels[current_level].y * levels[current_level].x);
			f32 z = (f32)(levels[current_level].y * levels[current_level].z);
			f32 dy = (f32)(levels[current_level].y / 2);
			
			texture = texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(4, 4), "front", 196, color_range, 0, 0);
			material.setTexture(0, texture);
			create_wall_plane_selector(tileSize, tileCountX, &material, textureRepeatCountX, core::vector3d<f32>(-dy, (f32)levels[current_level].y / 2, z / 2 - dy), core::vector3d<f32>(90.0f, 90.0f, 0.0f));
			
			texture = texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(4, 4), "back", 196, 0, 0, color_range);
			material.setTexture(0, texture);
			create_wall_plane_selector(tileSize, tileCountX, &material, textureRepeatCountX, core::vector3d<f32>(x - dy, (f32)levels[current_level].y / 2, z / 2 - dy), core::vector3d<f32>(90.0f, -90.0f, 0.0f));
			
			texture = texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(4, 4), "right", 196, 0, color_range, 0);
			material.setTexture(0, texture);
			create_wall_plane_selector(tileSize, tileCountZ, &material, textureRepeatCountZ, core::vector3d<f32>(x / 2 - dy, (f32)levels[current_level].y / 2, z - dy), core::vector3d<f32>(-90.0f, 0.0f, 0.0f));
			
			texture = texture_generator(video::ECF_A8R8G8B8, core::dimension2d<u32>(4, 4), "left", 196, color_range, color_range, 0);
			material.setTexture(0, texture);
			create_wall_plane_selector(tileSize, tileCountZ, &material, textureRepeatCountZ, core::vector3d<f32>(x / 2 - dy, (f32)levels[current_level].y / 2, -dy), core::vector3d<f32>(90.0f, 0.0f, 0.0f));
		}

		// ADD FLOOR CUBES
		for(list<scene_item>::iterator it = levels[current_level].floors.begin(); it != levels[current_level].floors.end(); ++it)
		{
			scene::IMeshSceneNode* cube = device->getSceneManager()->addCubeSceneNode((f32)levels[current_level].y * 1.0f, 0, -1, core::vector3d<f32>((f32)((*it).x) * levels[current_level].y, -(f32)(levels[current_level].y/2), (f32)((*it).z) * levels[current_level].y));
			if(cube)
			{
				if(!levels[current_level].use_floor_plane)
				{
					scene::ITriangleSelector* selector_floor = device->getSceneManager()->createTriangleSelectorFromBoundingBox(cube);
					meta->addTriangleSelector(selector_floor);
					selector_floor->drop();
				}
				cube->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
				cube->setMaterialFlag(video::EMF_LIGHTING, false);
				if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
					cube->setMaterialTexture(0, video_driver->getTexture(images[(*it).index]));
				else
					set_cube_texture(cube);
				if(!(*it).animation_idle.empty())
				{
					core::array<video::ITexture*> textures;
					for(u32 aidx = 0; aidx < (*it).animation_idle.size(); ++aidx)
						textures.push_back(video_driver->getTexture(images[(*it).animation_idle[aidx]]));
					scene::ISceneNodeAnimator* aidle = device->getSceneManager()->createTextureAnimator(textures, (*it).delay_animation_idle);
					cube->addAnimator(aidle);
					aidle->drop();
				}
				floors.push_back(cube);
			}
		}

		// ADD CEILING CUBES
		for(list<scene_item>::iterator it = levels[current_level].ceilings.begin(); it != levels[current_level].ceilings.end(); ++it)
		{
			scene::IMeshSceneNode* cube = device->getSceneManager()->addCubeSceneNode(levels[current_level].y * 1.0f, 0, -1, core::vector3d<f32>((f32)(*it).x * levels[current_level].y, (f32)levels[current_level].y * 1.5f, (f32)(*it).z * levels[current_level].y));
			if(cube)
			{
				cube->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
				cube->setMaterialFlag(video::EMF_LIGHTING, false);
				if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
					cube->setMaterialTexture(0, video_driver->getTexture(images[(*it).index]));
				else
					set_cube_texture(cube);
				if(!(*it).animation_idle.empty())
				{
					core::array<video::ITexture*> textures;
					for(u32 aidx = 0; aidx < (*it).animation_idle.size(); ++aidx)
						textures.push_back(video_driver->getTexture(images[(*it).animation_idle[aidx]]));
					scene::ISceneNodeAnimator* aidle = device->getSceneManager()->createTextureAnimator(textures, (*it).delay_animation_idle);
					cube->addAnimator(aidle);
					aidle->drop();
				}
				ceilings.push_back(cube);
			}
		}

		// HERO PARAMETERS
		core::vector3d<f32> camera_start_pos(levels[current_level].y * 2.0f, (f32)levels[current_level].y, levels[current_level].y * 2.0f);
		u32 hero_rect_size = hero_screen_height - hero_screen_line_width - hero_screen_line_width / 2;
		hero_source_rect = core::rect<s32>(0, 0, hero_rect_size, hero_rect_size);
		hero_node = device->getSceneManager()->addCubeSceneNode(levels[current_level].y / 2.0f, camera_fps);

		// ADD WALL CUBES AND OTHER OBJECTS
		u32 id_obj = (u32)IMAGE_TYPE_COUNT;
		u32 id_key = (u32)IMAGE_TYPE_COUNT;
		u32 id_door = (u32)IMAGE_TYPE_COUNT;
		u32 id_weapon = (u32)IMAGE_TYPE_COUNT;
		
		for(list<scene_item>::iterator it = levels[current_level].objects.begin(); it != levels[current_level].objects.end(); ++it)
		{
			scene::IMeshSceneNode* node = 0;
			IMAGE_TYPE img_type = (*it).type;
			switch(img_type)
			{
			case IMAGE_HERO:
				camera_start_pos = core::vector3d<f32>((f32)(*it).x * levels[current_level].y, (f32)levels[current_level].y / 2, (f32)(*it).z * levels[current_level].y);
				if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
				{
					hero_texture = video_driver->getTexture(images[(*it).index]);
					if(hero_texture)
					{
						core::dimension2d<u32> size = hero_texture->getOriginalSize();
						hero_source_rect = core::rect<s32>(0, 0, size.Width, size.Height);
					}
				}
				break;
			case IMAGE_WALL:
				node = device->getSceneManager()->addCubeSceneNode((f32)levels[current_level].y, 0, -1, core::vector3d<f32>((f32)((*it).x * levels[current_level].y), (f32)(levels[current_level].y / 2), (f32)((*it).z * levels[current_level].y)));
				if(node)
				{
					node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
					node->setMaterialFlag(video::EMF_LIGHTING, false);
					if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
						node->setMaterialTexture(0, video_driver->getTexture(images[(*it).index]));
					else
						set_cube_texture(node);
					if((*it).scale)
						node->setScale(core::vector3d<f32>((*it).scale));
					scene::ITriangleSelector* selector_wall = device->getSceneManager()->createTriangleSelectorFromBoundingBox(node);
					meta->addTriangleSelector(selector_wall);
					selector_wall->drop();
					walls.push_back(node);
				}
				break;
			case IMAGE_KEY:
			case IMAGE_WEAPON:
			case IMAGE_AMMO:
			case IMAGE_HEALTH:
			case IMAGE_ENEMY:
			case IMAGE_FRIEND:
			case IMAGE_OBJECT:
				if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
				{
					core::dimension2d<f32> billboard_scene_node_size((f32)levels[current_level].y * 1.0f, (f32)levels[current_level].y * 1.0f);
					if((*it).scale)
						billboard_scene_node_size = core::dimension2d<f32>((f32)levels[current_level].y * (*it).scale, (f32)levels[current_level].y * (*it).scale);
					core::vector3d<f32> billboard_scene_node_position((f32)(*it).x * levels[current_level].y, (f32)levels[current_level].y / 2, (f32)(*it).z * levels[current_level].y);
					if((*it).y)
						billboard_scene_node_position = core::vector3d<f32>((f32)(*it).x * levels[current_level].y, (f32)(*it).y, (f32)(*it).z * levels[current_level].y);
					scene::IBillboardSceneNode* billboard_node = device->getSceneManager()->addBillboardSceneNode(device->getSceneManager()->getRootSceneNode(), billboard_scene_node_size, billboard_scene_node_position, img_type);
					if(billboard_node)
					{
						billboard_node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
						billboard_node->setMaterialFlag(video::EMF_LIGHTING, false);
						billboard_node->setName((*it).name);
						billboard_node->setMaterialTexture(0, video_driver->getTexture(images[(*it).index]));
						switch(img_type)
						{
						case IMAGE_KEY:
							if((*it).name.empty())
							{
								billboard_node->setName(core::stringc("key_") + core::stringc(id_key));
								id_key += 1;
							}
							keys.push_back(billboard_node);
							break;
						case IMAGE_WEAPON:
							if((*it).name.empty())
							{
								billboard_node->setName(core::stringc("weapon_") + core::stringc(id_weapon));
								id_weapon += 1;
							}
							if(!(*it).animation_shot.empty())
							{
								core::array<video::ITexture*> textures;
								for(u32 aidx = 0; aidx < (*it).animation_shot.size(); ++aidx)
									textures.push_back(video_driver->getTexture(images[(*it).animation_shot[aidx]]));
								scene::ISceneNodeAnimator* anim = device->getSceneManager()->createTextureAnimator(textures, (*it).delay_animation_shot);
								billboard_node->addAnimator(anim);
								anim->drop();
							}
							weapons.push_back(billboard_node);
							break;
						case IMAGE_ENEMY:
						case IMAGE_FRIEND:
						case IMAGE_OBJECT:
							scene::ITriangleSelector* node_selector = device->getSceneManager()->createTriangleSelectorFromBoundingBox(billboard_node);
							billboard_node->setTriangleSelector(node_selector);
							meta->addTriangleSelector(node_selector);
							node_selector->drop();
							switch(img_type)
							{
							case IMAGE_ENEMY:
								enemies.push_back(billboard_node);
								break;
							case IMAGE_FRIEND:
								friends.push_back(new object_friend(billboard_node, (*it).messages, (*it).event_sounds));
								break;
							case IMAGE_OBJECT:
								objects.push_back(billboard_node);
								break;
							}
							break;
						}
						if(!(*it).animation_idle.empty())
						{
							core::array<video::ITexture*> textures;
							for(u32 aidx = 0; aidx < (*it).animation_idle.size(); ++aidx)
								textures.push_back(video_driver->getTexture(images[(*it).animation_idle[aidx]]));
							scene::ISceneNodeAnimator* anim = device->getSceneManager()->createTextureAnimator(textures, (*it).delay_animation_idle);
							billboard_node->addAnimator(anim);
							anim->drop();
						}
					}
				}
				else
					cout << endl << "--- FILE NOT FOUND " << images[(*it).index].c_str() << endl;
				break;
			case IMAGE_DOOR:
				node = device->getSceneManager()->addCubeSceneNode((f32)levels[current_level].y * 1.0f, 0, id_door, core::vector3d<f32>((f32)(*it).x * levels[current_level].y, (f32)levels[current_level].y / 2, (f32)(*it).z * levels[current_level].y));
				if(node)
				{
					node->setName(core::stringc("door_") + core::stringc(id_door) + "_" + (*it).name);
					id_door += 1;
					node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
					node->setMaterialFlag(video::EMF_LIGHTING, false);
					if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
						node->setMaterialTexture(0, video_driver->getTexture(images[(*it).index]));
					else
						set_cube_texture(node);
					scene::ITriangleSelector* selector_door = device->getSceneManager()->createTriangleSelectorFromBoundingBox(node);
					meta->addTriangleSelector(selector_door);
					selector_door->drop();
					if((*it).scale)
						node->setScale(core::vector3d<f32>((*it).scale));
					if(!(*it).animation_idle.empty())
					{
						core::array<video::ITexture*> textures;
						for(u32 aidx = 0; aidx < (*it).animation_idle.size(); ++aidx)
							textures.push_back(video_driver->getTexture(images[(*it).animation_idle[aidx]]));
						scene::ISceneNodeAnimator* anim = device->getSceneManager()->createTextureAnimator(textures, (*it).delay_animation_idle);
						node->addAnimator(anim);
						anim->drop();
					}
					doors.push_back(new object_door(node, (*it).key, (*it).event_sounds, (*it).goto_level_index));
				}
				break;
			}
		}

		// ADD LIGHT
		for(list<scene_item>::iterator it = levels[current_level].lights.begin(); it != levels[current_level].lights.end(); ++it)
		{
			scene::IVolumeLightSceneNode* node = device->getSceneManager()->addVolumeLightSceneNode(0, -1, 32, 32, video::SColor(0, 255, 255, 255), video::SColor(0, 0, 0, 0), core::vector3d<f32>((f32)(*it).x * levels[current_level].y, levels[current_level].y / 1.2f, (f32)(*it).z * levels[current_level].y), core::vector3d<f32>(0.0f, 0.0f, 0.0f), core::vector3d<f32>(100.0f, 1.0f, 100.0f));
			if(node)
			{
				node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
				node->setMaterialFlag(video::EMF_LIGHTING, false);
				if((*it).index < images.size() && device->getFileSystem()->existFile(images[(*it).index]))
					node->setMaterialTexture(0, video_driver->getTexture(images[(*it).index]));
			}
		}

		// SETUP CAMERA
		camera_maya->setTarget(core::vector3d<f32>(0, 600, 0));
		setActiveCamera(camera_fps);
		camera_fps->setPosition(camera_start_pos);
		
		scene::ISceneNodeAnimatorCollisionResponse *a = 0;
		core::list<scene::ISceneNodeAnimator*>::ConstIterator anim_it = camera_fps->getAnimators().begin();
		for (; anim_it != camera_fps->getAnimators().end(); ++anim_it)
		{
			a = (scene::ISceneNodeAnimatorCollisionResponse*)(*anim_it);
			if(a->getType() == scene::ESNAT_COLLISION_RESPONSE)
			{
				a->setTargetNode(camera_fps);
				break;
			}
		}

		warning_position = core::rect<s32>(10, menu_height + font_size, 0, 0);
		warning_color = video::SColor(255, 255, 255, 255);

		main_window->setVisible(false);

		game_sound_stop();
		if(play_sound && levels[current_level].index_music > -1 && levels[current_level].index_music < (int)sounds.size())
		{
			level_sound_play(sounds[(levels[current_level].index_music)]);
		}

		shot_enable = true;
		set_run_level();
	}

	void draw_logo()
	{
		if(logo)
		{
			video_driver->draw2DImage(logo, core::position2d<s32>(0, 0));
		}
	}

	void draw_weapon()
	{
		if(weapon_current > -1)
		{
			if(hero_weapons.empty())
				weapon_current = -1;
			else
			{
				video::ITexture* texture = hero_weapons[weapon_current];
				if(texture)
				{
					s32 x = device_parameters.WindowSize.Width/5*2;
					core::position2d<s32> destPos;
					video_driver->draw2DImage(texture, core::rect<s32>(x, device_parameters.WindowSize.Height/2, device_parameters.WindowSize.Width - x, device_parameters.WindowSize.Height - hero_screen_height), core::rect<s32>(core::position2di(), texture->getOriginalSize()), 0, 0, true);
				}
			}
		}
	}

	void draw_hero_screen()
	{
		u32 w = device_parameters.WindowSize.Width;
		u32 h = device_parameters.WindowSize.Height;
		u32 d = (hero_screen_line_width - 1) / 2;
		u32 x1 = w - d;
		u32 y1 = h - hero_screen_height;
		u32 y2 = h - d;
		u32 x1k = hero_screen_line_width;
		u32 y1k = y1;
		u32 x2k = hero_screen_height / 2;
		u32 y2k = y2 - hero_screen_height / 2;
		for(u32 i = 0; i < hero_keys.size(); ++i)
		{
			video::ITexture* texture = hero_keys[i];
			video_driver->draw2DImage(texture, core::rect<s32>(x1k, y1k, x2k, y2k), core::rect<s32>(core::position2d<s32>(), texture->getOriginalSize()), 0, 0, true);
			if(i == 1)
			{
				x1k = hero_screen_line_width;
				x2k = hero_screen_height / 2;
				y1k = h - hero_screen_height / 2;
				y2k = h - hero_screen_line_width;
			}
			else
			{
				x1k = x2k;
				x2k = hero_screen_height;
			}
			if(i > 3)
				break;
		}
		if(hero_texture)
			video_driver->draw2DImage(hero_texture, core::rect<s32>(w / 2 - hero_screen_height, y1, w / 2 + hero_screen_height, y2), hero_source_rect, 0, 0, true);
		core::stringw health_text(L"HEALTH "); health_text += core::stringw(health);
#if defined(USE_TTFONT)
		core::dimension2d<u32> health_size = font->getDimension(health_text);
#else
		core::dimension2d<u32> health_size = font->getDimension(health_text.c_str());
#endif
		font->draw(health_text, core::rect<s32>(w - health_size.Width - hero_screen_line_width, h - health_size.Height - hero_screen_line_width, w - hero_screen_line_width, h - hero_screen_line_width), warning_color);
		draw_2d_line(core::vector2d<s32>(0, y1), core::vector2d<s32>(w, y1), hero_screen_color, hero_screen_line_width);
		draw_2d_line(core::vector2d<s32>(0, y2), core::vector2d<s32>(w, y2), hero_screen_color, hero_screen_line_width);
		draw_2d_line(core::vector2d<s32>(d, y1), core::vector2d<s32>(d, h), hero_screen_color, hero_screen_line_width);
		draw_2d_line(core::vector2d<s32>(x1, y1), core::vector2d<s32>(x1, h), hero_screen_color, hero_screen_line_width);
	}

	bool open_door(scene::IMeshSceneNode* node, f32 speed = 0.1)
	{
		scene::ISceneNodeAnimator* animator = 0;
		core::list<scene::ISceneNodeAnimator*>::ConstIterator it = node->getAnimators().begin();
		for(; it != node->getAnimators().end(); ++it)
		{
			animator = *it;
			if(animator->getType() == scene::ESNAT_FLY_STRAIGHT)
			{
				if(animator->hasFinished())
				{
					node->removeAnimator(animator);
					break;
				}
				else
					return false;
			}
		}
		core::vector3d<f32> position_start = node->getPosition();
		core::vector3d<f32> position_end(position_start.X, position_start.Y + levels[current_level].y, position_start.Z);
		scene::ISceneNodeAnimator* anim = device->getSceneManager()->createFlyStraightAnimator(position_start, position_end, int(levels[current_level].y / speed), false, true);
		node->addAnimator(anim);
		anim->drop();
		return true;
	}

	void collision_doors()
	{
		if(hero_node)
		{
			bool have_key;
			int index_level_goto = -1;
			int sound_index = -1;
			core::stringc key_sound_open("open");
			for(u32 i = 0; i < doors.size(); ++i)
			{
				object_door* d = doors[i];
				scene::IMeshSceneNode* node = d->get_node();
				if(node->getTransformedBoundingBox().intersectsWithBox(hero_node->getTransformedBoundingBox()))
				{
					sound_index = d->get_sound(key_sound_open);
					have_key = true;
					core::stringc key = d->get_key();
					if(!key.empty())
					{
						if(!has_key_in_hero_keys(hero_keys, key))
						{
							font->draw(_("Door require key") + " " + key, warning_position, warning_color);
							have_key = false;
						}
					}
					if(have_key)
					{
						if(d->get_level_goto() > -1)
						{
							index_level_goto = d->get_level_goto();
							break;
						}
						if(open_door(node, d->get_speed_open()))
						{
							if(sound_index > -1)
								event_sound_play(sound_index);
						}
					}
				}
			}
			if(index_level_goto > -1)
			{
				if(sound_index > -1)
					event_sound_play(sound_index);
				load_level(index_level_goto);
			}
		}
	}

	void move_enemy(scene::ISceneNode* node)
	{
		const core::list<scene::ISceneNodeAnimator*> animators = node->getAnimators();
		if(animators.size())
		{
			core::list<scene::ISceneNodeAnimator*>::ConstIterator it = animators.begin();
			for (; it != animators.end(); ++it)
			{
				if((*it)->getType() == scene::ESNAT_FLY_STRAIGHT)
				{
					if((*it)->hasFinished())
						node->removeAnimator(*it);
					else
						return;
				}
			}
		}
		core::vector3d<f32> position_start = camera_fps->getPosition();
		core::vector3d<f32> position_end = node->getPosition();
		core::line3d<f32> line(position_start, position_end);
		core::triangle3d<f32> triangle;
		scene::ISceneNode* outNode;
		if(!collision_manager->getCollisionPoint(line, meta, position_end, triangle, outNode))
			position_start = camera_fps->getPosition();
		f32 length = (position_end - position_start).getLength();
		f32 speed = 0.1f;
		position_start.Y = node->getPosition().Y;
		position_end.Y = position_start.Y;
		scene::ISceneNodeAnimator* animator = device->getSceneManager()->createFlyStraightAnimator(position_end, position_start, (u32)(length / speed));
		node->addAnimator(animator);
		animator->drop();
		scene::IMeshSceneNode* bullet = device->getSceneManager()->addSphereSceneNode((f32)levels[current_level].y / 16.0f, 16, 0, -1, position_end);
		bullet->getMaterial(0).EmissiveColor = video::SColor(255, 255, 0, 0);
		speed = 0.5f;
		animator = device->getSceneManager()->createFlyStraightAnimator(position_end, position_start, (u32)(length / speed));
		bullet->addAnimator(animator);
		animator->drop();
		enemy_bullets.push_back(bullet);
	}

	void collision_keys()
	{
		for(u32 i = 0; i < keys.size(); ++i)
		{
			scene::IBillboardSceneNode* node = keys[i];
			if(node)
			{
				if(node->getTransformedBoundingBox().intersectsWithBox(hero_node->getTransformedBoundingBox()))
				{
					hero_keys.push_back(get_new_texture(node->getMaterial(0).getTexture(0), node->getName()));
					keys.erase(i);
					node->removeAnimators();
					node->remove();
				}
			}
		}
	}

	void collision_enemies()
	{
		for(u32 i = 0; i < enemies.size(); ++i)
		{
			scene::IBillboardSceneNode* node = enemies[i];
			f32 distance = node->getPosition().getDistanceFrom(camera_fps->getPosition());
			if(distance < levels[current_level].y * 3 && distance > levels[current_level].y)
				move_enemy(node);
		}
	}

	void collision_enemy_bullets()
	{
		for(u32 i = 0; i < enemy_bullets.size(); ++i)
		{
			scene::ISceneNode* node = enemy_bullets[i];
			const core::list<scene::ISceneNodeAnimator*> animators = node->getAnimators();
			if(animators.size())
			{
				scene::ISceneNodeAnimator* animator = *(animators.begin());
				if(animator->hasFinished())
				{
					node->removeAnimator(animator);
					remove_node(node);
					enemy_bullets.erase(i);
				}
				else
				{
					if(node->getTransformedBoundingBox().intersectsWithBox(hero_node->getTransformedBoundingBox()))
					{
						health -= 1;
						node->removeAnimator(animator);
						remove_node(node);
						enemy_bullets.erase(i);
					}
				}
			}
		}
	}

	void create_bullet()
	{
		core::vector3d<f32> position_start = camera_fps->getPosition();
		core::vector3d<f32> position_end = camera_fps->getTarget() - position_start;
		position_end.normalize();
		position_start = position_start + position_end * (f32)(levels[current_level].y / 2);
		position_end = position_start + (position_end * camera_fps->getFarValue());
		core::line3d<f32> line(position_start, position_end);
		core::triangle3d<f32> triangle;
		scene::ISceneNode *outNode;
		if(!collision_manager->getCollisionPoint(line, meta, position_end, triangle, outNode))
		{
			position_start = camera_fps->getPosition();
			position_end = camera_fps->getTarget() - position_start;
			position_end.normalize();
			position_start = position_start + position_end * (f32)(levels[current_level].y / 2);
			position_end = position_start + (position_end * camera_fps->getFarValue());
		}
		scene::IMeshSceneNode* node = device->getSceneManager()->addSphereSceneNode((f32)(levels[current_level].y / 10), 16, 0, -1, position_start);
		node->getMaterial(0).EmissiveColor = video::SColor(255, 120, 100, 135);
		f32 length = (position_end - position_start).getLength();
		const f32 speed = 5.0f;
		u32 time = (u32)(length / speed);
		scene::ISceneNodeAnimator* anim = device->getSceneManager()->createFlyStraightAnimator(position_start, position_end, time);
		node->addAnimator(anim);
		anim->drop();
		bullets.push_back(node);
	}

	void collision_bullet(u32 bullet_index, scene::ISceneNode* bullet, core::array<scene::IBillboardSceneNode*>& nodes)
	{
		for(u32 i = 0; i < nodes.size(); ++i)
		{
			scene::IBillboardSceneNode* node_obj = nodes[i];
			if(node_obj)
			{
				if(node_obj->getTransformedBoundingBox().intersectsWithBox(bullet->getTransformedBoundingBox()))
				{
					meta->removeTriangleSelector(node_obj->getTriangleSelector());
					remove_node(node_obj);
					nodes.erase(i);
					break;
				}
			}
		}
	}

	void collision_bullets()
	{
		for(u32 i = 0; i < bullets.size(); ++i)
		{
			scene::IMeshSceneNode* bullet = bullets[i];
			const core::list<scene::ISceneNodeAnimator*> animators = bullet->getAnimators();
			if(!animators.empty())
			{
				scene::ISceneNodeAnimator* animator = *(animators.begin());
				if(animator->hasFinished())
				{
					remove_node(bullet);
					bullets.erase(i);
				}
				else
				{
					collision_bullet(i, bullet, enemies);
					collision_bullet(i, bullet, objects);
				}
			}
		}
	}

	void collision_friends()
	{
		if(!dialog_help)
		{
			for(u32 i = 0; i < friends.size(); ++i)
			{
				scene::IBillboardSceneNode* node = friends[i]->get_node();
				core::vector3d<f32> node_pos = node->getPosition();
				core::vector3d<f32> camera_pos = camera_fps->getPosition();
				if(node_pos.getDistanceFrom(camera_pos) < levels[current_level].y)
				{
					const wchar_t* msg = friends[i]->get_message(core::stringc("collision_first"));
					if(msg)
					{
						dialog_help = device->getGUIEnvironment()->addMessageBox(core::stringw(node->getName()).c_str(), msg);
						camera_fps->setPosition(camera_pos + (f32)levels[current_level].y);
						camera_fps->setTarget(node_pos);
						setActiveCameraOff();
					}
				}
			}
		}
	}

	void collision_weapons()
	{
		for(u32 i = 0; i < weapons.size(); ++i)
		{
			scene::IBillboardSceneNode* node = weapons[i];
			if(node->getTransformedBoundingBox().intersectsWithBox(hero_node->getTransformedBoundingBox()))
			{
				hero_weapons.push_back(get_new_texture(node->getMaterial(0).getTexture(0), node->getName()));
				weapons.erase(i);
				node->removeAnimators();
				node->remove();
				if(weapon_current < 0)
					weapon_current = i;
			}
		}
	}

	void draw_level()
	{
		device->getSceneManager()->drawAll();
		collision_keys();
		collision_doors();
		collision_enemies();
		collision_enemy_bullets();
		collision_bullets();
		collision_friends();
		collision_weapons();
	}

	bool start(int level_index = -1)
	{
		bool result = false;
		if(BASS_Init(-1, 44100, 0, 0, 0))
		{
			game_sound_play();
		}
		if(device)
		{
			s32 line_end = app_properties.description.findFirst(10);
			if(line_end < 0) line_end = 32;
			device->setWindowCaption(_(app_properties.description.subString(0, line_end)).c_str());
			device->setResizable(true);
			video_driver = device->getVideoDriver();
			gui::IGUISkin* skin = device->getGUIEnvironment()->getSkin();
			for (u32 i=0; i < gui::EGDC_COUNT; ++i)
			{
				video::SColor col = skin->getColor((gui::EGUI_DEFAULT_COLOR)i);
				col.setAlpha(200);
				skin->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
			}
			menu_height = skin->getSize(gui::EGDS_MENU_HEIGHT);
			create_logo();
			create_font();
			create_main_window();
			core::dimension2d<s32> mw_size = main_window->getClientRect().getSize();

			video_driver->setFog(fog_color, fog_type, fog_start, fog_end, fog_density, fog_pixel, fog_range);

			i_geometry_creator = device->getSceneManager()->getGeometryCreator();
			collision_manager = device->getSceneManager()->getSceneCollisionManager();

			camera_maya = device->getSceneManager()->addCameraSceneNodeMaya(0, -1500.f, 200.f, 1500.f, CAMERA_MAYA, 70.f, false);

			irr::SKeyMap keyMap[10];
			keyMap[0].Action = EKA_MOVE_FORWARD;
			keyMap[0].KeyCode = KEY_UP;
			keyMap[1].Action = EKA_MOVE_FORWARD;
			keyMap[1].KeyCode = KEY_KEY_W;
			keyMap[2].Action = EKA_MOVE_BACKWARD;
			keyMap[2].KeyCode = KEY_DOWN;
			keyMap[3].Action = EKA_MOVE_BACKWARD;
			keyMap[3].KeyCode = KEY_KEY_S;
			keyMap[4].Action = EKA_STRAFE_LEFT;
			keyMap[4].KeyCode = KEY_LEFT;
			keyMap[5].Action = EKA_STRAFE_LEFT;
			keyMap[5].KeyCode = KEY_KEY_A;
			keyMap[6].Action = EKA_STRAFE_RIGHT;
			keyMap[6].KeyCode = KEY_RIGHT;
			keyMap[7].Action = EKA_STRAFE_RIGHT;
			keyMap[7].KeyCode = KEY_KEY_D;
			keyMap[8].Action = EKA_JUMP_UP;
			keyMap[8].KeyCode = KEY_KEY_J;
			keyMap[9].Action = EKA_CROUCH;
			keyMap[9].KeyCode = KEY_KEY_C;
			camera_fps = device->getSceneManager()->addCameraSceneNodeFPS(0, 100.0f, 0.5f, CAMERA_FPS, keyMap, 10, false, 5);
			camera_fps->setFarValue(9999.0f);

			meta = device->getSceneManager()->createMetaTriangleSelector();
			scene::ISceneNodeAnimatorCollisionResponse* camera_animator = device->getSceneManager()->createCollisionResponseAnimator(meta, camera_fps);
			camera_fps->addAnimator(camera_animator);
			camera_animator->drop();

			cursor_control = device->getCursorControl();

			video::SColor back_scolor(255, 100, 100, 140);

			if(level_index > -1)
				load_level(level_index);

			UserIEventReceiver* event_receiver = new UserIEventReceiver(*this);
			if(event_receiver)
				device->setEventReceiver(event_receiver);

			while(device->run() && !global_exit_from_app)
			{
				if(device->isWindowActive())
				{
					if(video_driver->beginScene(true, true, back_scolor))
					{
						if(get_run_level())
						{
							draw_level();
							draw_weapon();
							if(hero_screen_show)
								draw_hero_screen();
						}
						else
							draw_logo();
						if(device_parameters.WindowSize != video_driver->getScreenSize())
						{
							device_parameters.WindowSize = video_driver->getScreenSize();
							main_window->setRelativePosition(core::position2d<s32>((device_parameters.WindowSize.Width - mw_size.Width) / 2, (device_parameters.WindowSize.Height - mw_size.Height) / 2));
						}
						device->getGUIEnvironment()->drawAll();
						video_driver->endScene();
					}
				}
				else
					device->yield();
			}
			if(event_receiver)
				delete event_receiver;
			stop();
			result = true;
		}
		else
			cout << endl << "ERROR: Irrlicht device not created" << endl;
		return result;
	}

	void stop()
	{
		if(bass_handle_level)
			BASS_ChannelStop(bass_handle_level);
		if(bass_handle_game)
			BASS_ChannelStop(bass_handle_game);
		BASS_Free();
		if(device)
		{
			device->closeDevice();
			int w = 320, h = 240;
			if(device_parameters.WindowSize.Width > 320)
				w = (int)device_parameters.WindowSize.Width;
			if(device_parameters.WindowSize.Height > 240)
				h = (int)device_parameters.WindowSize.Height;
			config->set_int("window_width", w);
			config->set_int("window_height", h);
		}
	}

	~Game()
	{
		if(device)
			stop();
		if(config)
			delete config;
	}
};

class Application
{
	Game* game;

public:
	Application(const std::string& json_file = "project.json", int debug_level = 0)
	{
		game = new Game(json_file, debug_level);
	}
	bool run()
	{
		bool result = false;
		if(game)
			game->start();
		else
			cout << endl << "ERROR: Game not created" << endl;
		return result;
	}
	~Application()
	{
		if(game)
			delete game;
	}
};

#ifdef _IRR_WINDOWS_
#	pragma comment(lib, "Irrlicht.lib")
#	ifdef SUBSYSTEM_WINDOWS
#		pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#	else
#		pragma comment(linker, "/subsystem:console /ENTRY:mainCRTStartup")
#	endif
#endif

int main(int argc, char** argv)
{
	int debug_level = -1;
	setlocale(LC_ALL, "");
	
	Application* app = new Application("project.json");
	app->run();
	delete app;
	return 0;
}
