#include <iostream>
#include <chrono>
#include <Windows.h>
#include <string>

using namespace std;

int nMapWidth{ 16 };
int nMapHeight{ 16 };

float fPlayerX{ 8.0f };
float fPlayerY{ 3.0f };
float fPlayerA{ 0.0f }; // Player angle in radians

float fAOV{ 3.14159f / 4.0f }; // Angle of view in radians
float fDepth{ 16.0f }; // Depth of the raycaster

int main()
{
    const int nScreenWidth{ 120 };
    const int nScreenHeight{ 40 };

    // Create the canvas
    wchar_t* screen{ new wchar_t[nScreenWidth * nScreenHeight] };
    // Create elements in screen
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
    {
        if (i < 1200)
            screen[i] = L' ';
        else
            screen[i] = L'%';
    }
    screen[nScreenWidth * nScreenHeight - 1] = '\0'; // null terminator


    // Create console handler (custom screen buffer)
    HANDLE hConsole{ CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL) };
    // Set Active Screen Buffer so that console will show this buffer first
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten{ 0 };


    // Create world map
    std::wstring map{};

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


    // Get two time point to get frame rate
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();


    while (true) {
        // Get frame rate per loop
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elaspedTime = tp2 - tp1;
        tp1 = tp2;
        float fElaspedTime{ elaspedTime.count() };

        // Controls
        // Handle rotation
        if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
            fPlayerA += (0.1f) * fElaspedTime;

        if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
            fPlayerA -= (0.1f) * fElaspedTime;

        // Axis going across the screen
        for (int x = 0; x < nScreenWidth; x++)
        {
            // for each column, calculate the projected ray angle into the world space
            float fRayAngle{ (fPlayerA + fAOV / 2.0f) - ((float)x / (float)nScreenWidth) * fAOV };

            // Track distance from player to the wall
            float fDistanceToWall{ 0.0f };
            bool bHitWall{ false }; // Whether we hit a wall
            // Unit vector of the ray direction so that we can take one step on that direction to see if we hit a wall
            float fEyeX{ sinf(fRayAngle) };
            float fEyeY{ cosf(fRayAngle) };

            // Unit vector for the ray -> we can make one step on that direction to detect walls


            while (!bHitWall && fDistanceToWall < fDepth)
            {
                // If we didn't hit the wall, we can take a step in the ray direction
                fDistanceToWall += 0.1f;

                // because wall's boundaries are at integer coordinates(dot index), so we use intger casting to get the coordinates of the wall
                int nTestX{ (int)(fPlayerX + fEyeX * fDistanceToWall) };
                int nTestY{ (int)(fPlayerY + fEyeY * fDistanceToWall) };

                // Check if ray is out of bounds
                if ((nTestX < 0) || (nTestX >= nMapWidth) || (nTestY < 0) || (nTestY >= nMapHeight))
                {
                    bHitWall = true; // Out of bounds, we hit the wall
                    fDistanceToWall = fDepth; // Set distance to the maximum depth
                }
                else // If ray is still in bounds
                {
                    if (map[nTestY * nMapWidth + nTestX] == '#') // If we hit a wall
                    {
                        bHitWall = true;
                    }
                }
            }


            // Decide the height of the wall by distance to the wall
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = (float)(nScreenHeight / 2.0) + nScreenHeight / ((float)fDistanceToWall); // nFloor = nScreenHeight - nCeiling;

            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y < nCeiling) // If the pixel is above the nCeiling -> should be ceiling
                {
                    screen[y * nScreenWidth + x] = ' '; // Set to space
                }
                else if (y > nCeiling && y <= nFloor) // If the pixel is between the nCeiling and nFloor -> should be wall
                {
                    screen[y * nScreenWidth + x] = '#'; // Set to wall character
                }
                else // If the pixel is below the nFloor -> should be floor
                {
                    screen[y * nScreenWidth + x] = ' '; // Set to floor character
                }
            }

        }
        // Write cavas into console to show screen 
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }
    return 0;

}