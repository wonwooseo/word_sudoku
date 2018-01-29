#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;
#include "grid.h"

bool sortrule(string a, string b) {
	return a.length() > b.length(); //descending order (longest first)
}

// read grid file and create Grid object
Grid* readGrid(string filename) {
	Grid* newgrid = new Grid();
	
	fstream gridfile;
	gridfile.open(filename);
	
	// read 1 line from stream and store it in vector
	string line;
	vector<string> temp;
	int x = 0; // # of chars in one line
	int y = 0; // # of lines in file
	while(getline(gridfile, line)) {
		if(line.size() == 1) break;
		if(line.back() == '\r') line.erase(line.end() - 1);
		y++;
		temp.push_back(line);
		if(x != 0) continue;
		x = line.length();
	}

	// Declare 2D-array grid
	Grid::Cell** grid = new Grid::Cell*[y];
	int i;
	for(i = 0; i < y; i++) {
		grid[i] = new Grid::Cell[x];
	}

	// Initializing Cells
	int b = 0; // for blank count
	for(i = 0; i < y; i++) {
		string res = temp[i];
		int j;
		for(j = 0; j < x; j++) {
			if(res[j] == '_') {
				grid[i][j].val = ' ';
				b++;
			}
			else grid[i][j].val = res[j];
			grid[i][j].xc = j;
			grid[i][j].yc = i;
			grid[i][j].origval.push(grid[i][j].val);
		}
	}

	// cleanup and return
	newgrid->cells = grid;
	newgrid->blank = b;
	newgrid->node = 0;
	gridfile.close();
	return newgrid;
}

vector<string>* readWord(string filename) {
	vector<string>* v = new vector<string>;
	fstream wordfile;
	wordfile.open(filename);
	
	string line;
	while(getline(wordfile, line)) {
		if(line.size() == 1) break;
		if(line.back() == '\r') line.erase(line.end() - 1);
		for(size_t i = 0; i < line.length(); i++) {
			line[i] = toupper(line[i]);
		}
		v->push_back(line);
	}
	
	// Least constraining value(shortest word first)
	sort(v->begin(), v->end(), sortrule);
	return v;
}

void writeGrid(Grid* result, string filename) {
	fstream outfile;
	filename.erase(filename.end() - 4, filename.end());
	outfile.open(filename+"result.txt", fstream::out);
	for(int i = 0; i < 9; i++) {
		for(int j = 0; j < 9; j++) {
			Grid::Cell* tmp = &(result->cells[i][j]);
			outfile << tmp->val;
		}
		outfile << endl;
	}
	outfile.close();
	return;
}

void destroyGrid(Grid* input) {
	for(int i = 0; i < 9; i++) {
		delete[] input->cells[i];
	}
	delete[] input->cells;
	delete input;
	return;
}

// helper function for assigning & consistency checking
int assignvalue(Grid* initial, Grid::Cell* target, string word, char to) {
	int rollback = 0;
	if(to == 'h') {
		for(size_t j = 0; j < word.length(); j++) {
			Grid::Cell* tcell = &(initial->cells[target->yc][target->xc + j]);
			if(tcell->val == ' ') initial->blank--;
			tcell->val = word[j];
			tcell->origval.push(word[j]);
		}
		// row consistency check
		vector<char> rowchk;
		for(int j = 0; j < 9; j++) {
			char tmp = initial->cells[target->yc][j].val;
			if(tmp != ' ') rowchk.push_back(tmp);
		}
		size_t rchk = rowchk.size();
		sort(rowchk.begin(), rowchk.end());
		vector<char>::iterator rit = unique(rowchk.begin(), rowchk.end());
		rowchk.resize(distance(rowchk.begin(), rit));
		if(rchk != rowchk.size()) rollback = 1;
		// column consistency check
		if(rollback != 1) {
			vector<char> colchk;
			for(int j = 0; j < 9; j++) {
				char tmp = initial->cells[j][target->xc].val;
				if(tmp != ' ') colchk.push_back(tmp);
			}
			size_t cchk = colchk.size();
			sort(colchk.begin(), colchk.end());
			vector<char>::iterator cit = unique(colchk.begin(), colchk.end());
			colchk.resize(distance(colchk.begin(), cit));
			if(cchk != colchk.size()) rollback = 1;
		}
		// 3x3 consistency check
		if(rollback != 1) {
			int xsp = target->xc / 3;
			int ysp = 3 * (target->yc / 3);
			for( ; xsp < 3; xsp++) { // subgrid
				vector<char> sgridchk;
				for(int k = 0; k < 3; k++) { // subgrid y-coord
					for(int l = 0; l < 3; l++) { // subgrid x-coord
						char tmp = initial->cells[ysp + k][(3 * xsp) + l].val;
						if(tmp != ' ') sgridchk.push_back(tmp);
					}
				}
				// check if subgrid has duplicate entries
				size_t gchk = sgridchk.size();
				sort(sgridchk.begin(), sgridchk.end());
				vector<char>::iterator it = unique(sgridchk.begin(), sgridchk.end());
				sgridchk.resize(distance(sgridchk.begin(), it));
				if(gchk != sgridchk.size()) {
					rollback = 1; // Not consistent; ROLLBACK TO origval
					break;
				}
			}
		}
	}
	
	if(to == 'v') {
		for(size_t j = 0; j < word.length(); j++) {
			Grid::Cell* tcell = &(initial->cells[target->yc + j][target->xc]);
			if(tcell->val == ' ') initial->blank--;
			tcell->val = word[j];
			tcell->origval.push(word[j]);
		}
		// row consistency check
		vector<char> rowchk;
		for(int j = 0; j < 9; j++) {
			char tmp = initial->cells[target->yc][j].val;
			if(tmp != ' ') rowchk.push_back(tmp);
		}
		size_t rchk = rowchk.size();
		sort(rowchk.begin(), rowchk.end());
		vector<char>::iterator rit = unique(rowchk.begin(), rowchk.end());
		rowchk.resize(distance(rowchk.begin(), rit));
		if(rchk != rowchk.size()) rollback = 1;
		// column consistency check
		if(rollback != 1) {
			vector<char> colchk;
			for(int j = 0; j < 9; j++) {
				char tmp = initial->cells[j][target->xc].val;
				if(tmp != ' ') colchk.push_back(tmp);
			}
			size_t cchk = colchk.size();
			sort(colchk.begin(), colchk.end());
			vector<char>::iterator cit = unique(colchk.begin(), colchk.end());
			colchk.resize(distance(colchk.begin(), cit));
			if(cchk != colchk.size()) rollback = 1;
		}
		// 3x3 consistency check
		if(rollback != 1) {
			int xsp = 3 * (target->xc / 3);
			int ysp = target->yc / 3;
			for( ; ysp < 3; ysp++) { // subgrid
				vector<char> sgridchk;
				for(int k = 0; k < 3; k++) { // subgrid y-coord
					for(int l = 0; l < 3; l++) { // subgrid x-coord
						char tmp = initial->cells[(3 * ysp) + k][xsp + l].val;
						if(tmp != ' ') sgridchk.push_back(tmp);
					}
				}
				// check if subgrid has duplicate entries
				size_t chk = sgridchk.size();
				sort(sgridchk.begin(), sgridchk.end());
				vector<char>::iterator it = unique(sgridchk.begin(), sgridchk.end());
				sgridchk.resize(distance(sgridchk.begin(), it));
				if(chk != sgridchk.size()) {
					rollback = 1; // Not consistent; ROLLBACK TO origval
					break;
				}
			}
		}
	}
	return rollback;
}

// helper function for rollback
void rollback(Grid* initial, Grid::Cell* target, string word, char to) {
	if(to == 'h') {
		for(size_t j = 0; j < word.length(); j++) {
			Grid::Cell* tmpcell = &(initial->cells[target->yc][target->xc + j]);
			tmpcell->origval.pop();
			char tmp = tmpcell->origval.top();
			tmpcell->val = tmp;
			if(tmp == ' ') initial->blank++;
		}
	}
	else if(to == 'v') {
		for(size_t j = 0; j < word.length(); j++) {
			Grid::Cell* tmpcell = &(initial->cells[target->yc + j][target->xc]);
			tmpcell->origval.pop();
			char tmp = tmpcell->origval.top();
			tmpcell->val = tmp;
			if(tmp == ' ') initial->blank++;
		}
	}
}

// helper function for reverting changes in cells when failure
void revertchange(Grid* initial, Grid::Cell* target, vector<string>* wordbank, string word, char to) {
	if(to == 'h') {
		for(size_t j = 0; j < word.length(); j++) {
			Grid::Cell* tmpcell = &(initial->cells[target->yc][target->xc + j]);
			tmpcell->origval.pop();
			char tmp = tmpcell->origval.top();
			tmpcell->val = tmp;
			if(tmp == ' ') initial->blank++;
		}
	}
	else if(to == 'v') {
		for(size_t j = 0; j < word.length(); j++) {
			Grid::Cell* tmpcell = &(initial->cells[target->yc + j][target->xc]);
			tmpcell->origval.pop();
			char tmp = tmpcell->origval.top();
			tmpcell->val = tmp;
			if(tmp == ' ') initial->blank++;
		}
	}
	wordbank->insert(wordbank->begin(), word);
}

// Backtracking search function
int search(Grid* initial, vector<string>* wordbank) {
	// node expanded
	initial->node++;
	// completion check
	if(initial->blank == 0 && wordbank->size() == 0) {
		return 0;
	}
	
	// Variable selection & value assignation
	for(int y = 0; y < 9; y++) {
		for(int x = 0; x < 9; x++) {
			Grid::Cell* target = &(initial->cells[y][x]);
			string word = (*wordbank)[0];
			size_t c;
			char to = ' ';
			// check row
			if(target->xc + word.length() <= 9) { // check overflow
				for(c = 0; c < word.length(); c++) {
					if(initial->cells[target->yc][target->xc + c].val == ' ') continue;
					else if(initial->cells[target->yc][target->xc + c].val == word[c]) continue;
					else break;
				}
				if(c == word.length()) {
					to = 'h';
				}
			}
			// check colulmn
			if(target->yc + word.length() <= 9) { // check overflow
				for(c = 0; c < word.length(); c++) {
					if(initial->cells[target->yc + c][target->xc].val == ' ') continue;
					else if(initial->cells[target->yc + c][target->xc].val == word[c]) continue;
					else break;
				}
				if(c == word.length()) {
					if(to == ' ') to = 'v';
					if(to == 'h') to = 'a';
				}
			}
			if(to == ' ') continue; // value not consistent
			// assignation & consistency check
			if(to == 'h') {
				int rb = assignvalue(initial, target, word, to);
				if(rb == 1) {
					rollback(initial, target, word, to);
					continue;
				}
			}
			else if(to == 'v') {
				int rb = assignvalue(initial, target, word, to);
				if(rb == 1) {
					rollback(initial, target, word, to);
					continue;
				}
			}
			else { // if both h and v are possible; try h first and then v
				int rb = assignvalue(initial, target, word, 'h');
				if(rb == 1) {
					rollback(initial, target, word, 'h');
					int rb2 = assignvalue(initial, target, word, 'v');
					if(rb2 == 1) {
						rollback(initial, target, word, 'v');
						continue;
					}
					to = 'v'; // h already failed, only v possible
				} 
			}
		
			// Consistent;
			wordbank->erase(wordbank->begin());
			// recursive call to search, with updated grid
			int result = search(initial, wordbank);
			// returned result ok, return ok
			if(result != 1) return 0;
			// failure, revert change in cells, blank and wordbank
			if(to == 'h') revertchange(initial, target, wordbank, word, to);
			else if(to == 'v') revertchange(initial, target, wordbank, word, to);
			else { // h failed, now try v
				revertchange(initial, target, wordbank, word, 'h');
				int rb = assignvalue(initial, target, word, 'v');
				if(rb == 1) {
					rollback(initial, target, word, 'v');
					continue;
				}
				wordbank->erase(wordbank->begin());
				int result = search(initial, wordbank);
				if(result != 1) return 0;
				revertchange(initial, target, wordbank, word, 'v');
			}
		}
	}
	return 1;
}

// entry of program
int main(int argc, char* argv[]) {
	if(argc != 3) {
		cout << "Usage: ./wsudoku <grid file> <wordbank file>" << endl;
		return 1;
	}
	string gridname = argv[1];
	string wordname = argv[2];
	
	Grid* grid;
	grid = readGrid(gridname);
	
	vector<string>* v; 
	v = readWord(wordname);

	int result = search(grid, v);
	
	cout << "Number of nodes expanded: " << grid->node << endl;
	writeGrid(grid, gridname);
	destroyGrid(grid);
	delete v;
	
	if(result == 1) cout << "Failed" << endl;
	else cout << "Finished successfully" << endl;
	return result;
}

