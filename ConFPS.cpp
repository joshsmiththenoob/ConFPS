// ConFPS.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <chrono>
#include <Windows.h>
using namespace std;

int nScreenWidth{ 120 };
int nScreenHeight{ 40 };

float fPlayerX{ 8.0f };
float fPLayerY{ 11.0f };
float fPlayerA{ 0.0f };

int nMapWidth{ 16 };
int nMapHeight{ 16 };	

float fFOV{ 3.14157 / 4.0 };
float fDepth{ 16.0f }; // Max distance to check for walls because map size is 16x16

int main()
{
	// Create screen buffer (Player's View)
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


	// Using chrono library to show frame rate [fps]
	auto tp1{ chrono::system_clock::now() };
	auto tp2{ chrono::system_clock::now() };



	while (true)
	{
		// Show the frame rate when frame was refreshed & Control the speed of angle view rotation 
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elaspedTime = tp2 - tp1;
		tp1 = tp2;
		float fElaspedTime{ elaspedTime.count() };

		// Controls
		// Rotate the Angle of View of Player
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (0.5f) * fElaspedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (0.5f) * fElaspedTime;

		// Move&Back Forward in the map
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) 
		{
			fPlayerX += sinf(fPlayerA) * fElaspedTime * 5;
			fPlayerX += cosf(fPlayerA) * fElaspedTime * 5;
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{ 
			fPlayerX -= sinf(fPlayerA) * fElaspedTime * 5;
			fPlayerX -= cosf(fPlayerA) * fElaspedTime * 5;
		}

		// Axis going across the screen
		for (int x = 0; x < nScreenWidth; x++)
		{
			// for each column, calculate the projected ray angle into the world space
			float fRayAngle{ (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV };

			// Track distance from player to the wall
			float fDistanceToWall{ 0.0f };
			bool bHitWall{ false };

			// Unit vector of the ray direction so that we can take one step on that direction to see if we hit a wall
			float fEyeX{ sinf(fRayAngle) };
			float fEyeY{ cosf(fRayAngle) };
			
			while ((!bHitWall) && (fDistanceToWall < fDepth))
			{
				// If we didn't hit the wall, we can take a step in the ray direction
				fDistanceToWall += 0.1f;
				
				// because wall's boundaries are at integer coordinates(dot index), so we use intger casting to get the coordinates of the wall
				int nTestX{ (int)(fPlayerX + fEyeX * fDistanceToWall) };
				int nTestY{ (int)(fPLayerY + fEyeY * fDistanceToWall) };

				// On the interger test location, we need to test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true; // Out of bounds, so we hit the wall
					fDistanceToWall = fDepth; // Set distance to max depth
				}
				else
				{
					// Ray is inbounds -> to test if the ray's location is on a wall
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bHitWall = true; // We hit the wall
					}
				}



			}


			// Shading the wall: further to the wall (long distance) -> the wall will be darker, and vice versa. by using UTF-16 Unicde for ascii character 
			// reference: https://www.ascii-code.com/CP437
			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.0f) // Get very close to wall -> will be full-shaded
				nShade = 0x2588;
			else if (fDistanceToWall <= fDepth / 3.0f)
				nShade = 0x2593;
			else if (fDistanceToWall <= fDepth / 2.0f)
				nShade = 0x2592;
			else if (fDistanceToWall <= fDepth) // Get far to wall -> will be light-shaded
				nShade = 0x2591;
			else
				nShade = ' '; // So far, far way -> will see nothing


			// Shading the wall: further to the wall (long distance) -> the wall will be darker, and vice versa. by using UTF-16 Unicde for ascii character 
			// reference: https://www.ascii-code.com/CP437
			short nShadeOfFloor = ' ';
			if (fDistanceToWall <= fDepth / 4.0f) // Get very close to wall -> will be full-shaded
				nShadeOfFloor = 0x0040;
			else if (fDistanceToWall <= fDepth / 3.0f)
				nShadeOfFloor = 0x0025;
			else if (fDistanceToWall <= fDepth / 2.0f)
				nShadeOfFloor = 0x0023;
			else
				nShadeOfFloor = ' '; // So far, far way -> will see nothing
			short nFarFloor { 0x0023 };
			short nSuitableFloor{ 0x0025 };
			short nCloseFloor{ 0x0040 };



			// Calculate distance to ceiling and floor
			int nCeiling{ (int)((float)nScreenHeight / 2.0f - nScreenHeight / fDistanceToWall) };
			int nFloor{ nScreenHeight - nCeiling };
			

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y < nCeiling)
					screen[y * nScreenWidth + x] = ' '; // Ceiling
				else if (y >= nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade; // Wall: Use '#' in the beginning, but we can use shading method to shade the wall based on distance 
				else if (y > nFloor && y <= 30)
					screen[y * nScreenWidth + x] = nFarFloor; // Floor
				else if (y > 30 && y <= 35)
					screen[y * nScreenWidth + x] = nSuitableFloor; // Floor
				else
					screen[y * nScreenWidth + x] = nCloseFloor;

			}

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
