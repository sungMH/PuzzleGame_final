#include <bangtal.h>
#include <iostream>
#include <ctime>

using namespace bangtal;
using namespace std;


bool movable(int index,int blank ,int size) {//현재 위치, 빈칸 위치 ,퍼즐의 한 변의 길이 를 매개변수로 넣어야 한다.
	return ((index % size > 0 && index - 1 == blank) ||
		(index % size < size-1 && index + 1 == blank) ||
		((index > size-1) &&( index - size == blank)) ||
		((index < size*(size-1)) && (index +  size == blank)));
}

//퍼즐 조각의 위치를 서로 바꾸는 함수
//매개변수: 현재위치, 빈칸변수의 주소, 퍼즐 한 변의 길이, 게임 보드 배열, 현재 씬, 현재 오브젝트,퍼즐 시작 x좌표, 퍼즐 시작 y 좌표,한 조각의 길이
//이동 가능 한지 검사 후에 사용할 것이기 때문에 따로 movable은 포함하지 않는다.
void change_pieces(int index, int * blank, int size, ObjectPtr game_board[], ScenePtr scene, ObjectPtr object,int x, int y, int width){
	game_board[index]->locate(scene, x + ((*blank) % size) * width, y - ((*blank) / size) * width);
	game_board[(*blank)]->locate(scene, x + (index % size) * width, y - (index / size) * width);
	game_board[index] = game_board[(*blank)];
	game_board[(*blank)] = object;
	*blank = index;
}

//퍼즐이 완성 되었는지 체크하는 함수
//매개변수: 퍼즐 한 변의 길이, 게임 보드, 초기 보드, 시작 시각, 걸린 시간 저장할 변수 주소값,최고기록 저장할 변수 주소 값,게임 종료 화면 씬
void check(int size, ObjectPtr game_board[], ObjectPtr init_board[], time_t start_time, time_t * time_game, time_t * score,ScenePtr scene) {
	for (int k = 0; k < size*size; k++) {
		if (game_board[k] != init_board[k]) {
			break;
		}
		if (k == size*size-1) {
			auto end_time = time(NULL);
			*time_game = difftime(end_time, start_time);
			string buf;
			if (((*time_game) <  (*score)) && (0 < (*score))){//최고 기록 경신
				buf ="최고 기록 경신!"+ to_string(*time_game) + "초 걸렸습니다!";
			}else { 
				buf = to_string(*time_game) + "초 걸렸습니다!";
				*score = *time_game;
			}
			showMessage(buf.c_str());
			scene->enter();
		}
	}
}

//퍼즐을 상하좌우중 빈칸과 교환 하는 함수 -> 이동 하는 함수
//매개 변수: 빈 칸과 교환할 현재 위치 저장 변수 주소, 빈칸 위치, 퍼즐 한 변의 길이
void move(int *j, int blank, int size){

	do {
		switch (rand() % 4) {
		case 0://좌
			*j = blank - 1; break;
		case 1://우
			*j = blank + 1; break;
		case 2://상
			*j = blank - size; break;
		case 3: //하
			*j = blank + size; break;
		}
	} while ((*j < 0 || *j > size*size-1) || !(movable(*j, blank, size)));

}



int main() {

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	srand((unsigned int)time(NULL));
	time_t start_time;
	time_t score1 = -1, score2 = -1;//티모, 나서스 게임 최고 기록 기록할 곳

	auto scene = Scene::create("게임 시작", "images/시작화면.png");
	auto scene1 = Scene::create("꿀잼 티모", "images1/배경.png");
	auto scene2 = Scene::create("나서스", "images2/배경.png");
	auto scene3 = Scene::create("게임 끝", "images/시작화면.png");

	auto teemoButton = Object::create("images/티모게임.png", scene,590, 270);
	auto nasusButton = Object::create("images/나서스게임.png", scene, 580, 470);
	auto restartButton = Object::create("images/다시시작.png", scene3, 380, 270);
	auto endButton = Object::create("images/종료버튼.png", scene3, 680, 270);

	restartButton->setScale(0.2f);
	endButton->setScale(0.2f);

	restartButton->setOnMouseCallback([&](ObjectPtr , int, int, MouseAction)->bool {
		scene->enter();
		return true;
	});

	endButton->setOnMouseCallback([&](ObjectPtr, int, int, MouseAction)->bool {
		endGame();
		return true;

	});

	ObjectPtr game_board1[16];//4*4 티모 퍼즐
	ObjectPtr game_board2[9];// 3*3 나서스 퍼즐
	ObjectPtr init_board1[16];
	ObjectPtr init_board2[9];
	
	int blank1 = 15, size1 = 4;
	int blank2 = 8, size2 = 3;

	time_t time_game1 = 0, time_game2 = 0;//각 게임당 걸린 시간 최고 기록 저장할 변수

	//티모퍼즐 초기화
	for (int i = 0; i < 16; i++) {
		string filename = "images1/조각-" + to_string(i + 1) + ".png";
		game_board1[i] = Object::create(filename, scene1, 500 + (i % 4) * 150, 470 - (i / 4) * 150);
		init_board1[i] = game_board1[i];
		game_board1[i]->setOnMouseCallback([&](ObjectPtr object, int, int, MouseAction)->bool {
			int j;
			for (j = 0; j < 16; j++) {
				if (game_board1[j] == object) {
					break;
				}
			}
			if (j < 16) {
				if (movable(j, blank1, size1)) {
					change_pieces(j, &blank1, size1, game_board1, scene1, object, 500, 470, 150);
					check(size1, game_board1, init_board1, start_time, &time_game1, &score1, scene3);
				}
			}
			return true;
			});	
	}
	game_board1[blank1]->hide();

	//나서스퍼즐 초기화
	for (int i = 0; i < 9; i++) {
		string filename = "images2/조각-" + to_string(i + 1) + ".png";
		game_board2[i] = Object::create(filename, scene2, 300 + (i % 3) * 200, 420 - (i / 3) * 200);
		init_board2[i] = game_board2[i];
		game_board2[i]->setOnMouseCallback([&](ObjectPtr object, int, int, MouseAction)->bool {
			int j;
			for (j = 0; j < 9; j++) {
				if (game_board2[j] == object) {
					break;
				}
			}
			if (j < 9) {
				if (movable(j, blank2, size2)) {
					change_pieces(j, &blank2, size2, game_board2, scene2, object, 300, 420, 200);
					check(size2, game_board2, init_board2, start_time, &time_game2, &score2, scene3);
				}
			}
			return true;
			});
	}
	game_board2[blank2]->hide();


	int count1 = 0, count2 = 0;
	auto timer1 = Timer::create(0.1);
	timer1->setOnTimerCallback([&](TimerPtr timer)->bool{
		int j = 0;

		move(&j, blank1, size1);
		auto object = game_board1[j];
		change_pieces(j, &blank1, size1, game_board1, scene1, object, 500, 470, 150);
		
		count1++;
		if (count1 < 100) {
			timer->set(0.1f);
			timer->start();
		}
		return true;
	});

	auto timer2 = Timer::create(0.1);
	timer2->setOnTimerCallback([&](TimerPtr timer)->bool {
		int j = 0;

		move(&j, blank2, size2);
		auto object = game_board2[j];
		change_pieces(j, &blank2, size2, game_board2, scene2, object, 300, 420, 200);

		count2++;
		if (count2 < 80) {
			timer->set(0.1f);
			timer->start();
		}
		return true;
		});

	
	teemoButton->setOnMouseCallback([&](ObjectPtr o, int x, int y, MouseAction)->bool {
		scene1->enter();
		count1 = 0;
		timer1->start();
		start_time = time(NULL);
		return true;
		});
	nasusButton->setOnMouseCallback([&](ObjectPtr o, int x, int y, MouseAction)->bool {
		scene2->enter();
		count2 = 0;
		timer2->start();
		start_time = time(NULL);
		return true;
		});

	startGame(scene);
}