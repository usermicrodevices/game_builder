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
#include <string>

enum TextureType
{
	TT_FLOOR = 0,
	TT_WALL,
	TT_ROOF
};

enum WallType
{
	WT_DEFAULT = 0,
	WT_HERO,
	WT_DOOR,
	WT_KEY,
	WT_NPS
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
	wxImage thumbnail = wxNullImage;
	wxBitmap bitmap;
};

typedef std::unordered_map<int, Texture> TextureContainer;

class Cell
{
public:
	int id = 0;
	int side = 50;
	int texture_floor = -1;
	int texture_wall = -1;
	int texture_roof = -1;
	WallType wtp = WT_DEFAULT;
	std::string script;

	Cell(int idx=0, int side_size=50, int tex_floor=-1, int tex_wall=-1, int tex_roof=-1, WallType wt=WT_DEFAULT, const std::string& scrpt=std::string())
	{
		id = idx;
		side = side_size;
		if(tex_floor > -1)
			texture_floor = tex_floor;
		if(tex_wall > -1)
			texture_wall = tex_wall;
		if(tex_roof > -1)
			texture_roof = tex_roof;
		if(wt != WT_DEFAULT)
			wtp = wt;
		if(!scrpt.empty())
			script = scrpt;
	}

	int get_visible_texture()
	{
		if(texture_roof > -1)
			return texture_roof;
		if(texture_wall > -1)
			return texture_wall;
		return texture_floor;
	}
};

struct wxPointHash
{
	std::size_t operator()(const wxPoint& p) const
	{
		return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
	}
};

typedef std::unordered_map<wxPoint, Cell, wxPointHash> CellContainer;

class Data
{
private:
	Texture m_empty_texture;
	int m_cell_side;//cell side size in DIPs
	int m_count_cell_x, m_count_cell_y;
	wxSize m_cell_size;
	TextureContainer textures = {};
	CellContainer cells = {};

public:
	Data(int cell_side_size=50, int count_x=100, int count_y=100)
	{
		m_count_cell_x = count_x;
		m_count_cell_y = count_y;
		m_empty_texture = Texture();
		m_cell_side = cell_side_size;
		m_cell_size = wxSize(m_cell_side, m_cell_side);
		//std::cout << "textures count " << count_textures() << std::endl;
		//std::cout << "cells count " << count_cells() << std::endl;
	}

	~Data()
	{
		textures.clear();
		cells.clear();
	}

	int count_cell_x()
	{
		return m_count_cell_x;
	}

	int count_cell_y()
	{
		return m_count_cell_y;
	}

	void append_cell(int x, int y, int cell_side_size=50, int t_floor=-1, int t_wall=-1, int t_roof=-1, WallType wltp=WT_DEFAULT, const std::string& scrpt=std::string())
	{
		cells[wxPoint(x, y)] = Cell(cells.size(), cell_side_size, t_floor, t_wall, t_roof, wltp, scrpt);
	}

	void append_cell_default(int x, int y)
	{
		cells[wxPoint(x, y)] = Cell(cells.size(), m_cell_side);
	}

	bool has_data()
	{
		return count_textures() && count_cells();
	}

	int cell_side_size()
	{
		return m_cell_side;
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

	size_t cell_index(wxPoint p)
	{
		return cells[p].id;
	}

	Cell cell(wxPoint p)
	{
		return cells[p];
	}

	int count_textures()
	{
		return textures.size();
	}

	void set_cell_wall_type(wxPoint p, WallType value)
	{
		if(cells[p].wtp != value)
			cells[p].wtp = value;
	}

	void set_cell_script(wxPoint p, std::string value)
	{
		if(cells[p].script != value)
			cells[p].script = value;
	}

	void append_texture(int id, const wxString& path)
	{
		textures[id] = Texture(id, wxFileName(path), m_cell_size);
	}

	const Texture& add_texture(wxPoint p, const wxFileName& path, TextureType tt=TT_FLOOR)
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
		switch(tt)
		{
			case TT_WALL:
				cells[p].texture_wall = texid;
				break;
			case TT_ROOF:
				cells[p].texture_roof = texid;
				break;
			case TT_FLOOR:
				cells[p].texture_floor = texid;
				break;
			default:
				break;
		}
		return textures[texid];
	}

	const wxBitmap& get_texture_bitmap(int id)
	{
		return textures[id].bitmap;
	}

	const wxBitmap& get_visible_texture_bitmap(Cell& cell)
	{
		return textures[cell.get_visible_texture()].bitmap;
	}

	const Texture& get_texture(int id)
	{
		return textures[id];
	}

	const Texture& get_texture(wxPoint p, TextureType tt=TT_FLOOR)
	{
		int texid = -1;
		switch(tt)
		{
			case TT_WALL:
				texid = cells[p].texture_wall;
				break;
			case TT_ROOF:
				texid = cells[p].texture_roof;
				break;
			case TT_FLOOR:
				texid = cells[p].texture_floor;
				break;
			default:
				break;
		}
		if(texid < 0)
			return m_empty_texture;
		return textures[texid];
	}

	void clear_texture(wxPoint p, TextureType tt=TT_FLOOR)
	{
		switch(tt)
		{
			case TT_WALL:
				cells[p].texture_wall = -1;
				break;
			case TT_ROOF:
				cells[p].texture_roof = -1;
				break;
			case TT_FLOOR:
				cells[p].texture_floor = -1;
				break;
			default:
				break;
		}
	}

	void set_texture(wxPoint p, const Texture& tex, TextureType tt=TT_FLOOR)
	{
		if(cells.find(p) == cells.end())
			cells[p] = Cell(cells.size(), m_cell_side);
		switch(tt)
		{
			case TT_WALL:
				cells[p].texture_wall = tex.id;
				break;
			case TT_ROOF:
				cells[p].texture_roof = tex.id;
				break;
			case TT_FLOOR:
				cells[p].texture_floor = tex.id;
				break;
			default:
				break;
		}
	}

	wxString ToString(const wxString& indentation = "")
	{
		wxString content(indentation+
		"{\n"+indentation+"\t\"count_x\":"<<m_count_cell_x<<",\n"+
		indentation+"\t\"count_y\":"<<m_count_cell_y<<",\n"+
		indentation+"\t\"default_side_size\":"<<m_cell_side<<",\n"+
		indentation+"\t\"textures\":\n"+indentation+"\t{\n");
		if(!textures.empty())
		{
			for(const auto& [k, v] : textures)
			{
				if(!v.path.GetFullPath().IsEmpty())
					content.Append(indentation+"\t\t"<<k<<":\n"+indentation+"\t\t{\n"+indentation+"\t\t\t\"path\":\""<<v.path.GetFullPath()<<"\"\n"+indentation+"\t\t},\n");
			}
		}
		content.Append(indentation+"\t},\n");
		if(cells.size())
		{
			content.Append(indentation+"\t\"cells\":\n"+indentation+"\t{");
			bool first_line = true;
			for(const auto& [k, v] : cells)
			{
				if(v.side != m_cell_side || v.texture_floor > -1 || v.texture_roof > -1 || v.texture_wall > -1)
				{
					if(first_line)
					{
						first_line = false;
						content.Append("\n");
					}
					else
						content.Append(",\n");
					content.Append(indentation+"\t\t\""<<k.x<<"-"<<k.y<<"\":\n"+indentation+"\t\t{\n");
					//content.Append(indentation+"\t\t\t\"id\":"<<v.id<<",\n");
					if(v.side != m_cell_side)
						content.Append(indentation+"\t\t\t\"side\":"<<v.side<<",\n");
					if(v.texture_floor > -1)
						content.Append(indentation+"\t\t\t\"floor\":"<<v.texture_floor<<",\n");
					if(v.texture_roof > -1)
						content.Append(indentation+"\t\t\t\"roof\":"<<v.texture_roof<<",\n");
					if(v.texture_wall > -1)
						content.Append(indentation+"\t\t\t\"wall\":"<<v.texture_wall<<",\n");
					if(v.wtp != WT_DEFAULT)
						content.Append(indentation+"\t\t\t\"type\":"<<v.wtp<<",\n");
					if(!v.script.empty())
						content.Append(indentation+"\t\t\t\"script\":\""<<v.script<<"\",\n");
					content.Append(indentation+"\t\t}");
				}
			}
			content.Append("\n"+indentation+"\t}");
		}
		content.Append("\n"+indentation+"}");
		return content;
	}

	wxString ToFile(const wxString &filename)
	{
		wxFile f(filename, wxFile::write);
		if(f.IsOpened())
		{
			f.Write(ToString());
			f.Close();
		}
		return filename;
	}
};
