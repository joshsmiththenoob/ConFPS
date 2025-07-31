// ConFPS.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <Windows.h>

using namespace std;

int nScreenWidth{ 120 };
int nScreenHeight{ 40 };

float fPlayerX{ 8.0f };
float fPlayerY{ 11.0f };
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
	map += L"#....#.........#";
	map += L"#..........#...#";
	map += L"#....#.....#...#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"#..###.........#";
	map += L"#..............#";
	map += L"#.....#........#";
	map += L"#.....#........#";
	map += L"#....###.......#";
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
		if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
			fPlayerA -= (0.5f) * fElaspedTime * 3;

		if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
			fPlayerA += (0.5f) * fElaspedTime * 3;

		// Move&Back Forward in the map
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) 
		{
			fPlayerX += sinf(fPlayerA) * fElaspedTime * 5;
			fPlayerY += cosf(fPlayerA) * fElaspedTime * 5;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') // If we hit the wall, we will not move -> Undo moving action
			{
				fPlayerX -= sinf(fPlayerA) * fElaspedTime * 5;
				fPlayerY -= cosf(fPlayerA) * fElaspedTime * 5;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{ 
			fPlayerX -= sinf(fPlayerA) * fElaspedTime * 5;
			fPlayerY -= cosf(fPlayerA) * fElaspedTime * 5;


			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') // If we hit the wall, we will not move -> Undo moving action
			{
				fPlayerX += sinf(fPlayerA) * fElaspedTime * 5;
				fPlayerY += cosf(fPlayerA) * fElaspedTime * 5;
			}
		}

		// Move Left&Right in the map
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
		{
			fPlayerX -= cosf(fPlayerA) * fElaspedTime * 5;
			fPlayerY += sinf(fPlayerA) * fElaspedTime * 5;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') // If we hit the wall, we will not move -> Undo moving action
			{
				fPlayerX += cosf(fPlayerA) * fElaspedTime * 5;
				fPlayerY -= sinf(fPlayerA) * fElaspedTime * 5;
			}
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			fPlayerX += cosf(fPlayerA) * fElaspedTime * 5;
			fPlayerY -= sinf(fPlayerA) * fElaspedTime * 5;
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') // If we hit the wall, we will not move -> Undo moving action
			{
				fPlayerX -= cosf(fPlayerA) * fElaspedTime * 5;
				fPlayerY += sinf(fPlayerA) * fElaspedTime * 5;
			}
		}


		// Axis going across the screen
		for (int x = 0; x < nScreenWidth; x++)
		{
			// for each column, calculate the projected ray angle into the world space
			float fRayAngle{ (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV };

			// Track distance from player to the wall
			float fDistanceToWall{ 0.0f };
			bool bHitWall{ false };
			bool bBoundary{ false }; // To make the wall more realistic, we need to detect if the wall is in the boundary

			// Unit vector of the ray direction so that we can take one step on that direction to see if we hit a wall
			float fEyeX{ sinf(fRayAngle) };
			float fEyeY{ cosf(fRayAngle) };
			
			while ((!bHitWall) && (fDistanceToWall < fDepth))
			{
				// If we didn't hit the wall, we can take a step in the ray direction
				fDistanceToWall += 0.1f;
				
				// because wall's boundaries are at integer coordinates(dot index), so we use intger casting to get the coordinates of the wall
				int nTestX{ (int)(fPlayerX + fEyeX * fDistanceToWall) };
				int nTestY{ (int)(fPlayerY + fEyeY * fDistanceToWall) };

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

						// if we hit the wall, we need to check if the ray is in the boundary of the wall
						std::vector<pair<float, float >> vBoundary; // To store the boundary of the wall, each element is a pair of (distance, dot product) p.s.: dot product represents the angle between two rays(vectors (ray from player and boundary))
						
						for (int tx = 0; tx < 2; tx++) // Check the two boundaries of the wall
						{
							for (int ty = 0; ty < 2; ty++)
							{
								// Get the boundary coordinates of the wall
								int nBoundaryX{ nTestX + tx };
								int nBoundaryY{ nTestY + ty };
								// Calculate the distance to the boundary
								float fBoundaryDistance{ sqrtf((nBoundaryX - fPlayerX) * (nBoundaryX - fPlayerX) + (nBoundaryY - fPlayerY) * (nBoundaryY - fPlayerY)) };
								// Calculate the dot product between the ray and the boundary
								float fDotProduct{ (fEyeX * (nBoundaryX - fPlayerX) + fEyeY * (nBoundaryY - fPlayerY)) / (sqrtf(fEyeX * fEyeX + fEyeY * fEyeY) * sqrtf((nBoundaryX - fPlayerX) * (nBoundaryX - fPlayerX) + (nBoundaryY - fPlayerY) * (nBoundaryY - fPlayerY))) };
								vBoundary.push_back(make_pair(fBoundaryDistance, fDotProduct));

							}
						}

						// Sort the boundary by distance (closet -> farthest)
						sort(vBoundary.begin(), vBoundary.end(), [](const pair<float, float>& left, const pair<float, float>& right) {
							return left.first < right.first;
							});


						//float fBound{ 0.05f }; // Boundary threshold to determine if the ray is in the boundary
						float fBound{ 0.01f }; // Boundary threshold to determine if the ray is in the boundary
						if (acos(vBoundary.at(0).second) < fBound)
							bBoundary = true; // If the angle between the ray and the boundary is less than the threshold, we consider it as a boundary
						if (acos(vBoundary.at(1).second) < fBound)
							bBoundary = true; // If the angle between the ray and the boundary is less than the threshold, we consider it as a boundary
						if (acos(vBoundary.at(2).second) < fBound)
							bBoundary = true; // If the angle between the ray and the boundary is less than the threshold, we consider it as a boundary
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
			
			if (bBoundary) // If the ray is in the boundary of the wall, we need to use a different shade
				nShade = ' '; // Use a different shade for the boundary


			// Shading the wall: further to the wall (long distance) -> the wall will be darker, and vice versa. by using UTF-16 Unicde for ascii character 
			// reference: https://www.ascii-code.com/CP437
			//short nShadeOfFloor = ' ';
			//if (fDistanceToWall <= fDepth / 4.0f) // Get very close to wall -> will be full-shaded
			//	nShadeOfFloor = 0x0040;
			//else if (fDistanceToWall <= fDepth / 3.0f)
			//	nShadeOfFloor = 0x0025;
			//else if (fDistanceToWall <= fDepth / 2.0f)
			//	nShadeOfFloor = 0x0023;
			//else
			//	nShadeOfFloor = ' '; // So far, far way -> will see nothing
			//short nFarFloor { 0x0023 };
			//short nSuitableFloor{ 0x0025 };
			//short nCloseFloor{ 0x0040 };



			// Calculate distance to ceiling and floor -> Render column of the Screen (View of Player)
			int nCeiling{ (int)((float)nScreenHeight / 2.0f - nScreenHeight / fDistanceToWall) };
			int nFloor{ nScreenHeight - nCeiling };
			

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y < nCeiling)
					screen[y * nScreenWidth + x] = ' '; // Ceiling
				else if (y >= nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade; // Wall: Use '#' in the beginning, but we can use shading method to shade the wall based on distance 
				//else if (y > nFloor && y <= 30)
				//	screen[y * nScreenWidth + x] = nFarFloor; // Floor
				//else if (y > 30 && y <= 35)
				//	screen[y * nScreenWidth + x] = nSuitableFloor; // Floor
				//else
				//	screen[y * nScreenWidth + x] = nCloseFloor;
				else {
					float b{ 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f)) };
					
					if (b < 0.25)
						nShade = '#';
					else if (b < 0.5)
						nShade = 'x';
					else if (b < 0.75)
						nShade = '.';
					else if (b < 0.9)
						nShade = '-';
					else
						nShade = ' ';
					screen[y * nScreenWidth + x] = nShade; // Wall: Use '#' in the beginning, but we can use shading method to shade the wall based on distance 

				}
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
