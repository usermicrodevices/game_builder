WORK_WITH_INI_FILE = False

IMAGE_HERO = 1
IMAGE_WALL = 2
IMAGE_DOOR = 3
IMAGE_KEY = 4
IMAGE_WEAPON = 5
IMAGE_AMMO = 6
IMAGE_HEALTH = 7
IMAGE_ENEMY = 8
IMAGE_FRIEND = 9
IMAGE_OBJECT = 10
IMAGE_TYPE_COUNT = 100

CAMERA_MAYA = -2
CAMERA_FPS = -3

GUI_ID_QUIT = 0x10000
GUI_ID_ABOUT = 0x10001
GUI_ID_LOAD = 0x10002
GUI_ID_SAVE = 0x10003
GUI_ID_OPTIONS = 0x10004
GUI_ID_CAMERA_NONE = 0x10005
GUI_ID_CAMERA_MAYA = 0x10006
GUI_ID_CAMERA_FPS = 0x10007
GUI_ID_LOG = 0x10008
GUI_ID_DRIVER = 0x10009

import sys
sys.path.append('lib')

import pyirrlicht as irr

GUI_ID_EDT_SOFTWARE = GUI_ID_DRIVER | irr.EDT_SOFTWARE
GUI_ID_EDT_BURNINGSVIDEO = GUI_ID_DRIVER | irr.EDT_BURNINGSVIDEO
GUI_ID_EDT_DIRECT3D9 = GUI_ID_DRIVER | irr.EDT_DIRECT3D9
GUI_ID_EDT_OPENGL = GUI_ID_DRIVER | irr.EDT_OPENGL

GUI_ID_WINDOW_OPTIONS = 0x1000E

GUI_ID_SOUND_ON_OFF = 0x1000F
GUI_ID_TEXTURE_FROM_FILE = 0x10010
GUI_ID_MENU_SOUND_VOLUME = 0x10011
GUI_ID_WINDOW_SOUND_VOLUME = 0x10012
GUI_ID_SOUND_VOLUME = 0x10013

GUI_ID_OPT_BACK_SOUND = 0x10100
GUI_ID_OPT_FULL_SCREEN = 0x10101
GUI_ID_OPT_HERO_SCREEN = 0x10102
GUI_ID_OPT_SAVE = 0x10103

common_codec = 'cp1251'

if 'debug_level' not in globals():
	debug_level = -1

import os, sys
if sys.hexversion >= 0x03000000:
	type_str = bytes
	type_unicode = str
else:
	type_str = str
	type_unicode = unicode

from locale import getdefaultlocale

try:
	import pybass
except:
	pybass = None
	print('++++ PYBASS module not accessible!!!')
music_formats = ('it', 'mo3', 'mod', 's3m', 'xm')

app_name = ''
if game_properties:
	app_name = game_properties['name']
else:
	app_name = os.path.basename(sys.argv[0].split('.')[0])

# simple language translator
default_locale = getdefaultlocale()[0]
translation_catalog = 'lang'
if not os.path.isdir(translation_catalog):
	os.mkdir(translation_catalog)
translation_catalog += '//'# only for python 2.4.4 version and isdir function
translation_file_name = translation_catalog + app_name + '_' + default_locale + '.lng'
if not os.path.exists(translation_file_name):
	default_locale = 'en_US'
	translation_file_name = translation_catalog + app_name + '_' + default_locale + '.lng'
if not os.path.exists(translation_file_name):
	f = open(translation_file_name, 'w')
	f.close()
translation_file = open(translation_file_name, 'r+')
translations = {}
for line in translation_file.readlines():
	if len(line.strip()) > 2 and line.find('='):
		key, value = line.split('=', 1)
		translations[key.strip()] = value.strip()
def _(source = ''):
	if not source in translations:
		try:
			translation_file.write('\n' + source + ' = ' + source + '\n')
			translation_file.flush()
		except:
			pass
		translations[source] = source
	try:
		return type_unicode(translations[source], common_codec)
	except:
		return translations[source]

class config(object):
	def __init__(self, *args, **kwargs):
		self.is_file = kwargs.pop('is_file', True)
		self.file_name = kwargs.pop('file_name', 'config.ini')
		self._items_ = {}
		self.load()
	def load(self):
		if self.is_file:
			if os.path.exists(self.file_name):
				for line in open(self.file_name, 'r').readlines():
					if len(line.strip()) > 2 and line.find('='):
						key, value = line.split('=', 1)
						self._items_[key.strip()] = eval(value.strip())
	def read_from_file(self):
		result = {}
		if self.is_file:
			if os.path.exists(self.file_name):
				for line in open(self.file_name, 'r').readlines():
					if len(line.strip()) > 2 and line.find('='):
						key, value = line.split('=', 1)
						result[key.strip()] = eval(value.strip())
		return result
	def save(self):
		if self.is_file:
			f = open(self.file_name, 'w')
			for key, value in self._items_.items():
				f.write(key + ' = ' + repr(value) + '\n')
			f.close()
	def items(self):
		return self._items_.items()
	def get(self, key, default = ''):
		if not key in self._items_:
			self._items_[key] = default
			if self.is_file:
				open(self.file_name, 'a').write(key + ' = ' + repr(default) + '\n')
		return self._items_[key]
	def get_bool(self, key, default = ''):
		return bool(self.get(key, default))
	def get_int(self, key, default = ''):
		return int(self.get(key, default))
	def get_long(self, key, default = ''):
		return long(self.get(key, default))
	def get_float(self, key, default = ''):
		return float(self.get(key, default))
	def set(self, key, value = ''):
		self._items_[key] = value
	def set_many(self, keys_values):
		if len(keys_values) > 0:
			for key, value in keys_values:
				self._items_[key] = value
	def keys(self):
		return self._items_.keys()
	def values(self):
		return self._items_.values()
	def has_key(self, key):
		return key in self._items_
	def __getitem__(self, key):
		if key in self._items_:
			return self._items_[key]
		else:
			return None
	def __setitem__(self, key, value):
		self._items_[key] = value
	def __delitem__(self, key):
		if key in self._items_:
			del self._items_[key]
	def __len__(self):
		return len(self._items_)
	def __contains__(self, key):
		return key in self._items_
	def close(self):
		if self.is_file:
			items_from_file = self.read_from_file()
			if self._items_.keys() != items_from_file.keys() or self._items_.values() != items_from_file.values():
				self.save()

class UserIEventReceiver(irr.IEventReceiver):
	game = None
	KeyIsDown = {}
	for key in range(irr.KEY_KEY_CODES_COUNT):
		KeyIsDown[key] = False
	def create_bullet(self):
		start = self.game.camera[1].getPosition()
		end = self.game.camera[1].getTarget() - start
		end.normalize()
		start = start + end * self.game.world_height / 2.0
		end = start + (end * self.game.camera[1].getFarValue())
		triangle = irr.triangle3df()
		line = irr.line3df(start, end)
		bullet = 'nohit'
		if self.game.collision_manager.getCollisionPnt(line, self.game.meta, end, triangle):# collides with wall
			bullet = 'hit'
		else:# doesnt collide with wall
			start = self.game.camera[1].getPosition()
			end = self.game.camera[1].getTarget() - start
			end.normalize()
			start = start + end * self.game.world_height / 2.0
			end = start + (end * self.game.camera[1].getFarValue())
		node = self.game.scene_manager.addSphereSceneNode(self.game.world_height / 16.0, position = start)
		node.getMaterial(0).EmissiveColor = irr.SColor(255, 120, 100, 135)
		node.setName('bullet: %s on %s' % (bullet, end))
		length = (end - start).getLength()
		speed = 5.0
		try:
			time = int(length / speed)
		except:
			time = 1
		anim = self.game.scene_manager.createFlyStraightAnimator(start, end, time)
		node.addAnimator(anim)
		anim.drop()
		del_anim = self.game.scene_manager.createDeleteAnimator(time)
		node.addAnimator(del_anim)
		return (node, del_anim)
	def OnEvent(self, evt):
		event = irr.SEvent(evt)
		if self.game.dialog_help:
			try:
				self.game.dialog_help.getID()
			except:
				self.game.dialog_help = None
				if self.game.run_level:
					self.game.cursor_control.setVisible(False)
					self.game.setActiveCamera(self.game.camera[1])
					self.game.shot_enable = True
				else:
					self.game.cursor_control.setVisible(True)
					self.game.setActiveCamera(self.game.camera[0])
					self.game.shot_enable = False
		event_type = event.EventType
		if event_type is irr.EET_KEY_INPUT_EVENT:
			self.KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown
			if irr.KEY_HELP < event.KeyInput.Key < irr.KEY_KEY_A:
				self.game.weapon_current = event.KeyInput.Key - irr.KEY_KEY_0
		elif event_type is irr.EET_MOUSE_INPUT_EVENT:
			mouse_event_type = event.MouseInput.EventType
			if mouse_event_type == irr.EMIE_LMOUSE_PRESSED_DOWN:
				self.game.weapon_current_texture += 1
			elif mouse_event_type == irr.EMIE_LMOUSE_LEFT_UP:
				if self.game.shot_enable and not self.game.dialog_help and not self.game.dialog_options:
					self.game.weapon_current_texture = 0
					self.game.append_bullet(self.create_bullet())
		elif event_type == irr.EET_GUI_EVENT:
			gui_event_type = event.GUIEvent.EventType
			caller = event.GUIEvent.Caller
			caller_id = caller.getID()
			if gui_event_type == irr.EGET_BUTTON_CLICKED:
				# MAIN DIALOG BUTTONS EVENTS
				if caller_id == GUI_ID_LOAD:
					self.game.load_level()
				elif caller_id == GUI_ID_ABOUT:
					self.game.show_about_dialog()
				elif caller_id == GUI_ID_OPTIONS:
					self.game.show_options_dialog()
				elif caller_id == GUI_ID_QUIT:
					self.KeyIsDown[irr.KEY_ESCAPE] = True
				# OPTIONS DIALOG BUTTONS EVENTS
				elif caller_id == GUI_ID_OPT_SAVE:
					#print('+++ DialogOptions GUI_ID_OPT_SAVE')
					# TODO: save options to ini file
					self.game.close_options_dialog()
			# CHECK IF OPTIONS DIALOG EVENT CLOSE
			elif gui_event_type == irr.EGET_ELEMENT_CLOSED:
				if self.game.dialog_options:
					self.game.close_options_dialog()
					#~ self.game.dialog_options = None
			# OPTIONS DIALOG CHECKBOXES EVENTS
			elif gui_event_type == irr.EGET_CHECKBOX_CHANGED:
				boolean_value = caller.as_IGUICheckBox().isChecked()
				if caller_id == GUI_ID_OPT_BACK_SOUND:
					if self.game.run_level:
						self.game.sound_on_off()
					else:
						self.game.play_sound = boolean_value
				elif caller_id == GUI_ID_OPT_FULL_SCREEN:
					self.game.full_screen = boolean_value
				elif caller_id == GUI_ID_OPT_HERO_SCREEN:
					self.game.hero_screen_show = boolean_value
			elif gui_event_type == irr.EGET_MESSAGEBOX_OK:
				self.game.dialog_help = None
				if self.game.run_level:
					self.game.cursor_control.setVisible(False)
					self.game.setActiveCamera(self.game.camera[1])
			elif gui_event_type == irr.EGET_SCROLL_BAR_CHANGED:
				scroll_bar = caller.as_IGUIScrollBar()
				if caller_id == GUI_ID_SOUND_VOLUME:
					self.game.set_sound_volume(scroll_bar.getPos())
			elif gui_event_type == irr.EGET_MENU_ITEM_SELECTED:
				self.game.dialog_help = None
				menu = caller.as_IGUIContextMenu()
				menu_id = menu.getItemCommandId(menu.getSelectedItem())
				if menu_id == GUI_ID_QUIT:
					self.KeyIsDown[irr.KEY_ESCAPE] = True
				elif menu_id == GUI_ID_CAMERA_NONE:
					self.game.setActiveCameraOff()
					self.game.cursor_control.setVisible(True)
				elif menu_id == GUI_ID_CAMERA_MAYA:
					self.game.setActiveCamera(self.game.camera[0])
					self.game.cursor_control.setVisible(True)
				elif menu_id == GUI_ID_CAMERA_FPS:
					self.game.setActiveCamera(self.game.camera[1])
					self.game.cursor_control.setVisible(False)
				elif menu_id == GUI_ID_EDT_SOFTWARE:
					self.game.set_device_type(irr.EDT_SOFTWARE)
				elif menu_id == GUI_ID_EDT_OPENGL:
					self.game.set_device_type(irr.EDT_OPENGL)
				elif menu_id == GUI_ID_EDT_DIRECT3D9:
					self.game.set_device_type(irr.EDT_DIRECT3D9)
				elif menu_id == GUI_ID_EDT_BURNINGSVIDEO:
					self.game.set_device_type(irr.EDT_BURNINGSVIDEO)
				elif menu_id == GUI_ID_SOUND_ON_OFF:
					self.game.sound_on_off()
				elif menu_id == GUI_ID_MENU_SOUND_VOLUME:
					self.game.create_sound_volume()
				elif menu_id == GUI_ID_LOAD:
					self.game.load_level()
				elif menu_id == GUI_ID_ABOUT:
					self.game.show_about_dialog()
		return False
	def IsKeyDown(self, keyCode):
		return self.KeyIsDown[keyCode]

class game:
	def __init__(self, *args, **kwargs):
		self.config_file_name = kwargs.pop('config_file_name', app_name + '.ini')
		self.config = config(file_name = self.config_file_name, is_file = WORK_WITH_INI_FILE)
		self.time_delay = self.config.get_int('time_delay', 2)
		self.play_sound = self.config.get_bool('play_sound', True)
		self.sound_volume = self.config.get_float('sound_volume', 0.3)
		self.sound_file = self.config.get('sound_parameters', 'sounds//jinbels.mod')
		self.bass_handle = None
		self.sound_playing = False
		self.menu_options = None
		if pybass:
			if not pybass.BASS_Init(-1, 44100, 0, 0, 0):
				print('+++ ERROR BASS_Init')
			#~ else:#set volume channel better then global volume
				#~ pybass.BASS_SetVolume(self.sound_volume)
		self.device_type = self.config.get_int('device_type', game_properties['driver_type'])#irr.EDT_OPENGL
		self.full_screen = self.config.get_bool('full_screen', game_properties['full_screen'])
		self.window_size = irr.vector2du(self.config.get_int('window_width', 640), self.config.get_int('window_height', 480))
		self.stencil_buffer = self.config.get_bool('stencil_buffer', game_properties['stencil_buffer'])
		self.vsync = self.config.get_bool('vsync', game_properties['vsync'])
		self.device_parameters = irr.SIrrlichtCreationParameters()
		self.device_parameters.DriverType = self.config.get_int('driver_type', game_properties['driver_type'])
		self.device_parameters.WindowSize = self.window_size
		self.device_parameters.Bits = self.config.get_int('bits', game_properties['bits'])
		self.device_parameters.AntiAlias = 4#irr.EAAM_LINE_SMOOTH
		self.device_parameters.WithAlphaChannel = True
		self.device = irr.createDeviceEx(self.device_parameters)
		self.menu_device_types = {}
		self.dialog_help = None
		self.fog_enable = self.config.get_bool('fog_enable', False)
		self.fog_color = irr.SColor(*self.config.get('fog_color', (0,255,255,255)))
		self.fog_type = self.config.get_int('fog_type', irr.EFT_FOG_LINEAR)
		self.fog_start = self.config.get_float('fog_start', 500.0)
		self.fog_end = self.config.get_float('fog_end', 1000.0)
		self.fog_density = self.config.get_float('fog_density', 0.01)
		self.fog_pixel = self.config.get_bool('fog_pixel', False)
		self.fog_range = self.config.get_bool('fog_range', False)
		# logo
		self.logo_file = 'graphics/irrlicht_logo.png'
		self.logo = None
		self.logo_size = None
		self.logo_rect = None
		# len tile
		self.tile_count = world_full_size
		# world height
		self.world_height = 200
		# world size
		self.world_size = (world_full_size[0] * self.world_height, world_full_size[1] * self.world_height)
		# create empty containers for objects: doors; keys; bullets and etc.
		self.empty_objects()
		# hero settings
		self.hero_screen_show = self.config.get_bool('hero_screen_show', True)
		self.hero_screen_height = 100
		self.hero_screen_line_width = 11
		self.hero_screen_color = irr.SColor(255, 100, 100, 255)
		self.hero_node = None
		# shot
		self.shot_enable = False
		# levels
		self.run_level = False
		self.dialog_options = None

	def drop_node(self, x):
		if x:
			x.removeAnimators()
			try:
				x.remove()
			except:
				pass
			#~ else:
				#~ x.drop()
			#~ x = None

	def empty_objects(self):
		self.floors = []
		self.ceilings = []
		self.walls = []
		self.doors = []
		self.keys = []
		self.bullets = []
		self.objects = []
		self.friends = []
		self.enemies = []
		self.enemy_bullets = []
		self.weapon_current_texture = 0
		self.weapon_current = -1
		self.weapons = []
		self.hero_weapons = {}
		self.hero_texture = None
		self.level_textures = {}
		self.hero_keys = {}
		self.health = 100

	def get_channel_info(self, handle):
		channel_info = pybass.BASS_CHANNELINFO()
		if not pybass.BASS_ChannelGetInfo(handle, channel_info):
			print('+++ BASS_ChannelGetInfo error %s' % pybass.get_error_description(pybass.BASS_ErrorGetCode()))
		return channel_info

	def get_type_handle(self, handle):
		return self.get_channel_info(handle).ctype

	def set_sound_volume(self, value):
		self.sound_volume = value / 100.0
		if pybass:
			#~ pybass.BASS_SetVolume(self.sound_volume)
			if self.bass_handle:#set volume channel better then global volume
				pybass.BASS_ChannelSetAttribute(self.bass_handle, pybass.BASS_ATTRIB_VOL, self.sound_volume)
		self.config.set('sound_volume', self.sound_volume)

	def create_sound_handle(self, file_path):
		handle = 0
		if not os.path.isfile(file_path):
			print('+++ %s not found' % file_path)
		else:
			sound_format = 'wav'
			if file_path.split('.') > 1:
				sound_format = file_path.split('.')[-1]
			if sound_format in music_formats:
				handle = pybass.BASS_MusicLoad(False, irr.type_str(file_path), 0, 0, pybass.BASS_MUSIC_PRESCAN | pybass.BASS_SAMPLE_LOOP, 0)
			else:
				handle = pybass.BASS_StreamCreateFile(False, irr.type_str(file_path), 0, 0, 0)
			if not handle:
				if sound_format in music_formats:
					print('+++ BASS_MusicLoad %s' % pybass.get_error_description(pybass.BASS_ErrorGetCode()))
				else:
					print('+++ BASS_StreamCreateFile %s' % pybass.get_error_description(pybass.BASS_ErrorGetCode()))
				print('+++ sound file is %s' % file_path)
			else:
				pybass.BASS_ChannelSetAttribute(handle, pybass.BASS_ATTRIB_VOL, self.sound_volume)
		return handle

	def create_sound_volume(self):
		e = self.guienv.getRootGUIElement().getElementFromId(GUI_ID_WINDOW_SOUND_VOLUME, True)
		if e:
			e.remove()
		window = self.guienv.addWindow(irr.recti(10,45,400,150), False, _('Sound volume window'), id = GUI_ID_WINDOW_SOUND_VOLUME)
		self.guienv.addStaticText(_('Select sound volume'), irr.recti(10,30,380,60), True, False, window)
		scrollbar = self.guienv.addScrollBar(True, irr.recti(10,80,380,100), window, GUI_ID_SOUND_VOLUME)
		scrollbar.setPos(int(self.sound_volume * 100))

	def event_sound_play(self, index):
		if pybass:
			handle = self.create_sound_handle(self.get_file_name(sounds.get(index, '')))
			if handle:
				sound_playing = pybass.BASS_ChannelPlay(handle, False)
				#~ pybass.BASS_ChannelSlideAttribute(handle, pybass.BASS_ATTRIB_VOL, 0, 1000)#Fadeout a channel's volume over a period of 1 second

	def level_sound_play(self, file_path = None):
		if pybass:
			if not self.bass_handle:
				self.bass_handle = self.create_sound_handle(file_path)
			if self.bass_handle:
				self.sound_playing = pybass.BASS_ChannelPlay(self.bass_handle, False)

	def level_sound_stop(self, close_handle = False):
		self.sound_playing = False
		if self.bass_handle:
			pybass.BASS_ChannelStop(self.bass_handle)
			if close_handle:
				type_handle = self.get_type_handle(self.bass_handle)
				if type_handle >= pybass.BASS_CTYPE_MUSIC_MOD:
					if not pybass.BASS_MusicFree(self.bass_handle):
						print('+++ BASS_MusicFree error %s' % pybass.get_error_description(pybass.BASS_ErrorGetCode()))
					else:
						self.bass_handle = 0
				elif type_handle >= pybass.BASS_CTYPE_STREAM:
					if not pybass.BASS_StreamFree(self.bass_handle):
						print('+++ BASS_StreamFree error %s' % pybass.get_error_description(pybass.BASS_ErrorGetCode()))
					else:
						self.bass_handle = 0

	def sound_on_off(self):
		if self.play_sound:
			self.play_sound = False
			if self.bass_handle and self.sound_playing:
				self.sound_playing = False
				pybass.BASS_ChannelStop(self.bass_handle)
		else:
			self.play_sound = True
			self.level_sound_play()
		if self.menu_options:
			self.menu_options.setItemChecked(self.menu_sound, self.play_sound)
		self.config.set('play_sound', self.play_sound)

	def texture_generator(self, image_format = irr.ECF_R8G8B8, image_size = irr.dimension2du(2, 2), texture_name = 'texture_01', alpha_value = 128, red = (0, 255), green = (0, 255), blue = (0, 255)):
		return irr.generate_texture(self.video_driver, image_format, image_size, texture_name, alpha_value, red, green, blue)

	def create_wall_plane_selector(self, tileSize, tileCount, material, textureRepeatCount, pos = None, rotation = None):
		i_mesh = self.i_geometry_creator.createPlaneMesh(tileSize, tileCount, material, textureRepeatCount)
		i_mesh_scene_node = self.scene_manager.addMeshSceneNode(i_mesh)
		if pos:
			i_mesh_scene_node.setPosition(pos)
		if rotation:
			i_mesh_scene_node.setRotation(rotation)
		selector = self.scene_manager.createTriangleSelector(i_mesh, i_mesh_scene_node)
		i_mesh_scene_node.setTriangleSelector(selector)
		return selector

	def set_cube_texture(self, node, color_is_green = False, texture_name = 'cube'):
		material = node.getMaterial(0)
		material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
		if color_is_green:
			texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(4, 4), texture_name, 128, (0, 0), (100, 255), (0, 0))
		else:
			texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(4, 4), texture_name, 128, (100, 255), (0, 0), (0, 0))
		material.setTexture(0, texture)
		material.BackfaceCulling = False
		material.FogEnable = self.fog_enable
		material.AmbientColor = irr.SColor(128, 255, 0, 0)
		material.DiffuseColor = irr.SColor(128, 255, 0, 0)
		material.EmissiveColor = irr.SColor(128, 255, 0, 0)
		material.SpecularColor = irr.SColor(128, 255, 0, 0)

	def get_file_name(self, file_name):
		if sys.hexversion < 0x03000000:
			if isinstance(file_name, irr.type_unicode):
				return file_name.encode(common_codec)
			else:
				return file_name
		else:
			return file_name

	def show_warning(self):
		self.guienv.addMessageBox(_('Warning'), _('For finish this operation you need restart game!'))

	def set_device_type(self, new_device_type = 0):
		self.device_type = new_device_type
		for dev_type, menu_index in self.menu_device_types.items():
			self.menu_device_type.setItemChecked(menu_index, (self.device_type == dev_type))
		self.config.set('device_type', self.device_type)
		self.show_warning()

	def setActiveCamera(self, camera):
		if self.device and camera:
			active = self.scene_manager.getActiveCamera()
			if active:
				active.setInputReceiverEnabled(False)
			camera.setInputReceiverEnabled(True)
			self.scene_manager.setActiveCamera(camera)

	def setActiveCameraOff(self):
		self.cursor_control.setVisible(True)
		cam = self.scene_manager.getActiveCamera()
		if cam:
			cam.setInputReceiverEnabled(not cam.isInputReceiverEnabled())

	def create_font(self):
		self.font_size = 24
		font_ext = '.ttf'
		font_path = os.environ['SYSTEMROOT']+'/Fonts/'
		self.font_file = font_path + 'arial' + font_ext
		self.font = irr.CGUITTFont(self.guienv, self.font_file, self.font_size)
		if self.font:
			self.skin.setFont(self.font)
			self.font.drop()
		else:
			print('++++ ERROR vect_font not created !!!')

	def show_about_dialog(self):
		self.guienv.addMessageBox(_('About'), game_properties['description'])

	def show_help_dialog(self):
		self.setActiveCameraOff()
		self.dialog_help = self.guienv.addMessageBox(_('Help'), _('F2 - options; ESC - exit'))

	def show_options_dialog(self):
		if not self.dialog_options:
			self.dialog_options = self.guienv.addWindow(irr.recti(5,5,640,480), True, _('Options'), self.guienv.getRootGUIElement(), GUI_ID_WINDOW_OPTIONS)
			self.guienv.addCheckBox(self.play_sound, irr.recti(10,30,500,60), self.dialog_options, GUI_ID_OPT_BACK_SOUND, _('Enable background sound'))
			self.guienv.addCheckBox(self.full_screen, irr.recti(10,80,500,110), self.dialog_options, GUI_ID_OPT_FULL_SCREEN, _('Enable full screen'))
			self.guienv.addCheckBox(self.hero_screen_show, irr.recti(10,130,500,160), self.dialog_options, GUI_ID_OPT_HERO_SCREEN, _('Enable hero screen'))
			self.guienv.addStaticText(_('Select sound volume'), irr.recti(10,180,500,210), False, False, self.dialog_options)
			scrollbar = self.guienv.addScrollBar(True, irr.recti(10,220,500,240), self.dialog_options, GUI_ID_SOUND_VOLUME)
			scrollbar.setPos(int(self.sound_volume * 100))
			self.guienv.addButton(irr.recti(10,440,200,470), self.dialog_options, GUI_ID_OPT_SAVE, _('Save to file'))
		else:
			if not self.dialog_options.isVisible():
				self.dialog_options.setVisible(True)
		self.cursor_control.setVisible(True)

	def close_options_dialog(self):
		if self.dialog_options:
			self.dialog_options.remove()
			self.dialog_options = None
			if self.run_level:
				self.cursor_control.setVisible(False)

	def create_top_menu(self):
		self.menu = self.guienv.addMenu()
		self.menu.addItem(_('File'), -1, True, True)
		self.menu.addItem(_('View'), -1, True, True)
		self.menu.addItem(_('Options'), -1, True, True)
		self.menu.addItem(_('Help'), -1, True, True)

		submenu = self.menu.getSubMenu(0)
		submenu.addItem(_('Load game'), GUI_ID_LOAD)
		submenu.addItem(_('Save game'), GUI_ID_SAVE)
		submenu.addSeparator()
		submenu.addItem(_('Quit'), GUI_ID_QUIT)

		submenu = self.menu.getSubMenu(1)
		submenu.addItem(_('Camera off'), GUI_ID_CAMERA_NONE)
		submenu.addItem(_('Perspective view'), GUI_ID_CAMERA_MAYA)
		submenu.addItem(_('First Person view'), GUI_ID_CAMERA_FPS)

		self.menu_options = self.menu.getSubMenu(2)
		self.menu_options.addItem(_('Start/stop log'), GUI_ID_LOG)
		self.menu_options.addItem(_('Choose graphics driver'), GUI_ID_DRIVER, True, True)
		self.menu_device_type = self.menu_options.getSubMenu(1)
		self.menu_device_types[irr.EDT_SOFTWARE] = self.menu_device_type.addItem(_('Software'), GUI_ID_EDT_SOFTWARE, checked = (self.device_type == irr.EDT_SOFTWARE))
		self.menu_device_types[irr.EDT_OPENGL] = self.menu_device_type.addItem(_('OpenGL'), GUI_ID_EDT_OPENGL, checked = (self.device_type == irr.EDT_OPENGL))
		self.menu_device_types[irr.EDT_DIRECT3D9] = self.menu_device_type.addItem(_('DirectX 9'), GUI_ID_EDT_DIRECT3D9, checked = (self.device_type == irr.EDT_DIRECT3D9))
		self.menu_device_types[irr.EDT_BURNINGSVIDEO] = self.menu_device_type.addItem(_('Burningsvideo'), GUI_ID_EDT_BURNINGSVIDEO, checked = (self.device_type == irr.EDT_BURNINGSVIDEO))
		self.menu_sound = self.menu_options.addItem(_('Sound on/off'), GUI_ID_SOUND_ON_OFF, checked = self.play_sound)
		self.menu_sound_volume = self.menu_options.addItem(_('Sound volume'), GUI_ID_MENU_SOUND_VOLUME)

		submenu = self.menu.getSubMenu(3)
		submenu.addItem(_('About'), GUI_ID_ABOUT)

	def create_logo(self):
		if os.path.isfile(self.logo_file):
			self.logo = self.video_driver.getTexture(self.logo_file)
			if self.logo:
				self.logo_size = self.logo.getOriginalSize()
				self.logo_rect = irr.recti(0, 0, self.logo_size.X, self.logo_size.Y)

	def create_main_window(self, width = -1, height = -1):
		if width < 0:
			width = 200
		if height < 0:
			height = self.font_size * 16
		x1 = (self.window_size.X - width) / 2
		y1 = (self.window_size.Y - height) / 2
		x2 = self.window_size.X - x1
		y2 = self.window_size.Y - y1
		mw_rect = irr.recti(x1, y1, x2, y2)
		main_window = self.guienv.addWindow(mw_rect, False, _('Main menu'))
		main_window.getCloseButton().setVisible(False)
		main_window.setDraggable(False)
		main_window.setDrawTitlebar(False)
		main_window.setTabStop(True)
		x = 10
		y = self.font_size
		w = width - x
		h = y * 2
		d = h / 2
		self.guienv.addButton(irr.recti(x, y, w, y + h), main_window, GUI_ID_LOAD, _('Start game'), _('Start new game')).setTabStop(True)
		y += h + d
		self.guienv.addButton(irr.recti(x, y, w, y + h), main_window, GUI_ID_SAVE, _('Save game'), _('Save game to file')).setTabStop(True)
		y += h + d
		self.guienv.addButton(irr.recti(x, y, w, y + h), main_window, GUI_ID_OPTIONS, _('Options'), _('Setup game settings')).setTabStop(True)
		y += h + d
		self.guienv.addButton(irr.recti(x, y, w, y + h), main_window, GUI_ID_ABOUT, _('About'), _('Game description')).setTabStop(True)
		y += h + d
		self.guienv.addButton(irr.recti(x, y, w, y + h), main_window, GUI_ID_QUIT, _('Quit'), _('Exit from game')).setTabStop(True)
		return main_window, width, height

	def load_level(self, index = 0):
		if index not in levels:
			self.clear_level()
			self.run_level = False
			self.shot_enable = False
			self.cursor_control.setVisible(True)
			self.main_window.setVisible(True)
			return

		self.tile_count = (levels[index]['properties']['x'], levels[index]['properties']['y'])
		self.world_height = levels[index]['properties']['height']
		self.world_size = (levels[index]['properties']['x'] * self.world_height, levels[index]['properties']['y'] * self.world_height)
		self.cursor_control.setVisible(False)

		# texture from file or dynamic generator
		texture = None

		file_name = 'graphics//skydome.jpg'
		if os.path.isfile(file_name):
			texture = self.video_driver.getTexture(file_name)
		else:
			texture = self.texture_generator(irr.ECF_R8G8B8, irr.dimension2du(64, 64), 'skydome', 0, (150, 200), (150, 200), (0, 0))
		sky_node = self.scene_manager.addSkyDomeSceneNode(texture)

		# top and bottom plane
		tileSize = irr.dimension2df(self.world_height, self.world_height)
		tileCount = irr.dimension2du(self.tile_count[0], self.tile_count[1])
		textureRepeatCount = irr.dimension2df(self.tile_count[0], self.tile_count[1])
		material = irr.SMaterial()
		material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
		material.FogEnable = self.fog_enable
		material.EmissiveColor = irr.SColor(128, 255, 255, 255)
		use_floor_plane = levels[index]['properties']['use_floor_plane']
		if use_floor_plane:
			file_name = 'graphics//stones.jpg'
			if os.path.isfile(file_name):
				material.setTexture(0, self.video_driver.getTexture(file_name))
			else:
				texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(2, 2), 'bottom', 0)
				material.setTexture(0, texture)
			i_mesh_bottom = self.i_geometry_creator.createPlaneMesh(tileSize, tileCount, material, textureRepeatCount)
			i_mesh_scene_node_bottom = self.scene_manager.addOctreeSceneNode(i_mesh_bottom)
			i_mesh_scene_node_bottom.setPosition(irr.vector3df(self.world_size[0]/2, 0, self.world_size[1]/2))
			selector_bottom = self.scene_manager.createOctreeTriangleSelector(i_mesh_bottom, i_mesh_scene_node_bottom)
			i_mesh_scene_node_bottom.setTriangleSelector(selector_bottom)
			self.meta.addTriangleSelector(selector_bottom)
		if levels[index]['properties']['use_ceiling_plane']:
			file_name = 'graphics//opengllogo.png'
			if self.device_type == irr.EDT_SOFTWARE:
				file_name = 'graphics//irrlichtlogo3.png'
			elif self.device_type == irr.EDT_BURNINGSVIDEO:
				file_name = 'graphics//burninglogo.png'
			elif self.device_type in (irr.EDT_DIRECT3D8, irr.EDT_DIRECT3D9):
				file_name = 'graphics//directxlogo.png'
			if os.path.isfile(file_name):
				material.setTexture(0, self.video_driver.getTexture(file_name))
			else:
				texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(2, 2), 'top', 0)
				material.setTexture(0, texture)
			i_mesh_top = self.i_geometry_creator.createPlaneMesh(tileSize, tileCount, material, textureRepeatCount)
			i_mesh_scene_node_top = self.scene_manager.addOctreeSceneNode(i_mesh_top)
			i_mesh_scene_node_top.setPosition(irr.vector3df(self.world_size[0]/2 ,self.world_height, self.world_size[1]/2))
			i_mesh_scene_node_top.setRotation(irr.vector3df(180, 0, 0))
			selector_top = self.scene_manager.createOctreeTriangleSelector(i_mesh_top, i_mesh_scene_node_top)
			i_mesh_scene_node_top.setTriangleSelector(selector_top)
			self.meta.addTriangleSelector(selector_top)

		# left, right, front and back finish walls
		if levels[index]['properties']['use_border_planes']:
			x = self.world_size[0]
			z = self.world_size[1]
			tileCountX = irr.dimension2du(self.tile_count[1], 1)
			textureRepeatCountX = irr.dimension2df(self.tile_count[1], 1)
			tileCountZ = irr.dimension2du(self.tile_count[0], 1)
			textureRepeatCountZ = irr.dimension2df(self.tile_count[0], 1)
			# front material, plane and selector
			material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
			texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(4, 4), 'front', 196, (250, 255), (0, 0), (0, 0))
			material.setTexture(0, texture)
			selector_left = self.create_wall_plane_selector(tileSize, tileCountX, material, textureRepeatCountX, irr.vector3df(0, self.world_height / 2, z / 2), irr.vector3df(90,90,0))
			# back material, plane and selector
			material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
			texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(4, 4), 'back', 196, (0, 0), (0, 0), (250, 255))
			material.setTexture(0, texture)
			selector_right = self.create_wall_plane_selector(tileSize, tileCountX, material, textureRepeatCountX, irr.vector3df(x, self.world_height / 2, z / 2), irr.vector3df(90,-90,0))
			# right material, plane and selector
			material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
			texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(4, 4), 'right', 196, (0, 0), (250, 255), (0, 0))
			material.setTexture(0, texture)
			selector_front = self.create_wall_plane_selector(tileSize, tileCountZ, material, textureRepeatCountZ, irr.vector3df(x / 2, self.world_height / 2, z), irr.vector3df(-90,0,0))
			# left material, plane and selector
			material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
			texture = self.texture_generator(irr.ECF_A8R8G8B8, irr.dimension2du(4, 4), 'left', 196, (250, 255), (250, 255), (0, 0))
			material.setTexture(0, texture)
			selector_back = self.create_wall_plane_selector(tileSize, tileCountZ, material, textureRepeatCountZ, irr.vector3df(x / 2, self.world_height / 2, 0), irr.vector3df(90,0,0))
			self.meta.addTriangleSelector(selector_front)
			self.meta.addTriangleSelector(selector_back)
			self.meta.addTriangleSelector(selector_left)
			self.meta.addTriangleSelector(selector_right)

		# ADD FLOOR CUBES
		for coord, image in levels[index]['data_floors'].iteritems():
			cube = self.scene_manager.addCubeSceneNode(self.world_height * 1.0, position = irr.vector3df(coord[0] * self.world_height, -self.world_height/2, coord[1] * self.world_height))
			if cube:
				if not use_floor_plane:
					self.meta.addTriangleSelector(self.scene_manager.createTriangleSelectorFromBoundingBox(cube))
				material = cube.getMaterial(0)
				material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
				cube.setMaterialFlag(irr.EMF_LIGHTING, False)
				image_file_name = self.get_file_name(images.get(image[0], ''))
				if os.path.isfile(image_file_name):
					if not image_file_name in self.level_textures:
						self.level_textures[image_file_name] = self.video_driver.getTexture(image_file_name)
					cube.setMaterialTexture(0, self.level_textures[image_file_name])
				else:
					self.set_cube_texture(cube)
				# ANIMATION IDLE
				if len(image[9]):
					textures = irr.array(irr.ITexture)
					for aindex in image[9]:
						anim_image = self.get_file_name(images.get(aindex, ''))
						if not anim_image in self.level_textures:
							self.level_textures[anim_image] = self.video_driver.getTexture(anim_image)
						textures.push_back(self.level_textures[anim_image])
					aidle = self.scene_manager.createTextureAnimator(textures, image[8])
					cube.addAnimator(aidle)
					aidle.drop()
				self.floors.append(cube)

		# ADD CEILING CUBES
		for coord, image in levels[index]['data_ceilings'].iteritems():
			cube = self.scene_manager.addCubeSceneNode(self.world_height * 1.0, position = irr.vector3df(coord[0] * self.world_height, self.world_height * 1.5, coord[1] * self.world_height))
			if cube:
				material = cube.getMaterial(0)
				material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
				cube.setMaterialFlag(irr.EMF_LIGHTING, False)
				image_file_name = self.get_file_name(images.get(image[0], ''))
				if os.path.isfile(image_file_name):
					if not image_file_name in self.level_textures:
						self.level_textures[image_file_name] = self.video_driver.getTexture(image_file_name)
					cube.setMaterialTexture(0, self.level_textures[image_file_name])
				else:
					self.set_cube_texture(cube)
				# ANIMATION IDLE
				if len(image[9]):
					textures = irr.array(irr.ITexture)
					for aindex in image[9]:
						anim_image = self.get_file_name(images.get(aindex, ''))
						if not anim_image in self.level_textures:
							self.level_textures[anim_image] = self.video_driver.getTexture(anim_image)
						textures.push_back(self.level_textures[anim_image])
					aidle = self.scene_manager.createTextureAnimator(textures, image[8])
					cube.addAnimator(aidle)
					aidle.drop()
				self.ceilings.append(cube)

		# HERO PARAMETERS
		camera_start_pos = irr.vector3df(self.world_height * 2, self.world_height, self.world_height * 2)
		self.hero_rect_size = self.hero_screen_height - self.hero_screen_line_width - self.hero_screen_line_width / 2
		self.hero_source_rect = irr.recti(0, 0, self.hero_rect_size, self.hero_rect_size)
		self.hero_node = self.scene_manager.addCubeSceneNode(self.world_height / 2.0, self.camera[1])

		# ADD WALL CUBES AND OTHER OBJECTS
		speed_open_door = 0.2
		id_obj = IMAGE_TYPE_COUNT
		id_key = IMAGE_TYPE_COUNT
		id_door = IMAGE_TYPE_COUNT
		id_weapon = IMAGE_TYPE_COUNT
		for coord, image in levels[index]['data_objects'].iteritems():
			node = None
			image_file_name = self.get_file_name(images.get(image[0], ''))
			if image[1] == IMAGE_HERO:
				camera_start_pos = irr.vector3df(coord[0] * self.world_height, self.world_height / 2, coord[1] * self.world_height)
				if os.path.isfile(image_file_name):
					self.hero_texture = self.video_driver.getTexture(image_file_name)
					if self.hero_texture:
						size = self.hero_texture.getOriginalSize()
						self.hero_source_rect = irr.recti(0, 0, size.X, size.Y)
			elif image[1] == IMAGE_WALL:
				node = self.scene_manager.addCubeSceneNode(self.world_height * 1.0, position = irr.vector3df(coord[0] * self.world_height, self.world_height / 2, coord[1] * self.world_height))
				if node:
					material = node.getMaterial(0)
					material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
					node.setMaterialFlag(irr.EMF_LIGHTING, False)
					if os.path.isfile(image_file_name):
						if not image_file_name in self.level_textures:
							self.level_textures[image_file_name] = self.video_driver.getTexture(image_file_name)
						node.setMaterialTexture(0, self.level_textures[image_file_name])
					else:
						self.set_cube_texture(node)
					self.meta.addTriangleSelector(self.scene_manager.createTriangleSelectorFromBoundingBox(node))
					self.walls.append(node)
			elif image[1] in (IMAGE_KEY, IMAGE_WEAPON, IMAGE_AMMO, IMAGE_HEALTH, IMAGE_ENEMY, IMAGE_FRIEND, IMAGE_OBJECT):
				if not os.path.isfile(image_file_name):
					print('--- FILE NOT FOUND %s' % image_file_name)
				else:
					billboard_scene_node_size = irr.dimension2df(self.world_height * 1.0, self.world_height * 1.0)
					if image[5]:
						scale = eval(image[5])
						if isinstance(scale, float):
							billboard_scene_node_size = irr.dimension2df(self.world_height * scale, self.world_height * scale)
					billboard_scene_node_position = irr.vector3df(coord[0] * self.world_height, self.world_height / 2, coord[1] * self.world_height)
					if image[6]:
						pos_y = eval(image[6])
						if isinstance(pos_y, (int, float)):
							billboard_scene_node_position = irr.vector3df(coord[0] * self.world_height, pos_y, coord[1] * self.world_height)
					node = self.scene_manager.addBillboardSceneNode(self.scene_manager.getRootSceneNode(), billboard_scene_node_size, billboard_scene_node_position, image[1])
					if node:
						material = node.getMaterial(0)
						material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
						node.setMaterialFlag(irr.EMF_LIGHTING, False)
						node.setName(image[2])
						if not image_file_name in self.level_textures:
							self.level_textures[image_file_name] = self.video_driver.getTexture(image_file_name)
						node.setMaterialTexture(0, self.level_textures[image_file_name])
						if image[1] == IMAGE_KEY:
							if not image[2]:
								node.setName('key_%d'%id_key)
								id_key += 1
							self.keys.append((node, self.level_textures[image_file_name]))
						elif image[1] == IMAGE_WEAPON:
							if not image[2]:
								node.setName('weapon_%d'%id_weapon)
								id_weapon += 1
							weapon_textures = [self.level_textures[image_file_name]]
							if len(image[17]):# ANIMATION SHOT
								for aindex in image[17]:
									anim_image = self.get_file_name(images.get(aindex, ''))
									if not anim_image in self.level_textures:
										self.level_textures[anim_image] = self.video_driver.getTexture(anim_image)
									weapon_textures.append(self.level_textures[anim_image])
							self.weapons.append((node, image[4] if image[4] > -1 else 0, weapon_textures))
						elif image[1] in (IMAGE_ENEMY, IMAGE_FRIEND, IMAGE_OBJECT):
							node_selector = self.scene_manager.createTriangleSelectorFromBoundingBox(node)
							node.setTriangleSelector(node_selector)
							self.meta.addTriangleSelector(node_selector)
							#~ node_selector.drop()
							if image[1] == IMAGE_ENEMY:
								self.enemies.append(node)
							elif image[1] == IMAGE_FRIEND:
								self.friends.append([node, image[18], image[19]])#, False])
							elif image[1] == IMAGE_OBJECT:
								self.objects.append(node)
			elif image[1] == IMAGE_DOOR:
				node_pos = irr.vector3df(coord[0] * self.world_height, self.world_height / 2, coord[1] * self.world_height)
				node = self.scene_manager.addCubeSceneNode(self.world_height * 1.0, id = id_door, position = node_pos)
				if node:
					node.setName('door_%d_%s' % (id_door, image[2]))
					id_door += 1
					material = node.getMaterial(0)
					material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
					node.setMaterialFlag(irr.EMF_LIGHTING, False)
					if os.path.isfile(image_file_name):
						if not image_file_name in self.level_textures:
							self.level_textures[image_file_name] = self.video_driver.getTexture(image_file_name)
						node.setMaterialTexture(0, self.level_textures[image_file_name])
					else:
						self.set_cube_texture(node)
					self.meta.addTriangleSelector(self.scene_manager.createTriangleSelectorFromBoundingBox(node))
					self.doors.append([node, speed_open_door, image[3], image[4], image[18]])#[door, speed_open_door, key, level_goto, sounds]
			if node:
				if image[5]:
					scale = eval(image[5])
					if isinstance(scale, float):
						node.setScale(irr.vector3df(scale, scale, scale))
				# ANIMATION IDLE
				if len(image[9]):
					textures = irr.array(irr.ITexture)
					for aindex in image[9]:
						anim_image = self.get_file_name(images.get(aindex, ''))
						if not anim_image in self.level_textures:
							self.level_textures[anim_image] = self.video_driver.getTexture(anim_image)
						textures.push_back(self.level_textures[anim_image])
					aidle = self.scene_manager.createTextureAnimator(textures, image[8])
					node.addAnimator(aidle)
					aidle.drop()

		# ADD LIGHT
		for coord, image in levels[index]['data_lights'].iteritems():
			node = self.scene_manager.addVolumeLightSceneNode(0, -1, 32, 32, irr.SColor(0, 255, 255, 255), irr.SColor(0, 0, 0, 0), position = irr.vector3df(coord[0] * self.world_height, self.world_height / 2, coord[1] * self.world_height), scale = irr.vector3df(100.0, 1.0, 100.0))
			if node:
				material = node.getMaterial(0)
				material.MaterialType = irr.EMT_TRANSPARENT_ALPHA_CHANNEL
				node.setMaterialFlag(irr.EMF_LIGHTING, False)
				image_file_name = self.get_file_name(images.get(image[0], ''))
				if os.path.isfile(image_file_name):
					if not image_file_name in self.level_textures:
						self.level_textures[image_file_name] = self.video_driver.getTexture(image_file_name)
					node.setMaterialTexture(0, self.level_textures[image_file_name])

		# SETUP CAMERA
		self.camera[0].setTarget(irr.vector3df(0, 600, 0))
		self.setActiveCamera(self.camera[1])
		self.camera[1].setPosition(camera_start_pos)
		# for valid positioning camera we must setup target for collision response animator
		animators = self.camera[1].getAnimators()
		for i in range(animators.size()):
			animator = animators[i]
			if animator.getType() == irr.ESNAT_COLLISION_RESPONSE:
				irr.ISceneNodeAnimatorCollisionResponse(animator).setTargetNode(self.camera[1])
				break

		self.warning_position = irr.recti(10, self.menu_height + self.font_size, 0, 0)
		self.warning_color = irr.SColor(255, 255, 255, 255)

		#~ self.menu.setVisible(False)
		self.main_window.setVisible(False)
		self.run_level = True
		self.shot_enable = True

		if self.play_sound:
			self.level_sound_play(levels[index]['properties']['sound'])

	def main_window_show(self):
		self.main_window.setVisible(True)
		self.guienv.setFocus(self.main_window)

	def draw_logo(self):
		if self.logo:
			self.video_driver.draw2DImage3(self.logo, irr.recti(self.window_size.X - self.logo_size.X, self.window_size.Y - self.logo_size.Y, self.window_size.X, self.window_size.Y), self.logo_rect, useAlphaChannelOfTexture = True)

	def draw_weapon(self):
		if self.weapon_current > -1:
			if len(self.hero_weapons) == 0:
				self.weapon_current = -1
			else:
				textures = None
				try:
					textures = self.hero_weapons[self.weapon_current]
				except:
					print('LOG USER INPUT: WEAPONS not include item %d' % self.weapon_current)
					self.weapon_current -= 1#reset to nearest item
				else:
					texture = None
					try:
						texture = textures[self.weapon_current_texture]
					except:
						self.weapon_current_texture = 0
					else:
						x = self.window_size.X/5*2
						size_texture = texture.getOriginalSize()
						self.video_driver.draw2DImage3(texture, irr.recti(x, self.window_size.Y/2, self.window_size.X - x, self.window_size.Y - self.hero_screen_height), irr.recti(0, 0, size_texture.X, size_texture.Y), useAlphaChannelOfTexture = True)
						if self.weapon_current_texture > 0:#CONTINUE SHOT ANIMATION
							self.weapon_current_texture += 1

	def draw_hero_screen(self):
		w = self.window_size.X
		h = self.window_size.Y
		d = (self.hero_screen_line_width - 1) / 2
		x1 = w - d
		y1 = h - self.hero_screen_height
		y2 = h - d
		x1k = self.hero_screen_line_width
		y1k = y1
		x2k = self.hero_screen_height / 2
		y2k = y2 - self.hero_screen_height / 2
		i = 0
		for key_tex in self.hero_keys.itervalues():
			key_img_size = key_tex.getOriginalSize()
			self.video_driver.draw2DImage3(key_tex, irr.recti(x1k, y1k, x2k, y2k), irr.recti(0, 0, key_img_size.X, key_img_size.Y), useAlphaChannelOfTexture = True)
			if i == 1:
				x1k = self.hero_screen_line_width
				x2k = self.hero_screen_height / 2
				y1k = h - self.hero_screen_height / 2
				y2k = h - self.hero_screen_line_width
			else:
				x1k = x2k
				x2k = self.hero_screen_height
			i += 1
			if i > 3:
				break
		if self.hero_texture:
			self.video_driver.draw2DImage3(self.hero_texture, irr.recti(w / 2 - self.hero_screen_height, y1, w / 2 + self.hero_screen_height, y2), self.hero_source_rect, useAlphaChannelOfTexture = True)
		health_text = 'HEALTH %d%%' % self.health
		health_size = self.font.getDimension(health_text)
		self.font.draw(health_text, irr.recti(w - health_size.X - self.hero_screen_line_width, h - health_size.Y - self.hero_screen_line_width, w - self.hero_screen_line_width, h - self.hero_screen_line_width), self.warning_color)
		self.video_driver.draw2DLineW(irr.vector2di(0, y1), irr.vector2di(w, y1), self.hero_screen_color, self.hero_screen_line_width)
		self.video_driver.draw2DLineW(irr.vector2di(0, y2), irr.vector2di(w, y2), self.hero_screen_color, self.hero_screen_line_width)
		self.video_driver.draw2DLineW(irr.vector2di(d, y1), irr.vector2di(d, h), self.hero_screen_color, self.hero_screen_line_width)
		self.video_driver.draw2DLineW(irr.vector2di(x1, y1), irr.vector2di(x1, h), self.hero_screen_color, self.hero_screen_line_width)

	def open_door(self, node, speed = 0.1):
		animators = node.getAnimators()
		for i in range(animators.size()):
			animator = animators[i]
			if animator.getType() == irr.ESNAT_FLY_STRAIGHT:
				if animator.hasFinished():
					node.removeAnimator(animator)
					animator.drop()
				else:
					return False
		start = node.getPosition()
		end = irr.vector3df(start.X, start.Y + self.world_height, start.Z)
		anim = self.scene_manager.createFlyStraightAnimator(start, end, int(self.world_height / speed), pingpong = True)
		node.addAnimator(anim)
		anim.drop()
		return True

	def collision_doors(self):
		i = 0
		index_level_goto = -1
		for node, speed, key, level_goto, evt_snds in self.doors:
			if node.getTransformedBoundingBox().intersectsWithBox(self.hero_node.getTransformedBoundingBox()):
				have_key = True
				if key:
					if not key in self.hero_keys:
						self.font.draw(_('Door require key') + ' ' + key, self.warning_position, self.warning_color)
						have_key = False
				if have_key:
					if level_goto > -1:
						index_level_goto = level_goto
						break
					if self.open_door(node, speed):
						if 'open' in evt_snds:
							self.event_sound_play(evt_snds['open'])
			i += 1
		if index_level_goto > -1:
			if 'open' in evt_snds:
				self.event_sound_play(evt_snds['open'])
			self.clear_level()
			self.load_level(index_level_goto)

	def move_enemy(self, node):
		animators = node.getAnimators()
		for i in range(animators.size()):
			animator = animators[i]
			if animator.getType() == irr.ESNAT_FLY_STRAIGHT:
				if animator.hasFinished():
					node.removeAnimator(animator)
					animator.drop()
				else:
					return
		start = self.camera[1].getPosition()
		end = node.getPosition()
		triangle = irr.triangle3df()
		line = irr.line3df(start, end)
		# is not collides with wall
		if not self.collision_manager.getCollisionPnt(line, self.meta, end, triangle):
			start = self.camera[1].getPosition()
		length = (end - start).getLength()
		speed = 0.1
		try:
			time = int(length / speed)
		except:
			time = 1
		start.Y = end.Y = node.getPosition().Y
		anim = self.scene_manager.createFlyStraightAnimator(end, start, time)
		try:
			node.addAnimator(anim)
		except:
			pass
		anim.drop()
		# CREATE ENEMY'S BULLET
		bullet = self.scene_manager.addSphereSceneNode(self.world_height / 16.0, position = end)
		bullet.getMaterial(0).EmissiveColor = irr.SColor(255, 255, 0, 0)
		speed = 0.5
		try:
			time = int(length / speed)
		except:
			time = 1
		animator = self.scene_manager.createFlyStraightAnimator(end, start, time)
		bullet.addAnimator(animator)
		animator.drop()
		self.enemy_bullets.append(bullet)

	def collision_keys(self):
		for node, texture in self.keys:
			if node.getTransformedBoundingBox().intersectsWithBox(self.hero_node.getTransformedBoundingBox()):
				self.hero_keys[node.getName()] = texture
				self.keys.remove((node, texture))
				node.removeAnimators()
				node.removeAll()
				node.remove()

	def collision_enemies(self):
		for node in self.enemies:
			if node.getPosition().getDistanceFrom(self.camera[1].getPosition()) < self.world_height * 3:
				self.move_enemy(node)

	def collision_enemy_bullets(self):
		for node in self.enemy_bullets:
			animator = node.getAnimators()[0]
			if animator.hasFinished():
				node.removeAnimator(animator)
				animator.drop()
				self.enemy_bullets.remove(node)
				delete_animator = self.scene_manager.createDeleteAnimator(1)
				node.addAnimator(delete_animator)
				delete_animator.drop()
			else:
				if node.getTransformedBoundingBox().intersectsWithBox(self.hero_node.getTransformedBoundingBox()):
					self.health -= 1
					node.removeAnimator(animator)
					animator.drop()
					self.enemy_bullets.remove(node)
					delete_animator = self.scene_manager.createDeleteAnimator(1)
					node.addAnimator(delete_animator)
					delete_animator.drop()

	def append_bullet(self, node):
		self.bullets.append(node)

	def collision_bullet(self, node, animator, nodes):
		for node_obj in nodes:
			if node_obj:
				if node_obj.getTransformedBoundingBox().intersectsWithBox(node.getTransformedBoundingBox()):
					nodes.remove(node_obj)
					selector = node_obj.getTriangleSelector()
					self.meta.removeTriangleSelector(selector)
					selector.drop()
					delete_animator = self.scene_manager.createDeleteAnimator(1)
					node_obj.addAnimator(delete_animator)
					node.addAnimator(delete_animator)
					delete_animator.drop()
					if (node, animator) in self.bullets:
						self.bullets.remove((node, animator))
					animator.drop()

	def collision_bullets(self):
		for node, animator in self.bullets:
			if animator.hasFinished():
				self.bullets.remove((node, animator))
				animator.drop()
			else:
				self.collision_bullet(node, animator, self.enemies)
				self.collision_bullet(node, animator, self.objects)

	def collision_friends(self):
		if not self.dialog_help:
			i = 0
			for node, evt_snds, messages in self.friends:
				node_pos = node.getPosition()
				camera_pos = self.camera[1].getPosition()
				if node_pos.getDistanceFrom(camera_pos) < self.world_height:
					if 'collision_first' in evt_snds:
						self.event_sound_play(evt_snds['collision_first'])
					if 'collision_first' in messages:# and not is_opened:
						self.dialog_help = self.guienv.addMessageBox(node.getName(), _(messages['collision_first']))
						self.cursor_control.setVisible(True)
						self.setActiveCameraOff()
						self.camera[1].setPosition(camera_pos+self.world_height)
						self.camera[1].setTarget(node_pos)
				i = i + 1

	def collision_weapons(self):
		for node, key, textures in self.weapons:
			if node.getTransformedBoundingBox().intersectsWithBox(self.hero_node.getTransformedBoundingBox()):
				self.hero_weapons[key] = textures
				self.weapons.remove((node, key, textures))
				node.removeAnimators()
				node.removeAll()
				node.remove()
				if self.weapon_current < 0:
					self.weapon_current = key

	def draw_level(self):
		self.scene_manager.drawAll()
		self.collision_keys()
		self.collision_doors()
		self.collision_enemies()
		self.collision_enemy_bullets()
		self.collision_bullets()
		self.collision_friends()
		self.collision_weapons()

	def clear_level(self):
		self.meta.removeAllTriangleSelectors()
		self.drop_node(self.hero_node)
		self.hero_node = None
		for node in self.floors:
			self.drop_node(node)
		for node in self.ceilings:
			self.drop_node(node)
		for node in self.walls:
			self.drop_node(node)
		for node, texture in self.keys:
			self.drop_node(node)
		for item in self.friends:
			self.drop_node(item[0])
		for node in self.enemies:
			self.drop_node(node)
		for node in self.objects:
			self.drop_node(node)
		for door in self.doors:
			self.drop_node(door[0])
		for node, key, textures in self.weapons:
			self.drop_node(node)
		self.hero_keys = None
		self.hero_weapons = None
		for name, texture in self.level_textures.iteritems():
			self.video_driver.removeTexture(texture)
		if self.hero_texture:
			self.video_driver.removeTexture(self.hero_texture)
		self.level_textures.clear()
		self.empty_objects()
		self.level_sound_stop(True)

	def start(self, current_level = -1):
		if self.device:
			self.device.setWindowCaption(_(app_name))
			self.device.setResizable(True)
			self.video_driver = self.device.getVideoDriver()
			self.scene_manager = self.device.getSceneManager()
			self.guienv = self.device.getGUIEnvironment()

			if irr.is_frozen():
				self.video_driver.SetIcon(101)
			else:
				try:
					self.video_driver.SetIcon(1)
				except:
					self.video_driver.SetIcon()

			self.skin = self.guienv.getSkin()
			for i in range(irr.EGDC_COUNT):
				color = self.skin.getColor(i)
				color.setAlpha(220)
				self.skin.setColor(i, color)
			self.menu_height = self.skin.getSize(irr.EGDS_MENU_HEIGHT)
			self.create_logo()
			self.create_font()
			#~ self.create_top_menu()
			self.main_window, self.mw_width, self.mw_height = self.create_main_window()

			self.video_driver.setFog(self.fog_color, self.fog_type, self.fog_start, self.fog_end, self.fog_density, self.fog_pixel, self.fog_range)

			self.i_geometry_creator = self.scene_manager.getGeometryCreator()
			self.collision_manager = self.scene_manager.getSceneCollisionManager()

			keyMap = irr.SKeyMap(10)
			keyMap.set(0, irr.EKA_MOVE_FORWARD, irr.KEY_UP)
			keyMap.set(1, irr.EKA_MOVE_FORWARD, irr.KEY_KEY_W)
			keyMap.set(2, irr.EKA_MOVE_BACKWARD, irr.KEY_DOWN)
			keyMap.set(3, irr.EKA_MOVE_BACKWARD, irr.KEY_KEY_S)
			keyMap.set(4, irr.EKA_STRAFE_LEFT, irr.KEY_LEFT)
			keyMap.set(5, irr.EKA_STRAFE_LEFT, irr.KEY_KEY_A)
			keyMap.set(6, irr.EKA_STRAFE_RIGHT, irr.KEY_RIGHT)
			keyMap.set(7, irr.EKA_STRAFE_RIGHT, irr.KEY_KEY_D)
			keyMap.set(8, irr.EKA_JUMP_UP, irr.KEY_KEY_J)
			keyMap.set(9, irr.EKA_CROUCH, irr.KEY_KEY_C)
			self.camera = (self.scene_manager.addCameraSceneNodeMaya(id = CAMERA_MAYA), self.scene_manager.addCameraSceneNodeFPS(id = CAMERA_FPS, keyMapArray = keyMap, keyMapSize = keyMap.length, jumpSpeed = 5))
			self.camera[1].setFarValue(9999.0)
			#~ self.camera[1].setProjectionMatrix(irr.matrix4(), True)
			#~ m = self.camera[1].getProjectionMatrix()
			#~ self.camera[1].setProjectionMatrix(m, True)
			#~ self.camera[1].setAspectRatio(1.5)
			#~ self.camera[1].setUpVector(irr.vector3df(0, 200, 0))

			self.meta = self.scene_manager.createMetaTriangleSelector()
			camera_animator = self.scene_manager.createCollisionResponseAnimator(self.meta, self.camera[1])
			self.camera[1].addAnimator(camera_animator)
			camera_animator.drop()

			self.cursor_control = self.device.getCursorControl()

			self.back_scolor = irr.SColor(255, 100, 100, 140)

			self.event_receiver = UserIEventReceiver()
			self.event_receiver.game = self
			self.device.setEventReceiver(self.event_receiver)

			if current_level > -1:
				self.load_level(current_level)

			# MAIN GAME LOOP
			while self.device.run():
				if self.device.isWindowActive():
					if self.event_receiver.KeyIsDown[irr.KEY_ESCAPE]:
						self.event_receiver.KeyIsDown[irr.KEY_ESCAPE] = False
						if self.dialog_options:
							self.close_options_dialog()
						elif self.run_level:
							self.run_level = False
							self.shot_enable = False
							self.cursor_control.setVisible(True)
							self.clear_level()
							self.main_window_show()
						else:
							break
					elif self.event_receiver.KeyIsDown[irr.KEY_F2]:
						self.event_receiver.KeyIsDown[irr.KEY_F2] = False
						self.show_options_dialog()
					elif self.event_receiver.KeyIsDown[irr.KEY_F1] and not self.dialog_help:
						self.event_receiver.KeyIsDown[irr.KEY_F1] = False
						self.show_help_dialog()
					if self.video_driver.beginScene(True, True, self.back_scolor):
						if self.run_level:
							self.draw_level()

							self.draw_weapon()

							if self.hero_screen_show:
								self.draw_hero_screen()
						else:
							self.draw_logo()

						if self.window_size != self.video_driver.getScreenSize():
							self.window_size = irr.vector2du(self.video_driver.getScreenSize())
							self.main_window.setRelativePosition(irr.position2di((self.window_size.X - self.mw_width) / 2, (self.window_size.Y - self.mw_height) / 2))

						self.guienv.drawAll()
						self.video_driver.endScene()
				else:
					self.device._yield()

			self.event_receiver.game = None
			self.stop()
		else:
			print('++++ ERROR Irrlicht createDevice')

	def stop(self):
		if pybass:
			if self.bass_handle:
				pybass.BASS_ChannelStop(self.bass_handle)
			pybass.BASS_Stop()
			pybass.BASS_Free()
		if self.video_driver:
			self.video_driver.removeAllHardwareBuffers()
			self.video_driver.removeAllTextures()
			self.video_driver.drop()
		if self.device:
			self.device.closeDevice()
			self.device.drop()
			self.device = None
			if self.window_size.X < 320:
				self.window_size.X = 320
			if self.window_size.Y < 240:
				self.window_size.Y = 240
			self.config.set_many((('window_width', int(self.window_size.X)), ('window_height', int(self.window_size.Y))))
		self.config.close()

	def __del__(self):
		if self.device:
			self.stop()

#~ if __name__ == '__main__':
#~ if irr.is_frozen():
	#~ set_executable(sys.argv[0])
	#~ freeze_support()

g = game()
g.start(debug_level)

#~ if pybass:
	#~ pybass.ctypes.windll.kernel32.FreeLibraryAndExitThread(pybass.bass_module._handle, 0)
#~ irr.ctypes.windll.kernel32.FreeLibraryAndExitThread(irr.c_module._handle, 0)
