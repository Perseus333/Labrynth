#include <iostream>
#include <vector>
#include <random>
#include <tuple>
#include <functional>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Edit this values
const int globalSize = 100;
const bool strictLines = true;
const bool debugMode = false;
// From 1-100%
const int density = 100;


class Cell {
    public:
    int x;
    int y;
    int state;
    bool blocked;
};


class Maze {
    public:
    int sideLength;
    vector<vector<Cell>> cells;
    // Could be done as an array but there is no point
    vector<int> startPos;

    void populateMaze() {
        cells.resize(sideLength);
        for (int y = 0; y < sideLength; ++y) {
            for (int x = 0; x < sideLength; ++x) {
                Cell newCell;
                newCell.x = x;
                newCell.y = y;
                newCell.state = 0;
                if ((x == 0) && (y == 0)) {
                    newCell.blocked = true;
                }
                else if ((x == 0) || (y == 0) || (x == sideLength-1) || (y == sideLength-1)) {
                    newCell.blocked = true;
                }
                else {
                    newCell.blocked = false;
                }
                cells[y].push_back(newCell);
            }
        }
        if (strictLines) {
            for (int y = 0; y < sideLength; y+=2) {
                for (int x = 0; x < sideLength; x+=2) {
                    cells[y][x].blocked = true;
                }
            }
        }
    }


    void printMaze() {
        int rowNum = 0;
        for (vector<Cell> row : cells) {
            for (Cell cell : row) {
                if (cell.state == 1) {
                    if (rowNum == sideLength-1) {
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
                    if (cell.blocked == true) {
                        if (debugMode) {
                        cout << "❎";
                        }
                        else {
                        cout << "⬛";
                        }
                        
                    }
                    else if (cell.state == 0) {
                        cout << "⬛";
                    }
                    else {
                        // B for broken
                        cout << "🟥";
                    }
                }
                cout << " ";
            }
            cout << "\n";
            ++rowNum;
        }
    cout << "\n\n\n";
    }


    void pickStart() {
        int startY = 0;
        int startX = rand() % sideLength;
        cells[startY][startX].state = 1;
        cells[startY][startX].blocked = false;
        startPos = {startY, startX};
    }
    
    int calculateDensity() {
        // It doesn't coount the blocked cells in the edges
        int amountOfCells = (globalSize-2)*(globalSize-2);
        // To not count the blocked cells in between
        if (strictLines) {
            amountOfCells = (amountOfCells * 3) / 4;
        }
        int amountOfPaths = (amountOfCells * density) / 100;
        return amountOfPaths;
    }


    vector<pair<Cell, int>> checkSurroundings(Cell& cell) {
        // Arbitrary order: clockwise from top: up, right, down, left
        vector<int> surroundingStatus;
        vector<pair<Cell, int>> report;
        vector<reference_wrapper<Cell>> surroundingCells;
        vector<pair<int, int>> directions;
        directions = {
            {-1, 0},   // nn (north)
            {0, 1},    // ee (east)
            {1, 0},    // ss (south)
            {0, -1}    // ww (west)
        };

        for (const auto& direction : directions) {
            int newY = cell.y + direction.first;
            int newX = cell.x + direction.second;

            // Check if the new position is within bounds
            if (newY >= 0 && newY < sideLength && newX >= 0 && newX < sideLength) {
                surroundingCells.push_back(cells[newY][newX]);
            }
        }


        for (Cell& cell : surroundingCells) {
            int state;
            if (!cell.blocked) {
                state = cell.state;
            }
            else {
                state = 3;
            }
            report.push_back(make_pair(cell, state));
        }
        return report;
    }

    bool suitableCarving(Cell wall) {
        bool suitable = true;
        vector<pair<Cell, int>> wallStatus = checkSurroundings(wall);
        int surroundingPaths = 0;
        for (pair<Cell, int> packet : wallStatus) {
            if (packet.second == 1) {
                ++surroundingPaths;
            }
        }
        // If a wall has two or more pahts adjacent it means that when putting a path there a loop will be created
        if (surroundingPaths > 1) {
            suitable = false;
        }
        return suitable;
    }


    vector<Cell> getAllPaths() {

        vector<Cell> pathsList;

        for (int y = 0; y < sideLength; ++y) {
            for (int x = 0; x < sideLength; ++x) {

                Cell& cell = cells[y][x];

                if (cell.state == 1) {
                    pathsList.push_back(cell);
                }
            }
        }
        return pathsList;
    }


    void carvePath(int depthGoal) {
        int depth = 0;
        int startY = startPos[0];
        int startX = startPos[1];
        
        // Assuming cells is a 2D vector of Cell objects
        // Initialize prevCell to a non-valid cell if such an object exists, or just to a valid cell in your grid.
        Cell& prevCell = cells[startY][startX]; 
        Cell& currCell = cells[startY][startX];

        while (depth < depthGoal) {
            vector<Cell*> pathCandidates; // Vector to store pointers to Cells
            vector<pair<Cell, int>> surroundingStatus = checkSurroundings(currCell);

            for (pair<Cell, int>& adjacent : surroundingStatus) {
                if ((&adjacent.first != &prevCell) && (adjacent.second != 3)) {
                    bool suitable = suitableCarving(adjacent.first);
                    if (suitable) {
                        pathCandidates.push_back(&adjacent.first); // Store pointer to Cell
                    }
                }
            }

            // Ensure there are path candidates before trying to select one
            if (!pathCandidates.empty()) {
                Cell& pathChosen = *pathCandidates[rand() % pathCandidates.size()];
                // int tunnelLength = rand() % maxStraightTunnel;
                int yDiff = pathChosen.y - currCell.y;
                int xDiff = pathChosen.x - currCell.x;
                cells[currCell.y + yDiff][currCell.x + xDiff].state = 1;
                /*
                for (int i = 1; i <= tunnelLength; ++i) {
                    cells[currCell.y + yDiff * i][currCell.x + xDiff * i].state = 1;
                }
                */

                prevCell = currCell;  // Update prevCell to be the current cell
                currCell = pathChosen; // Move to the chosen cell
                ++depth;
            }
            else {
                // If it gets stuck, restart from a random existing path cell
                vector<Cell> pathsList = getAllPaths();
                if (!pathsList.empty()) {
                    Cell& newStartCell = pathsList[rand() % pathsList.size()];
                    prevCell = currCell; // Keep track of the previous cell
                    currCell = newStartCell; // Start from a new path cell
                } else {
                    // Edge case: No paths found
                    break;
                }
            }
            // printMaze();
        }
    }

    void linkGoal() {
        Cell goalConnectionPoint;
        goalConnectionPoint.x = 999;
        for (int row = sideLength-1; row >= 0; --row) {
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
    auto start = high_resolution_clock::now();

    Maze maze;
    maze.sideLength = globalSize;
    maze.populateMaze();
    if (debugMode) {
        maze.printMaze();
    }
    maze.pickStart();
    maze.carvePath(maze.calculateDensity());
    maze.linkGoal();

    auto stop = high_resolution_clock::now();
    
    // Not calculate the printing time
    maze.printMaze();

    // Calculate the duration
    auto duration = duration_cast<milliseconds>(stop - start);

    // Print the duration in milliseconds
    cout << "Time taken to generate the maze: " << duration.count() << " ms" << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    generateMaze(globalSize);
}