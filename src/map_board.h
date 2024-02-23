class MapBoardCtrl : public wxScrolledWindow
{
	public:

		MapBoardCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, wxAuiManager* mgr = nullptr)
		: wxScrolledWindow(parent, id, pos, size, wxNO_BORDER)
		{
			m_mgr = mgr;
		}
		

	private:

		wxAuiManager* m_mgr;
		static const int CellSideInDIPs = 50;
		static const int MapInDIPs = 3000;
		
		void OnPaint(wxPaintEvent& WXUNUSED(evt))
		{
			wxString s;
			int h, w, height;
			
			size_t mapInPx = FromDIP(MapInDIPs);
			size_t cellSideInPx = FromDIP(CellSideInDIPs);
			
			wxPaintDC dc(this);
			wxSize size = GetClientSize();
			
			//wxRect cellSelected(wxSize(cellSideInPx, cellSideInPx));
			//cellSelected.SetLeft(0);
			//cellSelected.Offset(mouse_x, mouse_y);
			//dc.DrawRectangle(cellSelected);
			
			dc.SetBrush(wxBrush(wxColour(200, 0, 0, 50)));
			dc.SetPen(wxPen(wxColour(0, 0, 255, 128), 10));
			dc.DrawRectangle(0, 0, size.x, size.y);

			dc.SetPen(wxPen(wxColour(0, 200, 0, 128), 5));
			dc.DrawLine(0, 0, size.x, size.y);
			dc.DrawLine(0, size.y, size.x, 0);
			
			dc.SetPen(wxPen(wxColour(0, 255, 0, 128), 10));
			for(size_t xpos = cellSideInPx; xpos < mapInPx; xpos+=CellSideInDIPs)
				dc.DrawLine(xpos, 0, xpos, mapInPx);
			for(size_t ypos = cellSideInPx; ypos < mapInPx; ypos+=CellSideInDIPs)
				dc.DrawLine(0, ypos, mapInPx, ypos);
			
			s.Printf("Size: %d x %d", size.x, size.y);
			
			dc.SetFont(*wxNORMAL_FONT);
			dc.GetTextExtent(s, &w, &height);
			height += FromDIP(3);
			dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2));
			
			if (m_mgr)
			{
				wxAuiPaneInfo pi = m_mgr->GetPane(this);
				
				s.Printf("Layer: %d", pi.dock_layer);
				dc.GetTextExtent(s, &w, &h);
				dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));
				
				s.Printf("Dock: %d Row: %d", pi.dock_direction, pi.dock_row);
				dc.GetTextExtent(s, &w, &h);
				dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));
				
				s.Printf("Position: %d", pi.dock_pos);
				dc.GetTextExtent(s, &w, &h);
				dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));
				
				s.Printf("Proportion: %d", pi.dock_proportion);
				dc.GetTextExtent(s, &w, &h);
				dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*4));
			}
		}
		
		void OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
		{
			// intentionally empty
		}
		
		void OnSize(wxSizeEvent& WXUNUSED(evt))
		{
			Refresh();
		}	
		
		wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MapBoardCtrl, wxScrolledWindow)
EVT_PAINT(MapBoardCtrl::OnPaint)
EVT_SIZE(MapBoardCtrl::OnSize)
EVT_ERASE_BACKGROUND(MapBoardCtrl::OnEraseBackground)
wxEND_EVENT_TABLE()
