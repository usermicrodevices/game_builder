///////////////////////////////////////////////////////////////////////////////
// Name:        plugins_python.h
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-11-09
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject* log_message(PyObject* self, PyObject* args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_message", &text))
		wxLogMessage(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_message"));
	Py_RETURN_NONE;
}

static PyObject* log_info(PyObject* self, PyObject* args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_info", &text))
		wxLogInfo(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_info"));
	Py_RETURN_NONE;
}

static PyObject* log_error(PyObject* self, PyObject* args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_error", &text))
		wxLogError(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_error"));
	Py_RETURN_NONE;
}

static PyObject* log_warning(PyObject* self, PyObject* args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_warning", &text))
		wxLogWarning(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_warning"));
	Py_RETURN_NONE;
}

///////////////// MapTexture /////////////////
typedef struct
{
	PyObject_HEAD
	std::shared_ptr<Texture> ptr;
	PyObject* idx;
	PyObject* path;
} MapTexture;

static PyObject* MapTexture_new(PyTypeObject* type, PyObject* Py_UNUSED(args) = nullptr, PyObject* Py_UNUSED(kwds) = nullptr)
{
	MapTexture* self = (MapTexture*)type->tp_alloc(type, 0);
	return (PyObject*)self;
}

static void MapTexture_dealloc(MapTexture* self)
{
#if DEBUG
	std::cout << "!!! MapTexture_dealloc " << self->idx << std::endl;
#endif
	Py_XDECREF(self->path);
	Py_TYPE(self)->tp_free((PyObject*) self);
#if DEBUG
	std::cout << "	MapTexture_dealloc !!!" << std::endl;
#endif
}

static int MapTexture_init(MapTexture* self, PyObject* args, PyObject* Py_UNUSED(kwds) = nullptr)
{
	self->idx = PyTuple_GetItem(args, 0);
	self->path = PyTuple_GetItem(args, 1);
	return 0;
}

static PyMemberDef MapTexture_members[] =
{
	{"idx", Py_T_OBJECT_EX, offsetof(MapTexture, idx), 0, "texture index"},
	{"path", Py_T_OBJECT_EX, offsetof(MapTexture, path), 0, "texture path"},
	{NULL}
};

static PyObject* MapTexture_is_ok(MapTexture* self, PyObject* Py_UNUSED(ignored))
{
	if (self->ptr == NULL)
	{
		PyErr_SetString(PyExc_AttributeError, "ptr");
		return NULL;
	}
	return self->ptr->IsOk() ? Py_True : Py_False;
}

static PyObject* MapTexture_change_image(MapTexture* self, PyObject* args)
{
	if (self->ptr == NULL)
	{
		PyErr_SetString(PyExc_AttributeError, "ptr");
		return NULL;
	}
	Py_ssize_t size;
	return self->ptr->change_image(PyUnicode_AsWideCharString(PyTuple_GetItem(args, 0), &size)) ? Py_True : Py_False;
}

static PyMethodDef MapTexture_methods[] = {
	{"is_ok", (PyCFunction) MapTexture_is_ok, METH_NOARGS, "check texture"},
	{"change_image", (PyCFunction) MapTexture_change_image, METH_VARARGS, "change image of texture"},
	{NULL}
};

static PyTypeObject MapTextureType = {
	.ob_base = PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "MapTexture",
	.tp_basicsize = sizeof(MapTexture),
	.tp_itemsize = 0,
	.tp_dealloc = (destructor) MapTexture_dealloc,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_doc = PyDoc_STR("Texture objects"),
	.tp_methods = MapTexture_methods,
	.tp_members = MapTexture_members,
	.tp_init = (initproc) MapTexture_init,
	.tp_new = MapTexture_new
};

///////////////// MapCell /////////////////
typedef struct
{
	PyObject_HEAD
	std::shared_ptr<Cell> ptr;
	PyObject* idx;
	PyObject* side;
	PyObject* texture_floor;
	PyObject* texture_wall;
	PyObject* texture_roof;
	PyObject* wtp;
	PyObject* script;
} MapCell;

static PyObject* MapCell_new(PyTypeObject* type, PyObject* Py_UNUSED(args) = nullptr, PyObject* Py_UNUSED(kwds) = nullptr)
{
	MapCell* self = (MapCell*)type->tp_alloc(type, 0);
	return (PyObject*)self;
}

static void MapCell_dealloc(MapCell* self)
{
#if DEBUG
	std::cout << "!!! MapCell_dealloc " << self->idx << std::endl;
#endif
	Py_XDECREF(self->script);
	Py_TYPE(self)->tp_free((PyObject*) self);
#if DEBUG
	std::cout << "	MapCell_dealloc !!!" << std::endl;
#endif
}

static int MapCell_init(MapCell* self, PyObject* args, PyObject* Py_UNUSED(kwds) = nullptr)
{
	self->idx = PyTuple_GetItem(args, 0);
	self->side = PyTuple_GetItem(args, 1);
	self->texture_floor = PyTuple_GetItem(args, 2);
	self->texture_wall = PyTuple_GetItem(args, 3);
	self->texture_roof = PyTuple_GetItem(args, 4);
	self->wtp = PyTuple_GetItem(args, 5);
	self->script = PyTuple_GetItem(args, 6);
	return 0;
}

static PyMemberDef MapCell_members[] =
{
	{"idx", Py_T_OBJECT_EX, offsetof(MapCell, idx), 0, "cell index"},
	{"side", Py_T_OBJECT_EX, offsetof(MapCell, side), 0, "cell side"},
	{"texture_floor", Py_T_OBJECT_EX, offsetof(MapCell, texture_floor), 0, "cell texture_floor"},
	{"texture_wall", Py_T_OBJECT_EX, offsetof(MapCell, texture_wall), 0, "cell texture_wall"},
	{"texture_roof", Py_T_OBJECT_EX, offsetof(MapCell, texture_roof), 0, "cell texture_roof"},
	{"wtp", Py_T_OBJECT_EX, offsetof(MapCell, wtp), 0, "cell wall type"},
	{"script", Py_T_OBJECT_EX, offsetof(MapCell, script), 0, "script code"},
	{NULL}
};

static PyObject* MapCell_get_visible_texture(MapCell* self, PyObject* Py_UNUSED(ignored))
{
	if (self->ptr == NULL)
	{
		PyErr_SetString(PyExc_AttributeError, "ptr");
		return NULL;
	}
	return PyLong_FromLong(self->ptr->get_visible_texture());
}

static PyMethodDef MapCell_methods[] = {
	{"get_visible_texture", (PyCFunction) MapCell_get_visible_texture, METH_NOARGS, "get visible texture"},
	{NULL}
};

static PyTypeObject MapCellType = {
	.ob_base = PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "MapCell",
	.tp_basicsize = sizeof(MapCell),
	.tp_itemsize = 0,
	.tp_dealloc = (destructor) MapCell_dealloc,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_doc = PyDoc_STR("Cell objects"),
	.tp_methods = MapCell_methods,
	.tp_members = MapCell_members,
	.tp_init = (initproc) MapCell_init,
	.tp_new = MapCell_new
};

///////////////// MainFrame /////////////////
typedef struct
{
	PyObject_HEAD
	GBFrame* ptr;
	PyObject* name;
} MainFrame;

static PyObject* MainFrame_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	MainFrame* self = (MainFrame*)type->tp_alloc(type, 0);
	if (self != NULL)
	{
		self->ptr = (GBFrame*)wxApp::GetMainTopWindow();
		self->name = PyUnicode_FromString(self->ptr->GetName().c_str());
		if (self->name == NULL)
		{
			Py_DECREF(self);
			return NULL;
		}
	}
	return (PyObject*)self;
}

static void MainFrame_dealloc(MainFrame* self)
{
#if DEBUG
	std::cout << "!!! MainFrame_dealloc " << self->name << std::endl;
#endif
	//Py_XDECREF(self->ptr);
	Py_XDECREF(self->name);
	Py_TYPE(self)->tp_free((PyObject*) self);
#if DEBUG
	std::cout << "	MainFrame_dealloc !!!" << std::endl;
#endif
}

static int MainFrame_init(MainFrame* self, PyObject* Py_UNUSED(args), PyObject* Py_UNUSED(kwds))
{
	return 0;
}

static PyMemberDef MainFrame_members[] =
{
	{"name", Py_T_OBJECT_EX, offsetof(MainFrame, name), 0, "frame name"},
	{NULL}
};

static PyObject* MainFrame_levels(MainFrame* self, PyObject* Py_UNUSED(ignored))
{
	if (self->ptr == NULL)
	{
		PyErr_SetString(PyExc_AttributeError, "ptr");
		return NULL;
	}
	PyObject* levels = PyDict_New();//Py_BuildValue("{}");
	for(const auto& [idx, level_data] : self->ptr->GetDatas())
	{
		PyObject* textures = PyDict_New();
		for(const auto& [id, texture] : level_data->get_textures())
		{
			if(texture)
			{
				PyObject* map_texture = MapTexture_new(&MapTextureType, nullptr, nullptr);
				((MapTexture*)map_texture)->ptr = texture;
				PyObject* args = PyTuple_Pack(2, PyLong_FromLong(texture->id), PyUnicode_FromString(texture->path.GetFullPath().c_str()));
				MapTexture_init((MapTexture*)map_texture, args);
				PyDict_SetItem(textures, PyLong_FromLong(id), map_texture);
			}
#if DEBUG
			else
				std::cout << "!!! MainFrame_levels texture is nullptr !!!" << std::endl;
#endif
		}
		PyObject* cells = PyDict_New();
		for(const auto& [point, cell] : level_data->get_cells())
		{
			PyObject* map_cell = MapCell_new(&MapCellType, nullptr, nullptr);
			((MapCell*)map_cell)->ptr = cell;
			PyObject* args = PyTuple_Pack(7, PyLong_FromLong(cell->id), PyLong_FromLong(cell->side), PyLong_FromLong(cell->texture_floor), PyLong_FromLong(cell->texture_wall), PyLong_FromLong(cell->texture_roof), PyLong_FromLong(cell->wtp), PyUnicode_FromString(cell->script.c_str()));
			MapCell_init((MapCell*)map_cell, args);
			std::string key = std::to_string(point.x) + "-" + std::to_string(point.x);
			PyDict_SetItemString(cells, key.c_str(), map_cell);
		}
		PyObject* level = PyDict_New();
		PyDict_SetItemString(level, "textures", textures);
		PyDict_SetItemString(level, "cells", cells);
		if(PyDict_SetItem(levels, PyLong_FromLong(idx), level) < 0)
			return NULL;
	}
	return levels;
}

static PyObject* MainFrame_boards(MainFrame* self, PyObject* Py_UNUSED(ignored))
{
	if (self->ptr == NULL)
	{
		PyErr_SetString(PyExc_AttributeError, "ptr");
		return NULL;
	}
	PyObject* boards = PyDict_New();//Py_BuildValue("{}");
	for(const auto& [k, map_board] : self->ptr->GetBoards())
	{
		if (PyMapping_SetItemString(boards, k, PyLong_FromVoidPtr(map_board)) < 0)
			return NULL;
	}
	return boards;
}

static PyMethodDef MainFrame_methods[] = {
	{"levels", (PyCFunction) MainFrame_levels, METH_NOARGS, "list of the levels"},
	{"boards", (PyCFunction) MainFrame_boards, METH_NOARGS, "list of the boards"},
	{NULL}
};

static PyTypeObject MainFrameType = {
	.ob_base = PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "game_builder.MainFrame",
	.tp_basicsize = sizeof(MainFrame),
	.tp_itemsize = 0,
	.tp_dealloc = (destructor) MainFrame_dealloc,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_doc = PyDoc_STR("MainFrame objects"),
	.tp_methods = MainFrame_methods,
	.tp_members = MainFrame_members,
	.tp_init = (initproc) MainFrame_init,
	.tp_new = MainFrame_new
};

static PyMethodDef GbMethods[] = {
	{"log_message", log_message, METH_VARARGS, "Write string to log as message."},
	{"log_info", log_info, METH_VARARGS, "Write string to log as info."},
	{"log_error", log_error, METH_VARARGS, "Write string to log as error."},
	{"log_warning", log_warning, METH_VARARGS, "Write string to log as warning."},
	//{"main_frame", get_main_frame, METH_VARARGS, "Return main frame pointer."},
	{NULL, NULL, 0, NULL}
};

static PyModuleDef GbModule =
{
	PyModuleDef_HEAD_INIT,
	"game_builder",
	"internal module for access to app functionals",
	-1,
	GbMethods,
	NULL,
	NULL,
	NULL,
	NULL
};

PyMODINIT_FUNC PyInit_gb(void)
{
	PyObject* m;
	if (PyType_Ready(&MapTextureType) < 0)
		return NULL;
	if (PyType_Ready(&MapCellType) < 0)
		return NULL;
	if (PyType_Ready(&MainFrameType) < 0)
		return NULL;
	m = PyModule_Create(&GbModule);
	if (m == NULL)
		return NULL;
	if (PyModule_AddObjectRef(m, "MainFrame", (PyObject*) &MainFrameType) < 0)
	{
		Py_DECREF(m);
		return NULL;
	}
	return m;
}

void exception(PyConfig config, PyStatus status)
{
	//std::cout << "EXCEPTION status.exitcode " << static_cast<int16_t>(status.exitcode) << std::endl;
	PyConfig_Clear(&config);
	if (PyStatus_IsExit(status))
		wxLogError(wxString::Format("Py_ExitStatusException = %d", status.exitcode));
	Py_ExitStatusException(status);
}

void run_plugin(wxString plugin_filename)
{
	int status_code = 0;
	wxLogInfo(plugin_filename);
	FILE* pfp = fopen(plugin_filename, "r");
	PyConfig config;
	PyStatus status;

	PyConfig_InitPythonConfig(&config);
	config.isolated = 1;
	config.module_search_paths_set = 1;

	status = PyConfig_SetString(&config, &config.home, (wxGetCwd()+"/python").wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyConfig_SetString(&config, &config.platlibdir, (wxGetCwd()+"/python/lib").wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyConfig_SetString(&config, &config.prefix, wxGetCwd().wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyWideStringList_Append(&(config.module_search_paths), (wxGetCwd()+"/python").wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyWideStringList_Append(&(config.module_search_paths), (wxGetCwd()+"/python/modules").wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyWideStringList_Append(&(config.module_search_paths), (wxGetCwd()+"/python/modules/site-packages").wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyConfig_SetString(&config, &config.program_name, plugin_filename.AfterLast('/').BeforeLast('.').wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status = PyConfig_SetString(&config, &config.run_filename, plugin_filename.wc_str());
	if (PyStatus_Exception(status)) {exception(config, status);}

	status_code = PyImport_AppendInittab("game_builder", &PyInit_gb);
	if (status_code > -1)
		wxLogInfo(wxString::Format("PyImport_AppendInittab success: %d; now you can 'import game_builder'", status_code));
	else
		wxLogInfo(wxT("PyImport_AppendInittab error: table of built-in modules could not be extended"));

	status = Py_InitializeFromConfig(&config);
#if DEBUG
	std::cout << "!!! Py_InitializeFromConfig !!!" << std::endl;
#endif
	if (PyStatus_Exception(status)) {exception(config, status);}
	PyConfig_Clear(&config);

#if DEBUG
	std::cout << "!!! PyRun_SimpleFile !!!" << std::endl;
#endif
	wxLogInfo(wxString::Format("PyRun_SimpleFile result = %d", PyRun_SimpleFile(pfp, plugin_filename)));

	wxLogInfo(wxString::Format("Py_FinalizeEx result = %d", Py_FinalizeEx()));
}
