///////////////////////////////////////////////////////////////////////////////
// Name:        gb.cpp
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-02-21
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

#include "gb.h"

void GBApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxApp::OnInitCmdLine(parser);
	parser.AddParam("project", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}

// bool GBApp::OnCmdLineParsed(wxCmdLineParser& parser)
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
    if(!wxHandleFatalExceptions())
		return false;

	if ( !wxApp::OnInit() )
		return false;

	wxInitAllImageHandlers();

	const wxLanguageInfo* langInfo = wxUILocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
	const wxString langDesc = langInfo ? langInfo->Description : wxString("the default system language");
	if ( !wxUILocale::UseDefault() )
		wxLogWarning("Failed to initialize the default system locale.");

	wxFileTranslationsLoader::AddCatalogLookupPathPrefix(".");
	wxTranslations* const trans = new wxTranslations();
	wxTranslations::Set(trans);
	if ( !trans->AddCatalog("internat") )
		wxLogError(_("Couldn't find/load 'internat' catalog for %s."), langDesc);
	trans->AddCatalog("wxstd");
#ifdef USE_COREUTILS_MO
	wxFileTranslationsLoader::AddCatalogLookupPathPrefix("/usr/share/locale");
	g_loadedCoreutilsMO = trans->AddCatalog("coreutils");
#endif // USE_COREUTILS_MO

	path_projects = wxGetCwd()+"/projects";
	path_assets = wxGetCwd()+"/assets";
	path_assets_images = path_assets+"/images";
	path_plugins = wxGetCwd()+"/plugins";

	GBFrame* frame = new GBFrame(nullptr, wxID_ANY, _("Game Builder Application"), wxDefaultPosition, wxWindow::FromDIP(wxSize(800, 600), nullptr));
	frame->Show();

	if(!wxFileName::DirExists(path_projects))
	{
		if(wxFileName::Mkdir(path_projects))
			wxLogMessage("✅"+path_projects);
		else
			wxLogMessage("❌"+path_projects);
	}

	if(!wxFileName::DirExists(path_assets))
	{
		if(wxFileName::Mkdir(path_assets))
			wxLogMessage("✅"+path_assets);
		else
			wxLogMessage("❌"+path_assets);
	}

	if(!wxFileName::DirExists(path_assets_images))
	{
		if(wxFileName::Mkdir(path_assets_images))
			wxLogMessage("✅"+path_assets_images);
		else
			wxLogMessage("❌"+path_assets_images);
	}

	if(!wxFileName::DirExists(path_plugins))
	{
		if(wxFileName::Mkdir(path_plugins))
			wxLogMessage("✅"+path_plugins);
		else
			wxLogMessage("❌"+path_plugins);
	}

	return true;
}

int GBApp::OnExit()
{
#if DEBUG
    std::cout << "!!! GBApp::OnExit !!!" << std::endl;
#endif
    return 0;
}

void GBApp::ShowException()
{
    wxString error;
    try {
        throw;
    } catch (const std::exception& e) {
        error = e.what();
    } catch ( ... ) {
        error = "UNKNOWN ERROR";
    }
    wxLogError("Unexpected exception has occurred: %s, the program will terminate.", error);
#if DEBUG
    std::cout << "!!! GBApp::OnExceptionInMainLoop !!!" << std::endl;
    std::cout << "!!! " << error << " !!!" << std::endl;
#endif
}

void GBApp::OnFatalException()
{
#if DEBUG
    std::cout << "!!! GBApp::OnFatalException !!!" << std::endl;
#endif
    ShowException();
}

bool GBApp::OnExceptionInMainLoop()
{
    ShowException();
    return false;
}


wxIMPLEMENT_DYNAMIC_CLASS(GBFrame, wxFrame);

GBFrame::GBFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(parent, id, title, pos, size, style)
{
    m_mgr.SetManagedWindow(this);

    //SetIcon(wxIcon(gb_icon_xpm));
    SetIcons(wxIconBundle(gb_icon_xpm));

    m_notebook_style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
    m_notebook_theme = 0;

    wxMenuBar* mb = new wxMenuBar;

    wxMenu* menu_file = new wxMenu;
    menu_file->Append(ID_AddLevel, wxT("➕New Level\tCtrl-N"));
    menu_file->Append(wxID_OPEN, wxT("📂Open\tCtrl-O"));
    menu_file->Append(ID_OpenLevel, wxT("🗂Open Level\tCtrl-L"));
    menu_file->Append(wxID_SAVE, wxT("💾Save\tCtrl-S"));
    menu_file->Append(wxID_FILE, wxT("📥Save level\tCtrl-M"));
    menu_file->Append(wxID_EXIT, wxT("🚫Quit\tCtrl-Q"));

    wxMenu* menu_view = new wxMenu;
    menu_view->Append(ID_ShowLog, _("Show Log"));
    menu_view->Append(ID_ShowTree, _("Show Tree"));
    menu_view->Append(ID_ShowNotebook, _("Show Notebook"));
    menu_view->Append(ID_ShowNotebook, _("Show Cell Properties"));
    menu_view->Append(ID_DrawCellCoords, _("Draw Cell Coords on/off"));

    wxMenu* menu_options = new wxMenu;
    menu_options->AppendRadioItem(ID_TransparentHint, _("Transparent Hint"));
    menu_options->AppendRadioItem(ID_VenetianBlindsHint, _("Venetian Blinds Hint"));
    menu_options->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
    menu_options->AppendRadioItem(ID_NoHint, _("No Hint"));
    menu_options->AppendSeparator();
    menu_options->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
    menu_options->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
    menu_options->AppendCheckItem(ID_NoVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
    menu_options->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
    menu_options->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
    if ( !wxAuiManager::AlwaysUsesLiveResize() )
        menu_options->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
    menu_options->AppendSeparator();
    menu_options->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
    menu_options->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
    menu_options->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
    menu_options->AppendSeparator();
    menu_options->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
    menu_options->AppendSeparator();
    menu_options->Append(ID_Settings, _("Settings Pane"));

    wxMenu* menu_notebook = new wxMenu;
    menu_notebook->AppendRadioItem(ID_NotebookArtGloss, _("Glossy Theme (Default)"));
    menu_notebook->AppendRadioItem(ID_NotebookArtSimple, _("Simple Theme"));
    menu_notebook->AppendSeparator();
    menu_notebook->AppendRadioItem(ID_NotebookNoCloseButton, _("No Close Button"));
    menu_notebook->AppendRadioItem(ID_NotebookCloseButton, _("Close Button at Right"));
    menu_notebook->AppendRadioItem(ID_NotebookCloseButtonAll, _("Close Button on All Tabs"));
    menu_notebook->AppendRadioItem(ID_NotebookCloseButtonActive, _("Close Button on Active Tab"));
    menu_notebook->AppendSeparator();
    menu_notebook->AppendRadioItem(ID_NotebookAlignTop, _("Tab Top Alignment"));
    menu_notebook->AppendRadioItem(ID_NotebookAlignBottom, _("Tab Bottom Alignment"));
    menu_notebook->AppendSeparator();
    menu_notebook->AppendCheckItem(ID_NotebookAllowTabMove, _("Allow Tab Move"));
    menu_notebook->AppendCheckItem(ID_NotebookAllowTabExternalMove, _("Allow External Tab Move"));
    menu_notebook->AppendCheckItem(ID_NotebookAllowTabSplit, _("Allow Notebook Split"));
    menu_notebook->AppendCheckItem(ID_NotebookScrollButtons, _("Scroll Buttons Visible"));
    menu_notebook->AppendCheckItem(ID_NotebookWindowList, _("Window List Button Visible"));
    menu_notebook->AppendCheckItem(ID_NotebookTabFixedWidth, _("Fixed-width Tabs"));

    menu_perspectives = new wxMenu;
    menu_perspectives->Append(ID_CreatePerspective, _("Create Perspective"));
    menu_perspectives->Append(ID_CopyPerspectiveCode, _("Copy Perspective Data To Clipboard"));
    menu_perspectives->AppendSeparator();
    menu_perspectives->Append(ID_FirstPerspective+0, _("Default Startup"));
    menu_perspectives->Append(ID_FirstPerspective+1, _("All Panes"));

    menu_plugins = new wxMenu;
    wxArrayString files;
    if(wxDir::GetAllFiles(wxGetApp().path_plugins, &files, "*.py"))
    {
        for(size_t i = 0; i < files.GetCount(); i++)
        {
            wxString fpath = files.Item(i);
            int id = ID_FirstPlugin+1+i;
            m_plugins[id] = fpath;
            menu_plugins->Append(id, fpath.AfterLast('/').BeforeLast('.'));
        }
    }

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, wxT("ℹ️About"));

    mb->Append(menu_file, _("&File"));
    mb->Append(menu_view, _("&View"));
    mb->Append(menu_perspectives, _("&Perspectives"));
    mb->Append(menu_options, _("&Options"));
    mb->Append(menu_notebook, _("&Notebook"));
    mb->Append(menu_plugins, _("P&lugins"));
    mb->Append(help_menu, _("&Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));

    SetMinSize(FromDIP(wxSize(400,300)));

    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->AddTool(ID_AddLevel, "&New Level", wxArtProvider::GetBitmapBundle(wxART_NEW), _("New Level"));
    tb1->AddTool(wxID_OPEN, "&Open", wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN), _("Open another project from file"));
    tb1->AddTool(ID_OpenLevel, "Open Level", wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN), _("Open another level from file"));
    tb1->AddTool(wxID_SAVE, "&Save", wxArtProvider::GetBitmapBundle(wxART_FLOPPY), _("Save current project to file"));
    tb1->AddTool(wxID_FILE, "Save &Level", wxArtProvider::GetBitmapBundle(wxART_CDROM), _("Save current level to file"));
    tb1->AddSeparator();
    tb1->AddTool(wxID_COPY, "&Copy", wxArtProvider::GetBitmapBundle(wxART_COPY), _("Copy selected"));
    tb1->AddTool(wxID_PASTE, "&Paste", wxArtProvider::GetBitmapBundle(wxART_PASTE), _("Paste from buffer"));
    tb1->AddSeparator();
    tb1->AddTool(wxID_EXIT, "&Quit", wxArtProvider::GetBitmapBundle(wxART_QUIT), _("Quit from app"));
    tb1->Realize();

    m_tree_ctrl = CreateTreeCtrl();
    m_mgr.AddPane(m_tree_ctrl, wxAuiPaneInfo().Name("tree").Caption("Tree Pane").Left().Layer(1).Position(1).CloseButton(true).MaximizeButton(true));

    int iconSize = m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
    iconSize &= ~1;

    wxTextCtrl* m_logTextCtrl = CreateTextCtrl("Start from \""+wxGetCwd()+"\"\n");
    wxLog::SetActiveTarget(new wxLogTextCtrl(m_logTextCtrl));

    m_mgr.AddPane(m_logTextCtrl, wxAuiPaneInfo().Name("log").Caption("Log").Bottom().Layer(1).Position(1).Icon(wxArtProvider::GetBitmapBundle(wxART_WARNING, wxART_OTHER, wxSize(iconSize, iconSize))));

    m_mgr.AddPane(new SettingsPanel(this,this), wxAuiPaneInfo().Name("settings").Caption("Dock Manager Settings").Dockable(false).Float().Hide());

    m_propGridManager = new wxPropertyGridManager(this, PGID, wxDefaultPosition, wxDefaultSize, wxPG_AUTO_SORT | wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
    m_propGridManager->SetExtraStyle(wxPG_EX_MODE_BUTTONS | wxPG_EX_NATIVE_DOUBLE_BUFFERING | wxPG_EX_MULTIPLE_SELECTION);
    wxPropertyGridPage* page = m_propGridManager->AddPage("Common");
    page->Append(new wxStringProperty("coords", wxPG_LABEL, ""));
    page->SetPropertyHelpString("coords", _("coords of cell"));
    /////////////////////////////////
    wxPGProperty* prop0 = page->Append(new wxPropertyCategory("Floor"));
    page->SetPropertyCell(prop0, 0, wxPG_LABEL, wxArtProvider::GetBitmap(wxART_REPORT_VIEW));
    page->Append(new wxImageFileProperty("path_floor", wxPG_LABEL, ""));
    page->SetPropertyHelpString("path_floor", _("this is floor image file"));
    /////////////////////////////////
    wxPGProperty* prop1 = page->Append(new wxPropertyCategory("Wall"));
    page->SetPropertyCell(prop1, 0, wxPG_LABEL, wxArtProvider::GetBitmap(wxART_REPORT_VIEW));
    page->Append(new wxImageFileProperty("path_wall", wxPG_LABEL, ""));
    page->SetPropertyHelpString("path_wall", _("this is wall image file"));
    /////////////////////////////////
    wxPGProperty* prop2 = page->Append(new wxPropertyCategory("Roof"));
    page->SetPropertyCell(prop2, 0, wxPG_LABEL, wxArtProvider::GetBitmap(wxART_REPORT_VIEW));
    page->Append(new wxImageFileProperty("path_roof", wxPG_LABEL, ""));
    page->SetPropertyHelpString("path_roof", _("this is roof image file"));
    /////////////////////////////////
    wxPGProperty* prop3 = page->Append(new wxPropertyCategory("Wall Type"));
    page->SetPropertyCell(prop3, 0, wxPG_LABEL, wxArtProvider::GetBitmap(wxART_REPORT_VIEW));
    wxPGChoices choices;
    choices.Add("default", WT_DEFAULT);
    choices.Add("hero", WT_HERO);
    choices.Add("door", WT_DOOR);
    choices.Add("key", WT_KEY);
    choices.Add("nps", WT_NPS);
    page->Append(new wxEnumProperty("wall_type", wxPG_LABEL, choices, WT_DEFAULT));
    page->SetPropertyHelpString("wall_type", _("this is wall type, like hero, wall, key, nps"));
    /////////////////////////////////
    wxPGProperty* prop4 = page->Append(new wxPropertyCategory("Script"));
    page->SetPropertyCell(prop4, 0, wxPG_LABEL, wxArtProvider::GetBitmap(wxART_REPORT_VIEW));
    page->Append(new wxLongStringProperty("script", wxPG_LABEL, ""));
    page->SetPropertyHelpString("script", _("this is script content, may be any programming code"));
    /////////////////////////////////
    m_propGridManager->GetGrid()->Freeze();
    m_mgr.AddPane(m_propGridManager, wxAuiPaneInfo().Name("property-cell").BestSize(200,200).Right().PaneBorder(false).Caption("properties").Dock().CloseButton(true));
    
    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name("notebook").CenterPane().PaneBorder(false).Caption("notebook").Dock().CloseButton(true).MaximizeButton(true));

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

void GBFrame::OnPluginRun(wxCommandEvent& event)
{
#ifdef PLUGINS_PYTHON
    run_plugin(m_plugins[event.GetId()]);
#else
    wxLogWarning(_("Required rebuild with Python support"));
#endif // PLUGINS_PYTHON
}

void GBFrame::OnNotebookFlag(wxCommandEvent& event)
{
    int id = event.GetId();

    if (id == ID_NotebookNoCloseButton || id == ID_NotebookCloseButton || id == ID_NotebookCloseButtonAll || id == ID_NotebookCloseButtonActive)
    {
        m_notebook_style &= ~(wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_CLOSE_ON_ALL_TABS);
        switch (id)
        {
            case ID_NotebookNoCloseButton: break;
            case ID_NotebookCloseButton: m_notebook_style |= wxAUI_NB_CLOSE_BUTTON; break;
            case ID_NotebookCloseButtonAll: m_notebook_style |= wxAUI_NB_CLOSE_ON_ALL_TABS; break;
            case ID_NotebookCloseButtonActive: m_notebook_style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB; break;
        }
    }

    switch (id)
    {
        case ID_NotebookAllowTabMove: m_notebook_style ^= wxAUI_NB_TAB_MOVE;
        case ID_NotebookAllowTabExternalMove: m_notebook_style ^= wxAUI_NB_TAB_EXTERNAL_MOVE;
        case ID_NotebookAllowTabSplit: m_notebook_style ^= wxAUI_NB_TAB_SPLIT;
        case ID_NotebookWindowList: m_notebook_style ^= wxAUI_NB_WINDOWLIST_BUTTON;
        case ID_NotebookScrollButtons: m_notebook_style ^= wxAUI_NB_SCROLL_BUTTONS;
        case ID_NotebookTabFixedWidth: m_notebook_style ^= wxAUI_NB_TAB_FIXED_WIDTH;
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
            event.Check(((wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
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
        menu_perspectives->AppendSeparator();
    }

    menu_perspectives->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
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
    //wxAuiNotebook* m_notebook_ctrl = (wxAuiNotebook*)evt.GetEventObject();
    //if(m_notebook_ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(MapBoardCtrl)))
    m_boards.erase(m_notebook_ctrl->GetPageText(evt.GetSelection()));
    //evt.Veto();
}

void GBFrame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    wxUnusedVar(ctrl);
    // selection should always be a valid index
    wxASSERT_MSG(ctrl->GetSelection() < (int)ctrl->GetPageCount(), wxString::Format("Invalid selection %d, only %d pages left", ctrl->GetSelection(), (int)ctrl->GetPageCount()));
    evt.Skip();
}

// void GBFrame::OnNotebookPageChanging(wxAuiNotebookEvent& evt)
// {
//     if ( evt.GetOldSelection() == 3 )
//     {
//         if ( wxMessageBox( "Are you sure you want to leave this page?\n(This demonstrates veto-ing)", "wxAUI", wxICON_QUESTION | wxYES_NO, this ) != wxYES )
//         {
//             evt.Veto();
//         }
//     }
// }

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

void GBFrame::OnShowProperties(wxCommandEvent& event)
{
    m_mgr.GetPane("property-cell").Show(event.GetId() == ID_ShowProperties);
    m_mgr.Update();
}

void GBFrame::OnDrawCellCoords(wxCommandEvent& WXUNUSED(event))
{
    MapBoardCtrl* map_board = m_boards[wxString::Format("level-%d", m_notebook_ctrl->GetSelection())];
    map_board->ToggleDrawCoords();
}

MapBoardCtrl* GBFrame::NewMapBoard(int id, std::shared_ptr<Data> d)
{
    if(id < 0)
        id = m_notebook_ctrl->GetPageCount();
    wxString level_name = wxString::Format("level-%d", id);
    MapBoardCtrl* map_ctrl = new MapBoardCtrl(m_notebook_ctrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL, level_name, m_tree_ctrl, m_propGridManager, d->has_data());
    map_ctrl->FillData(d);
    m_boards[level_name] = map_ctrl;
    return map_ctrl;
}

void GBFrame::AddLevel(int id, std::shared_ptr<Data> d)
{
    if(id < 0)
        id = m_notebook_ctrl->GetPageCount();
    //auto* const m_notebook_ctrl = wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook").window);
    wxBitmapBundle page_bmp = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
    MapBoardCtrl* map_ctrl = NewMapBoard(id, d);
    m_notebook_ctrl->Freeze();
    m_notebook_ctrl->AddPage(map_ctrl, map_ctrl->GetName(), true, page_bmp);
    m_notebook_ctrl->Thaw();
}

void GBFrame::OnAddLevel(wxCommandEvent& WXUNUSED(event))
{
    MapSettingsDialog dlg(this, m_map_settings_data);
    if(dlg.ShowModal() == wxID_OK)
    {
        int id = m_notebook_ctrl->GetPageCount();
        std::shared_ptr<Data> d = std::make_shared<Data>(dlg.m_sdata.m_cell_side_size, dlg.m_sdata.m_count_cell_x, dlg.m_sdata.m_count_cell_y);
        m_datas[id] = d;
        AddLevel(id, d);
    }
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

void GBFrame::ParseJsonBoards(wxTextFile& f, int id_level)
{
    int id_texture = -1;
    int count_x = 0;
    int count_y = 0;
    int default_side_size = 0;
    int id_floor = -1;
    int id_wall = -1;
    int id_roof = -1;
    WallType wall_type = WT_DEFAULT;
    wxString tex_path("");
    wxString script("");
    wxString tag("levels");
    wxPoint cell_point(0, 0);
    wxRegEx r_id_int("^.*(\\d+)(:)");
    wxRegEx r_count_x("^.*\"count_x\":(\\d+)");
    wxRegEx r_count_y("^.*\"count_y\":(\\d+)");
    wxRegEx r_default_side_size("^.*\"default_side_size\":(\\d+)");
    wxRegEx r_path("^.*\"path\":\"(.*)\"");
    wxRegEx r_id_coords("^.*\"(\\d+)-(\\d+)\"(:)");
    wxRegEx r_id_cell("^.*\"id\":(\\d+)");
    wxRegEx r_id_floor("^.*\"floor\":(\\d+)");
    wxRegEx r_id_wall("^.*\"wall\":(\\d+)");
    wxRegEx r_id_roof("^.*\"roof\":(\\d+)");
    wxRegEx r_wall_type("^.*\"type\":(\\d+)");
    wxRegEx r_script("^.*\"script\":\"(.*)\"");
    wxString str = f.GetNextLine();
    std::shared_ptr<Data> data;
    while(!tag.empty())
    {
        //wxLogDebug(wxString("📌 ") << str);
        str.Replace("\t", "");
        str.Replace("\n", "");
        if(str != "{" && str != "}" && str != "},")
        {
            if(tag == "cells")
            {
                if(r_id_coords.Matches(str, wxRE_NOTEMPTY))
                {
                    cell_point.x = wxAtoi(r_id_coords.GetMatch(str, 1));
                    cell_point.y = wxAtoi(r_id_coords.GetMatch(str, 2));
                    wxLogDebug(wxString("🅧") << cell_point.x << "; 🅨" << cell_point.y);
                }
                else if(r_id_floor.Matches(str, wxRE_NOTEMPTY))
                {//tag = "floor";
                    id_floor = wxAtoi(r_id_floor.GetMatch(str, 1));
                    wxLogDebug(wxString("🆔🖼") << id_floor);
                }
                else if(r_id_wall.Matches(str, wxRE_NOTEMPTY))
                {//tag = "wall";
                    id_wall = wxAtoi(r_id_wall.GetMatch(str, 1));
                    wxLogDebug(wxString("🆔🧱") << id_wall);
                }
                else if(r_id_roof.Matches(str, wxRE_NOTEMPTY))
                {//tag = "roof";
                    id_roof = wxAtoi(r_id_roof.GetMatch(str, 1));
                    wxLogDebug(wxString("🆔🏗") << id_roof);
                }
                else if(r_wall_type.Matches(str, wxRE_NOTEMPTY))
                {//tag = "type";
                    wall_type = (WallType)wxAtoi(r_wall_type.GetMatch(str, 1));
                    wxLogDebug(wxString("🆔🃏") << wall_type);
                }
                else if(r_script.Matches(str, wxRE_NOTEMPTY))
                {//tag = "script";
                    script = r_script.GetMatch(str, 1);
                    wxLogDebug(wxString("🆔🧩") << script);
                }
            }
            else if(r_id_int.Matches(str, wxRE_NOTEMPTY))
            {
                if(tag == "levels")
                {
                    count_x = 0;
                    count_y = 0;
                    default_side_size = 0;
                    tex_path = "";
                    id_level = wxAtoi(r_id_int.GetMatch(str, 1));
                    wxLogDebug(wxString("🔝🆔 ") << id_level);
                }
                else
                {
                    id_texture = wxAtoi(r_id_int.GetMatch(str, 1));
                    wxLogDebug(wxString("🎨🆔 ") << id_texture);
                }
            }
            else if(r_count_x.Matches(str, wxRE_NOTEMPTY))
            {
                tag = "count_x";
                count_x = wxAtoi(r_count_x.GetMatch(str, 1));
                wxLogDebug(wxString("🔝🅧") << count_x);
            }
            else if(r_count_y.Matches(str, wxRE_NOTEMPTY))
            {
                tag = "count_y";
                count_y = wxAtoi(r_count_y.GetMatch(str, 1));
                wxLogDebug(wxString("🔝🅨") << count_y);
            }
            else if(r_default_side_size.Matches(str, wxRE_NOTEMPTY))
            {
                tag = "default_side_size";
                default_side_size = wxAtoi(r_default_side_size.GetMatch(str, 1));
                wxLogDebug(wxString("🔝🅩") << default_side_size);
            }
            else if(str.Find("textures") != wxNOT_FOUND)
            {
                tag = "textures";
                wxLogDebug("🎨TEXTURES🎨");
                data = std::make_shared<Data>(default_side_size, count_x, count_y);
            }
            else if(r_path.Matches(str, wxRE_NOTEMPTY))
            {
                tag = "path";
                tex_path = r_path.GetMatch(str, 1);
                wxLogDebug(wxString("🎨🗂") << tex_path);
                if(data && id_texture > -1)
                {
                    data->append_texture(id_texture, tex_path);
                    id_texture = -1;
                }
            }
            else if(str.Find("cells") != wxNOT_FOUND)
            {
                tag = "cells";
                wxLogDebug("🛟CELLS🛟");
            }
            else
                wxLogDebug(str);
        }
        else if((str == "}" || str == "},") && tag == "cells")
        {
            if(data && cell_point != wxDefaultPosition)
            {
                data->append_cell(cell_point.x, cell_point.y, default_side_size, id_floor, id_wall, id_roof, wall_type, script.utf8_string());
            }
            cell_point = wxDefaultPosition;
            id_floor = -1;
            id_wall = -1;
            id_roof = -1;
            wall_type = WT_DEFAULT;
            script = "";
            if(str == "}")
                tag = "levels";
        }
        else if(str == "}")
        {
            if(tag != "levels")
                tag = "levels";
            if(id_texture > -1)
                id_texture = -1;
            else if(id_level > -1)
            {
                if(data->has_data())
                    AddLevel(id_level, data);
                id_level = -1;
            }
            else
            {
                //wxLogDebug(str << " TAG EMPTY");
                tag = "";
            }
        }
        str = f.GetNextLine();
    }
}

void GBFrame::ParseJsonFile(wxTextFile& f)
{
    wxString str = f.GetFirstLine();
    while(!f.Eof())
    {
        str.Replace("\t", "");
        str.Replace("\n", "");
        if(str == "{")
            wxLogDebug("🎬");
        else if(str == "}" || str == "},")
            wxLogDebug("🛑");
        else if(str.Find("levels") != wxNOT_FOUND)
        {
            wxLogDebug("🎒LEVELS🎒");
            ParseJsonBoards(f);
        }
        else
            wxLogDebug(str);
        str = f.GetNextLine();
    }
}

void GBFrame::OnOpenLevel(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JSON file (*.json)|*.json;*.JSON";
    wxFileDialog dlg(this, "Open JSON level", wxEmptyString, wxEmptyString, wildCard, wxFD_OPEN);
    if(dlg.ShowModal() == wxID_OK)
    {
        wxLogDebug(dlg.GetPath());
        wxTextFile f(dlg.GetPath());
        if(f.Open())
            ParseJsonBoards(f, m_notebook_ctrl->GetPageCount());
        else
            wxLogWarning("JSON file not opened");
    }
}

void GBFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JSON file (*.json)|*.json;*.JSON";
    wxFileDialog dlg(this, "Open JSON project", wxEmptyString, wxEmptyString, wildCard, wxFD_OPEN);
    if(dlg.ShowModal() == wxID_OK)
    {
        wxLogDebug(dlg.GetPath());
        wxTextFile f(dlg.GetPath());
        if(f.Open())
            ParseJsonFile(f);
        else
            wxLogWarning("JSON file not opened");
    }
}

void GBFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JSON data (*.json)|*.json;*.JSON";
    wxFileDialog dlg(this, "Save as JSON", wxEmptyString, wxEmptyString, wildCard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(dlg.ShowModal() == wxID_OK)
    {
        wxFile f(dlg.GetPath(), wxFile::write);
        if(f.IsOpened())
        {
            f.Write("{\n\t\"levels\":\n\t{\n");
            bool first_line = true;
            for(const auto& [k, map_board] : m_boards)
            {
                if(map_board->HasData())
                {
                    if(first_line)
                        first_line = false;
                    else
                        f.Write(",\n");
                    f.Write(wxString("\t\t")<<k.Mid(6)<<":\n");
                    f.Write(map_board->LevelToString("\t\t"));
                }
            }
            f.Write("\n\t}\n}");
            f.Close();
        }
    }
}

void GBFrame::OnSaveLevel(wxCommandEvent& WXUNUSED(event))
{
    wxString wildCard = "JSON data (*.json)|*.json;*.JSON";
    wxFileDialog dlg(this, "Save as JSON", wxEmptyString, wxEmptyString, wildCard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(dlg.ShowModal() == wxID_OK)
    {
        //wxWindow* current_page = m_notebook_ctrl->GetCurrentPage();
        MapBoardCtrl* map_board = m_boards[wxString::Format("level-%d", m_notebook_ctrl->GetSelection())];
        if(map_board->HasData())
            map_board->LevelToFile(dlg.GetPath());
        else
            wxLogWarning("CURRENT MAP EMPTY");
    }
}

void GBFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor busy;
    wxStopWatch stopwatch;
    m_tree_ctrl->Disable();
    m_propGridManager->Disable();
    for(const auto& [k, map_board] : m_boards)
    {
        map_board->Destroy();
        //delete map_board;//ERROR: double free or corruption (!prev)
    }
    //for(const auto& [k, data] : m_datas)
        //delete data;
    m_boards.clear();
    m_datas.clear();
    m_tree_ctrl->Destroy();
    m_propGridManager->Clear();
    m_propGridManager->Destroy();
    //m_logTextCtrl->Close();
    m_mgr.UnInit();
    wxYield();
    Close(true);
    long t = stopwatch.Time();
#if DEBUG
    std::cout << "!!! GBFrame::OnExit " << t << " ms" << std::endl;
#endif
}

void GBFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf("Game Builder\ncreate game without programming, but optional you always can replace core engine (source code included)\n🤖Compiler %s\n🚀Run under %s\n🗂️%s (%s)\n©️ Maxim Kolosov", __VERSION__, wxPlatformInfo::Get().GetOperatingSystemIdName(), wxVERSION_STRING, wxPlatformInfo::Get().GetBitnessName());
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
    return m_notebook_ctrl;
}

void GBFrame::SetCellWallType(WallType value)
{
    MapBoardCtrl* map_board = m_boards[wxString::Format("level-%d", m_notebook_ctrl->GetSelection())];
    map_board->set_cell_wall_type(value);
}

void GBFrame::SetCellScript(const wxString& value)
{
    MapBoardCtrl* map_board = m_boards[wxString::Format("level-%d", m_notebook_ctrl->GetSelection())];
    map_board->set_cell_script(value);
}

void GBFrame::OnPropertyGridChanging(wxPropertyGridEvent& event)
{
    wxAny v = event.GetPropertyValue();
    if(event.GetPropertyName() == "wall_type")
        SetCellWallType((WallType)v.As<int>());
    else if(event.GetPropertyName() == "script")
        SetCellScript(v.As<wxString>());
    else
        wxLogMessage(event.GetPropertyName() << " " << v.As<wxString>());
}
