#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>
#include <array>

 struct Sudoku {
	bool isValid;
	std::array<std::bitset<9>, 9> rows = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<9>, 9> columns = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<9>, 9> subgrid = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::vector<std::vector<char>> board;
};

 bool cellIsEmpty(char c) {
	return c == '0' || c == '.' ? true : false;
 }

 int charToInt(char c) {
	return (int) c - '0';
 }

 bool checkLine(std::string line) {

	for (char c : line) {
		if (!(isdigit(c) && (charToInt(c) >= 10))) {
			return false;
		}
	}

	return true;

 }

 int getSubgrid(int row, int col) {
	 return (int) (3 * (row / 3) + col / 3);
 }

 bool isValid(std::string line) {

	return line.size() == 81 ? true : false;

 }

 Sudoku lineToSudoku(std::string line) {

	Sudoku sudoku;
	sudoku.board.reserve(9);


	for (int i = 0; i < 9; i++) {
		std::vector<char> sudokuLine;
		sudokuLine.reserve(9);
		for (int j = 0; j < 9; j++) {
			int index = i * 9 + j;
			if (int a = charToInt(line[index]); (isdigit(line[i]) && (a < 10)) || (line[i] == '.')) {
				sudokuLine.push_back(line[index]);
				if (a != 0) {
					sudoku.rows[i].set(a - 1);
					sudoku.columns[j].set(a - 1);							
					sudoku.subgrid[getSubgrid(i, j)].set(a - 1);
				}
			}
			else {
				sudoku.isValid = false;
				return sudoku;
			}
		}
		sudoku.board.push_back(sudokuLine);
	}

	sudoku.isValid = true;
	return sudoku;

 }

 std::vector<Sudoku> loadBoards(std::ifstream& ifs) {
	 
	std::string line;
	std::vector<Sudoku> sudokus;									// all sudokus
	Sudoku sudoku;

	while (std::getline(ifs, line)){
		if (isValid(line)) {
			sudoku = lineToSudoku(line);
		}
		sudokus.push_back(sudoku);
	}

	if (ifs.eof())
	{
		//std::cout << "File read successful" << std::endl;
	}

	return sudokus;

 }

 void printBoard(Sudoku sudoku) {
	
	auto board = sudoku.board;
	
	for (int row = 0; row < (int)board.size(); row++) {
		if (row % 3 == 0) std::cout << "+ - - - - - - - - - - - +" << std::endl;
		for (int col = 0; col < (int)board[row].size(); col++) {
			if (col % 3 == 0) std::cout << "| ";
			std::cout << board[row][col];
			if (col == 8) std::cout << " |" << std::endl;
			else std::cout << ' ';
		}
	}
	std::cout << "+ - - - - - - - - - - - +" << std::endl;

}

 int nextRowNum(int row, int col) {
	return col == 8 ? ++row : row;
 }

 int nextColNum(int col) {
	 return (++col) % 9;
 }

 bool solveSudoku(Sudoku& sudoku, int row, int col){

	int nextCol = nextColNum(col);
	int nextRow = nextRowNum(row, col);

	if (row == 9) {
		 return true;						// sudoku is solved
	}

	if (!cellIsEmpty(sudoku.board[row][col])) {
		return solveSudoku(sudoku, nextRow, nextCol);
	}
	else {
		for (int i = 0; i < 9; i++) {
			if (sudoku.rows[row].test(i) || sudoku.columns[col].test(i) || sudoku.subgrid[getSubgrid(row, col)].test(i)) {
				continue;
			}
			else {
				char a = std::to_string(i+1)[0];
				sudoku.board[row][col] = a;
				sudoku.rows[row].set(i);
				sudoku.columns[col].set(i);
				sudoku.subgrid[getSubgrid(row, col)].set(i);
				if (solveSudoku(sudoku, nextRow, nextCol)) {
					return true;
				}
				sudoku.board[row][col] = std::to_string(0)[0];
				sudoku.rows[row].reset(i);
				sudoku.columns[col].reset(i);
				sudoku.subgrid[getSubgrid(row, col)].reset(i);
			}
		}
	}

	return false;
 }

 int main(int argc, char* argv[]) {

	std::ifstream ifs;
	std::ofstream ofs;


	if ((argc % 2) == 0) {
		std::cout << "Invalid number of parameters" << std::endl;
		return 1;
	}

	for (int i = 1; i < argc; i++){
		std::string arg = argv[i];

		if ((arg.compare("-i") == 0)) {					// if -i is recognised, next argument will be the input file
			if (i != (argc - 1)) {
				ifs.open(argv[i + 1]);
				i++;									// increment and read the parameter after that
				continue;
			}
			else {
				std::cout << "Invalid input file, please use -i <filename> to select a valid input file" << std::endl;
				return 1;
			}
		}
		else if ((arg.compare("-o") == 0)) {			// if -i is recognised, next argument will be the input file
			if (i != (argc - 1)) {
				ofs.open(argv[i + 1]);
				i++;									// increment and read the parameter after that
				continue;
			}
			else {
				std::cout << "Invalid output file, please use -o <filename> to select a valid output file" << std::endl;
				return 1;
			}
		}
	}

	std::vector<Sudoku> sudokus = loadBoards(ifs);

	//for (auto board : boards) {
	//	if (solveSudoku(board)) {
	//		printBoard(board);
	//	}
	//	//solveSudoku(sudoku, ofs);
	//}

	if (solveSudoku(sudokus[0], 0, 0)) {
		printBoard(sudokus[0]);
	}

	return 0;
}
