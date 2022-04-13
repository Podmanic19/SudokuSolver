#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>
#include <array>
#include <optional>

const int cellNum = 9;

enum class IO {

	standard, file

};

struct Sudoku {

	bool isValid = true;
	std::array<std::bitset<cellNum>, cellNum> rows    = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<cellNum>, cellNum> columns = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<cellNum>, cellNum> subgrid = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::vector<std::vector<char>> board;

};

bool cellIsEmpty(char c) {

	return c == '0' || c == '.' ? true : false;

}

int charToInt(char c) {
	return (int)c - '0';
}

int getSubgrid(int row, int col) {
	return (int)(3 * (row / 3) + col / 3);
}

bool isValid(std::string line) {

	return line.size() == (cellNum * cellNum) ? true : false;

}

Sudoku lineToSudoku(std::string line) {

	Sudoku sudoku;
	sudoku.board.reserve(cellNum);

	for (int i = 0; i < cellNum; i++) {
		std::vector<char> sudokuLine;
		sudokuLine.reserve(cellNum);
		for (int j = 0; j < cellNum; j++) {
			int index = i * cellNum + j;
			int a = line[index] == '.' || line[index] == 0 ? 0 : charToInt(line[index]);
			if ((isdigit(line[index]) && (a < 10)) || (line[index] == '.')) {				//chyba bola na tomto riadku
				sudokuLine.push_back(line[index]);
				if (a != 0) {
					if (sudoku.rows[i].test(a - 1) || sudoku.columns[j].test(a - 1) || sudoku.subgrid[getSubgrid(i, j)].test(a - 1)) {
						sudoku.isValid = false;
						return sudoku;
					}
					sudoku.rows[i].set(a - 1);
					sudoku.columns[j].set(a - 1);
					sudoku.subgrid[getSubgrid(i, j)].set(a - 1);
				}
			}
			else {
				exit(1);						// invalid character in line
			}
		}
		sudoku.board.push_back(sudokuLine);
	}

	sudoku.isValid = true;
	return sudoku;

}

std::vector<Sudoku> loadBoards(std::istream& ifs) {

	std::string line;
	std::vector<Sudoku> sudokus;
	Sudoku sudoku;

	while (std::getline(ifs, line)) {
		if (isValid(line)) {
			sudoku.isValid = true;
			sudoku = lineToSudoku(line);
		}
		else {
			std::cout << "Invalid line read in input.";
			exit(1);
		}
		sudokus.push_back(sudoku);
	}

	if (!ifs.eof()) {
		std::cout << "Error encountered while reading input file.";
		exit(1);
	}

	return sudokus;

}

void printSolution(Sudoku sudoku, std::ostream& ofs, bool last) {

	auto board = sudoku.board;
	if (!sudoku.isValid) {
		ofs << std::endl;
		return;
	}

	for (size_t row = 0; row < board.size(); row++) {
		for (int col = 0; col < (int)board[row].size(); col++) {
			ofs << board[row][col];
		}
	}

	if (!last) ofs << std::endl;

}

int nextRowNum(int row, int col) {

	return col == (cellNum - 1) ? ++row : row;

}

int nextColNum(int col) {

	return (++col) % cellNum;

}


bool solveSudoku(Sudoku& sudoku, int row, int col) {

	int nextCol = nextColNum(col);
	int nextRow = nextRowNum(row, col);

	if (row == cellNum) {
		return true;						// sudoku is solved
	}

	if (!cellIsEmpty(sudoku.board[row][col])) {
		return solveSudoku(sudoku, nextRow, nextCol);
	}
	else {
		for (int i = 0; i < cellNum; i++) {
			if (sudoku.rows[row].test(i) || sudoku.columns[col].test(i) || sudoku.subgrid[getSubgrid(row, col)].test(i)) {
				continue;											// i is not valid in this position
			}
			char a = std::to_string(i + 1)[0];						// guess i as the number
			sudoku.board[row][col] = a;
			sudoku.rows[row].set(i);
			sudoku.columns[col].set(i);
			sudoku.subgrid[getSubgrid(row, col)].set(i);
			if (solveSudoku(sudoku, nextRow, nextCol)) {			// guess successful
				return true;
			}
			sudoku.board[row][col] = std::to_string(0)[0];			// guess failed
			sudoku.rows[row].reset(i);
			sudoku.columns[col].reset(i);
			sudoku.subgrid[getSubgrid(row, col)].reset(i);
		}
	}

	return false;

}

void invalidFileError(std::string type) {

	std::string inSwitch = (type.compare("input") == 0) ? "-i" : "-o";
	std::cout << "Invalid " << type << " file, please use " << inSwitch << " <filename> to select a valid input file.";

}

bool badFiles(IO input, IO output, std::ifstream& ifs, std::ofstream& ofs) {

	if ((input == IO::file && ifs.fail()) && (output == IO::file && ofs.fail())) {
		invalidFileError("input");
		invalidFileError("output");
		return true;
	}
	if (input == IO::file && ifs.fail()) {
		invalidFileError("input");
		return true;
	}
	if (output == IO::file && ofs.fail()) {
		invalidFileError("output");
		return true;
	}

	return false;

}

int main(int argc, char* argv[]) {

	if ((argc % 2) == 0) {
		std::cout << "Invalid number of parameters" << std::endl;
		return 1;
	}

	std::ifstream ifs;
	std::ofstream ofs;
	IO input = IO::standard;
	IO output = IO::standard;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if ((arg.compare("-i") == 0)) {					// if -i is recognised, next argument will be the input file
			if (i != (argc - 1)) {
				ifs.open(argv[i + 1]);
				input = IO::file;
				i++;									// increment and read the parameter after that
				continue;
			}
			else {
				invalidFileError("input");
				return 1;
			}
		}
		else if ((arg.compare("-o") == 0)) {			// if -i is recognised, next argument will be the input file
			if (i != (argc - 1)) {
				ofs.open(argv[i + 1]);
				output = IO::file;
				i++;									// increment and read the parameter after that
				continue;
			}
			else {
				invalidFileError("output");
				return 1;
			}
		}
		else {
			std::cout << "Invalid argument." << std::endl;
			return 1;
		}
	}

	if (badFiles(input, output, ifs, ofs)) return 1;

	std::vector<Sudoku> sudokus = loadBoards(input == IO::file ? ifs : std::cin);

	for (size_t i = 0; i < sudokus.size(); i++) {
		auto board = sudokus[i];
		bool solved = false;

		if (board.isValid) 
			solved = solveSudoku(board, 0, 0);
		if (!solved) 
			board.isValid = false;

		printSolution(board, output == IO::file ? ofs : std::cout, i == (sudokus.size() - 1));

	}

	if (ifs.is_open()) ifs.close();
	if (ofs.is_open()) ofs.close();

	return 0;

}