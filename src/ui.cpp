#include "ui.h"

#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>

#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <ciso646>
#else
#error Unsupported platform
#endif

UIManager::UIManager(Manager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	texture_manager(renderer->getTextureManager()),
	input_handler(parent->getInputHandler()),
	game_manager(parent->getGameManager()),
	tick(0),
	splash_deadline(10000), // time to leave splash on screen for
	menu_deadline(0),
	menu_counter(0),
	quiz_deadline(0),
	quiz_counter(0),
	in_menu(false),
	in_quiz(false)
{
	// prevent input before splash screen takes over
	game_manager->setPaused(true);

	// load splash texture
	splash = texture_manager->loadTexture("data/logo/splash.png");

	// load menu and quiz animation frames
	std::ifstream menu_frames_file("data/ui/menu/max_frame.txt");
	int menu_frames;

	menu_frames_file >> menu_frames;
	menu.resize(menu_frames);

	for (int i = 0; i < menu_frames; ++i) {
		std::ostringstream buf;
		buf << "data/ui/menu/" << i + 1 << ".png";
		menu[i] = texture_manager->loadTexture(buf.str());
	}

	std::ifstream quiz_frames_file("data/ui/quiz/max_frame.txt");
	int quiz_frames;

	quiz_frames_file >> quiz_frames;
	quiz.resize(quiz_frames);

	for (int i = 0; i < quiz_frames; ++i) {
		std::ostringstream buf;
		buf << "data/ui/quiz/" << i + 1 << ".png";
		quiz[i] = texture_manager->loadTexture(buf.str());
	}

	continue_btn = texture_manager->loadTexture("data/ui/button/continue_sel.png");
	documentation_btn = texture_manager->loadTexture("data/ui/button/documentation_sel.png");
	exit_btn = texture_manager->loadTexture("data/ui/button/exit_sel.png");
	submit_btn = texture_manager->loadTexture("data/ui/button/submit_sel.png");

	minimap = texture_manager->loadTexture("data/ui/minimap.png");
	point = texture_manager->loadTexture("data/ui/point.png");
}

void UIManager::displayQuiz(std::string question, std::vector<std::string> answers)
{
	in_quiz = true;
	this->question = question;
	this->answers = answers;
	// just in case
	this->answers.resize(3);
}

void UIManager::endQuiz()
{
	in_quiz = false;
}

void UIManager::operator()(uint64_t delta)
{
	tick += delta;

	// run start splash screen
	if (tick < splash_deadline) {
		game_manager->setPaused(true);

		renderer->addRenderItem(splash, 0, 0, false, false, 100, true);

		// option to skip
		if(input_handler->isEnter(true))
			splash_deadline = 0;

		// do not run any other ui code during splash
		return;
	}

	if (input_handler->isPause(true)) in_menu = not in_menu;

	if (in_menu) {
		// for buttons
		static int choice = 0;

		if (menu_counter < menu.size() - 1) {
			if (tick >= menu_deadline) {
				menu_deadline = tick + FRAMETIME;
				++menu_counter;
			}

			renderer->addRenderItem(menu[menu_counter], 0, 0, false, false, 10, true);

			choice = 0;
		} else {
			// render final frame
			renderer->addRenderItem(menu.back(), 0, 0, false, false, 10, true);
			
			// playtime
			uint64_t hours, minutes;
			std::ostringstream buf;

			getTime(game_manager->getPlaytime(), &hours, &minutes, nullptr);

			buf << std::setfill('0') << std::setw(2)
			    << hours << ':' << std::setw(2) << minutes;

			TextureAccess playtime_text = texture_manager->makeText(buf.str());
			renderer->addRenderItem(playtime_text, 213, 3, false, false, 11, true);

			// pickups remaining
			TextureAccess remaining_text = texture_manager->makeText(std::to_string(game_manager->getRemaining()));
			renderer->addRenderItem(remaining_text, 235, 18, false, false, 11, true);

			// hints
			std::list<std::string> hints = game_manager->getHints();
			// starting height
			int y = 46;

			for (auto it : hints)
				y = printText(it, 160, y, 26, 11) + 4;

			// minimap
			// get player position
			int player_x, player_y;
			game_manager->getPlayer()->getMapPos(&player_x, &player_y);

			/* 
			 * scale minimap and pointer position
			 * so that they align
			 * dirty and hacky but time is nigh
			 */
			renderer->addRenderItem(minimap, -25, -3 * player_y + 119, false, false, 11, true);
			renderer->addRenderItem(point, 3 * player_x - 29, 120, false, false, 11, true);

			// buttons
			if (input_handler->isPlayer(RIGHT, true) and choice < 2)
				++choice;

			if (input_handler->isPlayer(LEFT, true) and choice > 0)
				--choice;

			switch (choice) {
			case 0:
				renderer->addRenderItem(continue_btn, 156, 226, false, false, 11, true);
				break;

			case 1:
				renderer->addRenderItem(documentation_btn, 209, 226, false, false, 11, true);
				break;

			case 2:
				renderer->addRenderItem(exit_btn, 290, 226, false, false, 11, true);
				break;

			default:
				choice = 0;
				break;
			}

			if (input_handler->isEnter(true))
				switch (choice) {
				case 0:
					in_menu = false;
					break;

				case 1:
					openDocumentation();
					break;

				case 2:
					parent->quit();
					break;
				}
		}
	} else if (menu_counter > 0) {
		if (tick >= menu_deadline) {
			menu_deadline = tick + FRAMETIME;
			--menu_counter;
		}

		renderer->addRenderItem(menu[menu_counter], 0, 0, false, false, 10, true);
	}

	if (in_quiz) {
		// for buttons
		static int choice = 1;

		if (quiz_counter < quiz.size() - 1) {
			if (tick >= quiz_deadline) {
				quiz_deadline = tick + FRAMETIME;
				++quiz_counter;
			}

			renderer->addRenderItem(quiz[quiz_counter], 0, 0, false, false, 8, true);

			choice = 1;
		} else {
			renderer->addRenderItem(quiz.back(), 0, 0, false, false, 8, true);

			// question
			printText(question, 10, 5, 24, 9);

			// answers
			static std::vector<bool> selected(3);
			if (input_handler->isAnswer(1, true))
				selected[0] = not selected[0];
			if (input_handler->isAnswer(2, true))
				selected[1] = not selected[1];
			if (input_handler->isAnswer(3, true))
				selected[2] = not selected[2];

			COLOR color;
			TextureAccess digit;

			if (selected[0])
				color = GREEN;
			else
				color = BLACK;

			digit = texture_manager->makeText("1.", color);
			renderer->addRenderItem(digit, 160, 5, false, false, 9, true);
			printText(answers[0], 175, 5, 24, 9, color);

			if (selected[1])
				color = GREEN;
			else
				color = BLACK;

			digit = texture_manager->makeText("2.", color);
			renderer->addRenderItem(digit, 160, 80, false, false, 9, true);
			printText(answers[1], 175, 80, 24, 9, color);

			if (selected[2])
				color = GREEN;
			else
				color = BLACK;

			digit = texture_manager->makeText("3.", color);
			renderer->addRenderItem(digit, 160, 155, false, false, 9, true);
			printText(answers[2], 175, 155, 24, 9, color);

			// buttons
			if (input_handler->isPlayer(RIGHT, true) and choice < 1)
				++choice;

			if (input_handler->isPlayer(LEFT, true) and choice > 0)
				--choice;

			switch (choice) {
			case 0:
				renderer->addRenderItem(documentation_btn, 172, 225, false, false, 9, true);
				break;

			case 1:
				renderer->addRenderItem(submit_btn, 265, 225, false, false, 9, true);
				break;

			default:
				choice = 1;
				break;
			}

			if (input_handler->isEnter(true))
				switch (choice) {
				case 0:
					openDocumentation();
					break;

				case 1:
					game_manager->getQuizManager()->provideAnswer(selected);
					selected[0] = selected[1] = selected[2] = false;
					break;
				}
		}
	} else if (quiz_counter > 0) {
		if (tick >= quiz_deadline) {
			quiz_deadline = tick + FRAMETIME;
			--quiz_counter;
		}

		renderer->addRenderItem(quiz[quiz_counter], 0, 0, false, false, 8, true);
	}

	game_manager->setPaused(in_menu or in_quiz);
}

int UIManager::printText(std::string text, int x, int y, int max_char, int layer, COLOR color)
{
	/*
	 * function to print text with wrapping
	 * use ; to force newline
	 */

	// spacing between lines
	static const int SPACING = 12;
	unsigned int begin = 0;
	unsigned int end = 1;

	while (end < text.size()) {
		if (text[end] == ';' or end - begin + 1 >= max_char) {
			TextureAccess text_part = texture_manager->makeText(text.substr(begin, end - begin), color);
			renderer->addRenderItem(text_part, x, y, false, false, layer, true);

			begin = end;
			y += SPACING;

			if (text[begin] == ';') {
				++begin;
				++end;
			}
		}

		++end;
	}

	// print last part
	TextureAccess text_part = texture_manager->makeText(text.substr(begin), color);
	renderer->addRenderItem(text_part, x, y, false, false, layer, true);

	y += SPACING;

	// so that it can be continued
	return y;
}

void UIManager::openDocumentation()
{
	static std::filesystem::path course("data/course.pdf");

#ifdef __unix__
	if (fork() == 0)
		execl("/usr/bin/xdg-open", "xdg-open", course.string().c_str(), (char *)0);
#elif _WIN32
	course.make_preferred();
	ShellExecuteA(0, 0, course.string().c_str(), 0, 0, SW_SHOW);
#else
#error Unsupported platform
#endif
}
