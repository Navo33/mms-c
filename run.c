#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "API.h"  // Assuming this is the provided API for controlling the mouse

#define MAZE_SIZE 16

// Directions: 0 - North, 1 - East, 2 - South, 3 - West
const int dx[4] = {0, 1, 0, -1};
const int dy[4] = {1, 0, -1, 0};

int currentX = 0;
int currentY = 0;
int direction = 0;  // Start facing North

void logg(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void moveTo(int x, int y) {
    // Move the mouse to (x, y) from the current position
    while (currentX != x || currentY != y) {
        // Determine the direction to move
        if (x > currentX) {
            // Need to move east (right)
            while (direction != 1) {
                API_turnRight();
                direction = (direction + 1) % 4;
            }
        } else if (x < currentX) {
            // Need to move west (left)
            while (direction != 3) {
                API_turnRight();
                direction = (direction + 1) % 4;
            }
        } else if (y > currentY) {
            // Need to move north (up)
            while (direction != 0) {
                API_turnRight();
                direction = (direction + 1) % 4;
            }
        } else if (y < currentY) {
            // Need to move south (down)
            while (direction != 2) {
                API_turnRight();
                direction = (direction + 1) % 4;
            }
        }

        // Move forward once we have the correct direction
        API_moveForward();
        currentX = x;
        currentY = y;
    }
}

void executePath(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (!file) {
        fprintf(stderr, "Failed to open path file.\n");
        return;
    }

    int x, y;
    while (fscanf(file, "(%d, %d)\n", &x, &y) != EOF) {
        API_setColor(x, y, 'R');
        logg("Moving to the next position...");
        moveTo(x, y);  // Move to the specified (x, y) position
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    logg("Executing path...");
    executePath("path.txt");  // Path is loaded from path.txt and executed
    logg("Path execution complete.");
    return 0;
}
