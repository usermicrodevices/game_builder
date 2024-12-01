#pragma once

#if defined(__WXX11__)
#define USE_CONTEXT_MENU 0
#else
#define USE_CONTEXT_MENU 1
#endif

#include <memory>

#include "data.h"
#include "gb.h"

enum EraserType
{
	ET_NONE = 0,
	ET_FLOOR,
	ET_WALL,
	ET_ROOF
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
	Menu_Popup_Open_Wall,
	Menu_Popup_Set_Wall_Eraser,
	Menu_Popup_Open_Roof,
	Menu_Popup_Set_Roof_Eraser,

	Menu_PopupChoice
};


class MapBoardCtrl : public wxScrolledWindow
{

public:

	MapBoardCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxHSCROLL | wxVSCROLL, const wxString& name = "MapBoardCtrl", wxTreeCtrl* tree = nullptr, wxPropertyGridManager* prop_grid_mgr = nullptr, bool fill_test_data=false)
	: wxScrolledWindow(parent, id, pos, size, wxHSCROLL | wxVSCROLL, name)
	{
		m_tree = tree;
		m_prgrmgr = prop_grid_mgr;
		if(fill_test_data)
			FillData();
		//FitInside();
		//SetTargetWindow(parent);
		//LogMessage(wxString::Format("cells count %d", m_data->count_cells()));
		//LogMessage(wxString::Format("virtual_size %dx%d", m_virtual_size.x, m_virtual_size.y));
#if DEBUG
		std::cout << "!!! MapBoardCtrl parent " << wxString(wxTheApp->GetTopWindow()->GetClassInfo()->GetClassName()) << " !!!" << std::endl;
#endif
	}

	~MapBoardCtrl()
	{
		//std::cout << "!!! 0 ~MapBoardCtrl " << GetName() << " !!!" << std::endl;
		if(m_tree->IsEnabled())
		{
			//std::cout << "!!! 1 ~MapBoardCtrl " << GetName() << " !!!" << std::endl;
			if(!m_tree->IsEmpty())
				m_tree->UnselectAll();
			//std::cout << "!!! 2 ~MapBoardCtrl " << GetName() << " !!!" << std::endl;
			if(m_level_tree_item.IsOk())
			{
				m_tree->DeleteChildren(m_level_tree_item);
				m_tree->Delete(m_level_tree_item);
			}
		}
		if(m_prgrmgr->IsEnabled())
		{
			m_prgrmgr->GetGrid()->Freeze();
			m_prgrmgr->GetGrid()->Refresh();
		}
		//std::cout << "!!! 3 ~MapBoardCtrl " << GetName() << " !!!" << std::endl;
		//wxDELETE(m_tree);
		//wxDELETE(m_prgrmgr);
	}

	void FillData(std::shared_ptr<Data> d = std::make_shared<Data>())
	{
		m_level_tree_item = m_tree->AppendItem(m_tree->GetRootItem(), GetName(), 0);
		m_data = d;
		if(m_data->has_data())
		{
			if(m_level_tree_item.IsOk())
			{
				for(auto iter = m_data->cells_begin(); iter != m_data->cells_end(); ++iter)
				{
					auto point = iter->first;
					m_tree_items.Add(m_tree->AppendItem(m_level_tree_item, wxString::Format("%d-%d", point.x, point.y), 1));
				}
			}
		}
		else
		{
			int cell_side_size=50;
			int count_x=100;
			int count_y=100;
			m_data->init(cell_side_size, count_x, count_y);
			int x = 0, y = 0;
			for(int i=0; i<count_x; ++i)
			{
				x = i * cell_side_size;
				for(int j=0; j<count_y; ++j)
				{
					y = j * cell_side_size;
					m_data->append_cell_default(x, y);
					if(m_level_tree_item.IsOk())
						m_tree_items.Add(m_tree->AppendItem(m_level_tree_item, wxString::Format("%d-%d", x, y), 1));
				}
			}
		}
		int cell_side = m_data->cell_side_size();
		m_virtual_size = wxSize(cell_side*m_data->count_cell_x(), cell_side*m_data->count_cell_y());
		SetVirtualSize(m_virtual_size);
		SetScrollbars(cell_side, cell_side, m_virtual_size.x/cell_side, m_virtual_size.y/cell_side);
		if(m_level_tree_item.IsOk())
		{
			m_tree->EnsureVisible(m_level_tree_item);
			m_tree->ScrollTo(m_level_tree_item);
			m_tree->SelectItem(m_level_tree_item);
		}
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

	bool HasData()
	{
		return m_data->has_data();
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
			return m_data->ToString(indentation);
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
			wxLogMessage(m_data->ToFile(filename));
		}
		catch(std::exception& e)
		{
			wxLogMessage(e.what());
		}
	}

	void set_cell_wall_type(WallType value)
	{
		m_data->set_cell_wall_type(m_current_click_position, value);
	}

	void set_cell_script(const wxString& value)
	{
		m_data->set_cell_script(m_current_click_position, value.utf8_string());
	}

private:
	bool m_draw_coords = false;
	EraserType m_eraser = ET_NONE;
	bool m_togle_mouse = false;
	TextureType m_current_texture_type = TT_FLOOR;
	std::shared_ptr<Texture> m_current_texture = std::make_shared<Texture>("MapBoardCtrl");
	std::shared_ptr<Data> m_data;
	wxSize m_virtual_size;
	wxArrayTreeItemIds m_tree_items;
	wxTreeItemId m_level_tree_item;
	wxPoint m_current_mouse_position = wxDefaultPosition;
	wxPoint m_current_cell_position = wxDefaultPosition;
	wxPoint m_current_click_position = wxDefaultPosition;
	wxTreeCtrl* m_tree;
	wxPropertyGridManager* m_prgrmgr;

	void OnPaint(wxPaintEvent& WXUNUSED(event))
	{
		int cell_side = m_data->cell_side_size();
		size_t cellSideInPx = FromDIP(cell_side);
		size_t mapxInPx = FromDIP(m_virtual_size.x);
		size_t mapyInPx = FromDIP(m_virtual_size.y);

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

		int h, w;
		wxString str_point;
		dc.SetFont(*wxNORMAL_FONT);
		wxRect rect(view_x, view_y, size.x, size.y);
		for(auto iter = m_data->cells_begin(); iter != m_data->cells_end(); ++iter)
		{
			auto point = iter->first;
			auto cell = iter->second;
			int tex_id = cell->get_visible_texture();
			if(tex_id > -1)
			{
				wxBitmap bitmap = m_data->get_texture_bitmap(tex_id);
				if(bitmap.IsOk())
					dc.DrawBitmap(bitmap, point);
			}
			else if(m_draw_coords)
			{
				if(rect.Contains(point))
				{
					str_point.Printf("%d\n%d", point.x, point.y);
					dc.GetTextExtent(str_point, &w, &h);
					dc.DrawText(str_point, point.x+w/2, point.y+h/4);
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
			menu.Append(Menu_Popup_Set_Floor_Eraser, "🧹"+_("Floor eraser"));
			menu.AppendSeparator();
			menu.Append(Menu_Popup_Open_Wall, "📂🧱"+_("Open wall image"));
			menu.Append(Menu_Popup_Set_Wall_Eraser, "🪚"+_("Wall eraser"));
			menu.AppendSeparator();
			menu.Append(Menu_Popup_Open_Roof, "📂⛺️"+_("Open roof image"));
			menu.Append(Menu_Popup_Set_Roof_Eraser, "🪓"+_("Roof eraser"));
			menu.AppendSeparator();
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

	void set_pgprop_tex(wxPGProperty* pgprop, std::shared_ptr<Texture> texture)
	{
		if(!pgprop)
			wxLogError("wxPGProperty path");
		else if(!texture)
		{
#if DEBUG
			std::cout << "!!! MapBoardCtrl::set_pgprop_tex texture is nullptr !!!" << std::endl;
#endif
			wxLogError("MapBoardCtrl::set_pgprop_tex texture is nullptr");
		}
		else
		{
#if DEBUG
			std::cout << "!!! MapBoardCtrl::set_pgprop_tex " << texture->id << " !!!" << std::endl;
#endif
			if(texture->id > -1)
			{
#if DEBUG
				wxLogMessage("🩸"+texture->path.GetFullPath()+"🩸");
#endif
				//if(!pgprop->SetValueFromString(texture->path.GetFullPath(), wxPGPropValFormatFlags::FullValue|wxPGPropValFormatFlags::ReportError))
					//wxLogError("🩸wxPGProperty.SetValueFromString 🧵" + texture->path.GetFullPath()+"🧵");
				pgprop->SetValueFromString(texture->path.GetFullPath(), wxPGPropValFormatFlags::FullValue);
#if DEBUG
				std::cout << "!!! MapBoardCtrl::set_pgprop_tex " << texture->path.GetFullPath() << " !!!" << std::endl;
#endif
			}
			else
				pgprop->SetValueFromString("", wxPGPropValFormatFlags::FullValue);
		}
	}

	void refresh_pgproperty(std::shared_ptr<Cell> cell)
	{
		if(!m_prgrmgr->GetGrid()->IsFrozen())
			m_prgrmgr->GetGrid()->Freeze();
		std::shared_ptr<Texture> tex = m_data->get_texture(cell->texture_floor);
		if(tex)
			set_pgprop_tex(m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetPropertyByNameA("path_floor"), tex);
		tex = m_data->get_texture(cell->texture_wall);
		if(tex)
			set_pgprop_tex(m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetPropertyByNameA("path_wall"), tex);
		tex = m_data->get_texture(cell->texture_roof);
		if(tex)
			set_pgprop_tex(m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetPropertyByNameA("path_roof"), tex);
		wxPGProperty* prop_wt = m_prgrmgr->GetGrid()->wxPropertyGridInterface::	GetPropertyByNameA("wall_type");
		wxAny v = prop_wt->GetValue();
		if(v.As<int>() != (int)cell->wtp)
			prop_wt->SetValue(WXVARIANT((int)cell->wtp));
		if(m_current_cell_position.x > -1 && m_current_cell_position.y > -1)
		{
			wxPGProperty* coords = m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetProperty("coords");
			if(!coords)
				wxLogError("wxPGProperty coords");
			coords->SetValueFromString(wxString::Format("%d x %d", m_current_cell_position.x, m_current_cell_position.y));
		}
		wxPGProperty* prop_script = m_prgrmgr->GetGrid()->wxPropertyGridInterface::GetProperty("script");
		if(cell->script.empty())
			prop_script->SetValueFromString(wxString(""));
		else
			prop_script->SetValueFromString(wxString(cell->script));
		if(m_prgrmgr->GetGrid()->IsFrozen())
			m_prgrmgr->GetGrid()->Thaw();
	}

	void store_currents()
	{
		if(m_current_texture->IsOk())
		{
			m_data->set_texture(m_current_cell_position, m_current_texture, m_current_texture_type);
			if((int)m_tree_items.size() <= m_data->cell(m_current_cell_position)->id)
			{
				wxTreeItemId id = m_tree->AppendItem(m_level_tree_item, wxString::Format("%d-%d", m_current_cell_position.x, m_current_cell_position.y), 1);
				m_tree_items.Add(id);
			}
		}
		else
		{
			switch(m_eraser)
			{
				case ET_FLOOR:
					m_data->clear_texture(m_current_cell_position, TT_FLOOR);
					break;
				case ET_WALL:
					m_data->clear_texture(m_current_cell_position, TT_WALL);
					break;
				case ET_ROOF:
					m_data->clear_texture(m_current_cell_position, TT_ROOF);
					break;
				default:
					break;
			}
		}
	}

	void OnMotion(wxMouseEvent& event)
	{
		m_current_mouse_position = event.GetPosition();
		wxString s;
		s.Printf("Position: (%d,%d)", m_current_mouse_position.x, m_current_mouse_position.y);
		((wxFrame*)wxTheApp->GetTopWindow())->SetStatusText(s);
		//dynamic_cast<wxTheApp->GetTopWindow()->GetClassInfo()->GetClassName()>(wxTheApp->GetTopWindow())->SetStatusText(s);
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
		{
			const std::shared_ptr<Cell>& c = m_data->cell(m_current_cell_position);
			refresh_pgproperty(c);
			wxTreeItemId id = nullptr;
			if((int)m_tree_items.size() <= c->id)
			{
				id = m_tree->AppendItem(m_level_tree_item, wxString::Format("%d-%d", m_current_cell_position.x, m_current_cell_position.y), 1);
				m_tree_items.Add(id);
				wxLogDebug(wxString("🌿size=") << m_tree_items.size() << "; id=" << c->id);
			}
			else
			{
				wxLogDebug(wxString("🧊size=") << m_tree_items.size() << "; id=" << c->id);
				id = m_tree_items.Item((size_t)c->id);
			}
			if(id)
			{
				if(id.IsOk())
				{
					m_tree->EnsureVisible(id);
					m_tree->ScrollTo(id);
					m_tree->SelectItem(id);
				}
			}
		}
		m_togle_mouse = false;
		Refresh();
	}

	void OpenTexureFile(TextureType tt=TT_FLOOR)
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
			m_current_texture_type = tt;
			m_current_texture = m_data->add_texture(m_current_cell_position, wxFileName(path_result), m_current_texture_type);
			if(m_current_texture->IsOk())
				SetCursor(wxCursor(m_current_texture->thumbnail));
#if DEBUG
			std::cout << "!!! MapBoardCtrl::OpenTexureFile " << path_result << " !!!" << std::endl;
#endif
			refresh_pgproperty(m_data->cell(m_current_cell_position));
			m_eraser = ET_NONE;
			wxLogMessage(path_result);
		}
	}

	void SetEraser(EraserType et=ET_FLOOR)
	{
		if(m_current_texture->id > -1)
			m_current_texture = m_data->get_empty_texture();
		SetCursor(wxCURSOR_NO_ENTRY);
		m_eraser = et;
	}

	void OnOpenFloorFile(wxCommandEvent& WXUNUSED(event))
	{
		OpenTexureFile();
	}

	void OnOpenWallFile(wxCommandEvent& WXUNUSED(event))
	{
		OpenTexureFile(TT_WALL);
	}

	void OnOpenRoofFile(wxCommandEvent& WXUNUSED(event))
	{
		OpenTexureFile(TT_ROOF);
	}

	void OnSetFloorEraser(wxCommandEvent& WXUNUSED(event))
	{
		SetEraser();
	}

	void OnSetWallEraser(wxCommandEvent& WXUNUSED(event))
	{
		SetEraser(ET_WALL);
	}

	void OnSetRoofEraser(wxCommandEvent& WXUNUSED(event))
	{
		SetEraser(ET_ROOF);
	}

	void OnSetDefaultCursor(wxCommandEvent& WXUNUSED(event))
	{
		if(m_current_texture->id > -1)
			m_current_texture = m_data->get_empty_texture();
		SetCursor(*wxSTANDARD_CURSOR);
		m_eraser = ET_NONE;
	}

	wxDECLARE_EVENT_TABLE();
};

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
	EVT_MENU(Menu_Popup_Open_Wall, MapBoardCtrl::OnOpenWallFile)
	EVT_MENU(Menu_Popup_Set_Wall_Eraser, MapBoardCtrl::OnSetWallEraser)
	EVT_MENU(Menu_Popup_Open_Roof, MapBoardCtrl::OnOpenRoofFile)
	EVT_MENU(Menu_Popup_Set_Roof_Eraser, MapBoardCtrl::OnSetRoofEraser)
wxEND_EVENT_TABLE()
