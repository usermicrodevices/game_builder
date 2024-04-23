///////////////////////////////////////////////////////////////////////////////
// Name:        data.h
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-03-05
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/file.h>

#include <unordered_map>
//#include <map>

enum CellType
{
	CT_WALL = 0,
	CT_DOOR,
	CT_WINDOW,
	CT_NPS
};

class Texture
{
public:
	Texture(){};

	Texture(int index, const wxFileName& fullpath, const wxSize& size)
	{
		id = index;
		path = fullpath;
		//wxImage image(path.GetFullPath());//LOSS ALPHA
		bitmap = wxBitmap(path.GetFullPath());
		if(bitmap.IsOk())
		{
			thumbnail = bitmap.ConvertToImage();
			if(thumbnail.IsOk())
			{
				thumbnail = thumbnail.Rescale(size.GetWidth(), size.GetHeight());
				thumbnail.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, size.GetWidth()/2);
				thumbnail.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, size.GetHeight()/2);
				bitmap = wxBitmap(thumbnail);
			}
		}
	}

	bool IsOk()
	{
		return thumbnail.IsOk();
	}

	int id = -1;
	wxFileName path;
	wxImage thumbnail;// = wxNullImage;
	wxBitmap bitmap;
};

typedef std::unordered_map<int, Texture> TextureContainer;
//typedef std::map<int, Texture> TextureContainer;

class Cell
{
public:
	size_t id = -1;
	int side = 50;
	int texture_floor = -1;
	int texture_ceiling = -1;
	int texture_wall = -1;
	CellType ctp = CT_WALL;

	Cell(size_t idx=0, int side_size=50, int tex_floor=-1, int tex_ceiling=-1, int tex_wall=-1, CellType ct=CT_WALL)
	{
		id = idx;
		side = side_size;
		if(tex_floor > -1)
			texture_floor = tex_floor;
		if(tex_ceiling > -1)
			texture_ceiling = tex_ceiling;
		if(tex_wall > -1)
			texture_wall = tex_wall;
		if(ct != CT_WALL)
			ctp = ct;
	}
};

struct wxPointHash
{
	std::size_t operator()(const wxPoint& p) const
	{
		return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
	}
};
//class wxPointKey : public wxPoint
//{
//private:
	//int id;

//public:
	//wxPointKey(int vid=0, int vx=0, int vy=0)
	//{
		//id = vid;
		//x = vx;
		//y = vy;
	//}

	//bool operator <(const wxPointKey& p) const
	//{
		//return id < p.id;
	//}
//};

typedef std::unordered_map<wxPoint, Cell, wxPointHash> CellContainer;
//typedef std::map<wxPointKey, Cell> CellContainer;

class Data
{
private:
	Texture m_empty_texture;
	int m_cell_side;//cell side size in DIPs
	wxSize m_cell_size;
	wxSize m_virtual_size;
	wxArrayTreeItemIds tree_items;
	TextureContainer textures = {};
	CellContainer cells = {};

public:
	Data(){m_empty_texture = Texture();};//wxDECLARE_DYNAMIC_CLASS
	Data(wxTreeCtrl* tree, wxTreeItemId level_tree_item, int cell_side_size=50, int count_x=100, int count_y=100)
	{
		m_empty_texture = Texture();
		m_cell_side = cell_side_size;
		m_cell_size = wxSize(m_cell_side, m_cell_side);
		size_t idx = 0;
		int x = 0, y = 0;
		for(int i=0; i<count_x; ++i)
		{
			for(int j=0; j<count_y; ++j)
			{
				x = i * cell_side_size;
				y = j * cell_side_size;
				cells[wxPoint(x, y)] = Cell(idx, cell_side_size);
				tree_items.Add(tree->AppendItem(level_tree_item, wxString::Format("%d-%d", x, y), 1));
				++idx;
			}
		}
		m_virtual_size = wxSize(count_x*cell_side_size, count_y*cell_side_size);
	};

	~Data()
	{
		textures.clear();
		cells.clear();
	}

	int cell_side_size()
	{
		return m_cell_side;
	}

	int count_textures()
	{
		return textures.size();
	}

	int count_cells()
	{
		return cells.size();
	}

	auto cells_begin()
	{
		return cells.begin();
	}

	auto cells_end()
	{
		return cells.end();
	}

	wxSize virtual_size()
	{
		return m_virtual_size;
	}

	size_t cell_index(wxPoint p)
	{
		return cells[p].id;
	}

	Cell cell(wxPoint p)
	{
		return cells[p];
	}

	wxTreeItemId cell_tree_item(wxPoint p)
	{
		if(!tree_items.IsEmpty())
			return tree_items.Item(cells[p].id);
		return nullptr;
	}

	const Texture& add_texture_floor(wxPoint p, const wxFileName& path)
	{
		bool texture_exists = false;
		int texid = textures.size();
		if(texid)
		{
			for(const auto& [k, v] : textures)
			{
				if(v.path == path)
				{
					texid = k;
					texture_exists = true;
				}
			}
		}
		if(!texture_exists)
			textures[texid] = Texture(texid, path, m_cell_size);
		cells[p].texture_floor = texid;
		return textures[texid];
	}

	const Texture& get_texture_floor(wxPoint p)
	{
		int texid = cells[p].texture_floor;
		if(texid < 0)
			return m_empty_texture;
		return textures[texid];
	}

	void set_texture_floor(wxPoint p, const Texture& tex)
	{
		cells[p].texture_floor = tex.id;
	}

	const Texture& get_texture(int id)
	{
		return textures[id];
	}

	const wxBitmap& get_texture_bitmap(int id)
	{
		return textures[id].bitmap;
	}

	wxString ToFile(const wxString &filename)
	{
		wxFile f(filename, wxFile::write);
		if(f.IsOpened())
		{
			f.Write("{\n");
			f.Write("\t\"textures\":\n\t{\n");
			if(!textures.empty())
			{
				for(const auto& [k, v] : textures)
				{
					if(!v.path.GetFullPath().IsEmpty())
						f.Write(wxString::Format("\t\t%d:\n\t\t{\n\t\t\t\"path\":\"%s\"\n\t\t},\n", k, v.path.GetFullPath()));
				}
			}
			f.Write(wxString("\t},\n"));
			//f.Flush();
			if(cells.size())
			{
				f.Write("\t\"cells\":\n");
				f.Write(wxString("\t{\n"));
				bool first_line = true;
				for(const auto& [k, v] : cells)
				{
					if(first_line)
						first_line = false;
					else
						f.Write(",\n");
					f.Write(wxString::Format("\t\t\"%d-%d\":\n\t\t{\n", k.x, k.y));
					f.Write(wxString::Format("\t\t\t\"side\":%d,\n", v.side));
					if(v.texture_floor)
						f.Write(wxString::Format("\t\t\t\"floor\":%d,\n", v.texture_floor));
					if(v.texture_ceiling)
						f.Write(wxString::Format("\t\t\t\"ceiling\":%d,\n", v.texture_ceiling));
					if(v.texture_wall)
						f.Write(wxString::Format("\t\t\t\"wall\":%d,\n", v.texture_wall));
					if(v.texture_wall)
						f.Write(wxString::Format("\t\t\t\"wall\":%d,\n", v.texture_wall));
					if(v.ctp != CT_WALL)
						f.Write(wxString::Format("\t\t\t\"type\":%d,\n", (int)v.ctp));
					f.Write("\t\t}");
				}
				f.Write(wxString("\n\t}"));
			}
			f.Write("\n}");
			f.Close();
		}
		return filename;
	}
};

//wxIMPLEMENT_DYNAMIC_CLASS(Data, wxObject);
