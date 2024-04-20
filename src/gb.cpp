///////////////////////////////////////////////////////////////////////////////
// Name:        gb.cpp
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-02-21
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

#include "gb.h"


// void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
// {
//     parser.AddSwitch("n", OPTION_1, _("desc1"));
//     parser.AddSwitch("y", OPTION_2, _("desk2"));
//     wxApp::OnInitCmdLine(parser);
// }
// bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
// {
//     if ( !wxApp::OnCmdLineParsed(parser) )
//         return false;
//     if ( parser.Found(OPTION_1) )
//     {
//         m_option = ID_VALUE1;
//     }
//     if ( parser.Found(OPTION_2) )
//     {
//         if ( m_option == ID_VALUE3 )
//         {
//             wxLogWarning("--%s option overrides --%s", OPTION_2, OPTION_1);
//         }
//         m_option = ID_VALUE2;
//     }
//     return true;
// }

bool GBApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

// #if wxUSE_LIBPNG
//     wxImage::AddHandler( new wxPNGHandler );
// #endif
// #if wxUSE_LIBJPEG
//     wxImage::AddHandler(new wxJPEGHandler);
// #endif
// #if wxUSE_GIF
//     wxImage::AddHandler(new wxGIFHandler);
// #endif
// #if wxUSE_PCX
//     wxImage::AddHandler(new wxPCXHandler);
// #endif
// #if wxUSE_PNM
//     wxImage::AddHandler(new wxPNMHandler);
// #endif
// #if wxUSE_LIBTIFF
//     wxImage::AddHandler(new wxTIFFHandler);
// #endif
// #if wxUSE_TGA
//     wxImage::AddHandler(new wxTGAHandler);
// #endif
// #if wxUSE_IFF
//     wxImage::AddHandler(new wxIFFHandler);
// #endif
// #if wxUSE_XPM
//     wxImage::AddHandler(new wxXPMHandler);
// #endif
// #if wxUSE_ICO_CUR
//     wxImage::AddHandler(new wxICOHandler);
//     wxImage::AddHandler(new wxCURHandler);
//     wxImage::AddHandler(new wxANIHandler);
// #endif
    wxInitAllImageHandlers();

    const wxLanguageInfo* langInfo = wxUILocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
    const wxString langDesc = langInfo ? langInfo->Description : wxString("the default system language");
    if ( !wxUILocale::UseDefault() )
    {
        wxLogWarning("Failed to initialize the default system locale.");
    }
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix(".");
    wxTranslations* const trans = new wxTranslations();
    wxTranslations::Set(trans);
    if ( !trans->AddCatalog("internat") )
    {
        wxLogError(_("Couldn't find/load 'internat' catalog for %s."), langDesc);
    }
    trans->AddCatalog("wxstd");
#ifdef USE_COREUTILS_MO
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix("/usr/share/locale");
    g_loadedCoreutilsMO = trans->AddCatalog("coreutils");
#endif // USE_COREUTILS_MO

    wxFrame* frame = new GBFrame(nullptr, wxID_ANY, _("Game Builder Application"), wxDefaultPosition, wxWindow::FromDIP(wxSize(800, 600), nullptr));
    frame->Show();

    return true;
}

wxBEGIN_EVENT_TABLE(GBFrame, wxFrame)
    EVT_ERASE_BACKGROUND(GBFrame::OnEraseBackground)
    EVT_SIZE(GBFrame::OnSize)
    EVT_MENU(GBFrame::ID_AddLevel, GBFrame::OnAddLevel)
    EVT_MENU(GBFrame::ID_ShowLog, GBFrame::OnShowLog)
    EVT_MENU(GBFrame::ID_ShowTree, GBFrame::OnShowTree)
    EVT_MENU(GBFrame::ID_ShowNotebook, GBFrame::OnShowNotebook)
    EVT_MENU(GBFrame::ID_CreatePerspective, GBFrame::OnCreatePerspective)
    EVT_MENU(GBFrame::ID_CopyPerspectiveCode, GBFrame::OnCopyPerspectiveCode)
    EVT_MENU(ID_AllowFloating, GBFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentHint, GBFrame::OnManagerFlag)
    EVT_MENU(ID_VenetianBlindsHint, GBFrame::OnManagerFlag)
    EVT_MENU(ID_RectangleHint, GBFrame::OnManagerFlag)
    EVT_MENU(ID_NoHint, GBFrame::OnManagerFlag)
    EVT_MENU(ID_HintFade, GBFrame::OnManagerFlag)
    EVT_MENU(ID_NoVenetianFade, GBFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentDrag, GBFrame::OnManagerFlag)
    EVT_MENU(ID_LiveUpdate, GBFrame::OnManagerFlag)
    EVT_MENU(ID_AllowActivePane, GBFrame::OnManagerFlag)
    EVT_MENU(ID_NotebookTabFixedWidth, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookNoCloseButton, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButton, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonAll, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonActive, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabMove, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabExternalMove, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabSplit, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookScrollButtons, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookWindowList, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtGloss, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtSimple, GBFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAlignTop,     GBFrame::OnTabAlignment)
    EVT_MENU(ID_NotebookAlignBottom,  GBFrame::OnTabAlignment)
    //EVT_MENU(ID_NotebookNewTab, GBFrame::OnNotebookNewTab)
    //EVT_MENU(ID_NotebookDeleteTab, GBFrame::OnNotebookDeleteTab)
    EVT_MENU(ID_NoGradient, GBFrame::OnGradient)
    EVT_MENU(ID_VerticalGradient, GBFrame::OnGradient)
    EVT_MENU(ID_HorizontalGradient, GBFrame::OnGradient)
    EVT_MENU(ID_AllowToolbarResizing, GBFrame::OnToolbarResizing)
    EVT_MENU(ID_Settings, GBFrame::OnSettings)
    EVT_MENU(ID_CustomizeToolbar, GBFrame::OnCustomizeToolbar)
    EVT_MENU(wxID_SAVE, GBFrame::OnSave)
    EVT_MENU(wxID_EXIT, GBFrame::OnExit)
    EVT_MENU(wxID_ABOUT, GBFrame::OnAbout)
    EVT_UPDATE_UI(ID_NotebookTabFixedWidth, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookNoCloseButton, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButton, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonAll, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonActive, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabMove, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabExternalMove, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabSplit, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookScrollButtons, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookWindowList, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowFloating, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentHint, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VenetianBlindsHint, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_RectangleHint, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoHint, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HintFade, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoVenetianFade, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentDrag, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_LiveUpdate, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoGradient, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VerticalGradient, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HorizontalGradient, GBFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowToolbarResizing, GBFrame::OnUpdateUI)
    EVT_MENU_RANGE(GBFrame::ID_FirstPerspective, GBFrame::ID_FirstPerspective+1000,
                   GBFrame::OnRestorePerspective)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_DropDownToolbarItem, GBFrame::OnDropDownToolbarItem)
    //EVT_AUI_PANE_CLOSE(GBFrame::OnPaneClose)
    EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, GBFrame::OnAllowNotebookDnD)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, GBFrame::OnNotebookPageClose)
    EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, GBFrame::OnNotebookPageClosed)
    EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, GBFrame::OnNotebookPageChanging)
wxEND_EVENT_TABLE()


GBFrame::GBFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(parent, id, title, pos, size, style)
{
    m_mgr.SetManagedWindow(this);

    SetIcon(wxIcon(sample_xpm));

    m_notebook_style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
    m_notebook_theme = 0;

    wxMenuBar* mb = new wxMenuBar;

    wxMenu* file_menu = new wxMenu;
    file_menu->Append(ID_AddLevel, wxT("➕New Level\tCtrl-A"));
    file_menu->Append(wxID_OPEN, wxT("📂Open\tCtrl-O"));
    file_menu->Append(wxID_SAVE, wxT("💾Save\tCtrl-S"));
    file_menu->Append(wxID_EXIT, wxT("🚫Quit\tCtrl-Q"));

    wxMenu* view_menu = new wxMenu;
    view_menu->Append(ID_ShowLog, _("Show Log"));
    view_menu->Append(ID_ShowTree, _("Show Tree"));
    view_menu->Append(ID_ShowNotebook, _("Show Notebook"));

    wxMenu* options_menu = new wxMenu;
    options_menu->AppendRadioItem(ID_TransparentHint, _("Transparent Hint"));
    options_menu->AppendRadioItem(ID_VenetianBlindsHint, _("Venetian Blinds Hint"));
    options_menu->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
    options_menu->AppendRadioItem(ID_NoHint, _("No Hint"));
    options_menu->AppendSeparator();
    options_menu->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
    options_menu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
    options_menu->AppendCheckItem(ID_NoVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
    options_menu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
    options_menu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
    if ( !wxAuiManager::AlwaysUsesLiveResize() )
        options_menu->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
    options_menu->AppendSeparator();
    options_menu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
    options_menu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
    options_menu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
    options_menu->AppendSeparator();
    options_menu->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
    options_menu->AppendSeparator();
    options_menu->Append(ID_Settings, _("Settings Pane"));

    wxMenu* notebook_menu = new wxMenu;
    notebook_menu->AppendRadioItem(ID_NotebookArtGloss, _("Glossy Theme (Default)"));
    notebook_menu->AppendRadioItem(ID_NotebookArtSimple, _("Simple Theme"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendRadioItem(ID_NotebookNoCloseButton, _("No Close Button"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButton, _("Close Button at Right"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButtonAll, _("Close Button on All Tabs"));
    notebook_menu->AppendRadioItem(ID_NotebookCloseButtonActive, _("Close Button on Active Tab"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendRadioItem(ID_NotebookAlignTop, _("Tab Top Alignment"));
    notebook_menu->AppendRadioItem(ID_NotebookAlignBottom, _("Tab Bottom Alignment"));
    notebook_menu->AppendSeparator();
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabMove, _("Allow Tab Move"));
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabExternalMove, _("Allow External Tab Move"));
    notebook_menu->AppendCheckItem(ID_NotebookAllowTabSplit, _("Allow Notebook Split"));
    notebook_menu->AppendCheckItem(ID_NotebookScrollButtons, _("Scroll Buttons Visible"));
    notebook_menu->AppendCheckItem(ID_NotebookWindowList, _("Window List Button Visible"));
    notebook_menu->AppendCheckItem(ID_NotebookTabFixedWidth, _("Fixed-width Tabs"));

    m_perspectives_menu = new wxMenu;
    m_perspectives_menu->Append(ID_CreatePerspective, _("Create Perspective"));
    m_perspectives_menu->Append(ID_CopyPerspectiveCode, _("Copy Perspective Data To Clipboard"));
    m_perspectives_menu->AppendSeparator();
    m_perspectives_menu->Append(ID_FirstPerspective+0, _("Default Startup"));
    m_perspectives_menu->Append(ID_FirstPerspective+1, _("All Panes"));

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, wxT("ℹ️About"));

    mb->Append(file_menu, _("&File"));
    mb->Append(view_menu, _("&View"));
    mb->Append(m_perspectives_menu, _("&Perspectives"));
    mb->Append(options_menu, _("&Options"));
    mb->Append(notebook_menu, _("&Notebook"));
    mb->Append(help_menu, _("&Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));

    SetMinSize(FromDIP(wxSize(400,300)));

    wxAuiToolBarItemArray prepend_items;
    wxAuiToolBarItemArray append_items;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    append_items.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(ID_CustomizeToolbar);
    item.SetLabel(_("Customize..."));
    append_items.Add(item);

    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->AddTool(wxID_OPEN, "&Open", wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN), "Open another project from file");
    tb1->AddTool(wxID_SAVE, "&Save", wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE), "Save current project to file");
    tb1->AddSeparator();
    tb1->AddTool(wxID_COPY, "&Copy", wxArtProvider::GetBitmapBundle(wxART_COPY), "Copy selected");
    tb1->AddTool(wxID_PASTE, "&Paste", wxArtProvider::GetBitmapBundle(wxART_PASTE), "Paste from buffer");
    tb1->AddSeparator();
    tb1->AddTool(wxID_EXIT, "&Quit", wxArtProvider::GetBitmapBundle(wxART_QUIT), "Quit from app");
    tb1->SetCustomOverflowItems(prepend_items, append_items);
    tb1->Realize();

    m_tree_ctrl = CreateTreeCtrl();
    m_mgr.AddPane(m_tree_ctrl, wxAuiPaneInfo().Name("tree").Caption("Tree Pane").Left().Layer(1).Position(1).CloseButton(true).MaximizeButton(true));

    int iconSize = m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
    iconSize &= ~1;

    wxTextCtrl* m_logTextCtrl = CreateTextCtrl("Start...\n");
    m_log = wxLog::SetActiveTarget(new wxLogTextCtrl(m_logTextCtrl));

    m_mgr.AddPane(m_logTextCtrl, wxAuiPaneInfo().Name("log").Caption("Log").Bottom().Layer(1).Position(1).Icon(wxArtProvider::GetBitmapBundle(wxART_WARNING, wxART_OTHER, wxSize(iconSize, iconSize))));

    m_mgr.AddPane(new SettingsPanel(this,this), wxAuiPaneInfo().Name("settings").Caption("Dock Manager Settings").Dockable(false).Float().Hide());

    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name("notebook").CenterPane().PaneBorder(false).Caption("notebook").Dock().CloseButton(true).MaximizeButton(true));

    m_propGridManager = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_AUTO_SORT | wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
    m_propGrid = m_propGridManager->GetGrid();
    m_propGridManager->SetExtraStyle(wxPG_EX_MODE_BUTTONS | wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MULTIPLE_SELECTION);

    wxPropertyGridPage* page = m_propGridManager->AddPage("Texture");
    page->Append( new wxPropertyCategory("texture path") );
    m_mgr.AddPane(m_propGridManager, wxAuiPaneInfo().Name("property-grid").Right().PaneBorder(false).Caption("properties").Dock().CloseButton(true));
    
    // add the toolbars to the manager
    m_mgr.AddPane(tb1, wxAuiPaneInfo().Name("tb1").Caption("Toolbar").ToolbarPane().Top());

    // make some default perspectives
    wxString perspective_all = m_mgr.SavePerspective();

    //int i, count;
    //wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    //for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    //    if (!all_panes.Item(i).IsToolbar())
    //        all_panes.Item(i).Hide();
    //m_mgr.GetPane("tb1").Hide();
    m_mgr.GetPane("tree").Show().Left().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("log").Show().Bottom().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("notebook").Show();
    wxString perspective_default = m_mgr.SavePerspective();

    m_perspectives.Add(perspective_default);
    m_perspectives.Add(perspective_all);

    m_mgr.Update();
}

wxAuiDockArt* GBFrame::GetDockArt()
{
    return m_mgr.GetArtProvider();
}

void GBFrame::DoUpdate()
{
    m_mgr.Update();
}

void GBFrame::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void GBFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void GBFrame::OnSettings(wxCommandEvent& WXUNUSED(evt))
{
    wxAuiPaneInfo& floating_pane = m_mgr.GetPane("settings").Float().Show();

    if (floating_pane.floating_pos == wxDefaultPosition)
        floating_pane.FloatingPosition(GetStartPosition());

    m_mgr.Update();
}

void GBFrame::OnCustomizeToolbar(wxCommandEvent& WXUNUSED(evt))
{
    wxMessageBox(_("Customize Toolbar clicked"));
}

void GBFrame::OnGradient(wxCommandEvent& event)
{
    int gradient = 0;

    switch (event.GetId())
    {
        case ID_NoGradient:         gradient = wxAUI_GRADIENT_NONE; break;
        case ID_VerticalGradient:   gradient = wxAUI_GRADIENT_VERTICAL; break;
        case ID_HorizontalGradient: gradient = wxAUI_GRADIENT_HORIZONTAL; break;
    }

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, gradient);
    m_mgr.Update();
}

void GBFrame::OnToolbarResizing(wxCommandEvent& WXUNUSED(evt))
{
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t count = all_panes.GetCount();
    for (size_t i = 0; i < count; ++i)
    {
        wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
        if (toolbar)
        {
            all_panes[i].Resizable(!all_panes[i].IsResizable());
        }
    }

    m_mgr.Update();
}

void GBFrame::OnManagerFlag(wxCommandEvent& event)
{
    unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
    if (event.GetId() == ID_TransparentDrag ||
        event.GetId() == ID_TransparentHint ||
        event.GetId() == ID_HintFade)
    {
        wxMessageBox("This option is presently only available on wxGTK, wxMSW and wxMac");
        return;
    }
#endif

    int id = event.GetId();

    if (id == ID_TransparentHint ||
        id == ID_VenetianBlindsHint ||
        id == ID_RectangleHint ||
        id == ID_NoHint)
    {
        unsigned int flags = m_mgr.GetFlags();
        flags &= ~wxAUI_MGR_TRANSPARENT_HINT;
        flags &= ~wxAUI_MGR_VENETIAN_BLINDS_HINT;
        flags &= ~wxAUI_MGR_RECTANGLE_HINT;
        m_mgr.SetFlags(flags);
    }

    switch (id)
    {
        case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
        case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
        case ID_HintFade: flag = wxAUI_MGR_HINT_FADE; break;
        case ID_NoVenetianFade: flag = wxAUI_MGR_NO_VENETIAN_BLINDS_FADE; break;
        case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
        case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
        case ID_VenetianBlindsHint: flag = wxAUI_MGR_VENETIAN_BLINDS_HINT; break;
        case ID_RectangleHint: flag = wxAUI_MGR_RECTANGLE_HINT; break;
        case ID_LiveUpdate: flag = wxAUI_MGR_LIVE_RESIZE; break;
    }

    if (flag)
    {
        m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);
    }

    m_mgr.Update();
}


void GBFrame::OnNotebookFlag(wxCommandEvent& event)
{
    int id = event.GetId();

    if (id == ID_NotebookNoCloseButton ||
        id == ID_NotebookCloseButton ||
        id == ID_NotebookCloseButtonAll ||
        id == ID_NotebookCloseButtonActive)
    {
        m_notebook_style &= ~(wxAUI_NB_CLOSE_BUTTON |
                              wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                              wxAUI_NB_CLOSE_ON_ALL_TABS);

        switch (id)
        {
            case ID_NotebookNoCloseButton: break;
            case ID_NotebookCloseButton: m_notebook_style |= wxAUI_NB_CLOSE_BUTTON; break;
            case ID_NotebookCloseButtonAll: m_notebook_style |= wxAUI_NB_CLOSE_ON_ALL_TABS; break;
            case ID_NotebookCloseButtonActive: m_notebook_style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB; break;
        }
    }

    if (id == ID_NotebookAllowTabMove)
    {
        m_notebook_style ^= wxAUI_NB_TAB_MOVE;
    }
    if (id == ID_NotebookAllowTabExternalMove)
    {
        m_notebook_style ^= wxAUI_NB_TAB_EXTERNAL_MOVE;
    }
     else if (id == ID_NotebookAllowTabSplit)
    {
        m_notebook_style ^= wxAUI_NB_TAB_SPLIT;
    }
     else if (id == ID_NotebookWindowList)
    {
        m_notebook_style ^= wxAUI_NB_WINDOWLIST_BUTTON;
    }
     else if (id == ID_NotebookScrollButtons)
    {
        m_notebook_style ^= wxAUI_NB_SCROLL_BUTTONS;
    }
     else if (id == ID_NotebookTabFixedWidth)
    {
        m_notebook_style ^= wxAUI_NB_TAB_FIXED_WIDTH;
    }


    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

            if (id == ID_NotebookArtGloss)
            {
                nb->SetArtProvider(new wxAuiDefaultTabArt);
                m_notebook_theme = 0;
            }
             else if (id == ID_NotebookArtSimple)
            {
                nb->SetArtProvider(new wxAuiSimpleTabArt);
                m_notebook_theme = 1;
            }

            nb->SetWindowStyleFlag(m_notebook_style);
            nb->Refresh();
        }
    }
}


void GBFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    unsigned int flags = m_mgr.GetFlags();

    switch (event.GetId())
    {
        case ID_NoGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE);
            break;
        case ID_VerticalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL);
            break;
        case ID_HorizontalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL);
            break;
        case ID_AllowToolbarResizing:
        {
            wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
            const size_t count = all_panes.GetCount();
            for (size_t i = 0; i < count; ++i)
            {
                wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
                if (toolbar)
                {
                    event.Check(all_panes[i].IsResizable());
                    break;
                }
            }
            break;
        }
        case ID_AllowFloating:
            event.Check((flags & wxAUI_MGR_ALLOW_FLOATING) != 0);
            break;
        case ID_TransparentDrag:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_DRAG) != 0);
            break;
        case ID_TransparentHint:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_HINT) != 0);
            break;
        case ID_LiveUpdate:
            event.Check((flags & wxAUI_MGR_LIVE_RESIZE) != 0);
            break;
        case ID_VenetianBlindsHint:
            event.Check((flags & wxAUI_MGR_VENETIAN_BLINDS_HINT) != 0);
            break;
        case ID_RectangleHint:
            event.Check((flags & wxAUI_MGR_RECTANGLE_HINT) != 0);
            break;
        case ID_NoHint:
            event.Check(((wxAUI_MGR_TRANSPARENT_HINT |
                          wxAUI_MGR_VENETIAN_BLINDS_HINT |
                          wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
            break;
        case ID_HintFade:
            event.Check((flags & wxAUI_MGR_HINT_FADE) != 0);
            break;
        case ID_NoVenetianFade:
            event.Check((flags & wxAUI_MGR_NO_VENETIAN_BLINDS_FADE) != 0);
            break;

        case ID_NotebookNoCloseButton:
            event.Check((m_notebook_style & (wxAUI_NB_CLOSE_BUTTON|wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_CLOSE_ON_ACTIVE_TAB)) == 0);
            break;
        case ID_NotebookCloseButton:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_BUTTON) != 0);
            break;
        case ID_NotebookCloseButtonAll:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ALL_TABS) != 0);
            break;
        case ID_NotebookCloseButtonActive:
            event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ACTIVE_TAB) != 0);
            break;
        case ID_NotebookAllowTabSplit:
            event.Check((m_notebook_style & wxAUI_NB_TAB_SPLIT) != 0);
            break;
        case ID_NotebookAllowTabMove:
            event.Check((m_notebook_style & wxAUI_NB_TAB_MOVE) != 0);
            break;
        case ID_NotebookAllowTabExternalMove:
            event.Check((m_notebook_style & wxAUI_NB_TAB_EXTERNAL_MOVE) != 0);
            break;
        case ID_NotebookScrollButtons:
            event.Check((m_notebook_style & wxAUI_NB_SCROLL_BUTTONS) != 0);
            break;
        case ID_NotebookWindowList:
            event.Check((m_notebook_style & wxAUI_NB_WINDOWLIST_BUTTON) != 0);
            break;
        case ID_NotebookTabFixedWidth:
            event.Check((m_notebook_style & wxAUI_NB_TAB_FIXED_WIDTH) != 0);
            break;
        case ID_NotebookArtGloss:
            event.Check(m_notebook_style == 0);
            break;
        case ID_NotebookArtSimple:
            event.Check(m_notebook_style == 1);
            break;
    }
}

// void GBFrame::OnNotebookDeleteTab(wxCommandEvent& WXUNUSED(evt))
// {
//     auto* const book =
//         wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook").window);
// 
//     auto numPages = book->GetPageCount();
//     if ( !numPages )
//     {
//         wxLogWarning("No pages to delete.");
//         return;
//     }
// 
//     book->DeletePage(numPages - 1);
// }


// void GBFrame::OnPaneClose(wxAuiManagerEvent& evt)
// {
//     if (evt.pane->name == "test10")
//     {
//         int res = wxMessageBox("Are you sure you want to close/hide this pane?", "wxAUI", wxYES_NO, this);
//         if (res != wxYES)
//             evt.Veto();
//     }
// }

void GBFrame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dlg(this, "Enter a name for the new perspective:", "wxAUI perspective");

    dlg.SetValue(wxString::Format("Perspective %u", unsigned(m_perspectives.GetCount() + 1)));
    if (dlg.ShowModal() != wxID_OK)
        return;

    if (m_perspectives.GetCount() == 0)
    {
        m_perspectives_menu->AppendSeparator();
    }

    m_perspectives_menu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
    m_perspectives.Add(m_mgr.SavePerspective());
}

void GBFrame::OnCopyPerspectiveCode(wxCommandEvent& WXUNUSED(evt))
{
    wxString s = m_mgr.SavePerspective();

#if wxUSE_CLIPBOARD
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(s));
        wxTheClipboard->Close();
    }
#endif
}

void GBFrame::OnRestorePerspective(wxCommandEvent& evt)
{
    m_mgr.LoadPerspective(m_perspectives.Item(evt.GetId() - ID_FirstPerspective));
}

void GBFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
    {
        int res = wxMessageBox("Are you sure you want to close/hide this notebook page?",
            "wxAUI", wxYES_NO, this);
        if (res != wxYES)
            evt.Veto();
    }
}

void GBFrame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    wxUnusedVar(ctrl);

    // selection should always be a valid index
    wxASSERT_MSG( ctrl->GetSelection() < (int)ctrl->GetPageCount(),
        wxString::Format("Invalid selection %d, only %d pages left",
        ctrl->GetSelection(), (int)ctrl->GetPageCount()) );

    evt.Skip();
}

void GBFrame::OnNotebookPageChanging(wxAuiNotebookEvent& evt)
{
    if ( evt.GetOldSelection() == 3 )
    {
        if ( wxMessageBox( "Are you sure you want to leave this page?\n(This demonstrates veto-ing)", "wxAUI", wxICON_QUESTION | wxYES_NO, this ) != wxYES )
        {
            evt.Veto();
        }
    }
}

void GBFrame::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
    // explicitly allow all notebook drag and drop events
    evt.Allow();
}

wxPoint GBFrame::GetStartPosition()
{
    static int x = 0;
    x += FromDIP(20);
    wxPoint pt = ClientToScreen(wxPoint(0,0));
    return wxPoint(pt.x + x, pt.y + x);
}

void GBFrame::OnShowLog(wxCommandEvent& event)
{
    m_mgr.GetPane("log").Show(event.GetId() == ID_ShowLog);
    m_mgr.Update();
}

void GBFrame::OnShowTree(wxCommandEvent& event)
{
    m_mgr.GetPane("tree").Show(event.GetId() == ID_ShowTree);
    m_mgr.Update();
}

void GBFrame::OnShowNotebook(wxCommandEvent& event)
{
    m_mgr.GetPane("notebook").Show(event.GetId() == ID_ShowNotebook);
    m_mgr.Update();
}

void GBFrame::OnAddLevel(wxCommandEvent& WXUNUSED(event))
{
    auto* const book = wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook").window);
    wxString level_name = wxString::Format("level-%zu", book->GetPageCount() + 1);
    MapBoardCtrl* map_ctrl = new MapBoardCtrl(book, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL, level_name, &m_mgr, m_tree_ctrl);
    book->AddPage(map_ctrl, level_name, true);
    levels[level_name] = map_ctrl;
}

void GBFrame::OnDropDownToolbarItem(wxAuiToolBarEvent& evt)
{
    if (evt.IsDropDownClicked())
    {
        wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

        tb->SetToolSticky(evt.GetId(), true);

        wxMenu menuPopup;

        // TODO: Use GetBitmapBundle() when wxMenuItem is updated to use it too.
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, FromDIP(wxSize(16,16)));

        wxMenuItem* m1 =  new wxMenuItem(&menuPopup, 10001, _("Drop Down Item 1"));
        m1->SetBitmap(bmp);
        menuPopup.Append(m1);

        wxMenuItem* m2 =  new wxMenuItem(&menuPopup, 10002, _("Drop Down Item 2"));
        m2->SetBitmap(bmp);
        menuPopup.Append(m2);

        wxMenuItem* m3 =  new wxMenuItem(&menuPopup, 10003, _("Drop Down Item 3"));
        m3->SetBitmap(bmp);
        menuPopup.Append(m3);

        wxMenuItem* m4 =  new wxMenuItem(&menuPopup, 10004, _("Drop Down Item 4"));
        m4->SetBitmap(bmp);
        menuPopup.Append(m4);

        // line up our menu with the button
        wxRect rect = tb->GetToolRect(evt.GetId());
        wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);

        PopupMenu(&menuPopup, pt);

        // make sure the button is "un-stuck"
        tb->SetToolSticky(evt.GetId(), false);
    }
}


void GBFrame::OnTabAlignment(wxCommandEvent &evt)
{
    size_t i, count;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
        {
            wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

            long style = nb->GetWindowStyleFlag();
            style &= ~(wxAUI_NB_TOP | wxAUI_NB_BOTTOM);
            if (evt.GetId() == ID_NotebookAlignTop)
                style |= wxAUI_NB_TOP;
            else if (evt.GetId() == ID_NotebookAlignBottom)
                style |= wxAUI_NB_BOTTOM;
            nb->SetWindowStyleFlag(style);

            nb->Refresh();
        }
    }
}

void GBFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JSON data (*.json)|*.json;*.JSON";
    wxFileDialog dlg(this, "Save as JSON", wxEmptyString, wxEmptyString, wildCard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(dlg.ShowModal() == wxID_OK)
    {
        MapBoardCtrl* map_board = levels[wxT("level-0")];
        //map_board->LogMessage(dlg.GetPath());
        map_board->LevelToFile(dlg.GetPath());
    }
}

void GBFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void GBFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf("Game Builder\ncreate game without programming, but optional you always can replace core engine (source code included)\nBuild with GCC %s\nRun under %s\n(c) Copyright 2024, Maxim Kolosov", __VERSION__, wxPlatformInfo::Get().GetOperatingSystemIdName());
    wxMessageBox(msg, _("About Game Builder"), wxOK, this);
}

wxTextCtrl* GBFrame::CreateTextCtrl(const wxString& ctrl_text)
{
    wxString text = ctrl_text.empty() ? "\n" : ctrl_text;
    return new wxTextCtrl(this,wxID_ANY, text, wxPoint(0,0), FromDIP(wxSize(150,90)), wxNO_BORDER | wxTE_MULTILINE);
}

wxTreeCtrl* GBFrame::CreateTreeCtrl()
{
    wxTreeCtrl* m_tree = new wxTreeCtrl(this, wxID_ANY, wxPoint(0,0), FromDIP(wxSize(160,250)), wxTR_DEFAULT_STYLE | wxNO_BORDER);

    wxSize size(16, 16);
    wxVector<wxBitmapBundle> images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, size));
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, size));
    m_tree->SetImages(images);

    wxTreeItemId root = m_tree->AddRoot("Game Builder Project", 0);
    m_tree->Expand(root);

    return m_tree;
}

wxAuiNotebook* GBFrame::CreateNotebook()
{
    wxSize client_size = GetClientSize();

    m_notebook_ctrl = new wxAuiNotebook(this, wxID_ANY, wxPoint(client_size.x, client_size.y), FromDIP(wxSize(430,200)), m_notebook_style);
    m_notebook_ctrl->Freeze();

    wxBitmapBundle page_bmp = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));

    MapBoardCtrl* map_ctrl = new MapBoardCtrl(m_notebook_ctrl, wxID_ANY, wxDefaultPosition, client_size, wxHSCROLL | wxVSCROLL, "level-0", &m_mgr, m_tree_ctrl);
    m_notebook_ctrl->AddPage(map_ctrl, "level-0" , false, page_bmp);
    levels[wxT("level-0")] = map_ctrl;

    //m_notebook_ctrl->SetPageToolTip(m_notebook_ctrl->GetPageCount()-1, "level-0");

    m_notebook_ctrl->Thaw();
    return m_notebook_ctrl;
}
