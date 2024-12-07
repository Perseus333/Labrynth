#include <iostream>
#include <array>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int SIZE = 20;
const int SIZE_SQR = SIZE * SIZE;

array<int, SIZE_SQR> cells;
array<int, SIZE_SQR> stack;

void generateMaze() {
    int sp = 0;  // Stack pointer
    int x = rand() % SIZE;
    int y = 0;
    int currPos = y * SIZE + x;

    cells.fill(1);
    cells[currPos] = 0;
    stack[sp++] = currPos;

    while (sp > 0) {
        currPos = stack[--sp];
        int cx = currPos % SIZE;
        int cy = currPos / SIZE;

        array<int, 4> neighbors;
        int np = 0; // Neighbors pointer

        if (cy > 1 && cells[(cy - 2) * SIZE + cx] == 1) neighbors[np++] = currPos - 2 * SIZE;
        if (cy < SIZE - 2 && cells[(cy + 2) * SIZE + cx] == 1) neighbors[np++] = currPos + 2 * SIZE;
        if (cx > 1 && cells[cy * SIZE + (cx - 2)] == 1) neighbors[np++] = currPos - 2;
        if (cx < SIZE - 2 && cells[cy * SIZE + (cx + 2)] == 1) neighbors[np++] = currPos + 2;

        if (np > 0) {
            int chosen = neighbors[rand() % np];
            cells[chosen] = 0;
            cells[(currPos + chosen) / 2] = 0;
            stack[sp++] = currPos;  // Push current cell back to stack
            stack[sp++] = chosen;   // Push chosen neighbor to stack
        }
    }
}

void printMaze() {
    for (int i = 0; i < SIZE_SQR; ++i) {
        if (i % SIZE == 0) cout << '\n';
        if (cells[i] == 0)
            cout << ((i < SIZE) ? "🟦 " : (i >= SIZE * (SIZE - 1)) ? "🟨 " : "⬜ ");
        else
            cout << "⬛ ";
    }
    cout << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));


    auto start = high_resolution_clock::now();
    generateMaze();
    auto end = high_resolution_clock::now();
    printMaze();

    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "Time taken: " << duration << " microseconds" << endl;

    return 0;
}
