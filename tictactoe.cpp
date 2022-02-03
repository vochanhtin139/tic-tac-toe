#include <iostream>
#include <cstring>
#include <time.h>
#include <cstdlib>
#include <sstream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <cctype>
#include <fstream>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define ENTER 13

//defining account stored in file
struct Users {
	char username[101], password[101];
};

//defining 
struct savedGame {
	char username[101], password[101], plr1_name[101], plr2_name[101];
	bool check_sound, check_PvE, check_move_suggestion, check_cheat_mode;
	int max_plr_name, board_color, time_limit, plr1_color, plr2_color, plr1_winning, plr2_winning, b_row, b_col, total_match;
};

//defining cells in board
struct Cell {
	char name[101];
	
	int length() {
		return strlen(name);
	}
};

//defining board
struct Board {
	struct Cell cell[101][101];
	int board_color;
	bool move_suggestion, PvE_mode, cheatMode;
	
	void resetBoard(int b_row, int b_col) {
		for (int i = 1; i <= b_row; i++) {
			for (int j = 1; j <= b_col; j++)
			cell[i][j].name[0] = 0;
		}
	}
	
	void markingCell(int x, int y, struct Cell plr_name) {
		strcpy(cell[x][y].name, plr_name.name);
	}
};

//variables definition
bool check_account = false, check_sound = false, checkLoadSavedGame = false;
struct Users current_user;
struct Board tic_tac_toe;
struct Cell plr1_name, plr2_name;
double time_limit = -1, last_time_limit = 0; 
int b_row, b_col, plr1_color, plr2_color, max_plr_name = 5, plr1_winning = 0, plr2_winning = 0, index_row = 0, index_col = 0, total_match = 0;

void setPlayerNameColor(int plr, int &plr_color) {
	char temp;
	
	cout << "Do you want to set player " << plr << "'s name color? [Y/N]: ";
	cin >> temp;
	
	if (temp == 'Y') {
		for (int i = 1; i <= 13; i++) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
			cout << "\t" << i << ". Sample Text\n";
		} 	
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		
		int color_code;
		cout << "Enter color code: ";
		cin >> color_code;
		
		plr_color = color_code;
		
		cout << endl;
	}
	else {
		plr_color = 7;
	}
}

void changeBoardBackgroundColor() {
	for (int i = 1; i <= 13; i++) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
		cout << "\t" << i << ". Sample Text\n";
	} 	
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	
	int color_code;
	cout << "Enter color code: ";
	cin >> color_code;
	
	tic_tac_toe.board_color = color_code;
}

//turn on background music
void turnOnBackgroundMusic() {
	//WARNING: You have to link -lwinmm to your project, otherwise, this project can't be compiled
	PlaySound(TEXT("background_music.wav"), NULL, SND_FILENAME|SND_LOOP|SND_ASYNC);
	check_sound = true;
}

void getDataInput() {
	char check_background, check_music, check_time_limit, check_move_suggestion, check_PvE, check_cheat_mode;
	tic_tac_toe.board_color = 7;
	
	//If this project can't execute on MacOS or Linux, please delete "#include <windows.h> and the next 3 following line and "changeBackgroundColor()" function:
	cout << "\nDo you want to change color of the board background? [Y/N]: ";
	cin >> check_background;
	if (check_background == 'Y') changeBoardBackgroundColor();
	
	cout << "\nDo you want to turn on background music? [Y/N]: ";
	cin >> check_music;
	if (check_music == 'Y') turnOnBackgroundMusic();
	
	cout << "\nDo you want to turn on time restriction mode? [Y/N]: ";
	cin >> check_time_limit;
	if (check_time_limit == 'Y') {
		cout << "Enter maxium value of time restriction mode (second): ";
		cin >> time_limit;
	}
	
	cout << "\nDo you want to turn on PvE mode? [Y/N]: ";
	cin >> check_PvE;
	if (check_PvE == 'Y') {
		cout << "WARNING: If you turn this feature on, the board size is set to 3x3 due to the complexity of Minimax algorithm with alpha-beta pruning\n";
		tic_tac_toe.PvE_mode = true;
	}
	else tic_tac_toe.PvE_mode = false;
	
	cout << "\nDo you want to turn on move suggestion? [Y/N]: ";
	cin >> check_move_suggestion;
	if (check_move_suggestion == 'Y') {
		if (!tic_tac_toe.PvE_mode) 
			cout << "WARNING: If you turn this feature on, the board size is set to 3x3 due to the complexity of Minimax algorithm with alpha-beta pruning\n";
		tic_tac_toe.move_suggestion = true;
	}
	else tic_tac_toe.move_suggestion = false;
	
	cout << "\nDo you want to turn on cheat mode (move replay)? [Y/N]: ";
	cin >> check_cheat_mode;
	if (check_cheat_mode == 'Y') tic_tac_toe.cheatMode = true;
	else tic_tac_toe.cheatMode = false;
	
	if (!tic_tac_toe.move_suggestion && !tic_tac_toe.PvE_mode) {
		cout << "\nEnter board size (row x col): ";
		cin >> b_row >> b_col;
	}
	else {
		b_row = 3;
		b_col = 3;
		cout << "\n";
	}
	
	cout << "\nEnter 1st player's icon (or name): ";
	cin.ignore();
	cin.getline(plr1_name.name, 101);
	if (max_plr_name < plr1_name.length()) max_plr_name = plr1_name.length();
	setPlayerNameColor(1, plr1_color);
	
	if (!tic_tac_toe.PvE_mode) {
		cout << "\nEnter 2nd player's icon (or name): ";
		cin.ignore();
		cin.getline(plr2_name.name, 101);
		if (max_plr_name < plr2_name.length()) max_plr_name = plr2_name.length();
		setPlayerNameColor(2, plr2_color);
	}
	else {
		strcpy(plr2_name.name, "COMPUTER_AI");
		if (max_plr_name < plr2_name.length()) max_plr_name = plr2_name.length();
		plr2_color = 11;
	}
	
	max_plr_name += 2;
}

//print ' --------'
void printRow(int len) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
	
	for (int i = 1; i <= b_col; i++) {
		cout << " ";
		for (int j = 1; j <= len; j++) cout << "-";
	} 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	
	cout << endl;
}

//print '|        '
void printCol(int len, int color, char description[101]) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
	cout << "|";
	
	for (int i = 1; i <= (len - strlen(description)) / 2; i++) cout << " ";
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	cout << description;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	
	for (int i = 1; i <= (len - strlen(description)) / 2 + (len - strlen(description)) % 2; i++) cout << " ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void printWinningCol(int len, int color, char description[101]) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
	cout << "|";
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	for (int i = 1; i <= (len - strlen(description)) / 2; i++) cout << " ";
	
	cout << description;

	for (int i = 1; i <= (len - strlen(description)) / 2 + (len - strlen(description)) % 2; i++) cout << " ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void printBoard(int pos, int x, int y, char current_plr[101]) {
	
	cout << "\n\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 79);
	if (time_limit > 0) cout << "\tTime remaining: " << time_limit - ((( (double) clock() ) / CLOCKS_PER_SEC) - last_time_limit) << "s\n\n";
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	cout << "\t";
	for (int i = 1; i <= b_col; i++) {
		char temp = 64 + i;
		for (int j = 1; j <= (max_plr_name - 1) / 2 + (max_plr_name - 1) % 2 + 1; j++) cout << " ";
		cout << temp;
		for (int j = 1; j <= (max_plr_name - 1) / 2; j++) cout << " ";
	}
	
	cout << "\n\n";
	
	for (int i = 1; i <= b_row; i++) {
		cout << "\t";
		printRow(max_plr_name);
		
		cout << "\t";
		for (int j = 1; j <= b_col; j++) printCol(max_plr_name, 7, "");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
		cout << "|\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		
		cout << i << "\t";
		for (int j = 1; j <= b_col; j++) {
			if (x == i && y == j) {
				char m_suggestion[101];
				m_suggestion[0] = 26;
				for (int z = 1; z <= max_plr_name - 2; z++) m_suggestion[z] = 32;
				m_suggestion[max_plr_name - 1] = 27;
				m_suggestion[max_plr_name] = '\0';
				
				if (strcmp(current_plr, plr1_name.name) == 0) printCol(max_plr_name, plr1_color, m_suggestion);
				else printCol(max_plr_name, plr2_color, m_suggestion);
			}
			else if (strcmp(tic_tac_toe.cell[i][j].name, plr1_name.name) == 0) printCol(max_plr_name, plr1_color, tic_tac_toe.cell[i][j].name);
			else printCol(max_plr_name, plr2_color, tic_tac_toe.cell[i][j].name);
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
		cout << "|\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		
		cout << "\t";
		for (int j = 1; j <= b_col; j++){
			if ((i - 1) * b_col + j == pos) printCol(max_plr_name, 79, "CHOOSE!");
			else printCol(max_plr_name, 7, "");	
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
		cout << "|\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	}
	
	cout << "\t";
	printRow(max_plr_name);
}

void printWinningBoard(int pos[3]) {
	cout << "\t";
	for (int i = 1; i <= b_col; i++) {
		char temp = 64 + i;
		for (int j = 1; j <= (max_plr_name - 1) / 2 + (max_plr_name - 1) % 2 + 1; j++) cout << " ";
		cout << temp;
		for (int j = 1; j <= (max_plr_name - 1) / 2; j++) cout << " ";
	}
	
	cout << "\n\n";
	
	for (int i = 1; i <= b_row; i++) {
		cout << "\t";
		printRow(max_plr_name);
		
		cout << "\t";
		for (int j = 1; j <= b_col; j++) {
			bool check = false;
			for (int z = 0; z < 3; z++) {
				if ((i - 1) * b_col + j == pos[z]) {
					printWinningCol(max_plr_name, 79, "");
					check = true;
					break;
				}
			}
			
			if (!check) printWinningCol(max_plr_name, 7, "");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
		cout << "|\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		
		cout << i << "\t";
		for (int j = 1; j <= b_col; j++) {
			if (strcmp(tic_tac_toe.cell[i][j].name, plr1_name.name) == 0) {
				bool check = false;
				
				for (int z = 0; z < 3; z++) {
					if ((i - 1) * b_col + j == pos[z]) {
						printWinningCol(max_plr_name, 79, tic_tac_toe.cell[i][j].name);
						check = true;
						break;
					}
				}
				
				if (!check) printWinningCol(max_plr_name, plr1_color, tic_tac_toe.cell[i][j].name);
			}
			else {
				bool check = false;
				
				for (int z = 0; z < 3; z++) {
					if ((i - 1) * b_col + j == pos[z]) {
						printWinningCol(max_plr_name, 79, tic_tac_toe.cell[i][j].name);
						check = true;
						break;
					}
				}
				
				if (!check) printWinningCol(max_plr_name, plr2_color, tic_tac_toe.cell[i][j].name);
			}
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
		cout << "|\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		
		cout << "\t";
		for (int j = 1; j <= b_col; j++) {
			bool check = false;
			
			for (int z = 0; z < 3; z++) {
				if ((i - 1) * b_col + j == pos[z]) {
					printWinningCol(max_plr_name, 79, "");
					check = true;
					break;
				}
			}
			
			if (!check) printWinningCol(max_plr_name, 7, "");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), tic_tac_toe.board_color);
		cout << "|\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	}
	
	cout << "\t";
	printRow(max_plr_name);
}

bool checkVertical(int x, int y, struct Cell board[101][101]) {
	if (strcmp(board[x][y].name, board[x + 1][y].name) == 0 && strcmp(board[x][y].name, board[x + 2][y].name) == 0)
		return true;
	return false;
}

bool checkHorizontal(int x, int y, struct Cell board[101][101]) {
	if (strcmp(board[x][y].name, board[x][y + 1].name) == 0 && strcmp(board[x][y].name, board[x][y + 2].name) == 0)
		return true;
	return false;
}

bool checkDiagonalRight(int x, int y, struct Cell board[101][101]) {
	if (strcmp(board[x][y].name, board[x + 1][y + 1].name) == 0 && strcmp(board[x][y].name, board[x + 2][y + 2].name) == 0)
		return true;
	return false;
}

bool checkDiagonalLeft(int x, int y, struct Cell board[101][101]) {
	if (x + 2 > b_row || y - 2 < 1) return false;
	
	if (strcmp(board[x][y].name, board[x + 1][y - 1].name) == 0 && strcmp(board[x][y].name, board[x + 2][y - 2].name) == 0)
		return true;
		
	return false;
}

char *checkWinning(int &xx, int &yy, int &mode, struct Cell board[101][101]) {
	int cnt = 0;
	
	for (int i = 1; i <= b_row; i++) {
		for (int j = 1; j <= b_col; j++) {
			if (board[i][j].length() != 0) {
				cnt++;
				
				if (checkVertical(i, j, tic_tac_toe.cell)) {
					xx = i; yy = j; mode = 1;
					return board[i][j].name;
				}
				else if (checkHorizontal(i, j, tic_tac_toe.cell)) {
					xx = i; yy = j; mode = 2;
					return board[i][j].name;
				}	
				else if (checkDiagonalRight(i, j, tic_tac_toe.cell)) {
					xx = i; yy = j; mode = 3;
					return board[i][j].name;
				}	
				else if (checkDiagonalLeft(i, j, tic_tac_toe.cell)) {
					xx = i; yy = j; mode = 4;
					return board[i][j].name;
				}		
			}
		}
	}
	
	if (cnt == b_row * b_col) return "TIE";
	
	return "\0";
}

int Minimax_PLR1(int b_row, int b_col, struct Cell board[101][101], bool flag) {
	int i, j, xx, yy, mode, value = 1, max_value = -1000, min_value = 1000;
	char check_winning[101];
	
	strcpy(check_winning, checkWinning(xx, yy, mode, board));
	
	if (strcmp(check_winning, plr1_name.name) == 0) return 10;
	if (strcmp(check_winning, plr2_name.name) == 0) return -10;
	if (strcmp(check_winning, "TIE") == 0) return 0;
	
	int score[101][101];
	for (int i = 1; i <= b_row; i++) {
		for (int j = 1; j <= b_col; j++)
		score[i][j] = 1; 
	}
	
	for (int i = 1; i <= b_row; i++) {
		for (int j = 1; j <= b_col; j++) {
			if (board[i][j].length() == 0) {
				if (min_value > max_value) {
					if (flag) {
						strcpy(board[i][j].name, plr1_name.name);
						value = Minimax_PLR1(b_row, b_col, board, false);
					}
					else {
						strcpy(board[i][j].name, plr2_name.name);
						value = Minimax_PLR1(b_row, b_col, board, true);
					}
					
					board[i][j].name[0] = 0;
					score[i][j] = value;
				}
			}
		}
	}
	
	if (flag) {
		max_value = -1000;
    	for (int i = 1; i <= b_row; i++) {
    		for (int j = 1; j <= b_col; j++) {
    			if (max_value < score[i][j] && score[i][j] != 1) {
    				max_value = score[i][j];
    				index_row = i;
    				index_col = j;
				}
			}
		}
		
		return max_value;
	}
	else {
		min_value = 1000;
		for (int i = 1; i <= b_row; i++) {
    		for (int j = 1; j <= b_col; j++) {
    			if (min_value > score[i][j] && score[i][j] != 1) {
    				min_value = score[i][j];
    				index_row = i;
    				index_col = j;
				}
			}
		}
		
		return min_value;
	}
}

int Minimax_PLR2(int b_row, int b_col, struct Cell board[101][101], bool flag) {
	int i, j, xx, yy, mode, value = 1, max_value = -1000, min_value = 1000;
	char check_winning[101];
	
	strcpy(check_winning, checkWinning(xx, yy, mode, board));
	if (strcmp(check_winning, plr2_name.name) == 0) return 10;
	if (strcmp(check_winning, plr1_name.name) == 0) return -10;
	if (strcmp(check_winning, "TIE") == 0) return 0;
	
	int score[101][101];
	for (int i = 1; i <= b_row; i++) {
		for (int j = 1; j <= b_col; j++)
		score[i][j] = 1; 
	}
	
	for (int i = 1; i <= b_row; i++) {
		for (int j = 1; j <= b_col; j++) {
			if (board[i][j].length() == 0) {
				if (min_value > max_value) {
					if (flag) {
						strcpy(board[i][j].name, plr2_name.name);
						value = Minimax_PLR2(b_row, b_col, board, false);
					}
					else {
						strcpy(board[i][j].name, plr1_name.name);
						value = Minimax_PLR2(b_row, b_col, board, true);
					}
					
					board[i][j].name[0] = 0;
					score[i][j] = value;
				}
			}
		}
	}
	
	if (flag) {
		max_value = -1000;
    	for (int i = 1; i <= b_row; i++) {
    		for (int j = 1; j <= b_col; j++) {
    			if (max_value < score[i][j] && score[i][j] != 1) {
    				max_value = score[i][j];
    				index_row = i;
    				index_col = j;
				}
			}
		}
		
		return max_value;
	}
	else {
		min_value = 1000;
		for (int i = 1; i <= b_row; i++) {
    		for (int j = 1; j <= b_col; j++) {
    			if (min_value > score[i][j] && score[i][j] != 1) {
    				min_value = score[i][j];
    				index_row = i;
    				index_col = j;
				}
			}
		}
		
		return min_value;
	}
}

void getPlayerPosition(int plr, int &xx, int &yy) {
	//find blank cell
	for (int i = 1; i <= b_row; i++) {
		for (int j = 1; j <= b_col; j++)
		if (tic_tac_toe.cell[i][j].length() == 0) {
			xx = i; yy = j;
			goto processing_data_input;	
		}
	}
	
	processing_data_input: {
		
		if (plr == 1) {
			cout << plr1_name.name << "'s TURN!!!\n";
			cout << plr1_name.name;
			
			if (tic_tac_toe.move_suggestion) Minimax_PLR1(b_row, b_col, tic_tac_toe.cell, true);
		}
		else {
			cout << plr2_name.name << "'s TURN!!!\n";
			cout << plr2_name.name;
			
			if (tic_tac_toe.move_suggestion) Minimax_PLR2(b_row, b_col, tic_tac_toe.cell, true);
		}
		
		char col_name = 64 + yy;
		cout << " is choosing cell(" << col_name << ", " << xx << ")";
		cout << "\nUse arrow keys on the keyboard to move the cursor around the board, press 'enter' to choose cell!";
		if (tic_tac_toe.cheatMode) cout << "\nMove cursor to your name-written cell to replay move!";
		
		if (plr == 1) printBoard((xx - 1) * b_col + yy, index_row, index_col, plr1_name.name);
		else printBoard((xx - 1) * b_col + yy, index_row, index_col, plr2_name.name);
		
		while (true) {
			int temp = getch();
			
			//clear screen
			for (int i = 1; i <= 100; i++) cout << "\n";
			
			if (temp == 13) {
				if (tic_tac_toe.cell[xx][yy].length() > 0 && strcmp(tic_tac_toe.cell[xx][yy].name, plr1_name.name) == 0 && plr == 1 && tic_tac_toe.cheatMode) {
					int t_xx, t_yy;
					tic_tac_toe.cell[xx][yy].name[0] = 0;
					getPlayerPosition(plr, t_xx, t_yy);
					strcpy(tic_tac_toe.cell[t_xx][t_yy].name, plr1_name.name);
					if (tic_tac_toe.move_suggestion) Minimax_PLR1(b_row, b_col, tic_tac_toe.cell, true);
				}
				else if (tic_tac_toe.cell[xx][yy].length() > 0 && strcmp(tic_tac_toe.cell[xx][yy].name, plr2_name.name) == 0 && plr == 2 && tic_tac_toe.cheatMode) {
					int t_xx, t_yy;
					tic_tac_toe.cell[xx][yy].name[0] = 0;
					getPlayerPosition(plr, t_xx, t_yy);
					strcpy(tic_tac_toe.cell[t_xx][t_yy].name, plr2_name.name);
					if (tic_tac_toe.move_suggestion) Minimax_PLR2(b_row, b_col, tic_tac_toe.cell, true);
				}
				else if (tic_tac_toe.cell[xx][yy].length() > 0) cout << "\nThe cell is occupied. Try another one!\n";
				else break;
			}
			
			if (plr == 1) {
				cout << plr1_name.name << "'s TURN!!!\n";
				cout << plr1_name.name;
			}
			else {
				cout << plr2_name.name << "'s TURN!!!\n";
				cout << plr2_name.name;
			}
			
			switch (temp) {
				case KEY_UP: {
					if (xx - 1 >= 1) xx--;
					break;
				}
				
				case KEY_DOWN: {
					if (xx + 1 <= b_row) xx++;
					break;
				}
				
				case KEY_LEFT: {
					if (yy - 1 >= 1) yy--;
					break;
				}
				
				case KEY_RIGHT: {
					if (yy + 1 <= b_col) yy++;
					break;
				}
			}
			
			char col_name = 64 + yy;
			cout << " is choosing cell(" << col_name << ", " << xx << ")\n";
			cout << "Use arrow keys on the keyboard to move the cursor around the board, press 'enter' to choose cell!";
			
			if (plr == 1) printBoard((xx - 1) * b_col + yy, index_row, index_col, plr1_name.name);
			else printBoard((xx - 1) * b_col + yy, index_row, index_col, plr2_name.name);
		}
	}
}

void saveGame() {
	int list_size = 0, position = 0;
	struct savedGame savedGameList[101];
	ifstream inpFile("saved_game.txt");
	
	while (!inpFile.eof()) {
		list_size++;
		inpFile >> savedGameList[list_size].username >> savedGameList[list_size].password;
		inpFile >> savedGameList[list_size].plr1_name >> savedGameList[list_size].plr2_name >> savedGameList[list_size].max_plr_name >> savedGameList[list_size].total_match;
		inpFile >> savedGameList[list_size].check_sound >> savedGameList[list_size].check_PvE >> savedGameList[list_size].check_move_suggestion >> savedGameList[list_size].check_cheat_mode;
		inpFile >> savedGameList[list_size].board_color >> savedGameList[list_size].time_limit >> savedGameList[list_size].plr1_color >> savedGameList[list_size].plr2_color >> savedGameList[list_size].plr1_winning >> savedGameList[list_size].plr2_winning >> savedGameList[list_size].b_row >> savedGameList[list_size].b_col;
		
		if (strcmp(savedGameList[list_size].username, current_user.username) == 0 && strcmp(savedGameList[list_size].password, current_user.password) == 0) position = list_size;
	}
	
	inpFile.close();
	
	if (position > 0) {
		strcpy(savedGameList[position].plr1_name, plr1_name.name);
		strcpy(savedGameList[position].plr2_name, plr2_name.name);
		savedGameList[position].max_plr_name = max_plr_name;
		savedGameList[position].total_match = total_match;
		savedGameList[position].check_sound = check_sound;
		savedGameList[position].check_PvE = tic_tac_toe.PvE_mode;
		savedGameList[position].check_move_suggestion = tic_tac_toe.move_suggestion;
		savedGameList[position].check_cheat_mode = tic_tac_toe.cheatMode;
		savedGameList[position].board_color = tic_tac_toe.board_color;
		savedGameList[position].time_limit = time_limit;
		savedGameList[position].plr1_color = plr1_color;
		savedGameList[position].plr2_color = plr2_color;
		savedGameList[position].plr1_winning =plr1_winning;
		savedGameList[position].plr2_winning = plr2_winning;
		savedGameList[position].b_row = b_row;
		savedGameList[position].b_col = b_col;
		
		ofstream outFile("saved_game.txt");
		for (int i = 1; i <= list_size; i++) {
			outFile << "\n\n" << savedGameList[i].username << " " << savedGameList[i].password << endl;
			outFile << savedGameList[i].plr1_name << " " << savedGameList[i].plr2_name << " " << savedGameList[position].max_plr_name << " " << savedGameList[position].total_match << "\n";
			outFile << savedGameList[i].check_sound << " " << savedGameList[i].check_PvE << " " << savedGameList[i].check_move_suggestion << " " << savedGameList[i].check_cheat_mode << endl;
			outFile << savedGameList[i].board_color << " " << savedGameList[i].time_limit << " " 
					<< savedGameList[i].plr1_color << " " << savedGameList[i].plr2_color << " " 
					<< savedGameList[i].plr1_winning << " " << savedGameList[i].plr2_winning << " "
					<< savedGameList[i].b_row << " " << savedGameList[i].b_col;
		}
		
		outFile.close();
	}
	else {
		ofstream outFile("saved_game.txt", ios::app);
		outFile << "\n\n" << current_user.username << " " << current_user.password << "\n";
		outFile << plr1_name.name << " " << plr2_name.name << " " << max_plr_name << " " << total_match << "\n";
		outFile << check_sound << " " << tic_tac_toe.PvE_mode << " " << tic_tac_toe.move_suggestion << " " << tic_tac_toe.cheatMode << "\n";
		outFile << tic_tac_toe.board_color << " " << time_limit << " " 
				<< plr1_color << " " << plr2_color << " " 
				<< plr1_winning << " " << plr2_winning << " "
				<< b_row << " " << b_col;
		
		outFile.close();
	}
}

bool checkAccountExistence(char username[101]) {
	struct Users temp;
	ifstream file("accounts.txt");
	
	while (!file.eof()) {
		file >> temp.username >> temp.password;
		
		if (strcmp(temp.username, username) == 0) {
			file.close();
			return true;
		}
	}
	
	file.close();
	return false;
}

void printStaticticalOutcome(int total_moves, int plr1_moves, int plr2_moves) {
	cout << "\n========== STATICTICAL OUTCOME(S) ==========";
	
	if (!check_account) cout << "\n\nGUEST MODE";
	else cout << "\n\nUser: " << current_user.username;
	
	cout << "\nTotal match(es): " << total_match;
	cout << "\nTie match(es): " << total_match - (plr1_winning + plr2_winning);
	cout << "\nTotal move(s) in the current game: " << total_moves;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), plr1_color);
	cout << "\nPlayer 1's moves in the current game (" << plr1_name.name << "): " << plr1_moves;
	cout << "\nPlayer 1's winning percentage: " << (double) ((plr1_winning * 100.0) / total_match) << "% (" << plr1_winning << " times)";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), plr2_color);
	cout << "\nPlayer 2's moves in the current game (" << plr2_name.name << "): " << plr2_moves;
	cout << "\nPlayer 2's winning percentage: " << (double) ((plr2_winning * 100.0) / total_match) << "% (" << plr2_winning << " times)";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	
	struct savedGame user;
	ifstream inpFile("saved_game.txt");
	
	while (!inpFile.eof()) {
		inpFile >> user.username >> user.password;
		inpFile >> user.plr1_name >> user.plr2_name >> user.max_plr_name >> user.total_match;
		inpFile >> user.check_sound >> user.check_PvE >> user.check_move_suggestion >> user.check_cheat_mode;
		inpFile >> user.board_color >> user.time_limit >> user.plr1_color >> user.plr2_color >> user.plr1_winning >> user.plr2_winning >> user.b_row >> user.b_col;
		
		if ((strcmp(user.username, current_user.username) != 0 || strcmp(user.password, current_user.password) != 0) && checkAccountExistence(user.username)) {
			cout << "\n\nUser: " << user.username;
			
			cout << "\nTotal match(es): " << user.total_match;
			cout << "\nTie match(es): " << user.total_match - (user.plr1_winning + user.plr2_winning);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), user.plr1_color);
			cout << "\nPlayer 1's winning percentage: " << (double) ((user.plr1_winning * 100.0) / user.total_match) << "% (" << user.plr1_winning << " times)";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), user.plr2_color);
			cout << "\nPlayer 2's winning percentage: " << (double) ((user.plr2_winning * 100.0) / user.total_match) << "% (" << user.plr2_winning << " times)";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		}
	}
	
	inpFile.close();
}

void startPlaying() {
	cout << "\nRandomizing who is gonna go first.......";
	
	int temp = rand() % 2; //random from 0 to 1
	if (!temp) cout << "\nPlayer 2 goes first!!! Press any key to start the game";
	else cout << "\nPlayer 1 goes first!!! Press any key to start the game";
	
	while (getch() <= 0) { }
	
	//set up variables before starting game
	int cnt = 0, cnt1 = 0, cnt2 = 0, x = 0, y = 0, mode = 0;
	char check_winning[101];
	
	strcpy(check_winning, checkWinning(x, y, mode, tic_tac_toe.cell));
	
	last_time_limit = ( (double) clock() ) / CLOCKS_PER_SEC; //set up time restriction mode
	
	//starting game
	while (strcmp(check_winning, "\0") == 0) {
		//check time restriction mode of the board is valid or not
		if ((time_limit > 0) && ((( (double) clock() ) / CLOCKS_PER_SEC) - last_time_limit > time_limit)) break;
		
		//count moves
		cnt++;
		
		//clear screen
		for (int i = 1; i <= 100; i++) cout << "\n";
		
		//check who will take turn
		int xx, yy;
		if (cnt % 2 != temp) {
			if (!tic_tac_toe.PvE_mode) {
				getPlayerPosition(2, xx, yy);
				tic_tac_toe.markingCell(xx, yy, plr2_name);
				cnt2++;
			}
			else {
				Minimax_PLR2(b_row, b_col, tic_tac_toe.cell, true);
				tic_tac_toe.markingCell(index_row, index_col, plr2_name);
				cnt2++;
			}
		}
		else {
			index_row = 0; index_col = 0;
			
			getPlayerPosition(1, xx, yy);
			tic_tac_toe.markingCell(xx, yy, plr1_name);
			cnt1++;
		}
		
		//check if anyone won the game or tie match
		strcpy(check_winning, checkWinning(x, y, mode, tic_tac_toe.cell));
	}
	
	if (strcmp(check_winning, "TIE") == 0) {
		int temp[4] = {0, 0, 0, 0};
		printWinningBoard(temp);
		cout << "\n" << check_winning << " MATCH!\n";
	}
	else if ((time_limit > 0) && ((( (double) clock() ) / CLOCKS_PER_SEC) - last_time_limit > time_limit)) {
		int temp[4] = {0, 0, 0, 0};
		printWinningBoard(temp);
		cout << "\nTIME'S UP!!!!!!\n";
	}
	else {
		switch (mode) {
			case 1: {
				int temp[3] = {(x - 1) * b_col + y, x * b_col + y, (x + 1) * b_col + y};
				printWinningBoard(temp);
				break;
			}
			case 2: {
				int temp[3] = {(x - 1) * b_col + y, (x - 1) * b_col + y + 1, (x - 1) * b_col + y + 2};
				printWinningBoard(temp);
				break;
			}
			case 3: {
				int temp[3] = {(x - 1) * b_col + y, x * b_col + y + 1, (x + 1) * b_col + y + 2};
				printWinningBoard(temp);
				break;
			}
			case 4: {
				int temp[3] = {(x - 1) * b_col + y, x * b_col + y - 1, (x + 1) * b_col + y - 2};
				printWinningBoard(temp);
				break;
			}
		}
		
		if (strcmp(check_winning, plr1_name.name) == 0) plr1_winning++;
		else plr2_winning++;
		
		cout << "\n" << check_winning << " WINS THE GAME!!! CONGRATS!!!!!!!!!\n";
	}
	
	printStaticticalOutcome(cnt, cnt1, cnt2);
}

void loadSavedGame() {
	bool isGameSaved = false;
	struct savedGame temp;
	ifstream inpFile("saved_game.txt");
	
	while (!inpFile.eof()) {
		inpFile >> temp.username >> temp.password;
		inpFile >> temp.plr1_name >> temp.plr2_name >> temp.max_plr_name >> temp.total_match;
		inpFile >> temp.check_sound >> temp.check_PvE >> temp.check_move_suggestion >> temp.check_cheat_mode;
		inpFile >> temp.board_color >> temp.time_limit >> temp.plr1_color >> temp.plr2_color >> temp.plr1_winning >> temp.plr2_winning >> temp.b_row >> temp.b_col;
		
		if (strcmp(temp.username, current_user.username) == 0 && strcmp(temp.password, current_user.password) == 0) {
			isGameSaved = true;
			break;
		}
	}
	
	inpFile.close();
	
	if (!isGameSaved) cout << "\nERROR: There wasn't any saved game on this account!\n";
	else {
		strcpy(plr1_name.name, temp.plr1_name);
		strcpy(plr2_name.name, temp.plr2_name);
		max_plr_name = temp.max_plr_name;
		total_match = temp.total_match;
		check_sound = temp.check_sound;
		tic_tac_toe.PvE_mode = temp.check_PvE;
		tic_tac_toe.move_suggestion = temp.check_move_suggestion;
		tic_tac_toe.cheatMode = temp.check_cheat_mode;
		tic_tac_toe.board_color = temp.board_color;
		time_limit = temp.time_limit;
		plr1_color = temp.plr1_color;
		plr2_color = temp.plr2_color;
		plr1_winning = temp.plr1_winning;
		plr2_winning = temp.plr2_winning;
		b_row = temp.b_row;
		b_col = temp.b_col;
		
		checkLoadSavedGame = true;
		
		if (tic_tac_toe.PvE_mode) turnOnBackgroundMusic();
	}
}

void deleteSavedGame(char username[101], char password[101]) {
	int list_size = 0, position = 0;
	struct savedGame savedGameList[101];
	ifstream inpFile("saved_game.txt");
	
	while (!inpFile.eof()) {
		list_size++;
		inpFile >> savedGameList[list_size].username >> savedGameList[list_size].password;
		inpFile >> savedGameList[list_size].plr1_name >> savedGameList[list_size].plr2_name >> savedGameList[list_size].max_plr_name >> savedGameList[list_size].total_match;
		inpFile >> savedGameList[list_size].check_sound >> savedGameList[list_size].check_PvE >> savedGameList[list_size].check_move_suggestion >> savedGameList[list_size].check_cheat_mode;
		inpFile >> savedGameList[list_size].board_color >> savedGameList[list_size].time_limit >> savedGameList[list_size].plr1_color >> savedGameList[list_size].plr2_color >> savedGameList[list_size].plr1_winning >> savedGameList[list_size].plr2_winning >> savedGameList[list_size].b_row >> savedGameList[list_size].b_col;
		
		if (strcmp(username, savedGameList[list_size].username) == 0 && strcmp(password, savedGameList[list_size].password) == 0) position = list_size;
	}
	
	inpFile.close();
	
	if (position > 0) {		
		ofstream outFile("saved_game.txt");
		for (int i = 1; i <= list_size; i++)
		if (i != position)  {
			outFile << "\n\n" << savedGameList[i].username << " " << savedGameList[i].password << endl;
			outFile << savedGameList[i].plr1_name << " " << savedGameList[i].plr2_name << " " << savedGameList[i].max_plr_name << " " << savedGameList[i].total_match << "\n";
			outFile << savedGameList[i].check_sound << " " << savedGameList[i].check_PvE << " " << savedGameList[i].check_move_suggestion << " " << savedGameList[i].check_cheat_mode << "\n";
			outFile << savedGameList[i].board_color << " " << savedGameList[i].time_limit << " " 
					<< savedGameList[i].plr1_color << " " << savedGameList[i].plr2_color << " " 
					<< savedGameList[i].plr1_winning << " " << savedGameList[i].plr2_winning << " "
					<< savedGameList[i].b_row << " " << savedGameList[i].b_col;
		}
		
		outFile.close();
	}
}

void deleteAccount(char username[101], char password[101]) {
	int position = 0, list_size = 0;
	struct Users user_list[101];
	ifstream inpFile("accounts.txt");
	
	while (!inpFile.eof()) {
		list_size++;
		inpFile >> user_list[list_size].username >> user_list[list_size].password;
		
		if (strcmp(username, user_list[list_size].username) == 0 && strcmp(password, user_list[list_size].password) == 0) position = list_size;
	}
	
	inpFile.close();
	
	ofstream outFile("accounts.txt");
			
	for (int i = 1; i <= list_size; i++) 
		if (i != position) outFile << "\n\n" << user_list[i].username << " " << user_list[i].password;
					
	outFile.close(); 
}

void signIn() {
	getAccountAgain: int list_size = 0, position = 0;
	struct Users user, temp, user_list[101];
	ifstream file("accounts.txt");
	
	cout << "\nUsername: ";
	cin >> user.username;
	cout << "Password: ";
	cin >> user.password;
	
	while (!file.eof()) {
		file >> temp.username >> temp.password;
		
		list_size++;
		strcpy(user_list[list_size].username, temp.username);
		strcpy(user_list[list_size].password, temp.password);
		
		if (strcmp(temp.username, user.username) == 0 && strcmp(temp.password, user.password) == 0) {
			check_account = true;
			position = list_size;
		}
	}
	
	file.close();
	
	if (check_account) {
		char isDelete;
	
		strcpy(current_user.username, user.username);
		strcpy(current_user.password, user.password);
		
		cout << "\nWelcome back, " << user.username << "!";
		cout << "\nDo you want to delete this account? [Y/N]: ";
		cin >> isDelete;
		
		if (isDelete == 'Y') {
			check_account = false;
			deleteAccount(current_user.username, current_user.password);
			deleteSavedGame(current_user.username, current_user.password);
			cout << "\nDeleted!";
		}
	}
	else {
		char isRepeated;
		
		cout << "\nERROR: There was something wrong with the input, please check your information!";
		cout << "\nWanna try again? [Y/N]: ";
		cin >> isRepeated;
		
		if (isRepeated == 'Y') goto getAccountAgain;
	}
}

void signUp() {
	signUpAgain: struct Users user;
	
	cout << "\nUsername: ";
	cin >> user.username;
	cout << "Password: ";
	cin >> user.password;
	
	if (checkAccountExistence(user.username)) {
		char checkSignUp;
		
		cout << "\nThis username has already existed, please try a different one!";
		cout << "\nWanna try again? [Y/N]: ";
		cin >> checkSignUp;
		
		if (checkSignUp == 'Y') goto signUpAgain;
	}
	
	ofstream file("accounts.txt", ios::out | ios::app);
	file << "\n\n" << user.username << " " << user.password;
	cout << "\nSigned up successfully!";
	
	check_account = true;
	strcpy(current_user.username, user.username);
	strcpy(current_user.password, user.password);
	
	file.close();
}

void accountManagement() {
	//check if file is created
	if (!fstream("accounts.txt")) ofstream("accounts.txt").close();
	if (!fstream("saved_game.txt")) ofstream("saved_game.txt").close();
	
	//select mode
	int choice = 0;
	cout << "1. Sign in (for saving game)\n";
	cout << "2. Sign up\n";
	cout << "3. Guest mode\n";
	cout << "\nEnter your choice: ";
	cin >> choice;
	
	if (choice == 1) signIn();
	else if (choice == 2) signUp();
}

int main() {
	//set up random function
	srand(time(NULL));
	
	//print welcome message
	cout << "Welcome to Tic Tac Toe game!!!!!!!\n";
	
	//account management
	accountManagement();
	
	//load game from files
	if (check_account) {
		char isLoadGame;
		
		cout << "\nDo you want to load saved game? [Y/N]: ";
		cin >> isLoadGame;
		
		if (isLoadGame == 'Y') loadSavedGame();
	}
	
	//get data from user's input
	if (!check_account || !checkLoadSavedGame) getDataInput(); 
	
	//set up replay function
	char temp = 'Y';
	while (temp != 'N') {
		//count matches
		total_match++;
		
		//reset board status
		tic_tac_toe.resetBoard(b_row, b_col);
		
		//start playing game
		startPlaying();
		cout << "\n\nWanna replay? [Y/N]: ";
		cin >> temp;
	}
	
	//ask user if they want to save game
	if (check_account) {
		char isUserSaveGame;
		
		cout << "\Do you want to save game? [Y/N]: ";
		cin >> isUserSaveGame;
		
		if (isUserSaveGame == 'Y') saveGame();
	}
	
	return 0;
}
