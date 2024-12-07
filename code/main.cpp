#include <iostream>
#include <random>
#include <chrono>
#include <string>

#include "algorithms/oneDimMaze.h"
// #include "algorithms/pathListOpt.h"

using namespace std;
using namespace std::chrono;

void chooseAlgorithm() {
    string options[2] = {"oneDimMaze", "pathListOpt"};
    cout << "Choose one of the following algorithms: \n";
    for (int i = 0; i < options.size(); ++i) {
        cout << "(" << i << "): " << options[i];
    }
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

void benchmarkMaze(int repetitions, int size) {
    oneDimMaze::Maze maze(size);
    long long totalTime = 0;

    for (int i = 0; i < repetitions; i++) {
        auto start = high_resolution_clock::now();
        oneDimMaze::runMaze(maze, false);
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

    chooseAlgorithm();

    int globalSize = 100;
    cout << "Enter Maze length: ";
    cin >> globalSize;

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
        benchmarkMaze(repetitions, globalSize);
    }
    else {
        oneDimMaze::Maze maze(globalSize);
        oneDimMaze::runMaze(maze, true);
    }
}