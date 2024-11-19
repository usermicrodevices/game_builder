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

static PyObject* log_message(PyObject *self, PyObject *args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_message", &text))
		wxLogMessage(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_message"));
	Py_RETURN_NONE;
}

static PyObject* log_info(PyObject *self, PyObject *args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_info", &text))
		wxLogInfo(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_info"));
	Py_RETURN_NONE;
}

static PyObject* log_error(PyObject *self, PyObject *args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_error", &text))
		wxLogError(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_error"));
	Py_RETURN_NONE;
}

static PyObject* log_warning(PyObject *self, PyObject *args)
{
	const char* text;
	if (PyArg_ParseTuple(args, "s:log_warning", &text))
		wxLogWarning(text);
	else
		wxLogError(wxT("PyArg_Parse error on log_warning"));
	Py_RETURN_NONE;
}

//static PyObject* get_main_frame(PyObject *self, PyObject *args)
//{
//	return PyCapsule_New((void*)wxApp::GetMainTopWindow(), "game_builder.main_frame", NULL);
//}

typedef struct
{
	PyObject_HEAD
	GBFrame* ptr;
	PyObject *name; /* name */
} MainFrame;

static void MainFrame_dealloc(MainFrame* self)
{
#if DEBUG
	std::cout << "!!! MainFrame_dealloc " << self->name << std::endl;
#endif
	//Py_XDECREF(self->ptr);
	Py_XDECREF(self->name);
	Py_TYPE(self)->tp_free((PyObject *) self);
#if DEBUG
	std::cout << "	MainFrame_dealloc !!!" << std::endl;
#endif
}

static PyObject* MainFrame_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	MainFrame *self;
	self = (MainFrame *) type->tp_alloc(type, 0);
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
	return (PyObject *) self;
}

static int MainFrame_init(MainFrame *self, PyObject *Py_UNUSED(args), PyObject *Py_UNUSED(kwds))
{
	return 0;
}

static PyMemberDef MainFrame_members[] =
{
	{"name", Py_T_OBJECT_EX, offsetof(MainFrame, name), 0, "frame name"},
	{NULL}
};

static PyObject* MainFrame_levels(MainFrame *self, PyObject *Py_UNUSED(ignored))
{
	if (self->ptr == NULL)
	{
		PyErr_SetString(PyExc_AttributeError, "ptr");
		return NULL;
	}
	PyObject *dict = Py_BuildValue("{}");
	for(const auto& [k, map_board] : self->ptr->GetBoards())
	{
		if (PyMapping_SetItemString(dict, k, PyLong_FromVoidPtr(map_board)) < 0)
			return NULL;
	}
	return dict;
}

static PyMethodDef MainFrame_methods[] = {
	{"levels", (PyCFunction) MainFrame_levels, METH_NOARGS, "Return the levels"},
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
	.tp_new = MainFrame_new,
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
	PyObject *m;
	if (PyType_Ready(&MainFrameType) < 0)
		return NULL;
	m = PyModule_Create(&GbModule);
	if (m == NULL)
		return NULL;
	if (PyModule_AddObjectRef(m, "MainFrame", (PyObject *) &MainFrameType) < 0)
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

	//config.home = (wchar_t*).wc_str();
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
	//std::cout << "Py_InitializeFromConfig" << std::endl;
	if (PyStatus_Exception(status)) {exception(config, status);}
	PyConfig_Clear(&config);

	//std::cout << "PyRun_SimpleFile" << std::endl;
	wxLogInfo(wxString::Format("PyRun_SimpleFile result = %d", PyRun_SimpleFile(pfp, plugin_filename)));

	wxLogInfo(wxString::Format("Py_FinalizeEx result = %d", Py_FinalizeEx()));
}
