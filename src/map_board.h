class MapBoardCtrl : public wxScrolledWindow
{
	public:

		MapBoardCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, wxAuiManager* mgr = nullptr)
		: wxScrolledWindow(parent, id, pos, size, wxNO_BORDER)
		{
			m_mgr = mgr;
		}


	private:

		static const int CellSideInDIPs = 50;
		static const int MapInDIPs = 3000;
		wxPoint current_mouse_position;
		wxAuiManager* m_mgr;

		void OnPaint(wxPaintEvent& WXUNUSED(evt))
		{
			size_t mapInPx = FromDIP(MapInDIPs);
			size_t cellSideInPx = FromDIP(CellSideInDIPs);

			wxPaintDC dc(this);
			wxSize size = GetClientSize();

			size_t x_cell_selected = (current_mouse_position.x < (int)cellSideInPx) ? 0 : abs(current_mouse_position.x / (int)cellSideInPx) * (int)cellSideInPx;
			size_t y_cell_selected = (current_mouse_position.y < (int)cellSideInPx) ? 0 : abs(current_mouse_position.y / (int)cellSideInPx) * (int)cellSideInPx;

			dc.SetPen(wxPen(wxColour(0, 0, 255, 128), 5));

			wxRect cellSelected(wxSize(cellSideInPx, cellSideInPx));
			cellSelected.SetLeft(0);
			cellSelected.Offset(x_cell_selected, y_cell_selected);
			dc.DrawRectangle(cellSelected);

			dc.SetBrush(wxBrush(wxColour(200, 0, 0, 50)));
			dc.DrawRectangle(0, 0, size.x, size.y);

			dc.SetPen(wxPen(wxColour(0, 255, 0, 128), 5));
			for(size_t xpos = cellSideInPx; xpos < mapInPx; xpos+=CellSideInDIPs)
				dc.DrawLine(xpos, 0, xpos, mapInPx);
			for(size_t ypos = cellSideInPx; ypos < mapInPx; ypos+=CellSideInDIPs)
				dc.DrawLine(0, ypos, mapInPx, ypos);

			//wxString s;
			//int h, w, height;
			
			//dc.SetPen(wxPen(wxColour(0, 200, 0, 128), 5));
			//dc.DrawLine(0, 0, size.x, size.y);
			//dc.DrawLine(0, size.y, size.x, 0);

			//s.Printf("Size: %d x %d", size.x, size.y);

			//dc.SetFont(*wxNORMAL_FONT);
			//dc.GetTextExtent(s, &w, &height);
			//height += FromDIP(3);
			//dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2));

			//if (m_mgr)
			//{
				//wxAuiPaneInfo pi = m_mgr->GetPane(this);

				//s.Printf("Layer: %d", pi.dock_layer);
				//dc.GetTextExtent(s, &w, &h);
				//dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));

				//s.Printf("Dock: %d Row: %d", pi.dock_direction, pi.dock_row);
				//dc.GetTextExtent(s, &w, &h);
				//dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));

				//s.Printf("Position: %d", pi.dock_pos);
				//dc.GetTextExtent(s, &w, &h);
				//dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));

				//s.Printf("Proportion: %d", pi.dock_proportion);
				//dc.GetTextExtent(s, &w, &h);
				//dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*4));
			//}
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

		wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MapBoardCtrl, wxScrolledWindow)
EVT_PAINT(MapBoardCtrl::OnPaint)
EVT_SIZE(MapBoardCtrl::OnSize)
EVT_ERASE_BACKGROUND(MapBoardCtrl::OnEraseBackground)
EVT_MOTION(MapBoardCtrl::OnMotion)
wxEND_EVENT_TABLE()
