#include <unordered_map>
#include <wx/filename.h>
#include <wx/gdicmn.h>
#include <wx/image.h>

enum CellType
{
	CT_WALL = 0,
	CT_DOOR,
	CT_WINDOW,
	CT_NPS
};

class Texture
{
	wxFileName path;
	wxBitmap image;
};

class Cell
{
private:
	int side;
	int texture_floor;
	int texture_ceiling;
	int texture_wall;
	CellType ctp = CT_WALL;

public:
	Cell(int side_size=50, int tex_floor=-1, int tex_ceiling=-1, int tex_wall=-1, CellType ct=CT_WALL)
	{
		side = side_size;
		texture_floor = tex_floor;
		texture_ceiling = tex_ceiling;
		texture_wall = tex_wall;
		ctp = ct;
	}
};

class Data
{
private:
	std::unordered_map<int, Texture> textures = {};

	struct wxPointHash
	{
		std::size_t operator()(const wxPoint& p) const
		{
			using std::hash;
			return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
		}
	};
	std::unordered_map<wxPoint, Cell, wxPointHash> cells{};

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
};
