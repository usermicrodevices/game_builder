///////////////////////////////////////////////////////////////////////////////
// Name:        data.h
// Purpose:     game builder: map and logic editor
// Author:      Maxim Kolosov
// Created:     2024-03-05
// Copyright:   (C) Copyright 2024, Maxim Kolosov
// Licence:     MIT Licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/file.h>

#include <unordered_map>

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
	wxFileName path;
	wxBitmap image;
};

typedef std::unordered_map<int, Texture> TextureContainer;

class Cell
{
public:
	int side;
	int texture_floor;
	int texture_ceiling;
	int texture_wall;
	CellType ctp = CT_WALL;

	Cell(int side_size=50, int tex_floor=-1, int tex_ceiling=-1, int tex_wall=-1, CellType ct=CT_WALL)
	{
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

typedef std::unordered_map<wxPoint, Cell, wxPointHash> CellContainer;

class Data
{
private:
	int m_cell_side;//cell side size in DIPs
	wxSize m_virtual_size;
	TextureContainer textures = {};
	CellContainer cells = {};

public:
	Data(int cell_side_size=50, int count_x=100, int count_y=100)
	{
		m_cell_side = cell_side_size;
		for(int i=0; i<count_x; ++i)
		{
			for(int j=0; j<count_y; ++j)
			{
				cells[wxPoint(i * cell_side_size, j * cell_side_size)] = Cell(cell_side_size);
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
					f.Write(wxString::Format("\t\t%d:\n\t\t{\t\t\t\"path\":\"%s\"\n\t\t},\n", k, v.path.GetFullPath()));
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
