#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "API.h"

#define MAZE_SIZE 16
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

// Current position and direction
int currentX = 0;
int currentY = 0;
int direction = NORTH;

// Maze mapping arrays
bool visited[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallNorth[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallEast[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallSouth[MAZE_SIZE][MAZE_SIZE] = {false};
bool wallWest[MAZE_SIZE][MAZE_SIZE] = {false};
int visitCount[MAZE_SIZE][MAZE_SIZE] = {0};  // Track how many times we've visited each cell

void logg(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

// Update wall information at current position
void updateWalls() {
    bool front = API_wallFront();
    bool right = API_wallRight();
    bool left = API_wallLeft();
    
    switch(direction) {
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
        switch(direction) {
            case NORTH: currentY++; break;
            case EAST:  currentX++; break;
            case SOUTH: currentY--; break;
            case WEST:  currentX--; break;
        }
        visitCount[currentX][currentY]++;
        visited[currentX][currentY] = true;
        
        // Color cells based on visit count
        if (visitCount[currentX][currentY] == 1) {
            API_setColor(currentX, currentY, 'G');  // First visit: Green
        } else if (visitCount[currentX][currentY] == 2) {
            API_setColor(currentX, currentY, 'Y');  // Second visit: Yellow
        } else if (visitCount[currentX][currentY] > 2) {
            API_setColor(currentX, currentY, 'R');  // Multiple visits: Red
        }
        return true;
    }
    return false;
}

// Check if a cell has unvisited neighbors with weighted preference
bool hasUnexploredNeighbors() {
    // Check in order of preference: forward, left, right, back
    int checkX, checkY;
    
    // Forward
    switch(direction) {
        case NORTH: checkX = currentX; checkY = currentY + 1; break;
        case EAST:  checkX = currentX + 1; checkY = currentY; break;
        case SOUTH: checkX = currentX; checkY = currentY - 1; break;
        case WEST:  checkX = currentX - 1; checkY = currentY; break;
    }
    
    if (checkX >= 0 && checkX < MAZE_SIZE && checkY >= 0 && checkY < MAZE_SIZE) {
        if (!visited[checkX][checkY] && !API_wallFront()) return true;
    }
    
    return false;
}

// Choose the best direction based on visit counts
int chooseBestDirection() {
    int minVisits = 999999;
    int bestDir = -1;
    
    // Check each possible direction
    if (!API_wallFront()) {
        int frontX = currentX, frontY = currentY;
        switch(direction) {
            case NORTH: frontY++; break;
            case EAST:  frontX++; break;
            case SOUTH: frontY--; break;
            case WEST:  frontX--; break;
        }
        if (frontX >= 0 && frontX < MAZE_SIZE && frontY >= 0 && frontY < MAZE_SIZE) {
            if (visitCount[frontX][frontY] < minVisits) {
                minVisits = visitCount[frontX][frontY];
                bestDir = direction;
            }
        }
    }
    
    if (!API_wallLeft()) {
        int leftX = currentX, leftY = currentY;
        switch((direction + 3) % 4) {
            case NORTH: leftY++; break;
            case EAST:  leftX++; break;
            case SOUTH: leftY--; break;
            case WEST:  leftX--; break;
        }
        if (leftX >= 0 && leftX < MAZE_SIZE && leftY >= 0 && leftY < MAZE_SIZE) {
            if (visitCount[leftX][leftY] < minVisits) {
                minVisits = visitCount[leftX][leftY];
                bestDir = (direction + 3) % 4;
            }
        }
    }
    
    if (!API_wallRight()) {
        int rightX = currentX, rightY = currentY;
        switch((direction + 1) % 4) {
            case NORTH: rightY++; break;
            case EAST:  rightX++; break;
            case SOUTH: rightY--; break;
            case WEST:  rightX--; break;
        }
        if (rightX >= 0 && rightX < MAZE_SIZE && rightY >= 0 && rightY < MAZE_SIZE) {
            if (visitCount[rightX][rightY] < minVisits) {
                minVisits = visitCount[rightX][rightY];
                bestDir = (direction + 1) % 4;
            }
        }
    }
    
    return bestDir;
}

int main(int argc, char* argv[]) {
    logg("Micromouse starting with improved exploration...");
    visited[0][0] = true;
    visitCount[0][0] = 1;
    API_setColor(0, 0, 'G');
    
    while (1) {
        if (API_wasReset()) {
            currentX = 0;
            currentY = 0;
            direction = NORTH;
            memset(visited, false, sizeof(visited));
            memset(wallNorth, false, sizeof(wallNorth));
            memset(wallEast, false, sizeof(wallEast));
            memset(wallSouth, false, sizeof(wallSouth));
            memset(wallWest, false, sizeof(wallWest));
            memset(visitCount, 0, sizeof(visitCount));
            visited[0][0] = true;
            visitCount[0][0] = 1;
            API_ackReset();
            API_setColor(0, 0, 'G');
            logg("Reset detected - restarting...");
            continue;
        }

        updateWalls();
        
        // First priority: unexplored cells
        if (hasUnexploredNeighbors()) {
            moveForward();
            char posStr[50];
            sprintf(posStr, "Position: (%d, %d), Direction: %d", currentX, currentY, direction);
            logg(posStr);
            continue;
        }
        
        // Second priority: least visited adjacent cell
        int bestDir = chooseBestDirection();
        if (bestDir != -1) {
            // Turn to the best direction
            while (direction != bestDir) {
                turnRight();
            }
            moveForward();
        } else {
            // If no good direction, try left wall following as fallback
            if (!API_wallLeft()) {
                turnLeft();
                if (moveForward()) continue;
            }
            while (API_wallFront()) {
                turnRight();
            }
            moveForward();
        }
        
        char posStr[50];
        sprintf(posStr, "Position: (%d, %d), Direction: %d", currentX, currentY, direction);
        logg(posStr);
    }
    
    return 0;
}