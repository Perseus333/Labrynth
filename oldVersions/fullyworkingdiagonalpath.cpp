#include <iostream>
#include <vector>
#include <random>
#include <tuple>
#include <functional>
#include <ctime>



using namespace std;

// Temporary debug values
const int globalSize = 30;
const int maxStraightTunnel = 5;

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
                if ((x == 0) || (y == 0) || (x == sideLength-1) || (y == sideLength-1)) {
                    newCell.blocked = true;
                }
                else {
                    newCell.blocked = false;
                }
                cells[y].push_back(newCell);
            }
        }
    }


    void printMaze() {
        for (vector<Cell> row : cells) {
            for (Cell cell : row) {
                if (cell.state == 1) {
                    cout << "⬜";
                }
                else {
                    if (cell.blocked == true) {
                        cout << "❎";
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
        }
    cout << "\n\n\n";
    }


    void pickStart() {
        int startY = 0;
        int startX = rand() % sideLength;
        cells[startY][startX].state = 1;
        startPos = {startY, startX};
    }
    

    vector<pair<Cell, int>> checkSurroundings(Cell& cell) {
        // Arbitrary order: clockwise from top: up, right, down, left
        vector<int> surroundingStatus;
        vector<pair<Cell, int>> report;
        vector<reference_wrapper<Cell>> surroundingCells;

        // Check if 'up' is within bounds and add it to the vector
        if (cell.y > 0) {
            surroundingCells.push_back(cells[cell.y - 1][cell.x]);
        }

        // Check if 'right' is within bounds and add it to the vector
        if (cell.x < sideLength - 1) {
            surroundingCells.push_back(cells[cell.y][cell.x + 1]);
        }

        // Check if 'down' is within bounds and add it to the vector
        if (cell.y < sideLength - 1) {
            surroundingCells.push_back(cells[cell.y + 1][cell.x]);
        }

        // Check if 'left' is within bounds and add it to the vector
        if (cell.x > 0) {
            surroundingCells.push_back(cells[cell.y][cell.x - 1]);
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
        Cell& prevCell = cells[0][0]; 
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
                // If it gets stuck start the path from a random path cell
                currCell.state = 0;
                vector<Cell> pathsList = getAllPaths();
                Cell newStartCell = pathsList[rand() % pathsList.size()];
                prevCell = currCell;
                currCell = newStartCell;
            }
            // printMaze();
        }
    }
};


void generateMaze(int sideLength) {
    Maze maze;
    maze.sideLength = globalSize;
    maze.populateMaze();
    maze.printMaze();
    maze.pickStart();
    maze.printMaze();
    maze.carvePath(120);
    maze.printMaze();
}


int main() {
    srand(static_cast<unsigned int>(time(0)));
    generateMaze(globalSize);
}