#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "API.h"


// Global variables (matching the Python version)
int x = 0;
int y = 0;
int orient = 0;

// Initialize cells array
int cells[16][16] = {0};

// Initialize flood array
int flood[16][16] = {
    {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14},
    {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},
    {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},
    {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},
    {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},
    {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},
    {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},
    {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},
    {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},
    {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},
    {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},
    {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},
    {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},
    {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},
    {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},
    {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14}
};

// Function declarations
void log_message(const char* string) {
    fprintf(stderr, "%s\n", string);
}

void updateWalls(int x, int y, int orient, bool L, bool R, bool F) {
    if (L && R && F) {
        if (orient == 0) cells[y][x] = 13;
        else if (orient == 1) cells[y][x] = 12;
        else if (orient == 2) cells[y][x] = 11;
        else if (orient == 3) cells[y][x] = 14;
    }
    else if (L && R && !F) {
        if (orient == 0 || orient == 2) cells[y][x] = 9;
        else if (orient == 1 || orient == 3) cells[y][x] = 10;
    }
    else if (L && F && !R) {
        if (orient == 0) cells[y][x] = 8;
        else if (orient == 1) cells[y][x] = 7;
        else if (orient == 2) cells[y][x] = 6;
        else if (orient == 3) cells[y][x] = 5;
    }
    else if (R && F && !L) {
        if (orient == 0) cells[y][x] = 7;
        else if (orient == 1) cells[y][x] = 6;
        else if (orient == 2) cells[y][x] = 5;
        else if (orient == 3) cells[y][x] = 8;
    }
    else if (F) {
        if (orient == 0) cells[y][x] = 2;
        else if (orient == 1) cells[y][x] = 3;
        else if (orient == 2) cells[y][x] = 4;
        else if (orient == 3) cells[y][x] = 1;
    }
    else if (L) {
        if (orient == 0) cells[y][x] = 1;
        else if (orient == 1) cells[y][x] = 2;
        else if (orient == 2) cells[y][x] = 3;
        else if (orient == 3) cells[y][x] = 4;
    }
    else if (R) {
        if (orient == 0) cells[y][x] = 3;
        else if (orient == 1) cells[y][x] = 4;
        else if (orient == 2) cells[y][x] = 1;
        else if (orient == 3) cells[y][x] = 2;
    }
}

bool isAccessible(int x, int y, int x1, int y1) {
    if (x == x1) {
        if (y > y1) {
            if (cells[y][x] == 4 || cells[y][x] == 5 || cells[y][x] == 6 || 
                cells[y][x] == 10 || cells[y][x] == 11 || cells[y][x] == 12 || cells[y][x] == 14) {
                return false;
            }
            return true;
        }
        else {
            if (cells[y][x] == 2 || cells[y][x] == 7 || cells[y][x] == 8 || 
                cells[y][x] == 10 || cells[y][x] == 12 || cells[y][x] == 13 || cells[y][x] == 14) {
                return false;
            }
            return true;
        }
    }
    else if (y == y1) {
        if (x > x1) {
            if (cells[y][x] == 1 || cells[y][x] == 5 || cells[y][x] == 8 || 
                cells[y][x] == 9 || cells[y][x] == 11 || cells[y][x] == 13 || cells[y][x] == 14) {
                return false;
            }
            return true;
        }
        else {
            if (cells[y][x] == 3 || cells[y][x] == 6 || cells[y][x] == 7 || 
                cells[y][x] == 9 || cells[y][x] == 11 || cells[y][x] == 12 || cells[y][x] == 13) {
                return false;
            }
            return true;
        }
    }
    return false;
}

void getSurrounds(int x, int y, int* surrounds) {
    surrounds[0] = x;        // x0
    surrounds[1] = y + 1;    // y0
    surrounds[2] = x + 1;    // x1
    surrounds[3] = y;        // y1
    surrounds[4] = x;        // x2
    surrounds[5] = y - 1;    // y2
    surrounds[6] = x - 1;    // x3
    surrounds[7] = y;        // y3
    
    if (surrounds[2] >= 16) surrounds[2] = -1;
    if (surrounds[1] >= 16) surrounds[1] = -1;
}

bool isConsistant(int x, int y) {
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    int val = flood[y][x];
    int minVals[4] = {-1, -1, -1, -1};
    int minCount = 0;

    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (x_curr >= 0 && y_curr >= 0) {
            if (isAccessible(x, y, x_curr, y_curr)) {
                minVals[i] = flood[y_curr][x_curr];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (minVals[i] == -1) continue;
        else if (minVals[i] == val + 1) continue;
        else if (minVals[i] == val - 1) minCount++;
    }

    return minCount > 0;
}

void makeConsistant(int x, int y) {
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    
    int minVals[4] = {1000, 1000, 1000, 1000};
    
    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (x_curr >= 0 && y_curr >= 0) {
            if (isAccessible(x, y, x_curr, y_curr)) {
                minVals[i] = flood[y_curr][x_curr];
            }
        }
    }

    int minVal = minVals[0];
    for (int i = 1; i < 4; i++) {
        if (minVals[i] < minVal) {
            minVal = minVals[i];
        }
    }
    
    flood[y][x] = minVal + 1;
}

// Stack implementation for floodFill
#define MAX_STACK_SIZE 1024
struct Stack {
    int data[MAX_STACK_SIZE];
    int top;
};

void initStack(struct Stack* stack) {
    stack->top = -1;
}

void push(struct Stack* stack, int value) {
    if (stack->top < MAX_STACK_SIZE - 1) {
        stack->data[++stack->top] = value;
    }
}

int pop(struct Stack* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return -1;
}

bool isEmpty(struct Stack* stack) {
    return stack->top == -1;
}

void floodFill(int x, int y, int xprev, int yprev) {
    if (!isConsistant(x, y)) {
        flood[y][x] = flood[yprev][xprev] + 1;
    }
    
    struct Stack stack;
    initStack(&stack);
    
    push(&stack, x);
    push(&stack, y);
    
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    
    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (x_curr >= 0 && y_curr >= 0) {
            if (isAccessible(x, y, x_curr, y_curr)) {
                push(&stack, x_curr);
                push(&stack, y_curr);
            }
        }
    }
    
    while (!isEmpty(&stack)) {
        int yrun = pop(&stack);
        int xrun = pop(&stack);
        
        if (isConsistant(xrun, yrun)) {
            continue;
        }
        
        makeConsistant(xrun, yrun);
        push(&stack, xrun);
        push(&stack, yrun);
        
        getSurrounds(xrun, yrun, surrounds);
        for (int i = 0; i < 4; i++) {
            int x_curr = surrounds[i*2];
            int y_curr = surrounds[i*2 + 1];
            
            if (x_curr >= 0 && y_curr >= 0) {
                if (isAccessible(xrun, yrun, x_curr, y_curr)) {
                    push(&stack, x_curr);
                    push(&stack, y_curr);
                }
            }
        }
    }
}

char toMove(int x, int y, int xprev, int yprev, int orient) {
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    int val = flood[y][x];
    int prev = 0;
    int minVals[4] = {1000, 1000, 1000, 1000};
    
    int noMovements = 0;
    
    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (isAccessible(x, y, x_curr, y_curr)) {
            if (x_curr == xprev && y_curr == yprev) {
                prev = i;
            }
            minVals[i] = flood[y_curr][x_curr];
            if (minVals[i] != 1000) noMovements++;
        }
    }
    
    int minVal = minVals[0];
    int minCell = 0;
    
    for (int i = 0; i < 4; i++) {
        if (minVals[i] < minVal) {
            if (noMovements == 1) {
                minVal = minVals[i];
                minCell = i;
            }
            else {
                if (i == prev) continue;
                minVal = minVals[i];
                minCell = i;
            }
        }
    }
    
    if (minCell == orient) return 'F';
    else if ((minCell == orient - 1) || (minCell == orient + 3)) return 'L';
    else if ((minCell == orient + 1) || (minCell == orient - 3)) return 'R';
    else return 'B';
}

void showFlood(int xrun, int yrun) {
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            // Assuming API.setText is implemented elsewhere
            // API.setText(x, y, flood[y][x]);
            printf("%d ", flood[y][x]);
        }
        printf("\n");
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Global variables (matching the Python version)
int x = 0;
int y = 0;
int orient = 0;

// Initialize cells array
int cells[16][16] = {0};

// Initialize flood array
int flood[16][16] = {
    {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14},
    {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},
    {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},
    {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},
    {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},
    {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},
    {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},
    {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},
    {7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7},
    {8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8},
    {9,8,7,6,5,4,3,2,2,3,4,5,6,7,8,9},
    {10,9,8,7,6,5,4,3,3,4,5,6,7,8,9,10},
    {11,10,9,8,7,6,5,4,4,5,6,7,8,9,10,11},
    {12,11,10,9,8,7,6,5,5,6,7,8,9,10,11,12},
    {13,12,11,10,9,8,7,6,6,7,8,9,10,11,12,13},
    {14,13,12,11,10,9,8,7,7,8,9,10,11,12,13,14}
};

// Function declarations
void log_message(const char* string) {
    fprintf(stderr, "%s\n", string);
}

void updateWalls(int x, int y, int orient, bool L, bool R, bool F) {
    if (L && R && F) {
        if (orient == 0) cells[y][x] = 13;
        else if (orient == 1) cells[y][x] = 12;
        else if (orient == 2) cells[y][x] = 11;
        else if (orient == 3) cells[y][x] = 14;
    }
    else if (L && R && !F) {
        if (orient == 0 || orient == 2) cells[y][x] = 9;
        else if (orient == 1 || orient == 3) cells[y][x] = 10;
    }
    else if (L && F && !R) {
        if (orient == 0) cells[y][x] = 8;
        else if (orient == 1) cells[y][x] = 7;
        else if (orient == 2) cells[y][x] = 6;
        else if (orient == 3) cells[y][x] = 5;
    }
    else if (R && F && !L) {
        if (orient == 0) cells[y][x] = 7;
        else if (orient == 1) cells[y][x] = 6;
        else if (orient == 2) cells[y][x] = 5;
        else if (orient == 3) cells[y][x] = 8;
    }
    else if (F) {
        if (orient == 0) cells[y][x] = 2;
        else if (orient == 1) cells[y][x] = 3;
        else if (orient == 2) cells[y][x] = 4;
        else if (orient == 3) cells[y][x] = 1;
    }
    else if (L) {
        if (orient == 0) cells[y][x] = 1;
        else if (orient == 1) cells[y][x] = 2;
        else if (orient == 2) cells[y][x] = 3;
        else if (orient == 3) cells[y][x] = 4;
    }
    else if (R) {
        if (orient == 0) cells[y][x] = 3;
        else if (orient == 1) cells[y][x] = 4;
        else if (orient == 2) cells[y][x] = 1;
        else if (orient == 3) cells[y][x] = 2;
    }
}

bool isAccessible(int x, int y, int x1, int y1) {
    if (x == x1) {
        if (y > y1) {
            if (cells[y][x] == 4 || cells[y][x] == 5 || cells[y][x] == 6 || 
                cells[y][x] == 10 || cells[y][x] == 11 || cells[y][x] == 12 || cells[y][x] == 14) {
                return false;
            }
            return true;
        }
        else {
            if (cells[y][x] == 2 || cells[y][x] == 7 || cells[y][x] == 8 || 
                cells[y][x] == 10 || cells[y][x] == 12 || cells[y][x] == 13 || cells[y][x] == 14) {
                return false;
            }
            return true;
        }
    }
    else if (y == y1) {
        if (x > x1) {
            if (cells[y][x] == 1 || cells[y][x] == 5 || cells[y][x] == 8 || 
                cells[y][x] == 9 || cells[y][x] == 11 || cells[y][x] == 13 || cells[y][x] == 14) {
                return false;
            }
            return true;
        }
        else {
            if (cells[y][x] == 3 || cells[y][x] == 6 || cells[y][x] == 7 || 
                cells[y][x] == 9 || cells[y][x] == 11 || cells[y][x] == 12 || cells[y][x] == 13) {
                return false;
            }
            return true;
        }
    }
    return false;
}

void getSurrounds(int x, int y, int* surrounds) {
    surrounds[0] = x;        // x0
    surrounds[1] = y + 1;    // y0
    surrounds[2] = x + 1;    // x1
    surrounds[3] = y;        // y1
    surrounds[4] = x;        // x2
    surrounds[5] = y - 1;    // y2
    surrounds[6] = x - 1;    // x3
    surrounds[7] = y;        // y3
    
    if (surrounds[2] >= 16) surrounds[2] = -1;
    if (surrounds[1] >= 16) surrounds[1] = -1;
}

bool isConsistant(int x, int y) {
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    int val = flood[y][x];
    int minVals[4] = {-1, -1, -1, -1};
    int minCount = 0;

    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (x_curr >= 0 && y_curr >= 0) {
            if (isAccessible(x, y, x_curr, y_curr)) {
                minVals[i] = flood[y_curr][x_curr];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (minVals[i] == -1) continue;
        else if (minVals[i] == val + 1) continue;
        else if (minVals[i] == val - 1) minCount++;
    }

    return minCount > 0;
}

void makeConsistant(int x, int y) {
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    
    int minVals[4] = {1000, 1000, 1000, 1000};
    
    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (x_curr >= 0 && y_curr >= 0) {
            if (isAccessible(x, y, x_curr, y_curr)) {
                minVals[i] = flood[y_curr][x_curr];
            }
        }
    }

    int minVal = minVals[0];
    for (int i = 1; i < 4; i++) {
        if (minVals[i] < minVal) {
            minVal = minVals[i];
        }
    }
    
    flood[y][x] = minVal + 1;
}

// Stack implementation for floodFill
#define MAX_STACK_SIZE 1024
struct Stack {
    int data[MAX_STACK_SIZE];
    int top;
};

void initStack(struct Stack* stack) {
    stack->top = -1;
}

void push(struct Stack* stack, int value) {
    if (stack->top < MAX_STACK_SIZE - 1) {
        stack->data[++stack->top] = value;
    }
}

int pop(struct Stack* stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return -1;
}

bool isEmpty(struct Stack* stack) {
    return stack->top == -1;
}

void floodFill(int x, int y, int xprev, int yprev) {
    if (!isConsistant(x, y)) {
        flood[y][x] = flood[yprev][xprev] + 1;
    }
    
    struct Stack stack;
    initStack(&stack);
    
    push(&stack, x);
    push(&stack, y);
    
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    
    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (x_curr >= 0 && y_curr >= 0) {
            if (isAccessible(x, y, x_curr, y_curr)) {
                push(&stack, x_curr);
                push(&stack, y_curr);
            }
        }
    }
    
    while (!isEmpty(&stack)) {
        int yrun = pop(&stack);
        int xrun = pop(&stack);
        
        if (isConsistant(xrun, yrun)) {
            continue;
        }
        
        makeConsistant(xrun, yrun);
        push(&stack, xrun);
        push(&stack, yrun);
        
        getSurrounds(xrun, yrun, surrounds);
        for (int i = 0; i < 4; i++) {
            int x_curr = surrounds[i*2];
            int y_curr = surrounds[i*2 + 1];
            
            if (x_curr >= 0 && y_curr >= 0) {
                if (isAccessible(xrun, yrun, x_curr, y_curr)) {
                    push(&stack, x_curr);
                    push(&stack, y_curr);
                }
            }
        }
    }
}

char toMove(int x, int y, int xprev, int yprev, int orient) {
    int surrounds[8];
    getSurrounds(x, y, surrounds);
    int val = flood[y][x];
    int prev = 0;
    int minVals[4] = {1000, 1000, 1000, 1000};
    
    int noMovements = 0;
    
    for (int i = 0; i < 4; i++) {
        int x_curr = surrounds[i*2];
        int y_curr = surrounds[i*2 + 1];
        
        if (isAccessible(x, y, x_curr, y_curr)) {
            if (x_curr == xprev && y_curr == yprev) {
                prev = i;
            }
            minVals[i] = flood[y_curr][x_curr];
            if (minVals[i] != 1000) noMovements++;
        }
    }
    
    int minVal = minVals[0];
    int minCell = 0;
    
    for (int i = 0; i < 4; i++) {
        if (minVals[i] < minVal) {
            if (noMovements == 1) {
                minVal = minVals[i];
                minCell = i;
            }
            else {
                if (i == prev) continue;
                minVal = minVals[i];
                minCell = i;
            }
        }
    }
    
    if (minCell == orient) return 'F';
    else if ((minCell == orient - 1) || (minCell == orient + 3)) return 'L';
    else if ((minCell == orient + 1) || (minCell == orient - 3)) return 'R';
    else return 'B';
}

void showFlood(int xrun, int yrun) {
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            // Assuming API.setText is implemented elsewhere
            // API.setText(x, y, flood[y][x]);
            printf("%d ", flood[y][x]);
        }
        printf("\n");
    }
}

int main() {
    int x = 0;
    int y = 0;
    int xprev = 0;
    int yprev = 0;
    int orient = 0;
    
    while (1) {
        // Get wall information
        bool L = API_wallLeft();
        bool R = API_wallRight();
        bool F = API_wallFront();
        
        updateWalls(x, y, orient, L, R, F);
        
        if (flood[y][x] != 0) {
            floodFill(x, y, xprev, yprev);
            
            char direction = toMove(x, y, xprev, yprev, orient);
            
            if (direction == 'L') {
                API_turnLeft();
                orient = (orient + 3) % 4;  // Update orientation for left turn
            }
            else if (direction == 'R') {
                API_turnRight();
                orient = (orient + 1) % 4;  // Update orientation for right turn
            }
            else if (direction == 'B') {
                API_turnLeft();
                orient = (orient + 3) % 4;
                API_turnLeft();
                orient = (orient + 3) % 4;
            }
            
            log_message("moveForward");
            showFlood(x, y);
            API_moveForward();
            
            // Update previous and current positions
            xprev = x;
            yprev = y;
            
            // Update current coordinates based on orientation
            switch (orient) {
                case 0:  // North
                    y++;
                    break;
                case 1:  // East
                    x++;
                    break;
                case 2:  // South
                    y--;
                    break;
                case 3:  // West
                    x--;
                    break;
            }
        }
        else {
            // We've reached the center
            while (1) {
                // Infinite loop when goal is reached
                // You might want to add some completion behavior here
            }
        }
    }
    
    return 0;
}