// ConFPS.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
# include <Windows.h>
using namespace std;

int nScreenWidth{ 120 };
int nScreenHeight{ 40 };

float fPlayerX{ 0.0f };
float fPLayerY{ 0.0f };
float fPlayerA{ 0.0f };

int nMapWidth{ 16 };
int nMapHeight{ 16 };	

int fFOV{ 3.14157 / 4.0 };

int main()
{
	// Create screen buffer
	wchar_t* screen{ new wchar_t[nScreenWidth * nScreenHeight] };
	HANDLE hConsole{ CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL) };
	SetConsoleActiveScreenBuffer(hConsole); // Set target console
	DWORD dwByteWritten{ 0 };

	wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";


	while (true)
	{
		// Axis going across the screen
		for (int x = 0; x < nScreenWidth; x++)
		{
			// for each column, calculate the projected ray angle into the world space
			float fRayAngle{ (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV };
		}


		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwByteWritten);


	}



	return 0;
	
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
