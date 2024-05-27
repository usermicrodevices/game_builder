///////////////////////////////////////////////////////////////////////////////
// Name:        map_settings_dialog.h
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-05-27
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <wx/valgen.h>
#include <wx/propdlg.h>

struct MapSettingsData
{
	MapSettingsData() :
	// m_loadLastOnStartup(false),
	// m_showToolTips(false),
	// m_autoSaveInterval(1),
	// m_applyTo(0),
	// m_bgStyle(0),
	// m_titleFontSize(10),
	m_cell_side_size(50),
	m_count_cell_x(100),
	m_count_cell_y(100)
	{
	}

	// bool m_loadLastOnStartup;
	// bool m_showToolTips;
	// int m_autoSaveInterval;
	// int m_applyTo;
	// int m_bgStyle;
	// int m_titleFontSize;
	int m_cell_side_size;
	int m_count_cell_x;
	int m_count_cell_y;
};

class MapSettingsDialog: public wxPropertySheetDialog
{
	wxDECLARE_CLASS(MapSettingsDialog);
public:
	MapSettingsData& m_sdata;

	MapSettingsDialog(wxWindow* win, MapSettingsData& settingsData)
	: m_sdata(settingsData)
	{
		SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
		int resizeBorder = wxRESIZE_BORDER;
		Create(win, wxID_ANY, _("Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | resizeBorder);
		CreateButtons(wxOK | wxCANCEL | wxHELP);
		wxBookCtrlBase* notebook = GetBookCtrl();
		wxPanel* generalSettings = CreateGeneralSettingsPage(notebook);
		wxPanel* advancedSettings = CreateAdvancedSettingsPage(notebook);
		notebook->AddPage(generalSettings, _("General"), true);
		//notebook->AddPage(advancedSettings, _("Advanced"), false);
		LayoutDialog();
	};

	~MapSettingsDialog(){};

	wxPanel* CreateGeneralSettingsPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
		wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
		//// SIZE SIDE CELL
		wxBoxSizer* itemSizerSizeSideCell = new wxBoxSizer( wxHORIZONTAL );
		wxSpinCtrl* cell_side_size = new wxSpinCtrl(panel, ID_CELL_SIDE_SIZE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 1);
		cell_side_size->SetValidator(wxGenericValidator(&m_sdata.m_cell_side_size));
		itemSizerSizeSideCell->Add(new wxStaticText(panel, wxID_STATIC, _("cell side size")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		itemSizerSizeSideCell->Add(cell_side_size, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		item0->Add(itemSizerSizeSideCell, 0, wxGROW|wxALL, 0);
		//// COUNT CELL X
		wxBoxSizer* itemSizerCountCellX = new wxBoxSizer( wxHORIZONTAL );
		wxSpinCtrl* count_cell_x = new wxSpinCtrl(panel, ID_COUNT_CELL_X, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100000, 1);
		count_cell_x->SetValidator(wxGenericValidator(&m_sdata.m_count_cell_x));
		itemSizerCountCellX->Add(new wxStaticText(panel, wxID_STATIC, _("count cell x")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		itemSizerCountCellX->Add(count_cell_x, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		item0->Add(itemSizerCountCellX, 0, wxGROW|wxALL, 0);
		//// COUNT CELL Y
		wxBoxSizer* itemSizerCountCellY = new wxBoxSizer( wxHORIZONTAL );
		wxSpinCtrl* count_cell_y = new wxSpinCtrl(panel, ID_COUNT_CELL_Y, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100000, 1);
		count_cell_y->SetValidator(wxGenericValidator(&m_sdata.m_count_cell_y));
		itemSizerCountCellY->Add(new wxStaticText(panel, wxID_STATIC, _("count cell y")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		itemSizerCountCellY->Add(count_cell_y, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		item0->Add(itemSizerCountCellY, 0, wxGROW|wxALL, 0);
		//// LOAD LAST PROJECT
		// wxBoxSizer* itemSizerLoadLast = new wxBoxSizer( wxHORIZONTAL );
		// wxCheckBox* checkBoxLoadLast = new wxCheckBox(panel, ID_LOAD_LAST_PROJECT, "&Load last project on startup", wxDefaultPosition, wxDefaultSize);
		// checkBoxLoadLast->SetValidator(wxGenericValidator(&m_sdata.m_loadLastOnStartup));
		// itemSizerLoadLast->Add(checkBoxLoadLast, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		// item0->Add(itemSizerLoadLast, 0, wxGROW|wxALL, 0);
		//// AUTOSAVE
		// wxString autoSaveLabel = "&Auto-save every";
		// wxString minsLabel = "mins";
		// wxBoxSizer* itemSizerAutoSave = new wxBoxSizer( wxHORIZONTAL );
		// wxCheckBox* checkBoxAutoSave = new wxCheckBox(panel, ID_AUTO_SAVE, autoSaveLabel, wxDefaultPosition, wxDefaultSize);
		// itemSizerAutoSave->Add(checkBoxAutoSave, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		// wxSpinCtrl* spinCtrlAutoSave = new wxSpinCtrl(panel, ID_AUTO_SAVE_MINS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 60, 1);
		// spinCtrlAutoSave->SetValidator(wxGenericValidator(&m_sdata.m_autoSaveInterval));
		// itemSizerAutoSave->Add(spinCtrlAutoSave, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		// itemSizerAutoSave->Add(new wxStaticText(panel, wxID_STATIC, minsLabel), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		// item0->Add(itemSizerAutoSave, 0, wxGROW|wxALL, 0);
		//// TOOLTIPS
		// wxBoxSizer* itemSizerShowToolTips = new wxBoxSizer( wxHORIZONTAL );
		// wxCheckBox* checkBoxShowToolTips = new wxCheckBox(panel, ID_SHOW_TOOLTIPS, "Show &tooltips", wxDefaultPosition, wxDefaultSize);
		// checkBoxShowToolTips->SetValidator(wxGenericValidator(&m_sdata.m_showToolTips));
		// itemSizerShowToolTips->Add(checkBoxShowToolTips, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		// item0->Add(itemSizerShowToolTips, 0, wxGROW|wxALL, 0);
		////
		topSizer->Add( item0, wxSizerFlags(1).Expand().Border(wxALL, 5) );
		panel->SetSizerAndFit(topSizer);
		return panel;
	};

	wxPanel* CreateAdvancedSettingsPage(wxWindow* parent)
	{
		wxPanel* panel = new wxPanel(parent, wxID_ANY);
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
		wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
		//// PROJECT OR GLOBAL
		wxString globalOrProjectChoices[2];
		globalOrProjectChoices[0] = "&New projects";
		globalOrProjectChoices[1] = "&This project";
		wxRadioBox* projectOrGlobal = new wxRadioBox(panel, ID_APPLY_SETTINGS_TO, "&Apply settings to:", wxDefaultPosition, wxDefaultSize, 2, globalOrProjectChoices);
		//projectOrGlobal->SetValidator(wxGenericValidator(&m_sdata.m_applyTo));
		item0->Add(projectOrGlobal, 0, wxGROW|wxALL, 5);
		projectOrGlobal->SetSelection(0);
		//// BACKGROUND STYLE
		wxArrayString backgroundStyleChoices;
		backgroundStyleChoices.Add("Colour");
		backgroundStyleChoices.Add("Image");
		wxStaticBoxSizer* styleSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Background style:");
		wxStaticBox* const styleSizerBox = styleSizer->GetStaticBox();
		item0->Add(styleSizer, 0, wxGROW|wxALL, 5);
		wxBoxSizer* itemSizer2 = new wxBoxSizer( wxHORIZONTAL );
		wxChoice* choice2 = new wxChoice(styleSizerBox, ID_BACKGROUND_STYLE, wxDefaultPosition, wxDefaultSize, backgroundStyleChoices);
		//choice2->SetValidator(wxGenericValidator(&m_sdata.m_bgStyle));
		itemSizer2->Add(new wxStaticText(styleSizerBox, wxID_ANY, "&Window:"), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		itemSizer2->Add(5, 5, 1, wxALL, 0);
		itemSizer2->Add(choice2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		styleSizer->Add(itemSizer2, 0, wxGROW|wxALL, 5);
		//// FONT SIZE SELECTION
		wxStaticBoxSizer* itemSizer5 = new wxStaticBoxSizer(wxHORIZONTAL, panel, "Tile font size:");
		wxSpinCtrl* spinCtrl = new wxSpinCtrl(itemSizer5->GetStaticBox(), ID_FONT_SIZE, wxEmptyString);
		//spinCtrl->SetValidator(wxGenericValidator(&m_sdata.m_titleFontSize));
		itemSizer5->Add(spinCtrl, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
		item0->Add(itemSizer5, 0, wxGROW|wxLEFT|wxRIGHT, 5);
		topSizer->Add( item0, wxSizerFlags(1).Expand().Border(wxALL, 5) );
		topSizer->AddSpacer(5);
		panel->SetSizerAndFit(topSizer);
		return panel;
	};

protected:

	enum {
	ID_CELL_SIDE_SIZE = 100,
	ID_COUNT_CELL_X,
	ID_COUNT_CELL_Y,
	ID_SHOW_TOOLTIPS,
	ID_AUTO_SAVE,
	ID_AUTO_SAVE_MINS,
	ID_LOAD_LAST_PROJECT,
	ID_APPLY_SETTINGS_TO,
	ID_BACKGROUND_STYLE,
	ID_FONT_SIZE
	};

	wxDECLARE_EVENT_TABLE();
};
wxIMPLEMENT_CLASS(MapSettingsDialog, wxPropertySheetDialog);
wxBEGIN_EVENT_TABLE(MapSettingsDialog, wxPropertySheetDialog)
wxEND_EVENT_TABLE()	  
