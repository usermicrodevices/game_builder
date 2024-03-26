///////////////////////////////////////////////////////////////////////////////
// Name:        gb.h
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-02-21
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include <wx/debug.h>

#include "wx/app.h"
#include "wx/grid.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/choice.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/bmpbuttn.h"
#include "wx/log.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/stattext.h"
#include <wx/filedlg.h>

#include "wx/aui/aui.h"

#include "gb.xpm"

#include "map_board.h"

typedef std::unordered_map<wxString, MapBoardCtrl*> LevelContainer;

#if defined(__WXOSX__) || defined(__WXGTK3__)
#define wxDRAWING_DC_SUPPORTS_ALPHA 1
#else
#define wxDRAWING_DC_SUPPORTS_ALPHA 0
#endif // __WXOSX__ || __WXGTK3__


class GBApp : public wxApp
{
public:
	bool OnInit() override;
};

wxDECLARE_APP(GBApp);
wxIMPLEMENT_APP(GBApp);


class GBFrame : public wxFrame
{
	enum
	{
		ID_CreateTree = wxID_HIGHEST,
		ID_CreateText,
		ID_CreateHTML,
		ID_CreateNotebook,
		ID_CreateSizeReport,
		ID_GridContent,
		ID_TextContent,
		ID_TreeContent,
		ID_HTMLContent,
		ID_NotebookContent,
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
		ID_CustomizeToolbar,
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
		ID_NotebookNewTab,
		ID_NotebookDeleteTab,

		ID_SampleItem,

		ID_FirstPerspective = ID_CreatePerspective+1000
	};

public:
	GBFrame(wxWindow* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

	wxAuiDockArt* GetDockArt();
	void DoUpdate();


private:

	long m_notebook_style;
	long m_notebook_theme;
	wxArrayString m_perspectives;
	wxMenu* m_perspectives_menu;
	wxLog *m_log;
	wxTextCtrl* m_logTextCtrl;
	wxAuiManager m_mgr;
	LevelContainer levels = {};

	wxTextCtrl* CreateTextCtrl(const wxString& text = wxEmptyString);
	wxTreeCtrl* CreateTreeCtrl();
	wxPoint GetStartPosition();
	wxHtmlWindow* CreateHTMLCtrl(wxWindow* parent = nullptr);
	wxAuiNotebook* CreateNotebook();

	wxString GetIntroText();

	void OnSave(wxCommandEvent& event);

	void OnEraseBackground(wxEraseEvent& evt);
	void OnSize(wxSizeEvent& evt);

	void OnCreateTree(wxCommandEvent& evt);
	void OnCreateHTML(wxCommandEvent& evt);
	void OnCreateNotebook(wxCommandEvent& evt);
	void OnCreateText(wxCommandEvent& evt);
	void OnCreateMapBoardCtrl(wxCommandEvent& evt);
	void OnChangeContentPane(wxCommandEvent& evt);
	void OnDropDownToolbarItem(wxAuiToolBarEvent& evt);
	void OnCreatePerspective(wxCommandEvent& evt);
	void OnCopyPerspectiveCode(wxCommandEvent& evt);
	void OnRestorePerspective(wxCommandEvent& evt);
	void OnSettings(wxCommandEvent& evt);
	void OnCustomizeToolbar(wxCommandEvent& evt);
	void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);
	void OnNotebookPageClose(wxAuiNotebookEvent& evt);
	void OnNotebookPageClosed(wxAuiNotebookEvent& evt);
	void OnNotebookPageChanging(wxAuiNotebookEvent &evt);
	void OnExit(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnTabAlignment(wxCommandEvent &evt);

	void OnGradient(wxCommandEvent& evt);
	void OnToolbarResizing(wxCommandEvent& evt);
	void OnManagerFlag(wxCommandEvent& evt);
	void OnNotebookFlag(wxCommandEvent& evt);
	void OnUpdateUI(wxUpdateUIEvent& evt);

	void OnNotebookNewTab(wxCommandEvent& evt);
	void OnNotebookDeleteTab(wxCommandEvent& evt);

	void OnPaneClose(wxAuiManagerEvent& evt);

	wxDECLARE_EVENT_TABLE();
};


class SettingsPanel : public wxPanel
{
	enum
	{
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

	SettingsPanel(wxWindow* parent, GBFrame* frame)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
	m_frame(frame)
	{
		//wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);

		//vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);

		wxBoxSizer* s1 = new wxBoxSizer(wxHORIZONTAL);
		m_border_size = new wxSpinCtrl(this, ID_PaneBorderSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
		s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s1->Add(new wxStaticText(this, wxID_ANY, "Pane Border Size:"));
		s1->Add(m_border_size);
		s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s1->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
		//vert->Add(s1, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

		wxBoxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
		m_sash_size = new wxSpinCtrl(this, ID_SashSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
		s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s2->Add(new wxStaticText(this, wxID_ANY, "Sash Size:"));
		s2->Add(m_sash_size);
		s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s2->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
		//vert->Add(s2, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

		wxBoxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
		m_caption_size = new wxSpinCtrl(this, ID_CaptionSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
		s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s3->Add(new wxStaticText(this, wxID_ANY, "Caption Size:"));
		s3->Add(m_caption_size);
		s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s3->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));
		//vert->Add(s3, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

		//vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);


		wxBitmap b = CreateColorBitmap(*wxBLACK);

		wxBoxSizer* s4 = new wxBoxSizer(wxHORIZONTAL);
		m_background_color = new wxBitmapButton(this, ID_BackgroundColor, b, wxDefaultPosition, FromDIP(wxSize(50,25)));
		s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s4->Add(new wxStaticText(this, wxID_ANY, "Background Color:"));
		s4->Add(m_background_color);
		s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s4->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s5 = new wxBoxSizer(wxHORIZONTAL);
		m_sash_color = new wxBitmapButton(this, ID_SashColor, b, wxDefaultPosition, wxSize(50,25));
		s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s5->Add(new wxStaticText(this, wxID_ANY, "Sash Color:"));
		s5->Add(m_sash_color);
		s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s5->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s6 = new wxBoxSizer(wxHORIZONTAL);
		m_inactive_caption_color = new wxBitmapButton(this, ID_InactiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
		s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s6->Add(new wxStaticText(this, wxID_ANY, "Normal Caption:"));
		s6->Add(m_inactive_caption_color);
		s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s6->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s7 = new wxBoxSizer(wxHORIZONTAL);
		m_inactive_caption_gradient_color = new wxBitmapButton(this, ID_InactiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
		s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s7->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Gradient:"));
		s7->Add(m_inactive_caption_gradient_color);
		s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s7->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s8 = new wxBoxSizer(wxHORIZONTAL);
		m_inactive_caption_text_color = new wxBitmapButton(this, ID_InactiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
		s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s8->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Text:"));
		s8->Add(m_inactive_caption_text_color);
		s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s8->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s9 = new wxBoxSizer(wxHORIZONTAL);
		m_active_caption_color = new wxBitmapButton(this, ID_ActiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
		s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s9->Add(new wxStaticText(this, wxID_ANY, "Active Caption:"));
		s9->Add(m_active_caption_color);
		s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s9->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s10 = new wxBoxSizer(wxHORIZONTAL);
		m_active_caption_gradient_color = new wxBitmapButton(this, ID_ActiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
		s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s10->Add(new wxStaticText(this, wxID_ANY, "Active Caption Gradient:"));
		s10->Add(m_active_caption_gradient_color);
		s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s10->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s11 = new wxBoxSizer(wxHORIZONTAL);
		m_active_caption_text_color = new wxBitmapButton(this, ID_ActiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
		s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s11->Add(new wxStaticText(this, wxID_ANY, "Active Caption Text:"));
		s11->Add(m_active_caption_text_color);
		s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s11->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s12 = new wxBoxSizer(wxHORIZONTAL);
		m_border_color = new wxBitmapButton(this, ID_BorderColor, b, wxDefaultPosition, wxSize(50,25));
		s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s12->Add(new wxStaticText(this, wxID_ANY, "Border Color:"));
		s12->Add(m_border_color);
		s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s12->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxBoxSizer* s13 = new wxBoxSizer(wxHORIZONTAL);
		m_gripper_color = new wxBitmapButton(this, ID_GripperColor, b, wxDefaultPosition, wxSize(50,25));
		s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s13->Add(new wxStaticText(this, wxID_ANY, "Gripper Color:"));
		s13->Add(m_gripper_color);
		s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
		s13->SetItemMinSize((size_t)1, FromDIP(wxSize(180, 20)));

		wxGridSizer* grid_sizer = new wxGridSizer(2);
		grid_sizer->SetHGap(FromDIP(5));
		grid_sizer->Add(s1);  grid_sizer->Add(s4);
		grid_sizer->Add(s2);  grid_sizer->Add(s5);
		grid_sizer->Add(s3);  grid_sizer->Add(s13);
		grid_sizer->Add(FromDIP(1),FromDIP(1)); grid_sizer->Add(s12);
		grid_sizer->Add(s6);  grid_sizer->Add(s9);
		grid_sizer->Add(s7);  grid_sizer->Add(s10);
		grid_sizer->Add(s8);  grid_sizer->Add(s11);

		wxBoxSizer* cont_sizer = new wxBoxSizer(wxVERTICAL);
		cont_sizer->Add(grid_sizer, 1, wxEXPAND | wxALL, FromDIP(5));
		SetSizer(cont_sizer);
		GetSizer()->SetSizeHints(this);

		m_border_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
		m_sash_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
		m_caption_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));

		UpdateColors();
	}

private:

	wxBitmap CreateColorBitmap(const wxColour& c)
	{
		wxImage image;
		wxSize size = FromDIP(wxSize(25, 14));
		image.Create(size);
		for (int x = 0; x < size.x; ++x)
		{
			for (int y = 0; y < size.y; ++y)
			{
				wxColour pixcol = c;
				if (x == 0 || x == size.x || y == 0 || y == size.y)
					pixcol = *wxBLACK;
				image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
			}
		}
		return wxBitmap(image);
	}

	void UpdateColors()
	{
		wxColour bk = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
		m_background_color->SetBitmapLabel(CreateColorBitmap(bk));

		wxColour cap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
		m_inactive_caption_color->SetBitmapLabel(CreateColorBitmap(cap));

		wxColour capgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
		m_inactive_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(capgrad));

		wxColour captxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
		m_inactive_caption_text_color->SetBitmapLabel(CreateColorBitmap(captxt));

		wxColour acap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
		m_active_caption_color->SetBitmapLabel(CreateColorBitmap(acap));

		wxColour acapgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
		m_active_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(acapgrad));

		wxColour acaptxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
		m_active_caption_text_color->SetBitmapLabel(CreateColorBitmap(acaptxt));

		wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
		m_sash_color->SetBitmapLabel(CreateColorBitmap(sash));

		wxColour border = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
		m_border_color->SetBitmapLabel(CreateColorBitmap(border));

		wxColour gripper = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);
		m_gripper_color->SetBitmapLabel(CreateColorBitmap(gripper));
	}

	void OnPaneBorderSize(wxSpinEvent& event)
	{
		m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,
										 event.GetPosition());
		m_frame->DoUpdate();
	}

	void OnSashSize(wxSpinEvent& event)
	{
		m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_SASH_SIZE,
										 event.GetPosition());
		m_frame->DoUpdate();
	}

	void OnCaptionSize(wxSpinEvent& event)
	{
		m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,
										 event.GetPosition());
		m_frame->DoUpdate();
	}

	void OnSetColor(wxCommandEvent& event)
	{
		wxColourDialog dlg(m_frame);
		dlg.SetTitle(_("Color Picker"));
		if (dlg.ShowModal() != wxID_OK)
			return;

		int var = 0;
		switch (event.GetId())
		{
			case ID_BackgroundColor:              var = wxAUI_DOCKART_BACKGROUND_COLOUR; break;
			case ID_SashColor:                    var = wxAUI_DOCKART_SASH_COLOUR; break;
			case ID_InactiveCaptionColor:         var = wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR; break;
			case ID_InactiveCaptionGradientColor: var = wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR; break;
			case ID_InactiveCaptionTextColor:     var = wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR; break;
			case ID_ActiveCaptionColor:           var = wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR; break;
			case ID_ActiveCaptionGradientColor:   var = wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR; break;
			case ID_ActiveCaptionTextColor:       var = wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR; break;
			case ID_BorderColor:                  var = wxAUI_DOCKART_BORDER_COLOUR; break;
			case ID_GripperColor:                 var = wxAUI_DOCKART_GRIPPER_COLOUR; break;
			default: return;
		}

		m_frame->GetDockArt()->SetColor(var, dlg.GetColourData().GetColour());
		m_frame->DoUpdate();
		UpdateColors();
	}

private:

	GBFrame* m_frame;
	wxSpinCtrl* m_border_size;
	wxSpinCtrl* m_sash_size;
	wxSpinCtrl* m_caption_size;
	wxBitmapButton* m_inactive_caption_text_color;
	wxBitmapButton* m_inactive_caption_gradient_color;
	wxBitmapButton* m_inactive_caption_color;
	wxBitmapButton* m_active_caption_text_color;
	wxBitmapButton* m_active_caption_gradient_color;
	wxBitmapButton* m_active_caption_color;
	wxBitmapButton* m_sash_color;
	wxBitmapButton* m_background_color;
	wxBitmapButton* m_border_color;
	wxBitmapButton* m_gripper_color;

	wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(SettingsPanel, wxPanel)
EVT_SPINCTRL(ID_PaneBorderSize, SettingsPanel::OnPaneBorderSize)
EVT_SPINCTRL(ID_SashSize, SettingsPanel::OnSashSize)
EVT_SPINCTRL(ID_CaptionSize, SettingsPanel::OnCaptionSize)
EVT_BUTTON(ID_BackgroundColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_SashColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_InactiveCaptionColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_InactiveCaptionGradientColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_InactiveCaptionTextColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_ActiveCaptionColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_ActiveCaptionGradientColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_ActiveCaptionTextColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_BorderColor, SettingsPanel::OnSetColor)
EVT_BUTTON(ID_GripperColor, SettingsPanel::OnSetColor)
wxEND_EVENT_TABLE()
