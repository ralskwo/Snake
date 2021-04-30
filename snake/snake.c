#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<stdlib.h>
#include<time.h>

#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define ESC 0x1b

// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15
#define _CRT_SECURE_NO_WARNINGS

#define BOX_X 3
#define BOX_Y 2
#define BOX_WIDTH 40
#define BOX_HEIGHT 24

#define WALL_X 13
#define WALL_Y1 9
#define WALL_Y2 18
#define WALL_LENGTH 20

typedef struct _ITEM {
	int x, y;
}ITEM;

ITEM item; 

void gotoxy(int x, int y, char* ch) { 
	COORD pos = { 2 * x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	printf("%s", ch);
} 

void removeCursor(void) { // 커서를 안보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void textcolor(int fg_color, int bg_color){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void cls(int bg_color, int text_color){ // 화면 지우기고 원하는 배경색으로 설정한다.
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

void draw_BOX() { // 가장자리 그리기
	int i;

	textcolor(GRAY1, BLACK);
	for (i = 0; i < BOX_WIDTH; i++) {
		gotoxy(BOX_X + i, BOX_Y, "■");
	}

	for (i = BOX_Y + 1; i < BOX_Y + BOX_HEIGHT - 1; i++) {
		gotoxy(BOX_X, i, "■");
		gotoxy(BOX_X + BOX_WIDTH - 1, i, "■");
	}

	for (i = 0; i < BOX_WIDTH; i++) {
		gotoxy(BOX_X + i, BOX_Y + BOX_HEIGHT - 1, "■");
	}

}

void draw_WALL() { // 내부 벽 그리기
	int i;
	
	for (i = 0; i < WALL_LENGTH; i++) {
		gotoxy(WALL_X + i, WALL_Y1, "■");
	}

	for (i = 0; i < WALL_LENGTH; i++) {
		gotoxy(WALL_X + i, WALL_Y2, "■");
	}
}

void erase_WALL() { // 내부 벽 지우기
	int i;
	
	for (i = 0; i < WALL_LENGTH; i++) {
		gotoxy(WALL_X + i, WALL_Y1, " ");
	}

	for (i = 0; i < WALL_LENGTH; i++) {
		gotoxy(WALL_X + i, WALL_Y2, " ");
	}
}

int x[100], y[100];

int score;
int best_score = 0;
int last_score = 0;

int speed;
int length;

void food() { // 먹이
	int i;

	int overlap_snake = 0; 
	int overlap_wall = 0;
	int r = 0;  
	int item_check = 0; 

	gotoxy(BOX_X, BOX_Y + BOX_HEIGHT, " PRESENT SCORE: "); 
	printf("%3d\t LAST SCORE: %3d\t BEST SCORE: %3d", score, last_score, best_score);

	while (1) {
		overlap_snake = 0;
		overlap_wall = 0;
		srand((unsigned)time(NULL) + r); 

		item_check = rand() % 4;

		item.x = (rand() % (BOX_WIDTH - 2)) + 1;
		item.y = (rand() % (BOX_HEIGHT - 2)) + 1;

		for (i = 0; i < length; i++) { // 뱀에 먹이 생김 방지
			if (item.x == x[i] && item.y == y[i]) {
				overlap_snake = 1; 
				r++;
				break;
			}
		}

		for (i = 0; i <= WALL_LENGTH; i++) { // 내부 벽에 먹이 생김 방지
			if ((item.x == WALL_X - BOX_X + i && item.y == WALL_Y1 - BOX_Y) || (item.x == WALL_X - BOX_X + i && item.y == WALL_Y2 - BOX_Y)) {
				overlap_wall = 1;
				r++;
				break;
			}
		}  
		if (overlap_snake == 1 || overlap_wall == 1) continue; 

		item_check = rand() % 5 + 1;
		switch (item_check) {
		case 1:
		case 5:
			textcolor(WHITE, BLACK);
			gotoxy(BOX_X + item.x, BOX_Y + item.y, "◆");  //일반 아이템
			speed -= 3;  
			textcolor(GRAY1, BLACK);
			break;

		case 2:
			textcolor(YELLOW2, BLACK);
			gotoxy(BOX_X + item.x, BOX_Y + item.y, "★"); //점수 2배
			score += 10;
			speed -= 3;  
			textcolor(GRAY1, BLACK);
			break;

		case 3:
			textcolor(RED2, BLACK);
			gotoxy(BOX_X + item.x, BOX_Y + item.y, "▲");  //가속 아이템
			speed -= 9; //속도 대폭 증가 
			textcolor(GRAY1, BLACK);
			break;

		case 4:
			if (speed < 100) {
				textcolor(BLUE2, BLACK);
				gotoxy(BOX_X + item.x, BOX_Y + item.y, "▼");  //감속 아이템
				speed += 12; //속도 대폭 감소 
				textcolor(GRAY1, BLACK);
			}
			else {
				textcolor(WHITE, BLACK);
				gotoxy(BOX_X + item.x, BOX_Y + item.y, "◆"); //속도가 너무 낮으면 일반으로 적용	
				speed -= 3;
				textcolor(GRAY1, BLACK);
			}
			break;
		}
		break;
	}
}

int dir;
int key;

void Init() {
	int i;
	system("cls"); 
	draw_BOX();

	while (_kbhit())
		_getch();  

	dir = LEFT; 
	speed = 100; 
	length = 5;  
	score = 0; 
	textcolor(GREEN2, BLACK);
	for (i = 0; i < length; i++) { 
		x[i] = BOX_WIDTH / 2 + i;
		y[i] = BOX_HEIGHT / 2;
		gotoxy(BOX_X + x[i], BOX_Y + y[i], "□");
	}

	gotoxy(BOX_X + x[0], BOX_Y + y[0], "◈"); 
	textcolor(GRAY1, BLACK);
	food(); 
}

void title() {
	
	while (_kbhit())
		_getch();  
	
	draw_BOX();     
	for (int i = BOX_Y + 1; i < BOX_Y + BOX_HEIGHT - 1; i++) {
		for (int j = BOX_X + 1; j < BOX_X + BOX_WIDTH - 1; j++) gotoxy(j, i, "  ");
	}

	textcolor(CYAN2, BLACK);
	gotoxy(BOX_X + 8, BOX_Y + 4, "▣▣▣▣  ▣▣▣▣  ▣▣▣▣  ▣    ▣  ▣▣▣▣");
	gotoxy(BOX_X + 8, BOX_Y + 5, "▣        ▣    ▣  ▣    ▣  ▣  ▣    ▣      ");
	gotoxy(BOX_X + 8, BOX_Y + 6, "▣▣▣▣  ▣    ▣  ▣▣▣▣  ▣▣      ▣▣▣▣");
	gotoxy(BOX_X + 8, BOX_Y + 7, "      ▣  ▣    ▣  ▣    ▣  ▣  ▣    ▣      ");
	gotoxy(BOX_X + 8, BOX_Y + 8, "▣▣▣▣  ▣    ▣  ▣    ▣  ▣    ▣  ▣▣▣▣");


	textcolor(YELLOW2, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 11, " < PRESS ANY KEY TO START > ");

	textcolor(WHITE, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 13, "   ◇ ↑, ↓, ← ,→ : Move    ");
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 14, "   ◇ ESC : Quit              ");
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 15, "   ◇ INFO : ◆ - Normal       ");

	textcolor(RED2, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 16, "           : ▲ - Speed Up     ");
	textcolor(BLUE2, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 17, "           : ▼ - Speed Down   ");
	textcolor(YELLOW2, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 18, "           : ★ - Double Score ");
	textcolor(GREEN2, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 20, "   ◇ EDIT :지식정보학부 1414048 김민규");

	textcolor(YELLOW2, BLACK);
	while (1) {
		removeCursor();
		if (_kbhit()) { 
			key = _getch();
			
			if (key == ESC) 
				exit(0); 
			else 
				break;
		}

		gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 11, " < PRESS ANY KEY TO START > ");
		Sleep(400);
		gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 11, "                            ");
		Sleep(400);
	}
	textcolor(GRAY1, BLACK);
	Init(); // 게임 시작
}

void game_over() { //게임종료 함수 
	erase_WALL();
	draw_BOX();
	
	for (int i = BOX_Y + 1; i < BOX_Y + BOX_HEIGHT - 1; i++) {
		for (int j = BOX_X + 1; j < BOX_X + BOX_WIDTH - 1; j++) gotoxy(j, i, "  ");
	}

	textcolor(RED2, BLACK);
	gotoxy(BOX_X + 6, BOX_Y + 3, "▣▣▣▣  ▣▣▣▣  ▣    ▣  ▣▣▣▣");
	gotoxy(BOX_X + 6, BOX_Y + 4, "▣        ▣    ▣  ▣▣▣▣  ▣      ");
	gotoxy(BOX_X + 6, BOX_Y + 5, "▣  ▣▣  ▣▣▣▣  ▣ ▣ ▣  ▣▣▣▣");
	gotoxy(BOX_X + 6, BOX_Y + 6, "▣    ▣  ▣    ▣  ▣    ▣  ▣      ");
	gotoxy(BOX_X + 6, BOX_Y + 7, "▣▣▣▣  ▣    ▣  ▣    ▣  ▣▣▣▣");

	gotoxy(BOX_X + 16, BOX_Y + 9,  "▣▣▣▣  ▣    ▣  ▣▣▣▣  ▣▣▣▣");
	gotoxy(BOX_X + 16, BOX_Y + 10, "▣    ▣  ▣    ▣  ▣        ▣    ▣");
	gotoxy(BOX_X + 16, BOX_Y + 11, "▣    ▣   ▣  ▣   ▣▣▣▣  ▣▣▣▣");
	gotoxy(BOX_X + 16, BOX_Y + 12, "▣    ▣    ▣▣    ▣        ▣  ▣  ");
	gotoxy(BOX_X + 16, BOX_Y + 13, "▣▣▣▣     ▣     ▣▣▣▣  ▣    ▣");


	textcolor(MAGENTA2, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 3, BOX_Y + 17, " SCORE : ");
	printf("%d", last_score = score);
	textcolor(WHITE, BLACK);
	gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 19, " Press any keys to restart.. ");


	if (score > best_score) {
		best_score = score;
		textcolor(YELLOW2, BLACK);
		gotoxy(BOX_X + (BOX_WIDTH / 2) - 5, BOX_Y + 15, "☆--- BEST SCORE ---☆");
	}
	
	while (1) {
		if (_kbhit()) { 
			key = _getch();

			if (key == ESC)
				exit(0);  
			else
				break;  
		}
		textcolor(WHITE, BLACK);
		gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 19, " Press any keys to restart.. ");
		Sleep(400);
		gotoxy(BOX_X + (BOX_WIDTH / 2) - 7, BOX_Y + 19, "                             ");
		Sleep(400);
		
	}
	textcolor(GRAY1, BLACK);
	Sleep(400);

	title();
}

void move(int dir) {
	int i;

	if (x[0] == item.x && y[0] == item.y) { //아이템 먹기
		score += 10;  
		food(); 
		length++; 
		x[length - 1] = x[length - 2];  
		y[length - 1] = y[length - 2];
	}

	if (x[0] == 0 || x[0] == BOX_WIDTH - 1 || y[0] == 0 || y[0] == BOX_HEIGHT - 1) { //가장자리 벽과 충돌
		game_over();
		return;
	}

	for (i = 1; i < length; i++) {  //꼬리와 충돌
		if (x[0] == x[i] && y[0] == y[i]) {
			game_over();
			return;
		}
	}

	for (i = 0; i < WALL_LENGTH; i++) { //내부 벽과 충돌
		if ((x[0] == WALL_X - BOX_X + i && y[0] == WALL_Y1 - BOX_Y) || (x[0] == WALL_X - BOX_X + i && y[0] == WALL_Y2 - BOX_Y)) {
			game_over();
			return;
		}
	}

	gotoxy(BOX_X + x[length - 1], BOX_Y + y[length - 1], "  ");   //지나간 자리 지우기
	for (i = length - 1; i > 0; i--) {  
		x[i] = x[i - 1];
		y[i] = y[i - 1];
	}

	// 다시 그리기
	textcolor(GREEN2, BLACK);
	gotoxy(BOX_X + x[0], BOX_Y + y[0], "□"); 

	if (dir == LEFT)
		--x[0]; 

	if (dir == RIGHT)
		++x[0];

	if (dir == UP) 
		--y[0];

	if (dir == DOWN)
		++y[0];

	gotoxy(BOX_X + x[i], BOX_Y + y[i], "◈");  
	textcolor(GRAY1, BLACK);
}

int main() {
	title();
	removeCursor();
	
	while (1) {
		draw_WALL();
		
		if (_kbhit())
			do {
				key = _getch();
			} while (key == 224);

			Sleep(speed);

			switch (key) {   
			case LEFT:
			case RIGHT:
			case UP:
			case DOWN:
				if ((dir == LEFT && key != RIGHT) || (dir == RIGHT && key != LEFT) || (dir == UP && key != DOWN) ||
					(dir == DOWN && key != UP))
					dir = key;
				key = 0;  
				break;
				break;
		
			case ESC:  
				exit(0);
			}
			move(dir);
	}
}
