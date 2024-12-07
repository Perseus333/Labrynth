#ifndef oneDimMaze
#define oneDimMaze

#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <array>

using namespace std;
using namespace std::chrono;
const bool debugMode = false;

namespace oneDimMaze {
    class Maze {
    public:
        int sideLength;
        int mazeSize = sideLength * sideLength;
        bool* cells; // true=path; false=wall;
        int startPos;

        Maze(int size) : sideLength(size), mazeSize(size * size) {
            cells = new bool[mazeSize]; // Allocate memory dynamically
        }

        ~Maze() {
            delete[] cells; // Free memory when the object is destroyed
        }

        void printMaze() {
            for (int pos = 0; pos < mazeSize; ++pos) {
                // Detects new line
                if (pos % sideLength == 0) {
                    cout << "\n";
                }
                if (cells[pos] == true) {
                    if (pos < sideLength) {
                        // Start
                        cout << "🟦";
                    }
                    else if (pos > (mazeSize - sideLength)) {
                        // Goal
                        cout << "🟨";
                    }
                    else {
                        cout << "⬜";
                    }
                }
                else {
                    cout << "⬛";
                }
            }
            cout << endl;
        }

        void pickStart() {
            // The starting cell at y=1
            int start = ((rand() % (sideLength-2))+1)+sideLength;
            // The cell at y=0
            cells[start-sideLength] = true;
            // Two cells are picked at start because 
            // the cell at y=0 can only move to the cell below it
            cells[start+sideLength] = true;
            startPos = start;
        }

        void carvePath() {
            // Set all the cells as walls
            for (int i = 0; i < mazeSize; ++i) {
                cells[i] = false;
            }
            vector<int>pathList = {};
            int cell = startPos;

            int directions[4] = {
                -2*sideLength + 0,   // nn (north)
                0*sideLength + 2,   // ee (east)
                2*sideLength + 0,   // ss (south)
                0*sideLength - 2    // ww (west)
            };

            // Main loop
            while (true) {
                if (debugMode) printMaze();
                // Checks all four directions
                vector<int>validNeighbors = {};

                for (const int direction : directions) {
                    int newPos = cell + direction;
                    int newX = newPos % sideLength;
                    int newY = newPos / sideLength;
                    // Check if it's within bounds of the maze and if it's not already a path
                    if ((newX >= 1 && newX < sideLength - 1 && newY >= 1 && newY < sideLength - 1) && !cells[newPos]) {
                        validNeighbors.push_back(newPos);
                    }
                }
                if (validNeighbors.empty()) {
                    if (!pathList.empty()) {
                        pathList.pop_back();
                        cell = pathList.back();
                    }
                    else {
                        break;
                    }
                }
                else {
                    int randomIndex = rand() % validNeighbors.size();
                    int newPos = validNeighbors[randomIndex];
                    
                    int newY = newPos / sideLength;
                    int newX = newPos % sideLength;
                    int cellY = cell / sideLength;
                    int cellX = cell % sideLength;
                    
                    cells[(((cellY + newY) / 2)*sideLength) + ((cellX + newX) / 2)] = true;
                    cells[(newY*sideLength)+newX] = true;
                
                    cell = newPos;
                    pathList.push_back(newPos);
                }
            }
        }

        void linkGoal() {
            int goalConnectionPoint;
            bool connection = false;
            for (int pos = mazeSize-1; pos > 0; --pos) {
                if (cells[pos] == true) {
                    goalConnectionPoint = pos;
                    connection = true;
                    break;
                }
                if (connection) break;
            }

            for (int linkPos = goalConnectionPoint; linkPos < mazeSize; linkPos += sideLength) {
                cells[linkPos] = true;
            }
        }
    };

    void runMaze(Maze& maze, bool print) {
        maze.pickStart();
        maze.carvePath();
        maze.linkGoal();
        if (print) {
            maze.printMaze();
        }
    }
}

#endif //oneDimMaze
