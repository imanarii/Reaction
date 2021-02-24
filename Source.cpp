#include <iostream>
#include <chrono>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <string>
#include <thread>
#include <algorithm>
#include <conio.h>

std::vector<float> results{};
std::fstream scores;

std::chrono::duration<float> duration;

bool isTooEarly = true, pressTooSoon = false;

void mainMenu();
float avg();

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}


// Reads the file and converts the string to float values which are stored in the results vector
void addToVecFromFile()
{
	float t;
	std::string str;
	std::ifstream scores("scores.txt");
	results.clear();
	while (std::getline(scores, str))
	{
		t = stof(str);
		results.push_back(t);
	}
}


// Displays past scores
void history()
{
	addToVecFromFile();
	system("cls");
	std::cout << "Score history: \n";
	int count = 1;
	for (auto i : results )
	{
		std::cout << count << ".) " << i << "\n";
		count++;
	}
	float min = *min_element(results.begin(), results.end());
	std::cout << "\naverage score: " << avg() << "\t";
	std::cout << "Best score: " << min << "\n\n";
	//mainMenu();
}


void mainMenu()
{
	static bool first = true;
	
	if (first)
	{
		std::cout << "This is a test of your reaction time. When promted you must click the left mouse button as quickly as possible." << std::endl;
		std::cout << "press Enter when you are ready to start";
	}
	else
		std::cout << "press Enter when you are ready to start";
	
	first = false;
	
	while (true)
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
			break;
		else if (GetAsyncKeyState(VK_TAB) & 1)
			history();
	}

	
}

// Calculates Average Score
float avg()
{
	double sum = 0.0f;
	
	if (results.size() >= 2)
	{
		for (auto i : results)
			sum += i;
		sum = sum / results.size();
	}
	
	return sum;
}

inline void timerFunc()
{
	

	auto start = std::chrono::high_resolution_clock::now();
	while (true)
	{
		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
			break;
	}
	auto end = std::chrono::high_resolution_clock::now();
	duration = (end - start) * 1000;
}

void tooEarlyFunc()
{
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

	using namespace std::literals::chrono_literals;
	
	while (isTooEarly && !pressTooSoon)
	{
		std::this_thread::sleep_for(50ms);

		if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
		{
			system("cls");
			std::cout << "Too Early!!" << std::endl;
			pressTooSoon = true;
		}
	}
}


int main()
{
	ShowConsoleCursor(false);
	
	HANDLE hInput;
	DWORD prev_mode;
	hInput = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(hInput, &prev_mode);
	SetConsoleMode(hInput, prev_mode & ENABLE_EXTENDED_FLAGS);


	while (true)
	{

		isTooEarly = true;
		pressTooSoon = false;

		addToVecFromFile();
		srand(GetTickCount64());
		float rando = rand() % 6000 + 850;

		mainMenu();

		system("cls");
		std::cout << "Get ready...";
		std::thread tooEarlyThread(tooEarlyFunc);
		Sleep(rando);
		isTooEarly = false;
		tooEarlyThread.join();
		
		if (!pressTooSoon)
		{
			
			system("color 40");
			std::thread timerThread(timerFunc);
			system("cls");
			std::cout << "********* CLICK *********" << std::flush; 
			timerThread.join();


			scores.open("scores.txt", std::ios::app);
			scores << duration.count() << "\n";
			scores.close();

			std::string grade = (duration.count() < 200) ? "Super Fast" : (duration.count() < 250 ) ? "Pretty Fast" : (duration.count() < 300 && duration.count() > 250) ? "Normal" : "Super Slow";

			addToVecFromFile();

			system("cls");
			system("color 0f");
			std::cout << "\t" << grade << "\nTime: " << duration.count() << "\t"
				<< "Average: " << avg() << "\n" << "To veiw all past scores press tab\n\n";
		}
		else
			mainMenu();
	}

	return 0;
}