#pragma once

#if defined(__WXX11__)
#define USE_CONTEXT_MENU 0
#else
#define USE_CONTEXT_MENU 1
#endif

#include "data.h"

enum EraserType
{
	NONE = 0,
	FLOOR,
	WALL,
	CEILING
};

enum
{
	Menu_Popup_ToBeDeleted = 2000,
	Menu_Popup_ToBeGreyed,
	Menu_Popup_ToBeChecked,
	Menu_Popup_Submenu,
	Menu_Popup_Set_Default_Cursor,
	Menu_Popup_Open_Floor,
	Menu_Popup_Set_Floor_Eraser,

	Menu_PopupChoice
};


class MapBoardCtrl : public wxScrolledWindow
{
	wxDECLARE_DYNAMIC_CLASS(MapBoardCtrl);

public:
		MapBoardCtrl(){};//wxDECLARE_DYNAMIC_CLASS

		MapBoardCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = "scrolledWindow", wxAuiManager* mgr = nullptr, wxTreeCtrl* tree = nullptr, wxPropertyGridManager* prop_grid_mgr = nullptr)
		: wxScrolledWindow(parent, id, pos, size, wxHSCROLL | wxVSCROLL)
		{
			m_mgr = mgr;
			m_tree = tree;
			m_prgrmgr = prop_grid_mgr;
			m_level_tree_item = tree->AppendItem(m_tree->GetRootItem(), "Level 0", 0);
			m_data = Data(m_tree, m_level_tree_item);
			wxSize virtual_size = m_data.virtual_size();
			int cell_side = m_data.cell_side_size();
			SetVirtualSize(virtual_size);
			SetScrollbars(cell_side, cell_side, virtual_size.x/cell_side, virtual_size.y/cell_side);
			//FitInside();
			//SetTargetWindow(parent);
			LogMessage(wxString::Format("cells count %d", m_data.count_cells()));
			LogMessage(wxString::Format("virtual_size %dx%d", virtual_size.x, virtual_size.y));
			//std::cout << "textures count " << m_data.count_textures() << std::endl;
			//std::cout << "cells count " << m_data.count_cells() << std::endl;
		}

		void LogMessage(const wxString &value)
		{
			try
			{
				wxLogMessage(value);
			}
			catch(std::exception& e)
			{
				wxLogMessage(e.what());
			}
		}

		void ToggleDrawCoords()
		{
			m_draw_coords = !m_draw_coords;
			wxClientDC(this).Clear();
			Refresh();
			Update();
		}

		wxString LevelToString(const wxString& indentation = "")
		{
			try
			{
				return m_data.ToString(indentation);
			}
			catch(std::exception& e)
			{
				wxLogMessage(e.what());
			}
			return wxT("");
		}

		void LevelToFile(const wxString &filename)
		{
			try
			{
				wxLogMessage(m_data.ToFile(filename));
			}
			catch(std::exception& e)
			{
				wxLogMessage(e.what());
			}
		}

	private:
		bool m_draw_coords = false;
		EraserType m_eraser = EraserType::NONE;
		bool m_togle_mouse = false;
		Texture m_current_texture = Texture();
		Data m_data;
		wxTreeItemId m_level_tree_item;
		wxPoint m_current_mouse_position = wxDefaultPosition;
		wxPoint m_current_cell_position = wxDefaultPosition;
		wxPoint m_current_click_position = wxDefaultPosition;
		wxAuiManager* m_mgr;
		wxTreeCtrl* m_tree;
		wxPropertyGridManager* m_prgrmgr;

		void OnPaint(wxPaintEvent& WXUNUSED(event))
		{
			int cell_side = m_data.cell_side_size();
			size_t cellSideInPx = FromDIP(cell_side);
			wxSize virtual_size = m_data.virtual_size();
			size_t mapxInPx = FromDIP(virtual_size.x);
			size_t mapyInPx = FromDIP(virtual_size.y);

			wxPaintDC dc(this);
			DoPrepareDC(dc);
			//PrepareDC(dc);

			wxSize size = GetClientSize();

			int view_x=0, view_y=0;
			CalcUnscrolledPosition(view_x, view_y, &view_x, &view_y);

			m_current_cell_position.x = m_current_mouse_position.x < (int)cellSideInPx ? 0 : abs((m_current_mouse_position.x + view_x) / (int)cellSideInPx) * (int)cellSideInPx;
			m_current_cell_position.y = m_current_mouse_position.y < (int)cellSideInPx ? 0 : abs((m_current_mouse_position.y + view_y) / (int)cellSideInPx) * (int)cellSideInPx;

			dc.SetPen(wxPen(wxColour(0, 0, 255, 128), 5));
			wxRect cellSelected(wxSize(cellSideInPx, cellSideInPx));
			cellSelected.SetLeft(0);
			cellSelected.Offset(m_current_cell_position.x, m_current_cell_position.y);
			dc.DrawRectangle(cellSelected);

			dc.SetPen(wxPen(wxColour(0, 255, 0, 128), 5));

			for(size_t xpos = cellSideInPx; xpos < mapxInPx; xpos+=cell_side)
				dc.DrawLine(xpos, 0, xpos, mapxInPx);
			for(size_t ypos = cellSideInPx; ypos < mapyInPx; ypos+=cell_side)
				dc.DrawLine(0, ypos, mapyInPx, ypos);

			wxString s;
			int h, w;
			dc.SetFont(*wxNORMAL_FONT);
			wxRect rect(view_x, view_y, size.x, size.y);
			for(auto iter = m_data.cells_begin(); iter != m_data.cells_end(); ++iter)
			{
				auto point = iter->first;
				auto cell = iter->second;
				if(cell.texture_floor > -1)
				{
					wxBitmap bitmap = m_data.get_texture_bitmap(cell.texture_floor);
					if(bitmap.IsOk())
						dc.DrawBitmap(bitmap, point);
				}
				else if(m_draw_coords)
				{
					if(rect.Contains(point))
					{
						s.Printf("%d\n%d", point.x, point.y);
						dc.GetTextExtent(s, &w, &h);
						dc.DrawText(s, point.x+w/2, point.y+h/4);
					}
				}
			}

			if(m_current_click_position.x > -1)
			{
				dc.SetBrush(wxBrush(wxColour(0, 0, 0, 0)));
				dc.SetPen(wxPen(wxColour(0, 0, 255, 128), 5));
				wxRect cellSelected(wxSize(cellSideInPx, cellSideInPx));
				cellSelected.SetLeft(0);
				cellSelected.Offset(m_current_click_position.x, m_current_click_position.y);
				dc.DrawRoundedRectangle(cellSelected, 5);
			}

			//dc.SetBrush(wxBrush(wxColour(200, 0, 0, 50)));
			//dc.DrawRectangle(view_x, view_y, size.x, size.y);
		}

		void OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
		{
			// intentionally empty
		}

		void OnSize(wxSizeEvent& WXUNUSED(evt))
		{
			Refresh();
		}	

		void ShowContextMenu(const wxPoint& pos)
		{
			wxString s;
			wxMenu menu;

			s.Printf("Position: (%d,%d)", m_current_mouse_position.x, m_current_mouse_position.y);
			wxLogMessage(s);

			if ( wxGetKeyState(WXK_CONTROL) )
			{
				menu.SetTitle("Choose one of:");
				static const char *choices[] = { "First", "Second", "Therch" };
				for ( size_t n = 0; n < WXSIZEOF(choices); n++ )
					menu.Append(Menu_PopupChoice + n, choices[n]);

				const int rc = GetPopupMenuSelectionFromUser(menu, pos);
				if ( rc == wxID_NONE )
				{
					wxLogMessage("No selection");
				}
				else
				{
					wxLogMessage("You have selected \"%s\"", choices[rc - Menu_PopupChoice]);
				}
			}
			else
			{
				menu.Append(Menu_Popup_Open_Floor, "📂"+_("Open floor image"));
				menu.Append(Menu_Popup_Set_Floor_Eraser, "🧹"+_("Eraser"));
				menu.Append(Menu_Popup_Set_Default_Cursor, "↖"+_("Empty cursor"));
				PopupMenu(&menu, pos);
			}
		}

#if USE_CONTEXT_MENU
		void OnContextMenu(wxContextMenuEvent& event)
		{
			wxPoint point = event.GetPosition();
			// If from keyboard
			if (point.x == -1 && point.y == -1) {
				wxSize size = GetSize();
				point.x = size.x / 2;
				point.y = size.y / 2;
			} else {
				point = ScreenToClient(point);
			}
			ShowContextMenu(point);
		}
#else
		void OnRightUp(wxMouseEvent& event)
		{ ShowContextMenu(event.GetPosition()); }
#endif

		void refresh_pgproperty(const Texture& texture)
		{
			//std::cout << "texture " << texture.id << std::endl;
			wxPGProperty* path_floor = m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetProperty("path");
			if(!path_floor)
				wxLogError("wxPGProperty path");
			else
			{
				if(texture.id > -1)
				{
					//wxLogMessage("🩸"+texture.path.GetFullPath()+"🩸");
					//if(!path_floor->SetValueFromString(texture.path.GetFullPath(), wxPGPropValFormatFlags::FullValue|wxPGPropValFormatFlags::ReportError))
						//wxLogError("🩸wxPGProperty.SetValueFromString 🧵" + texture.path.GetFullPath()+"🧵");
					path_floor->SetValueFromString(texture.path.GetFullPath(), wxPGPropValFormatFlags::FullValue);
				}
				else
					path_floor->SetValueFromString("", wxPGPropValFormatFlags::FullValue);
			}

			if(m_current_cell_position.x > -1 && m_current_cell_position.y > -1)
			{
				wxPGProperty* coords = m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetProperty("coords");
				if(!coords)
					wxLogError("wxPGProperty coords");
				coords->SetValueFromString(wxString::Format("%d x %d", m_current_cell_position.x, m_current_cell_position.y));
			}
		}

		void store_currents()
		{
			if(m_current_texture.IsOk())
				m_data.set_texture_floor(m_current_cell_position, m_current_texture);
			else
			{
				switch (m_eraser)
				{
					case EraserType::FLOOR:
						m_data.set_texture_floor(m_current_cell_position);
				}
			}
		}

		void OnMotion(wxMouseEvent& event)
		{
			m_current_mouse_position = event.GetPosition();
			wxString s;
			s.Printf("Position: (%d,%d)", m_current_mouse_position.x, m_current_mouse_position.y);
			((wxFrame*)m_mgr->GetManagedWindow())->SetStatusText(s);
			if(m_togle_mouse)
				store_currents();
			Refresh();
		}

		void OnLeftDown(wxMouseEvent& WXUNUSED(event))
		{
			m_current_click_position = m_current_cell_position;
			m_togle_mouse = true;
			store_currents();
		}

		void OnLeftUp(wxMouseEvent& WXUNUSED(event))
		{
			m_current_click_position = m_current_cell_position;

			if(!m_prgrmgr)
				wxLogError(wxT("wxPropertyGridManager error"));
			else
				refresh_pgproperty(m_data.get_texture_floor(m_current_cell_position));

			wxTreeItemId id = m_data.cell_tree_item(m_current_cell_position);
			if(id)
			{
				m_tree->EnsureVisible(id);
				m_tree->ScrollTo(id);
				m_tree->SelectItem(id);
			}

			m_togle_mouse = false;
		}

		void OnOpenFloorFile(wxCommandEvent& WXUNUSED(event))
		{
			wxString dir_imgs("assets/images");
			wxFileDialog dialog(this, _("Please choose floor image"), dir_imgs, wxEmptyString, "*.jpg;*.png;*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);
			if (dialog.ShowModal() == wxID_OK)
			{
				wxString filename(dialog.GetFilename());
				wxString path_from(dialog.GetPath());
				wxString path_result(dir_imgs+"/"+filename);
				if (!wxFileExists(path_result))
				{
					if (!wxCopyFile(path_from, path_result))
					{
						wxLogError(wxT("Could not copy %s to %s !"), path_from.c_str(), path_result.c_str());
						return;
					}
				}
				m_current_texture = m_data.add_texture_floor(m_current_cell_position, wxFileName(path_result));
				if(m_current_texture.IsOk())
					SetCursor(wxCursor(m_current_texture.thumbnail));
				refresh_pgproperty(m_current_texture);
				m_eraser = EraserType::NONE;
				wxLogMessage(path_result);
			}
		}

		void OnSetDefaultCursor(wxCommandEvent& WXUNUSED(event))
		{
			if(m_current_texture.id > -1)
				m_current_texture = Texture();
			SetCursor(*wxSTANDARD_CURSOR);
			m_eraser = EraserType::NONE;
		}

		void OnSetFloorEraser(wxCommandEvent& WXUNUSED(event))
		{
			if(m_current_texture.id > -1)
				m_current_texture = Texture();
			SetCursor(wxCURSOR_NO_ENTRY);
			m_eraser = EraserType::FLOOR;
		}

		wxDECLARE_EVENT_TABLE();
};

wxIMPLEMENT_DYNAMIC_CLASS(MapBoardCtrl, wxScrolledWindow);

wxBEGIN_EVENT_TABLE(MapBoardCtrl, wxScrolledWindow)
	EVT_PAINT(MapBoardCtrl::OnPaint)
	EVT_SIZE(MapBoardCtrl::OnSize)
	EVT_ERASE_BACKGROUND(MapBoardCtrl::OnEraseBackground)
	EVT_MOTION(MapBoardCtrl::OnMotion)
#if USE_CONTEXT_MENU
	EVT_CONTEXT_MENU(MapBoardCtrl::OnContextMenu)
#else
	EVT_RIGHT_UP(MapBoardCtrl::OnRightUp)
#endif
	EVT_LEFT_DOWN(MapBoardCtrl::OnLeftDown)
	EVT_LEFT_UP(MapBoardCtrl::OnLeftUp)
	EVT_MENU(Menu_Popup_Set_Default_Cursor, MapBoardCtrl::OnSetDefaultCursor)
	EVT_MENU(Menu_Popup_Open_Floor, MapBoardCtrl::OnOpenFloorFile)
	EVT_MENU(Menu_Popup_Set_Floor_Eraser, MapBoardCtrl::OnSetFloorEraser)
wxEND_EVENT_TABLE()
