///////////////////////////////////////////////////////////////////////////////
// Name:        gb.h
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-02-21
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gb.xpm"

#define wxDEBUG_LEVEL 1
#define wxUSE_ON_FATAL_EXCEPTION 1

// For compilers that support precompilation, includes "wx/wx.h".
#ifndef WX_PRECOMP
#include <wx/wx.h>
#else
#include "wx/wxprec.h"
#endif

#ifndef WXWIN_COMPATIBILITY_3_2
#define WXWIN_COMPATIBILITY_3_2 1
#endif

#include <wx/app.h>
#include <wx/debug.h>

#include <wx/cmdline.h>
#include <wx/regex.h>

#include "wx/artprov.h"
#include "wx/bmpbuttn.h"
#include "wx/choice.h"
#include "wx/clipbrd.h"
#include "wx/colordlg.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/grid.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/log.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/spinctrl.h"
#include "wx/stattext.h"
#include "wx/statusbr.h"
#include "wx/textdlg.h"
#include "wx/toolbar.h"
#include "wx/treectrl.h"
#include "wx/wxhtml.h"
#include <wx/dir.h>
#include <wx/filedlg.h>

#include "wx/aui/aui.h"

#include "wx/propgrid/advprops.h"
#include "wx/propgrid/manager.h"
#include "wx/propgrid/propgrid.h"
#include <wx/propgrid/props.h>

#include "wx/translation.h"
#include "wx/uilocale.h"

// Under Linux we demonstrate loading an existing message catalog using
// coreutils package (which is always installed) as an example.
#ifdef __LINUX__
#define USE_COREUTILS_MO
static bool g_loadedCoreutilsMO = false;
#endif // __LINUX__

#if defined(__WXOSX__) || defined(__WXGTK3__)
#define wxDRAWING_DC_SUPPORTS_ALPHA 1
#else
#define wxDRAWING_DC_SUPPORTS_ALPHA 0
#endif // __WXOSX__ || __WXGTK3__

#include <map>
#include <string>

#include "map_settings_dialog.h"

class GBApp : public wxApp {
public:
  wxString path_projects, path_assets, path_assets_images, path_plugins;
  bool OnInit() override;
  int OnExit() override;
  void OnFatalException() override;
  bool OnExceptionInMainLoop() override;

private:
  void OnInitCmdLine(wxCmdLineParser &parser);
  void ShowException();
};

wxDECLARE_APP(GBApp);
wxIMPLEMENT_APP(GBApp);

class MapBoardCtrl;
#include "data.h"
typedef std::map<uint, std::shared_ptr<Data>> LevelDatas;
typedef std::map<wxString, MapBoardCtrl *> LevelBoards;
typedef std::unordered_map<int, wxString> PluginContainer;

class GBFrame : public wxFrame {
  wxDECLARE_DYNAMIC_CLASS(GBFrame);

  enum {
    PGID = 1,
    ID_AddLevel = wxID_HIGHEST,
    ID_OpenLevel,
    ID_ShowLog,
    ID_ShowTree,
    ID_ShowNotebook,
    ID_ShowProperties,
    ID_DrawCellCoords,
    ID_CreatePerspective,
    ID_CopyPerspectiveCode,
    ID_AllowFloating,
    ID_AllowActivePane,
    ID_TransparentHint,
    ID_VenetianBlindsHint,
    ID_RectangleHint,
    ID_NoHint,
    ID_HintFade,
    ID_NoVenetianFade,
    ID_TransparentDrag,
    ID_NoGradient,
    ID_VerticalGradient,
    ID_HorizontalGradient,
    ID_LiveUpdate,
    ID_AllowToolbarResizing,
    ID_Settings,
    ID_DropDownToolbarItem,
    ID_NotebookNoCloseButton,
    ID_NotebookCloseButton,
    ID_NotebookCloseButtonAll,
    ID_NotebookCloseButtonActive,
    ID_NotebookAllowTabMove,
    ID_NotebookAllowTabExternalMove,
    ID_NotebookAllowTabSplit,
    ID_NotebookWindowList,
    ID_NotebookScrollButtons,
    ID_NotebookTabFixedWidth,
    ID_NotebookArtGloss,
    ID_NotebookArtSimple,
    ID_NotebookAlignTop,
    ID_NotebookAlignBottom,
    ID_FirstPerspective = ID_CreatePerspective + 1000,
    ID_FirstPlugin = wxID_HIGHEST + 10000
  };

public:
  GBFrame(){};
  GBFrame(wxWindow *parent, wxWindowID id, const wxString &title,
          const wxPoint &pos = wxDefaultPosition,
          const wxSize &size = wxDefaultSize,
          long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

  wxAuiDockArt *GetDockArt();
  void DoUpdate();
  // void ActivateTreeItem(wxTreeItemId p);

  void SetCellWallType(WallType value);
  void SetCellScript(const wxString &value);

  LevelDatas GetDatas() { return m_datas; }
  LevelBoards GetBoards() { return m_boards; }

private:
  long m_notebook_style;
  long m_notebook_theme;
  wxArrayString m_perspectives;
  wxMenu *menu_perspectives, *menu_plugins;
  wxTreeCtrl *m_tree_ctrl;
  wxTextCtrl *m_logTextCtrl;
  wxAuiManager m_mgr;
  wxAuiNotebook *m_notebook_ctrl;
  LevelDatas m_datas = {};
  LevelBoards m_boards = {};
  wxPropertyGridManager *m_propGridManager;
  MapSettingsData m_map_settings_data;
  PluginContainer m_plugins = {};

  wxTextCtrl *CreateTextCtrl(const wxString &text = wxEmptyString);

  wxTreeCtrl *CreateTreeCtrl();
  // wxDataViewTreeCtrl* CreateTreeCtrl();

  wxPoint GetStartPosition();

  wxAuiNotebook *CreateNotebook();
  MapBoardCtrl *NewMapBoard(int id,
                            std::shared_ptr<Data> d = std::make_shared<Data>());
  void AddLevel(int id = -1,
                std::shared_ptr<Data> d = std::make_shared<Data>());

  void OnSave(wxCommandEvent &event);
  void OnSaveLevel(wxCommandEvent &event);

  void OnEraseBackground(wxEraseEvent &evt);
  void OnSize(wxSizeEvent &evt);

  void OnShowLog(wxCommandEvent &event);
  void OnShowTree(wxCommandEvent &event);
  void OnShowNotebook(wxCommandEvent &event);
  void OnShowProperties(wxCommandEvent &event);
  void OnDrawCellCoords(wxCommandEvent &event);
  void OnAddLevel(wxCommandEvent &event);
  void OnDropDownToolbarItem(wxAuiToolBarEvent &evt);
  void OnCreatePerspective(wxCommandEvent &evt);
  void OnCopyPerspectiveCode(wxCommandEvent &evt);
  void OnRestorePerspective(wxCommandEvent &evt);
  void OnSettings(wxCommandEvent &evt);
  void OnAllowNotebookDnD(wxAuiNotebookEvent &evt);
  void OnNotebookPageClose(wxAuiNotebookEvent &evt);
  void OnNotebookPageClosed(wxAuiNotebookEvent &evt);
  // void OnNotebookPageChanging(wxAuiNotebookEvent &evt);
  void OnExit(wxCommandEvent &evt);
  void OnAbout(wxCommandEvent &evt);
  void OnTabAlignment(wxCommandEvent &evt);

  void OnGradient(wxCommandEvent &evt);
  void OnToolbarResizing(wxCommandEvent &evt);
  void OnManagerFlag(wxCommandEvent &evt);
  void OnNotebookFlag(wxCommandEvent &evt);
  void OnUpdateUI(wxUpdateUIEvent &evt);

  // void OnNotebookDeleteTab(wxCommandEvent& evt);
  // void OnPaneClose(wxAuiManagerEvent& evt);

  void ParseJsonBoards(wxTextFile &f, int id_level = -1);
  void ParseJsonFile(wxTextFile &f);
  void OnOpen(wxCommandEvent &evt);
  void OnOpenLevel(wxCommandEvent &evt);

  void OnPropertyGridChanging(wxPropertyGridEvent &event);

  void OnPluginRun(wxCommandEvent &event);

  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(GBFrame, wxFrame) EVT_MENU(wxID_OPEN, GBFrame::OnOpen) EVT_MENU(
    GBFrame::ID_OpenLevel,
    GBFrame::OnOpenLevel) EVT_MENU(wxID_SAVE,
                                   GBFrame::OnSave) EVT_MENU(wxID_FILE,
                                                             GBFrame::
                                                                 OnSaveLevel)
    EVT_MENU(wxID_EXIT, GBFrame::OnExit) EVT_MENU(
        wxID_ABOUT,
        GBFrame::OnAbout) EVT_MENU(GBFrame::ID_AddLevel,
                                   GBFrame::
                                       OnAddLevel) EVT_MENU(GBFrame::ID_ShowLog,
                                                            GBFrame::OnShowLog)
        EVT_MENU(GBFrame::ID_ShowTree, GBFrame::OnShowTree) EVT_MENU(
            GBFrame::ID_ShowNotebook,
            GBFrame::OnShowNotebook) EVT_MENU(GBFrame::ID_ShowProperties,
                                              GBFrame::OnShowProperties)
            EVT_MENU(GBFrame::ID_DrawCellCoords, GBFrame::OnDrawCellCoords) EVT_MENU(
                GBFrame::ID_CreatePerspective,
                GBFrame::
                    OnCreatePerspective) EVT_MENU(GBFrame::
                                                      ID_CopyPerspectiveCode,
                                                  GBFrame::
                                                      OnCopyPerspectiveCode)
                EVT_MENU(ID_AllowFloating, GBFrame::OnManagerFlag) EVT_MENU(
                    ID_TransparentHint,
                    GBFrame::OnManagerFlag) EVT_MENU(ID_VenetianBlindsHint,
                                                     GBFrame::OnManagerFlag)
                    EVT_MENU(ID_RectangleHint, GBFrame::OnManagerFlag) EVT_MENU(
                        ID_NoHint,
                        GBFrame::OnManagerFlag) EVT_MENU(ID_HintFade,
                                                         GBFrame::OnManagerFlag)
                        EVT_MENU(ID_NoVenetianFade, GBFrame::OnManagerFlag) EVT_MENU(
                            ID_TransparentDrag,
                            GBFrame::OnManagerFlag) EVT_MENU(ID_LiveUpdate,
                                                             GBFrame::
                                                                 OnManagerFlag)
                            EVT_MENU(ID_AllowActivePane, GBFrame::OnManagerFlag) EVT_MENU(
                                ID_NotebookTabFixedWidth,
                                GBFrame::
                                    OnNotebookFlag) EVT_MENU(ID_NotebookNoCloseButton,
                                                             GBFrame::
                                                                 OnNotebookFlag)
                                EVT_MENU(
                                    ID_NotebookCloseButton,
                                    GBFrame::
                                        OnNotebookFlag) EVT_MENU(ID_NotebookCloseButtonAll,
                                                                 GBFrame::
                                                                     OnNotebookFlag)
                                    EVT_MENU(
                                        ID_NotebookCloseButtonActive,
                                        GBFrame::
                                            OnNotebookFlag) EVT_MENU(ID_NotebookAllowTabMove,
                                                                     GBFrame::
                                                                         OnNotebookFlag)
                                        EVT_MENU(
                                            ID_NotebookAllowTabExternalMove,
                                            GBFrame::OnNotebookFlag)
                                            EVT_MENU(ID_NotebookAllowTabSplit,
                                                     GBFrame::OnNotebookFlag)
                                                EVT_MENU(
                                                    ID_NotebookScrollButtons,
                                                    GBFrame::OnNotebookFlag)
                                                    EVT_MENU(
                                                        ID_NotebookWindowList,
                                                        GBFrame::OnNotebookFlag)
                                                        EVT_MENU(
                                                            ID_NotebookArtGloss,
                                                            GBFrame::
                                                                OnNotebookFlag)
                                                            EVT_MENU(
                                                                ID_NotebookArtSimple,
                                                                GBFrame::
                                                                    OnNotebookFlag)
                                                                EVT_MENU(
                                                                    ID_NotebookAlignTop,
                                                                    GBFrame::
                                                                        OnTabAlignment)
                                                                    EVT_MENU(
                                                                        ID_NotebookAlignBottom,
                                                                        GBFrame::
                                                                            OnTabAlignment)
    // EVT_MENU(ID_NotebookNewTab, GBFrame::OnNotebookNewTab)
    // EVT_MENU(ID_NotebookDeleteTab, GBFrame::OnNotebookDeleteTab)
    EVT_MENU(ID_NoGradient, GBFrame::OnGradient) EVT_MENU(
        ID_VerticalGradient,
        GBFrame::
            OnGradient) EVT_MENU(ID_HorizontalGradient,
                                 GBFrame::
                                     OnGradient) EVT_MENU(ID_AllowToolbarResizing,
                                                          GBFrame::
                                                              OnToolbarResizing)
        EVT_MENU(ID_Settings, GBFrame::OnSettings) EVT_UPDATE_UI(
            ID_NotebookTabFixedWidth,
            GBFrame::
                OnUpdateUI) EVT_UPDATE_UI(ID_NotebookNoCloseButton, GBFrame::OnUpdateUI)
            EVT_UPDATE_UI(ID_NotebookCloseButton, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                ID_NotebookCloseButtonAll,
                GBFrame::OnUpdateUI) EVT_UPDATE_UI(ID_NotebookCloseButtonActive,
                                                   GBFrame::OnUpdateUI)
                EVT_UPDATE_UI(ID_NotebookAllowTabMove, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                    ID_NotebookAllowTabExternalMove,
                    GBFrame::
                        OnUpdateUI) EVT_UPDATE_UI(ID_NotebookAllowTabSplit, GBFrame::OnUpdateUI)
                    EVT_UPDATE_UI(ID_NotebookScrollButtons, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                        ID_NotebookWindowList,
                        GBFrame::
                            OnUpdateUI) EVT_UPDATE_UI(ID_AllowFloating, GBFrame::OnUpdateUI)
                        EVT_UPDATE_UI(ID_TransparentHint, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                            ID_VenetianBlindsHint,
                            GBFrame::
                                OnUpdateUI) EVT_UPDATE_UI(ID_RectangleHint, GBFrame::OnUpdateUI)
                            EVT_UPDATE_UI(ID_NoHint, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                                ID_HintFade,
                                GBFrame::OnUpdateUI)
                                EVT_UPDATE_UI(ID_NoVenetianFade, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                                    ID_TransparentDrag,
                                    GBFrame::OnUpdateUI)
                                    EVT_UPDATE_UI(ID_LiveUpdate, GBFrame::OnUpdateUI) EVT_UPDATE_UI(
                                        ID_NoGradient,
                                        GBFrame::OnUpdateUI)
                                        EVT_UPDATE_UI(ID_VerticalGradient,
                                                      GBFrame::OnUpdateUI)
                                            EVT_UPDATE_UI(ID_HorizontalGradient,
                                                          GBFrame::OnUpdateUI)
                                                EVT_UPDATE_UI(
                                                    ID_AllowToolbarResizing,
                                                    GBFrame::OnUpdateUI)
                                                    EVT_MENU_RANGE(
                                                        GBFrame::
                                                            ID_FirstPerspective,
                                                        GBFrame::
                                                                ID_FirstPerspective +
                                                            1000,
                                                        GBFrame::
                                                            OnRestorePerspective)
                                                        EVT_MENU_RANGE(
                                                            GBFrame::
                                                                ID_FirstPlugin,
                                                            GBFrame::
                                                                    ID_FirstPlugin +
                                                                1000,
                                                            GBFrame::
                                                                OnPluginRun)
                                                            EVT_AUITOOLBAR_TOOL_DROPDOWN(
                                                                ID_DropDownToolbarItem,
                                                                GBFrame::
                                                                    OnDropDownToolbarItem)
    // EVT_AUI_PANE_CLOSE(GBFrame::OnPaneClose)
    EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, GBFrame::OnAllowNotebookDnD)
        EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, GBFrame::OnNotebookPageClose)
            EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, GBFrame::OnNotebookPageClosed)
    // EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, GBFrame::OnNotebookPageChanging)
    EVT_ERASE_BACKGROUND(GBFrame::OnEraseBackground) EVT_SIZE(GBFrame::OnSize)
        EVT_PG_CHANGING(PGID, GBFrame::OnPropertyGridChanging)
            wxEND_EVENT_TABLE()

#include "map_board.h"

                class SettingsPanel : public wxPanel {
  enum {
    ID_PaneBorderSize = wxID_HIGHEST,
    ID_SashSize,
    ID_CaptionSize,
    ID_BackgroundColor,
    ID_SashColor,
    ID_InactiveCaptionColor,
    ID_InactiveCaptionGradientColor,
    ID_InactiveCaptionTextColor,
    ID_ActiveCaptionColor,
    ID_ActiveCaptionGradientColor,
    ID_ActiveCaptionTextColor,
    ID_BorderColor,
    ID_GripperColor
  };

public:
  SettingsPanel(wxWindow *parent, GBFrame *frame)
      : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
        m_frame(frame) {
    // wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);
    // vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);

    wxBoxSizer *s1 = new wxBoxSizer(wxHORIZONTAL);
    m_border_size = new wxSpinCtrl(
        this, ID_PaneBorderSize,
        wxString::Format("%d", frame->GetDockArt()->GetMetric(
                                   wxAUI_DOCKART_PANE_BORDER_SIZE)),
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100),
        frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
    s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s1->Add(new wxStaticText(this, wxID_ANY, "Pane Border Size:"));
    s1->Add(m_border_size);
    s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s1->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
    // vert->Add(s1, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

    wxBoxSizer *s2 = new wxBoxSizer(wxHORIZONTAL);
    m_sash_size = new wxSpinCtrl(
        this, ID_SashSize,
        wxString::Format(
            "%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE)),
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100),
        frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
    s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s2->Add(new wxStaticText(this, wxID_ANY, "Sash Size:"));
    s2->Add(m_sash_size);
    s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s2->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
    // vert->Add(s2, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

    wxBoxSizer *s3 = new wxBoxSizer(wxHORIZONTAL);
    m_caption_size = new wxSpinCtrl(
        this, ID_CaptionSize,
        wxString::Format(
            "%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)),
        wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100),
        frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
    s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s3->Add(new wxStaticText(this, wxID_ANY, "Caption Size:"));
    s3->Add(m_caption_size);
    s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s3->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
    // vert->Add(s3, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));
    // vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);

    wxBitmap b = CreateColorBitmap(*wxBLACK);

    wxBoxSizer *s4 = new wxBoxSizer(wxHORIZONTAL);
    m_background_color =
        new wxBitmapButton(this, ID_BackgroundColor, b, wxDefaultPosition,
                           FromDIP(wxSize(50, 25)));
    s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s4->Add(new wxStaticText(this, wxID_ANY, "Background Color:"));
    s4->Add(m_background_color);
    s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s4->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s5 = new wxBoxSizer(wxHORIZONTAL);
    m_sash_color = new wxBitmapButton(this, ID_SashColor, b, wxDefaultPosition,
                                      wxSize(50, 25));
    s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s5->Add(new wxStaticText(this, wxID_ANY, "Sash Color:"));
    s5->Add(m_sash_color);
    s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s5->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s6 = new wxBoxSizer(wxHORIZONTAL);
    m_inactive_caption_color = new wxBitmapButton(
        this, ID_InactiveCaptionColor, b, wxDefaultPosition, wxSize(50, 25));
    s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s6->Add(new wxStaticText(this, wxID_ANY, "Normal Caption:"));
    s6->Add(m_inactive_caption_color);
    s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s6->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s7 = new wxBoxSizer(wxHORIZONTAL);
    m_inactive_caption_gradient_color =
        new wxBitmapButton(this, ID_InactiveCaptionGradientColor, b,
                           wxDefaultPosition, wxSize(50, 25));
    s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s7->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Gradient:"));
    s7->Add(m_inactive_caption_gradient_color);
    s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s7->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s8 = new wxBoxSizer(wxHORIZONTAL);
    m_inactive_caption_text_color =
        new wxBitmapButton(this, ID_InactiveCaptionTextColor, b,
                           wxDefaultPosition, wxSize(50, 25));
    s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s8->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Text:"));
    s8->Add(m_inactive_caption_text_color);
    s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s8->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s9 = new wxBoxSizer(wxHORIZONTAL);
    m_active_caption_color = new wxBitmapButton(
        this, ID_ActiveCaptionColor, b, wxDefaultPosition, wxSize(50, 25));
    s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s9->Add(new wxStaticText(this, wxID_ANY, "Active Caption:"));
    s9->Add(m_active_caption_color);
    s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s9->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s10 = new wxBoxSizer(wxHORIZONTAL);
    m_active_caption_gradient_color =
        new wxBitmapButton(this, ID_ActiveCaptionGradientColor, b,
                           wxDefaultPosition, wxSize(50, 25));
    s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s10->Add(new wxStaticText(this, wxID_ANY, "Active Caption Gradient:"));
    s10->Add(m_active_caption_gradient_color);
    s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s10->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s11 = new wxBoxSizer(wxHORIZONTAL);
    m_active_caption_text_color = new wxBitmapButton(
        this, ID_ActiveCaptionTextColor, b, wxDefaultPosition, wxSize(50, 25));
    s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s11->Add(new wxStaticText(this, wxID_ANY, "Active Caption Text:"));
    s11->Add(m_active_caption_text_color);
    s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s11->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s12 = new wxBoxSizer(wxHORIZONTAL);
    m_border_color = new wxBitmapButton(this, ID_BorderColor, b,
                                        wxDefaultPosition, wxSize(50, 25));
    s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s12->Add(new wxStaticText(this, wxID_ANY, "Border Color:"));
    s12->Add(m_border_color);
    s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s12->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxBoxSizer *s13 = new wxBoxSizer(wxHORIZONTAL);
    m_gripper_color = new wxBitmapButton(this, ID_GripperColor, b,
                                         wxDefaultPosition, wxSize(50, 25));
    s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s13->Add(new wxStaticText(this, wxID_ANY, "Gripper Color:"));
    s13->Add(m_gripper_color);
    s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
    s13->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

    wxGridSizer *grid_sizer = new wxGridSizer(2);
    grid_sizer->SetHGap(FromDIP(5));
    grid_sizer->Add(s1);
    grid_sizer->Add(s4);
    grid_sizer->Add(s2);
    grid_sizer->Add(s5);
    grid_sizer->Add(s3);
    grid_sizer->Add(s13);
    grid_sizer->Add(FromDIP(1), FromDIP(1));
    grid_sizer->Add(s12);
    grid_sizer->Add(s6);
    grid_sizer->Add(s9);
    grid_sizer->Add(s7);
    grid_sizer->Add(s10);
    grid_sizer->Add(s8);
    grid_sizer->Add(s11);

    wxBoxSizer *cont_sizer = new wxBoxSizer(wxVERTICAL);
    cont_sizer->Add(grid_sizer, 1, wxEXPAND | wxALL, FromDIP(5));
    SetSizer(cont_sizer);
    GetSizer()->SetSizeHints(this);

    m_border_size->SetValue(
        frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
    m_sash_size->SetValue(
        frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
    m_caption_size->SetValue(
        frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));

    UpdateColors();
  }

private:
  wxBitmap CreateColorBitmap(const wxColour &c) {
    wxImage image;
    wxSize size = FromDIP(wxSize(25, 14));
    image.Create(size);
    for (int x = 0; x < size.x; ++x) {
      for (int y = 0; y < size.y; ++y) {
        wxColour pixcol = c;
        if (x == 0 || x == size.x || y == 0 || y == size.y)
          pixcol = *wxBLACK;
        image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
      }
    }
    return wxBitmap(image);
  }

  void UpdateColors() {
    wxColour bk =
        m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
    m_background_color->SetBitmapLabel(CreateColorBitmap(bk));

    wxColour cap =
        m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
    m_inactive_caption_color->SetBitmapLabel(CreateColorBitmap(cap));

    wxColour capgrad = m_frame->GetDockArt()->GetColor(
        wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
    m_inactive_caption_gradient_color->SetBitmapLabel(
        CreateColorBitmap(capgrad));

    wxColour captxt = m_frame->GetDockArt()->GetColor(
        wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
    m_inactive_caption_text_color->SetBitmapLabel(CreateColorBitmap(captxt));

    wxColour acap =
        m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
    m_active_caption_color->SetBitmapLabel(CreateColorBitmap(acap));

    wxColour acapgrad = m_frame->GetDockArt()->GetColor(
        wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
    m_active_caption_gradient_color->SetBitmapLabel(
        CreateColorBitmap(acapgrad));

    wxColour acaptxt = m_frame->GetDockArt()->GetColor(
        wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
    m_active_caption_text_color->SetBitmapLabel(CreateColorBitmap(acaptxt));

    wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
    m_sash_color->SetBitmapLabel(CreateColorBitmap(sash));

    wxColour border =
        m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
    m_border_color->SetBitmapLabel(CreateColorBitmap(border));

    wxColour gripper =
        m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);
    m_gripper_color->SetBitmapLabel(CreateColorBitmap(gripper));
  }

  void OnPaneBorderSize(wxSpinEvent &event) {
    m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,
                                     event.GetPosition());
    m_frame->DoUpdate();
  }

  void OnSashSize(wxSpinEvent &event) {
    m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_SASH_SIZE,
                                     event.GetPosition());
    m_frame->DoUpdate();
  }

  void OnCaptionSize(wxSpinEvent &event) {
    m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,
                                     event.GetPosition());
    m_frame->DoUpdate();
  }

  void OnSetColor(wxCommandEvent &event) {
    wxColourDialog dlg(m_frame);
    dlg.SetTitle(_("Color Picker"));
    if (dlg.ShowModal() != wxID_OK)
      return;

    int var = 0;
    switch (event.GetId()) {
    case ID_BackgroundColor:
      var = wxAUI_DOCKART_BACKGROUND_COLOUR;
      break;
    case ID_SashColor:
      var = wxAUI_DOCKART_SASH_COLOUR;
      break;
    case ID_InactiveCaptionColor:
      var = wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR;
      break;
    case ID_InactiveCaptionGradientColor:
      var = wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR;
      break;
    case ID_InactiveCaptionTextColor:
      var = wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR;
      break;
    case ID_ActiveCaptionColor:
      var = wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR;
      break;
    case ID_ActiveCaptionGradientColor:
      var = wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR;
      break;
    case ID_ActiveCaptionTextColor:
      var = wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR;
      break;
    case ID_BorderColor:
      var = wxAUI_DOCKART_BORDER_COLOUR;
      break;
    case ID_GripperColor:
      var = wxAUI_DOCKART_GRIPPER_COLOUR;
      break;
    default:
      return;
    }

    m_frame->GetDockArt()->SetColor(var, dlg.GetColourData().GetColour());
    m_frame->DoUpdate();
    UpdateColors();
  }

private:
  GBFrame *m_frame;
  wxSpinCtrl *m_border_size;
  wxSpinCtrl *m_sash_size;
  wxSpinCtrl *m_caption_size;
  wxBitmapButton *m_inactive_caption_text_color;
  wxBitmapButton *m_inactive_caption_gradient_color;
  wxBitmapButton *m_inactive_caption_color;
  wxBitmapButton *m_active_caption_text_color;
  wxBitmapButton *m_active_caption_gradient_color;
  wxBitmapButton *m_active_caption_color;
  wxBitmapButton *m_sash_color;
  wxBitmapButton *m_background_color;
  wxBitmapButton *m_border_color;
  wxBitmapButton *m_gripper_color;

  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(SettingsPanel,
                    wxPanel) EVT_SPINCTRL(ID_PaneBorderSize,
                                          SettingsPanel::OnPaneBorderSize)
    EVT_SPINCTRL(ID_SashSize, SettingsPanel::OnSashSize)
        EVT_SPINCTRL(ID_CaptionSize, SettingsPanel::OnCaptionSize) EVT_BUTTON(
            ID_BackgroundColor,
            SettingsPanel::OnSetColor) EVT_BUTTON(ID_SashColor,
                                                  SettingsPanel::OnSetColor)
            EVT_BUTTON(ID_InactiveCaptionColor, SettingsPanel::OnSetColor)
                EVT_BUTTON(ID_InactiveCaptionGradientColor,
                           SettingsPanel::OnSetColor)
                    EVT_BUTTON(ID_InactiveCaptionTextColor,
                               SettingsPanel::OnSetColor)
                        EVT_BUTTON(ID_ActiveCaptionColor,
                                   SettingsPanel::OnSetColor)
                            EVT_BUTTON(ID_ActiveCaptionGradientColor,
                                       SettingsPanel::OnSetColor)
                                EVT_BUTTON(ID_ActiveCaptionTextColor,
                                           SettingsPanel::OnSetColor)
                                    EVT_BUTTON(ID_BorderColor,
                                               SettingsPanel::OnSetColor)
                                        EVT_BUTTON(ID_GripperColor,
                                                   SettingsPanel::OnSetColor)
                                            wxEND_EVENT_TABLE()

#ifdef PLUGINS_PYTHON
#include <plugins_python.h>
#endif // PLUGINS_PYTHON
