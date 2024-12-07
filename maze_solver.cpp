#include <iostream>
#include <vector>
#include <random>
#include <tuple>
#include <functional>
#include <ctime>
#include <chrono>
#include <array>

using namespace std;
using namespace std::chrono;

// Edit these values
const bool debugMode = false;;
const int globalSize = 100;

class Cell {
public:
    int x;
    int y;
    bool state; //true=path false=wall
};


class Maze {
public:
    static const int sideLength = globalSize;
    Cell cells[sideLength][sideLength];
    vector<int> startPos;

    void populateMaze() {
        for (int y = 0; y < sideLength; ++y) {
            for (int x = 0; x < sideLength; ++x) {
                cells[y][x] = {x, y, false};
            }
        }
    }


    void printMaze() {
        int rowNum = 0;
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                if (cell.state == true) {
                    if (rowNum == sideLength - 1) {
                        // Goal
                        cout << "🟨";
                    }
                    else if (rowNum == 0) {
                        // Start
                        cout << "🟦";
                    }
                    else {
                        cout << "⬜";
                    }
                }
                else {
                    cout << "⬛";
                }
                // cout << " "; // used to correct line height and character width differences
            }
            cout << "\n";
            ++rowNum;
        }
        cout << "\n\n\n";
    }

    void pickStart() {
        int startX = (rand() % (sideLength-2))+1;
        cells[0][startX].state = true;
        // Two cells are picked at start because 
        // the cell at y=0 can only move to the cell below it
        cells[1][startX].state = true;
        startPos = {1, startX};
    }    

    void carvePath() {
        vector<pair<int, int>> pathList = {};
        Cell cell = cells[startPos[0]][startPos[1]];

        vector<pair<int, int>> directions = {
                {-2, 0},   // nn (north)
                {0, 2},    // ee (east)
                {2, 0},    // ss (south)
                {0, -2}    // ww (west)
        };

        // Main loop
        while (true) {
            if (debugMode) printMaze();
            // Checks all four directions
            vector<pair<int,int>> validNeighbors = {};

            for (const auto& direction : directions) {
                int newY = cell.y + direction.first;
                int newX = cell.x + direction.second;
                // Check if it's within bounds of the maze and if it's not already a path
                if ((newX >= 1 && newX < sideLength - 1 && newY >= 1 && newY < sideLength - 1) && !cells[newY][newX].state) {
                    validNeighbors.push_back({newY, newX});
                }
            }
            if (validNeighbors.empty()) {
                if (!pathList.empty()) {
                    pathList.pop_back();
                    cell.y = pathList.back().first;
                    cell.x = pathList.back().second;
                }
                else {
                    break;
                }
            }
            else {
                int randomIndex = rand() % validNeighbors.size();
                auto [newY, newX] = validNeighbors[randomIndex];

                cells[(cell.y + newY) / 2][(cell.x + newX) / 2].state = true;
                cells[newY][newX].state = true;
               
                cell.x = newX;
                cell.y = newY;
                pathList.push_back({newY, newX});
            }
        }
    }

    void linkGoal() {
        Cell goalConnectionPoint;
        bool connection = false;
        for (int row = sideLength - 1; row >= 0; --row) {
            for (int cellPos = 0; cellPos <= sideLength; ++cellPos) {
                if (cells[row][cellPos].state == 1) {
                    goalConnectionPoint.y = row;
                    goalConnectionPoint.x = cellPos;
                    connection = true;
                    break;
                }
            }
            if (connection) break;
        }

        for (int yCoord = goalConnectionPoint.y; yCoord < sideLength; ++yCoord) {
            cells[yCoord][goalConnectionPoint.x].state = 1;
        }
    }
};


void generateMaze() {
    srand(static_cast<unsigned int>(time(0)));
    Maze maze;
    maze.populateMaze();
    if (debugMode) maze.printMaze();

    maze.pickStart();
    maze.carvePath();
    maze.linkGoal();
    maze.printMaze();
}

void displayProgressBar(int currentStep, int totalSteps) {
    int barWidth = 20; // Fixed width for the progress bar
    int progress = (currentStep * barWidth) / totalSteps;
    cout << "\r["; // Return cursor to the start of the line
    for (int i = 0; i < barWidth; ++i) {
        if (i < progress) {
            cout << "#"; // Filled portion
        } else {
            cout << "="; // Empty portion
        }
    }
    cout << "] (" << currentStep << "/" << totalSteps << ")..." << flush;
}

void benchmarkMaze(int repetitions) {
    Maze maze;
    long long totalTime = 0;

    for (int i = 0; i < repetitions; i++) {
        auto start = high_resolution_clock::now();
        maze.populateMaze();
        maze.pickStart();
        maze.carvePath();
        maze.linkGoal();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        totalTime += duration.count();

        displayProgressBar(i + 1, repetitions);
    }

    // Print the average time
    int averageTime = totalTime / repetitions;
    cout << "\nAverage time taken to generate the maze: " << averageTime << " μs" << endl;
}

int main() {
    bool benchmarkMode = false;
    cout << "Benchmark? (y/n)?";
    char input;
    cin >> input;
    if (input == 'y') {
        benchmarkMode = true;
    }

    if (benchmarkMode) {
        int repetitions;
        cout << "Select the amount of repetitions: ";
        cin >> repetitions;
        benchmarkMaze(repetitions);
    }
    else {
        generateMaze();
    }
}
