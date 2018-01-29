#include <stack>

class Grid {
	public:
		// cell member class
		class Cell {
			public:
			// value of cell
			char val;
			// x-coordinate of cell
			int xc;
			// y-coordinate of cell
			int yc;
			// value of cell before modification; for backtracking
			stack<char> origval;
		};
		
		// for node counting
		int node;
		
		// number of blank cells
		int blank;
		
		// pointer to 2d array
		Cell** cells;
};
