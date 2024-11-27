# Four Wheels Game - Pathfinding and Visualization

This code is a graphical application that visualizes the A* pathfinding algorithm applied to a puzzle game. The goal of the game is to transform an initial state of four digits (representing four wheels) into a target state, while avoiding forbidden states. The application uses **SFML** for rendering and provides interactive user input for configuring the puzzle and viewing the pathfinding process.

## Overview

The application allows the user to:
- Input an initial state of the puzzle (4 digits, e.g., `1 2 3 4`).
- Input a target state (4 digits, e.g., `5 6 7 8`).
- Specify forbidden states that should be avoided during the search process.
- Visualize the A* algorithm solving the puzzle, with the pathfinding process shown step by step.

The program includes the following major components:
1. **Graphical User Interface (GUI)**: Built with SFML for rendering the input interface, displaying logs, and visualizing the algorithm's progress.
2. **A* Algorithm**: Implements the A* search algorithm to find the shortest path between the initial and target states, avoiding forbidden states.
3. **Interactive Input**: Allows users to enter the puzzle state and forbidden states, and view the log of steps and the solution path.

## Algorithm Overview

### A* Search Algorithm

The application uses the **A* (A-star) algorithm** to find the optimal path from the initial state to the target state. The key features of the A* algorithm in this context are:
- **State Representation**: The puzzle is represented as a 4-digit state, where each digit represents a "wheel" in the puzzle.
- **Heuristic**: The heuristic used in this implementation is the difference between the current state and the target state, evaluated with a custom function that measures how close the current state is to the target.
- **Forbidden States**: Certain states are marked as forbidden and should be avoided by the algorithm.
- **Priority Queue**: A priority queue (using a `std::vector`) is used to store and retrieve states in the order of their priority (lowest cost, based on the heuristic function).

### Steps of the Algorithm

1. **Initialization**: The user inputs the initial state, target state, and a list of forbidden states.
2. **Pathfinding**: The A* algorithm is executed to find the optimal path from the initial state to the target state, avoiding the forbidden states.
3. **Visualization**: The states explored during the pathfinding process are visualized step by step, with each state being displayed as a set of "wheels" in the graphical window.

## Libraries Used

This project utilizes the following libraries:

### SFML (Simple and Fast Multimedia Library)
SFML is used for rendering the graphical interface and handling user input events. It allows us to display windows, render shapes (such as rectangles for the input box and wheels), and handle text input/output.

- **Installation**:
    - On Linux: `sudo apt install libsfml-dev`
    - On Windows: Follow the installation instructions on [SFML's website](https://www.sfml-dev.org/download.php).

## Setup and Usage

### Prerequisites

- A C++ compiler that supports C++11 or higher.
- SFML installed on your system.

### Usage

```shell
make
```

```shell
make clean
```






## Example
```bash
PROBLEM
In this problem we will be considering a game played with four wheels. Digits ranging from 0 to 9
are printed consecutively (clockwise) on the periphery of each wheel. The topmost digits of the wheels
form a four-digit integer. For example, in the following figure the wheels form the integer 8056. Each
wheel has two buttons associated with it. Pressing the button marked with a left arrow rotates the
wheel one digit in the clockwise direction and pressing the one marked with the right arrow rotates it
by one digit in the opposite direction.
The game starts with an initial configuration of the wheels. Say, in the initial configuration the
topmost digits form the integer S1S2S3S4. You will be given some (say, n) forbidden configurations
Fi1 Fi2 Fi3 Fi4
(1 ≤ i ≤ n) and a target configuration T1T2T3T4. Your job will be to write a program that
can calculate the minimum number of button presses required to transform the initial configuration to
the target configuration by never passing through a forbidden one.
Input
The first line of the input contains an integer N giving the number of test cases to follow.
The first line of each test case contains the initial configuration of the wheels specified by 4 digits.
Two consecutive digits are separated by a space. The next line contains the target configuration. The
third line contains an integer n giving the number of forbidden configurations. Each of the following n
lines contains a forbidden configuration. There is a blank line between two consecutive input sets.
Output
For each test case in the input print a line containing the minimum number of button presses required.
If the target configuration is not reachable then print ‘-1’.
Sample Input
2
8 0 5 6
6 5 0 8
5
8 0 5 7
8 0 4 7
5 5 0 8
7 5 0 8
6 4 0 8
0 0 0 0
5 3 1 7
8
0 0 0 1
0 0 0 9
0 0 1 0
0 0 9 0
0 1 0 0
0 9 0 0
1 0 0 0
9 0 0 0
Sample Output
14
-1
```