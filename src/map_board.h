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
		MapBoardCtrl(){}//wxDECLARE_DYNAMIC_CLASS

		MapBoardCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = "scrolledWindow", wxAuiManager* mgr = nullptr)
		: wxScrolledWindow(parent, id, pos, size, wxHSCROLL | wxVSCROLL)
		{
			m_mgr = mgr;
			m_data = Data();
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

		//void FillTreeCtrl(wxDataViewTreeCtrl* tree = nullptr)
		void FillTreeCtrl(wxTreeCtrl* tree = nullptr)
		{
			if(tree)
			{
				wxTreeItemId level0 = tree->AppendItem(tree->GetRootItem(), "Level 0", 0);
				for(auto iter = m_data.cells_begin(); iter != m_data.cells_end(); ++iter)
				{
					auto point = iter->first;
					tree->AppendItem(level0, wxString::Format("%d-%d", point.x, point.y), 1);
				}
				tree->Expand(level0);
			}
		}
		
	private:
		Data m_data;
		wxPoint current_mouse_position;
		wxAuiManager* m_mgr;

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

			size_t x_cell_selected = current_mouse_position.x < (int)cellSideInPx ? 0 : abs((current_mouse_position.x + view_x) / (int)cellSideInPx) * (int)cellSideInPx;
			size_t y_cell_selected = current_mouse_position.y < (int)cellSideInPx ? 0 : abs((current_mouse_position.y + view_y) / (int)cellSideInPx) * (int)cellSideInPx;

			dc.SetPen(wxPen(wxColour(0, 0, 255, 128), 5));

			wxRect cellSelected(wxSize(cellSideInPx, cellSideInPx));
			cellSelected.SetLeft(0);
			cellSelected.Offset(x_cell_selected, y_cell_selected);
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
				static const char *choices[] = { "Apple", "Banana", "Cherry" };
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
			else // normal case, control not pressed
			{
				menu.Append(wxID_OPEN, "Open image");
				//menu.Append(Menu_Popup_ToBeGreyed, s, "This menu item should be initially greyed out");
				//menu.AppendCheckItem(Menu_Popup_ToBeChecked, "To be &checked");
				//menu.AppendSeparator();

				//menu.Enable(Menu_Popup_ToBeGreyed, false);
				//menu.Check(Menu_Popup_ToBeChecked, true);

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
	wxEND_EVENT_TABLE()
