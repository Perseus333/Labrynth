#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <array>

using namespace std;
using namespace std::chrono;

// Edit these values
const bool debugMode = false;
const int globalSize = 100;


class Maze {
public:
    static const int sideLength = globalSize;
    static const int mazeSize = sideLength * sideLength;
    bool cells[mazeSize]; // true=path; false=wall;
    int startPos;

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


void generateMaze() {
    srand(static_cast<unsigned int>(time(0)));
    Maze maze;
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
