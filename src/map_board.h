#pragma once

#if defined(__WXX11__)
#define USE_CONTEXT_MENU 0
#else
#define USE_CONTEXT_MENU 1
#endif

#include "data.h"

enum
{
	Menu_Popup_ToBeDeleted = 2000,
	Menu_Popup_ToBeGreyed,
	Menu_Popup_ToBeChecked,
	Menu_Popup_Submenu,

	Menu_PopupChoice
};


class MapBoardCtrl : public wxScrolledWindow
{
	wxDECLARE_DYNAMIC_CLASS(MapBoardCtrl);

public:
		MapBoardCtrl(){};//wxDECLARE_DYNAMIC_CLASS

		MapBoardCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = "scrolledWindow", wxAuiManager* mgr = nullptr, wxTreeCtrl* tree = nullptr)
		: wxScrolledWindow(parent, id, pos, size, wxHSCROLL | wxVSCROLL)
		{
			m_mgr = mgr;
			m_tree = tree;
			level_tree_item = tree->AppendItem(m_tree->GetRootItem(), "Level 0", 0);
			m_data = Data(m_tree, level_tree_item);
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
		Texture current_texture = Texture();
		Data m_data;
		wxTreeItemId level_tree_item;
		wxPoint current_mouse_position;
		wxPoint current_cell_position;
		wxAuiManager* m_mgr;
		wxTreeCtrl* m_tree;

		void OnPaint(wxPaintEvent& event)
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

			current_cell_position.x = current_mouse_position.x < (int)cellSideInPx ? 0 : abs((current_mouse_position.x + view_x) / (int)cellSideInPx) * (int)cellSideInPx;
			current_cell_position.y = current_mouse_position.y < (int)cellSideInPx ? 0 : abs((current_mouse_position.y + view_y) / (int)cellSideInPx) * (int)cellSideInPx;

			dc.SetPen(wxPen(wxColour(0, 0, 255, 128), 5));

			wxRect cellSelected(wxSize(cellSideInPx, cellSideInPx));
			cellSelected.SetLeft(0);
			cellSelected.Offset(current_cell_position.x, current_cell_position.y);
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
				if(rect.Contains(point))
				{
					s.Printf("%d\n%d", point.x, point.y);
					dc.GetTextExtent(s, &w, &h);
					dc.DrawText(s, point.x+w/2, point.y+h/4);
				}
				auto cell = iter->second;
				if(cell.texture_floor)
				{
					wxBitmap bitmap = m_data.get_texture_bitmap(cell.texture_floor);
					if(bitmap.IsOk())
						dc.DrawBitmap(bitmap, point);
				}
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

		void OnMotion(wxMouseEvent& event)
		{
			current_mouse_position = event.GetPosition();
			wxString s;
			s.Printf("Position: (%d,%d)", current_mouse_position.x, current_mouse_position.y);
			((wxFrame*)m_mgr->GetManagedWindow())->SetStatusText(s);
			Refresh();
		}

		void ShowContextMenu(const wxPoint& pos)
		{
			wxString s;
			wxMenu menu;

			s.Printf("Position: (%d,%d)", current_mouse_position.x, current_mouse_position.y);
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
				menu.Append(wxID_OPEN, "Open image");
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

		void OnLeftUp(wxMouseEvent& WXUNUSED(event))
		{
			wxTreeItemId id = m_data.cell_tree_item(current_cell_position);
			if(id)
			{
				m_tree->EnsureVisible(id);
				m_tree->ScrollTo(id);
				m_tree->SelectItem(id);
			}
			if(current_texture.IsOk())
			{
				m_data.set_texture_floor(current_cell_position, current_texture);
			}
		}

		void OnFileOpen(wxCommandEvent& WXUNUSED(event))
		{
			wxFileDialog dialog(this, "Please choose image", wxEmptyString, wxEmptyString, "*.jpg;*.png;*.*", wxFD_OPEN);
			if (dialog.ShowModal() == wxID_OK)
			{
				wxString filename(dialog.GetPath());
				current_texture = m_data.add_texture_floor(current_cell_position, wxFileName(filename));
				if(current_texture.IsOk())
					SetCursor(wxCursor(current_texture.thumbnail));
				//wxLogError();
				wxLogMessage(filename);
			}
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
	EVT_LEFT_UP(MapBoardCtrl::OnLeftUp)
	EVT_MENU(wxID_OPEN, MapBoardCtrl::OnFileOpen)
wxEND_EVENT_TABLE()
