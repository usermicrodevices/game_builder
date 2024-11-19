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

	~Texture()
	{
#if DEBUG
		std::cout << "!!! ~Texture " << path.GetFullPath() << std::endl;
#endif
		path.Clear();
		thumbnail.Destroy();
#if DEBUG
		std::cout << "	~Texture !!!" << std::endl;
#endif
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

	~Cell()
	{
#if DEBUG
		std::cout << "!!! ~Cell " << id << std::endl;
#endif
		script.clear();
#if DEBUG
		std::cout << "	~Cell !!!" << std::endl;
#endif
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

typedef std::unordered_map<wxPoint, std::shared_ptr<Cell>, wxPointHash> CellContainer;

class Data
{
private:
	Texture m_empty_texture;
	int m_cell_side;//cell side size in DIPs
	int m_count_cell_x, m_count_cell_y;
	wxSize m_cell_size;
	TextureContainer m_textures = {};
	CellContainer m_cells = {};

public:
	// Data()
	// {
	// 	m_empty_texture = Texture();
	// }

	Data(int cell_side_size=50, int count_x=100, int count_y=100)
	{
		m_empty_texture = Texture();
		init(cell_side_size, count_x, count_y);
	}

	void init(int cell_side_size=50, int count_x=100, int count_y=100)
	{
		m_count_cell_x = count_x;
		m_count_cell_y = count_y;
		m_cell_side = cell_side_size;
		m_cell_size = wxSize(m_cell_side, m_cell_side);
	}

	Data& operator=(const Data& other)
	{
		m_count_cell_x = other.count_cell_x();
		m_count_cell_y = other.count_cell_y();
		m_empty_texture = other.get_empty_texture();
		m_cell_side = other.cell_side_size();
		m_cell_size = other.get_cell_size();
		m_textures = other.get_textures();
		m_cells = other.get_cells();
		return *this;
	}

	~Data()
	{
		//for(const auto& [k, v] : m_cells)
			//delete &v;//ERROR: double free or corruption (!prev)
		m_textures.clear();
		m_cells.clear();
#if DEBUG
		std::cout << "!!! ~Data !!!" << std::endl;
#endif
	}

	const wxSize& get_cell_size() const
	{
		return const_cast<Data*>(this)->m_cell_size;
	}

	const TextureContainer& get_textures() const
	{
		return const_cast<Data*>(this)->m_textures;
	}

	const CellContainer& get_cells() const
	{
		return const_cast<Data*>(this)->m_cells;
	}

	int count_cell_x() const
	{
		return const_cast<Data*>(this)->m_count_cell_x;
	}

	int count_cell_y() const
	{
		return const_cast<Data*>(this)->m_count_cell_y;
	}

	void append_cell(int x, int y, int cell_side_size=50, int t_floor=-1, int t_wall=-1, int t_roof=-1, WallType wltp=WT_DEFAULT, const std::string& scrpt=std::string())
	{
		m_cells[wxPoint(x, y)] = std::make_shared<Cell>(m_cells.size(), cell_side_size, t_floor, t_wall, t_roof, wltp, scrpt);
	}

	void append_cell_default(int x, int y)
	{
		m_cells[wxPoint(x, y)] = std::make_shared<Cell>(m_cells.size(), m_cell_side);
	}

	bool has_data() const
	{
		return count_textures() && count_cells();
	}

	int cell_side_size() const
	{
		return const_cast<Data*>(this)->m_cell_side;
	}

	int count_cells() const
	{
		return const_cast<Data*>(this)->m_cells.size();
	}

	auto cells_begin()
	{
		return m_cells.begin();
	}

	auto cells_end()
	{
		return m_cells.end();
	}

	size_t cell_index(wxPoint p)
	{
		return m_cells[p]->id;
	}

	std::shared_ptr<Cell> cell(wxPoint p)
	{
		return m_cells[p];
	}

	int count_textures() const
	{
		return const_cast<Data*>(this)->m_textures.size();
	}

	void set_cell_wall_type(wxPoint p, WallType value)
	{
		if(m_cells[p]->wtp != value)
			m_cells[p]->wtp = value;
	}

	void set_cell_script(wxPoint p, std::string value)
	{
		if(m_cells[p]->script != value)
			m_cells[p]->script = value;
	}

	void append_texture(int id, const wxString& path)
	{
		m_textures[id] = Texture(id, wxFileName(path), m_cell_size);
	}

	const Texture& add_texture(wxPoint p, const wxFileName& path, TextureType tt=TT_FLOOR)
	{
		bool texture_exists = false;
		int texid = m_textures.size();
		if(texid)
		{
			for(const auto& [k, v] : m_textures)
			{
				if(v.path == path)
				{
					texid = k;
					texture_exists = true;
				}
			}
		}
		if(!texture_exists)
			m_textures[texid] = Texture(texid, path, m_cell_size);
		switch(tt)
		{
			case TT_WALL:
				m_cells[p]->texture_wall = texid;
				break;
			case TT_ROOF:
				m_cells[p]->texture_roof = texid;
				break;
			case TT_FLOOR:
				m_cells[p]->texture_floor = texid;
				break;
			default:
				break;
		}
		return m_textures[texid];
	}

	const wxBitmap& get_texture_bitmap(int id)
	{
		return m_textures[id].bitmap;
	}

	const wxBitmap& get_visible_texture_bitmap(const std::shared_ptr<Cell>& cell)
	{
		return m_textures[cell->get_visible_texture()].bitmap;
	}

	const Texture& get_texture(int id)
	{
		return m_textures[id];
	}

	const Texture& get_empty_texture() const
	{
		return const_cast<Data*>(this)->m_empty_texture;
	}

	const Texture& get_texture(wxPoint p, TextureType tt=TT_FLOOR)
	{
		int texid = -1;
		switch(tt)
		{
			case TT_WALL:
				texid = m_cells[p]->texture_wall;
				break;
			case TT_ROOF:
				texid = m_cells[p]->texture_roof;
				break;
			case TT_FLOOR:
				texid = m_cells[p]->texture_floor;
				break;
			default:
				break;
		}
		if(texid < 0)
			return m_empty_texture;
		return m_textures[texid];
	}

	void clear_texture(wxPoint p, TextureType tt=TT_FLOOR)
	{
		switch(tt)
		{
			case TT_WALL:
				m_cells[p]->texture_wall = -1;
				break;
			case TT_ROOF:
				m_cells[p]->texture_roof = -1;
				break;
			case TT_FLOOR:
				m_cells[p]->texture_floor = -1;
				break;
			default:
				break;
		}
	}

	void set_texture(wxPoint p, const Texture& tex, TextureType tt=TT_FLOOR)
	{
		if(m_cells.find(p) == m_cells.end())
			m_cells[p] = std::make_shared<Cell>(m_cells.size(), m_cell_side);
		switch(tt)
		{
			case TT_WALL:
				m_cells[p]->texture_wall = tex.id;
				break;
			case TT_ROOF:
				m_cells[p]->texture_roof = tex.id;
				break;
			case TT_FLOOR:
				m_cells[p]->texture_floor = tex.id;
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
		if(!m_textures.empty())
		{
			for(const auto& [k, v] : m_textures)
			{
				if(!v.path.GetFullPath().IsEmpty())
					content.Append(indentation+"\t\t"<<k<<":\n"+indentation+"\t\t{\n"+indentation+"\t\t\t\"path\":\""<<v.path.GetFullPath()<<"\"\n"+indentation+"\t\t},\n");
			}
		}
		content.Append(indentation+"\t},\n");
		if(m_cells.size())
		{
			content.Append(indentation+"\t\"cells\":\n"+indentation+"\t{");
			bool first_line = true;
			for(const auto& [k, v] : m_cells)
			{
				if(v->side != m_cell_side || v->texture_floor > -1 || v->texture_roof > -1 || v->texture_wall > -1)
				{
					if(first_line)
					{
						first_line = false;
						content.Append("\n");
					}
					else
						content.Append(",\n");
					content.Append(indentation+"\t\t\""<<k.x<<"-"<<k.y<<"\":\n"+indentation+"\t\t{\n");
					//content.Append(indentation+"\t\t\t\"id\":"<<v->id<<",\n");
					if(v->side != m_cell_side)
						content.Append(indentation+"\t\t\t\"side\":"<<v->side<<",\n");
					if(v->texture_floor > -1)
						content.Append(indentation+"\t\t\t\"floor\":"<<v->texture_floor<<",\n");
					if(v->texture_roof > -1)
						content.Append(indentation+"\t\t\t\"roof\":"<<v->texture_roof<<",\n");
					if(v->texture_wall > -1)
						content.Append(indentation+"\t\t\t\"wall\":"<<v->texture_wall<<",\n");
					if(v->wtp != WT_DEFAULT)
						content.Append(indentation+"\t\t\t\"type\":"<<v->wtp<<",\n");
					if(!v->script.empty())
						content.Append(indentation+"\t\t\t\"script\":\""<<v->script<<"\",\n");
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
