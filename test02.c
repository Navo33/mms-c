#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "API.h"

#define MAZE_SIZE 16
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

const int dx[4] = {0, 1, 0, -1};
const int dy[4] = {1, 0, -1, 0};

int currentX = 0;
int currentY = 0;
int direction = NORTH;

bool visited[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallNorth[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallEast[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallSouth[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallWest[MAZE_SIZE][MAZE_SIZE] = {false};

typedef struct {
    int x, y;
} Cell;

Cell stack[MAZE_SIZE * MAZE_SIZE];
int stackTop = -1;

FILE* pathFile;

void logg(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void push(int x, int y) {
    stack[++stackTop] = (Cell){x, y};
}

Cell pop() {
    return stack[stackTop--];
}

Cell peek() {
    return stack[stackTop];
}

bool isStackEmpty() {
    return stackTop == -1;
}

void updateWalls() {
    bool front = API_wallFront();
    bool right = API_wallRight();
    bool left = API_wallLeft();

    switch (direction) {
        case NORTH:
            wallNorth[currentX][currentY] = front;
            wallEast[currentX][currentY] = right;
            wallWest[currentX][currentY] = left;
            break;
        case EAST:
            wallEast[currentX][currentY] = front;
            wallSouth[currentX][currentY] = right;
            wallNorth[currentX][currentY] = left;
            break;
        case SOUTH:
            wallSouth[currentX][currentY] = front;
            wallWest[currentX][currentY] = right;
            wallEast[currentX][currentY] = left;
            break;
        case WEST:
            wallWest[currentX][currentY] = front;
            wallNorth[currentX][currentY] = right;
            wallSouth[currentX][currentY] = left;
            break;
    }
}

void turnRight() {
    API_turnRight();
    direction = (direction + 1) % 4;
}

void turnLeft() {
    API_turnLeft();
    direction = (direction + 3) % 4;
}

bool moveForward() {
    if (!API_wallFront()) {
        API_moveForward();
        currentX += dx[direction];
        currentY += dy[direction];
        visited[currentX][currentY] = true;
        API_setColor(currentX, currentY, 'G');
        return true;
    }
    return false;
}

bool isCenter(int x, int y) {
    return (x == MAZE_SIZE / 2 || x == MAZE_SIZE / 2 - 1) &&
           (y == MAZE_SIZE / 2 || y == MAZE_SIZE / 2 - 1);
}

void savePath() {
    if (stackTop >= 0) {
        // Open the file for appending new path data
        freopen("path.txt", "w", pathFile); // Overwrite the file
        for (int i = 0; i <= stackTop; i++) {
            Cell cell = stack[i];
            fprintf(pathFile, "(%d, %d)\n", cell.x, cell.y);
        }
        fflush(pathFile);
    }
}

bool hasUnvisitedNeighbors(int x, int y) {
    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
            if (!visited[nx][ny] &&
                ((d == NORTH && !wallNorth[x][y]) ||
                 (d == EAST && !wallEast[x][y]) ||
                 (d == SOUTH && !wallSouth[x][y]) ||
                 (d == WEST && !wallWest[x][y]))) {
                return true;
            }
        }
    }
    return false;
}

void dfsExplore() {
    push(currentX, currentY);
    visited[currentX][currentY] = true;
    API_setColor(currentX, currentY, 'G');

    while (!isStackEmpty()) {
        Cell top = peek();
        currentX = top.x;
        currentY = top.y;

        updateWalls();

        if (isCenter(currentX, currentY)) {
            logg("Arrived at center...");
            savePath(); // Save the path when center is reached
            break; // Exit the loop once the center is found
        }

        if (hasUnvisitedNeighbors(currentX, currentY)) {
            for (int d = 0; d < 4; d++) {
                int nx = currentX + dx[d];
                int ny = currentY + dy[d];

                if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE &&
                    !visited[nx][ny] &&
                    ((d == NORTH && !wallNorth[currentX][currentY]) ||
                     (d == EAST && !wallEast[currentX][currentY]) ||
                     (d == SOUTH && !wallSouth[currentX][currentY]) ||
                     (d == WEST && !wallWest[currentX][currentY]))) {
                    while (direction != d) {
                        turnRight();
                    }
                    if (moveForward()) {
                        push(currentX, currentY);
                        break;
                    }
                }
            }
        } else {
            pop(); // Backtrack by popping from the stack
            if (!isStackEmpty()) {
                Cell prev = peek();
                int backDir = -1;

                if (prev.x == currentX - 1 && prev.y == currentY) backDir = WEST;
                else if (prev.x == currentX + 1 && prev.y == currentY) backDir = EAST;
                else if (prev.y == currentY - 1 && prev.x == currentX) backDir = SOUTH;
                else if (prev.y == currentY + 1 && prev.x == currentX) backDir = NORTH;

                while (direction != backDir) {
                    turnRight();
                }
                moveForward();
            }
        }
    }
}

int main(int argc, char* argv[]) {
    pathFile = fopen("path.txt", "w");
    if (!pathFile) {
        logg("Failed to open path file.");
        return 1;
    }

    logg("Micromouse DFS starting...");
    memset(visited, false, sizeof(visited));
    dfsExplore();

    fclose(pathFile);
    logg("DFS exploration complete.");

    return 0;
}
