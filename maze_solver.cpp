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
const int globalSize = 101;
const bool debugMode = false;

class Cell {
public:
    int x;
    int y;
    bool state; //true=path false=wall
};


class Maze {
public:
    int sideLength;
    vector<vector<Cell>> cells;
    vector<int> startPos;


    void populateMaze() {
        cells.resize(sideLength, vector<Cell>(sideLength)); 
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
                // cout << " ";
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
            if (debugMode) {
                printMaze();
            }
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
            
            if (!validNeighbors.empty()) {
                int randomIndex = rand() % validNeighbors.size();
                auto [newY, newX] = validNeighbors[randomIndex];

                cells[(cell.y + newY) / 2][(cell.x + newX) / 2].state = true;
                cells[newY][newX].state = true;
               
                cell.x = newX;
                cell.y = newY;
                pathList.push_back({newY, newX});
            }
            else {
                if (!pathList.empty()) {
                    pathList.pop_back();
                    cell.y = pathList.back().first;
                    cell.x = pathList.back().second;
                }
                else {
                    break;
                }
            }
        }
    }

    void linkGoal() {
        Cell goalConnectionPoint;
        goalConnectionPoint.x = 999;
        for (int row = sideLength - 1; row >= 0; --row) {
            for (int cellPos = 0; cellPos <= sideLength; ++cellPos) {
                if (cells[row][cellPos].state == 1) {
                    goalConnectionPoint.y = row;
                    goalConnectionPoint.x = cellPos;
                    break;
                }
            }
            if (goalConnectionPoint.x != 999) {
                break;
            }
        }

        for (int yCoord = goalConnectionPoint.y; yCoord < sideLength; ++yCoord) {
            cells[yCoord][goalConnectionPoint.x].state = 1;
        }
    }
};


void generateMaze(int sideLength) {
    // Start measuring time

    Maze maze;
    maze.sideLength = sideLength;
    auto start = high_resolution_clock::now();
    maze.populateMaze();
    if (debugMode) {
        maze.printMaze();
    }
    maze.pickStart();
    maze.carvePath();
    maze.linkGoal();
    auto stop = high_resolution_clock::now();


    // Not calculate the printing time
    maze.printMaze();

    // Calculate the duration
    auto duration = duration_cast<microseconds>(stop - start);

    // Print the duration in milliseconds
    cout << "Time taken to generate the maze: " << duration.count() << " μs" << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    generateMaze(globalSize);
}
