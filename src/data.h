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
	TextureContainer textures = {};
	CellContainer cells = {};

public:
	Data(int cell_side_size=50, int count_x=100, int count_y=100)
	{
		for(int i=0; i<count_x; ++i)
		{
			for(int j=0; j<count_y; ++j)
			{
				cells[wxPoint(i * cell_side_size, j * cell_side_size)] = Cell(cell_side_size);
			}
		}
	};

	~Data()
	{
		textures.clear();
		cells.clear();
	}

	int count_textures()
	{
		return textures.size();
	}
	
	int count_cells()
	{
		return cells.size();
	}
	
	wxString ToFile(const wxString &filename)
	{
		wxFile f(filename, wxFile::write);
		//std::cout << filename << std::endl;
		//auto tex = textures.cbegin();
		//std::cout << cells.size() << std::endl;
		//std::cout << tex->second;
		// try
		// {
		// 	textures.clear();
		// }
		// catch(std::exception& e)
		// {
		// 	std::cout << e.what();
		// }
		if(f.IsOpened())
		{
			f.Write("{\n");

			f.Write("\t\"textures\":\n\t{\n");
			if(!textures.empty())
			{
				f.Write("\t\"textures\":\n\t{\n");
				// for(const auto& [k, v] : textures)
				// {
				// 	f.Write(wxString::Format("%d:""%s"",\n", k, v.path.GetFullPath()));
				// }
				f.Write(wxString("},\n"));
			}
			//f.Flush();

			if(cells.size())
			{
				f.Write("""cells"":\n");
				f.Write(wxString("{\n"));
				for(const auto& [k, v] : cells)
				{
					f.Write(wxString::Format("""%d-%d"":\n", k.x, k.y));
					f.Write("{\n");
					f.Write(wxString::Format("""side"":%d,\n", v.side));
					if(v.texture_floor)
						f.Write(wxString::Format("""floor"":%d,\n", v.texture_floor));
					if(v.texture_ceiling)
						f.Write(wxString::Format("""ceiling"":%d,\n", v.texture_ceiling));
					if(v.texture_wall)
						f.Write(wxString::Format("""wall"":%d,\n", v.texture_wall));
					if(v.texture_wall)
						f.Write(wxString::Format("""wall"":%d,\n", v.texture_wall));
					if(v.ctp != CT_WALL)
						f.Write(wxString::Format("""type"":%d,\n", (int)v.ctp));
					f.Write("},\n");
				}
				f.Write(wxString("},\n"));
			}

			f.Write("}");
			//f.Flush();
			f.Close();
		}
		return filename;
	}
};

//wxIMPLEMENT_DYNAMIC_CLASS(Data, wxObject);
