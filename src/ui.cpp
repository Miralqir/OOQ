#include "ui.h"

#include <cctype>

UIManager::UIManager(Manager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	texture_manager(renderer->getTextureManager()),
	input_handler(parent->getInputHandler()),
	game_manager(parent->getGameManager()),
	paused(false),
	in_quiz(false)
{
	background = texture_manager->loadTexture("data/ui/crem_background.png");
	background_quiz = texture_manager->loadTexture("data/ui/background_quiz.png");
	exit_button = texture_manager->loadTexture("data/ui/new_exit.png");
	continue_button = texture_manager->loadTexture("data/ui/new_continue.png");
	selected_exit_button = texture_manager->loadTexture("data/ui/selected_exit_button.png");
	selected_continue_button = texture_manager->loadTexture("data/ui/selected_continue_button1.png");
	time_edit1 = texture_manager->loadTexture("data/font/:w.png");
	time_edit2 = texture_manager->loadTexture("data/font/:w.png");
	text_box_short1 = texture_manager->loadTexture("data/ui/txt_box.png");
	text_box_short2 = texture_manager->loadTexture("data/ui/txt_box.png");
	text_box_long = texture_manager->loadTexture("data/ui/text_box_2.png");
	text_box = texture_manager->loadTexture("data/ui/text_box.png");
	sign = texture_manager->loadTexture("data/ui/sign.png");

	for (char i = '0'; i <= '9'; ++i) {
		std::ostringstream buf;
		buf << "data/font/"
			<< i
			<< "b.png";
		digits_black.push_back(texture_manager->loadTexture(buf.str()));
	}

	for (char i = '0'; i <= '9'; ++i) {
		std::ostringstream buf;
		buf << "data/font/"
			<< i
			<< "w.png";
		digits_white.push_back(texture_manager->loadTexture(buf.str()));
	}

	for (char i = '0'; i <= '9'; ++i) {
		std::ostringstream buf;
		buf << "data/font/"
			<< i
			<< "g.png";
		digits_green.push_back(texture_manager->loadTexture(buf.str()));
	}

	for (char i = 'a'; i <= 'z'; ++i) {
		std::ostringstream buf;
		buf << "data/font/"
			<< i
			<< ".png";
		letters.push_back(texture_manager->loadTexture(buf.str()));
	}
}

void UIManager::renderText(std::string text, int pos_x, int pos_y, int max_x)
{
	transform(text.begin(), text.end(), text.begin(), ::toupper);
	int current_x = 0, current_y = 0;
	for(int i = 0; i < text.size(); i++){
		if(std::isalpha(text[i])){
			renderer->addRenderItem(letters[int(text[i]) - 65], pos_x + current_x, pos_y + current_y, false, false, 11, true);
			current_x += letters[int(text[i]) - 65]()->getWidth() + 1;
		}
		else if(text[i] == '?'){
			renderer->addRenderItem(sign, pos_x + current_x, pos_y + current_y, false, false, 11, true);
			current_x += sign()->getWidth() + 1;
		}
		else if(text[i] == ','){
			current_y += 10;
			current_x = 0;
		}
		else{
			current_x += 5;
		}
		if(current_x >= max_x){
			current_y += 10;
			current_x = 0;
		}
	}
}

void UIManager::renderNumber(std::vector<TextureAccess> type_digits, int number, int pos_x, int pos_y)
{
	int digit, get_digit;
	int nr_digits = countDigit(number);
	int current_pos = 0;
	for(int i = nr_digits - 1; i >= 0 ; i--){
		get_digit = std::pow(10, i);
		digit = (number / get_digit) % 10;
		renderer->addRenderItem(type_digits[digit], pos_x + current_pos, pos_y, false, false, 11, true);
		current_pos += type_digits[digit]()->getWidth() + 1;
	}
}

void UIManager::renderTimePart(uint64_t time, int pos_x, int pos_y, int* final_pos)
{
	uint64_t digit, get_digit;
	uint64_t nr_digits = countDigit(time);
	uint64_t current_pos = 0;
	for(int i = nr_digits - 1; i >= 0 ; i--){
		get_digit = std::pow(10, i);
		digit = (time / get_digit) % 10;
		renderer->addRenderItem(digits_white[digit], pos_x + current_pos, pos_y, false, false, 11, true);
		current_pos += digits_white[digit]()->getWidth() + 1;
	}
	*final_pos = current_pos;
}

void UIManager::renderTime(uint64_t time, int pos_x, int pos_y)
{
	int final_pos = 0;
	uint64_t hours, minutes, seconds;
	getTime(time, &hours, &minutes, &seconds);
	renderTimePart(hours, pos_x, pos_y, &final_pos);
	renderer->addRenderItem(time_edit1, pos_x + 6, pos_y, false, false, 11, true);
	renderTimePart(minutes, pos_x + final_pos + 6, pos_y, &final_pos);
	renderer->addRenderItem(time_edit2, pos_x + final_pos + 12, pos_y, false, false, 11, true);
	renderTimePart(seconds, pos_x + final_pos + 18, pos_y, &final_pos);
}

void UIManager::displayQuiz(std::string question, std::vector<std::string> answers)
{
	//paused = true;
	in_quiz = true;
	this->question = question;
	this->answers = answers;
	// just in case
	this->answers.resize(3);
}

void UIManager::endQuiz()
{
	//paused = false;
	in_quiz = false;
}

void UIManager::runTick(uint64_t delta)
{
	if (input_handler->isPause(true)) paused = not paused;

	if(paused)
	{
		static int choice = 0;
		if(input_handler->isPlayer(RIGHT, true))
		{
			choice = 1;
		}
		if(input_handler->isPlayer(LEFT, true))
		{
			choice = 0;
		}
		renderer->addRenderItem(background, 0, 0, false, false, 9, true);
		renderText("H I N T S", 50, 50, 100);
		renderer->addRenderItem(text_box, 10, 70, false, false, 10, true);
		renderer->addRenderItem(text_box_long, 210, 50, false, false, 10, true);
		renderTime(game_manager->getPlaytime(), 220, 53);
		renderText("Collected Lists", 175, 80, 100);
		renderer->addRenderItem(text_box_short1, 270, 78, false, false, 10, true);
		renderNumber(digits_green, game_manager->getCollected(), 282, 82);
		renderText("Remaining Lists", 175, 110, 100);
		renderer->addRenderItem(text_box_short2, 270, 108, false, false, 10, true);
		renderNumber(digits_green, game_manager->getRemaining(), 282, 112);
		if(choice){
			renderer->addRenderItem(continue_button, 190, 180, false, false, 10, true);
			renderer->addRenderItem(selected_exit_button, 250, 180, false, false, 10, true);
		}
		else{
			renderer->addRenderItem(selected_continue_button, 190, 180, false, false, 10, true);
			renderer->addRenderItem(exit_button, 250, 180, false, false, 10, true);
		}

		if(input_handler->isEnter(true)){
			if(choice){
				parent->quit();
			}
			else
				paused = false;
		}
	}

	if (in_quiz) {
		static std::vector<bool> selected(3);
		static auto digits = digits_green;
		renderer->addRenderItem(background_quiz, 0, 0, false, false, 9, true);
		renderText("Q U E S T I O N", 45, 4, 100);
		renderText("A N S W E R S", 200, 4, 100);
		renderText(question, 15, 15, 130);
		if (selected[0])
			digits = digits_green;
		else
			digits = digits_black;
		renderNumber(digits, 1, 165, 15);
		renderText(answers[0], 175, 15, 130);
		if (selected[1])
			digits = digits_green;
		else
			digits = digits_black;
		renderNumber(digits, 2, 165, 105);
		renderText(answers[1], 175, 105, 130);
		if (selected[2])
			digits = digits_green;
		else
			digits = digits_black;
		renderNumber(digits, 3, 165, 200);
		renderText(answers[2], 175, 200, 130);

		if (input_handler->isAnswer(1, true))
			selected[0] = not selected[0];
		if (input_handler->isAnswer(2, true))
			selected[1] = not selected[1];
		if (input_handler->isAnswer(3, true))
			selected[2] = not selected[2];

		if (input_handler->isEnter(true)) {
			//in_quiz = false;
			//game_manager->setPaused(false);
			game_manager->getQuizManager()->provideAnswer(selected);
			selected[0] = selected[1] = selected[2] = false;
		}
	}

	game_manager->setPaused(paused or in_quiz);
}
