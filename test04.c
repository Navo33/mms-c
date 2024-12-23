#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>
#include "API.h"

#define SIZE 16
#define START_X 0
#define START_Y 0
#define LARGEVAL 301 

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Directions
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define BUFFER_SIZE 32

int getInteger(char* command) {
    printf("%s\n", command);
    fflush(stdout);
    char response[BUFFER_SIZE];
    fgets(response, BUFFER_SIZE, stdin);
    int value = atoi(response);
    return value;
}

int getBoolean(char* command) {
    printf("%s\n", command);
    fflush(stdout);
    char response[BUFFER_SIZE];
    fgets(response, BUFFER_SIZE, stdin);
    int value = (strcmp(response, "true\n") == 0);
    return value;
}

int getAck(char* command) {
    printf("%s\n", command);
    fflush(stdout);
    char response[BUFFER_SIZE];
    fgets(response, BUFFER_SIZE, stdin);
    int success = (strcmp(response, "ack\n") == 0);
    return success;
}

int API_mazeWidth() {
    return getInteger("mazeWidth");
}

int API_mazeHeight() {
    return getInteger("mazeHeight");
}

int API_wallFront() {
    return getBoolean("wallFront");
}

int API_wallRight() {
    return getBoolean("wallRight");
}

int API_wallLeft() {
    return getBoolean("wallLeft");
}

int API_moveForward() {
    return getAck("moveForward");
}

void API_turnRight() {
    getAck("turnRight");
}

void API_turnLeft() {
    getAck("turnLeft");
}

void API_setWall(int x, int y, char direction) {
    printf("setWall %d %d %c\n", x, y, direction);
    fflush(stdout);
}

void API_clearWall(int x, int y, char direction) {
    printf("clearWall %d %d %c\n", x, y, direction);
    fflush(stdout);
}

void API_setColor(int x, int y, char color) {
    printf("setColor %d %d %c\n", x, y, color);
    fflush(stdout);
}

void API_clearColor(int x, int y) {
    printf("clearColor %d %d\n", x, y);
    fflush(stdout);
}

void API_clearAllColor() {
    printf("clearAllColor\n");
    fflush(stdout);
}

void API_setText(int x, int y, char* text) {
    printf("setText %d %d %s\n", x, y, text);
    fflush(stdout);
}

void API_clearText(int x, int y) {
    printf("clearText %d %d\n", x, y);
    fflush(stdout);
}

void API_clearAllText() {
    printf("clearAllText\n");
    fflush(stdout);
}

int API_wasReset() {
    return getBoolean("wasReset");
}

void API_ackReset() {
    getAck("ackReset");
}

typedef struct Node {

    short floodval;
    short row;
    short col;
    short visited;

    struct Node * left;
    struct Node * right;
    struct Node * up;
    struct Node * down;

} Node ;

typedef struct Maze {

	Node * map [SIZE][SIZE];	

} Maze;

// Stack Constants
#define SPI 1			// Stack Pointer Index
#define SSI 0 			// Stack Size Index
#define STACK_OFFSET 2
#define STACKSIZE 80

typedef struct Stack {
    short properties[STACK_OFFSET];
    Node * theStack[STACKSIZE];
} Stack ;

// Create new node
Node * newNode(const short i, const short j){

    Node * thisNode = (Node *)malloc(sizeof(Node));
    short halfSize = SIZE / 2;

    thisNode->row = i;
    thisNode->col = j;

    if (i < halfSize && j < halfSize) {
		thisNode->floodval = (halfSize - 1 - i) + (halfSize - 1 - j) ;
    } else if (i < halfSize && j >= halfSize) {
		thisNode->floodval = (halfSize - 1 - i) + (j - halfSize) ;
    } else if (i >= halfSize && j < halfSize) {
		thisNode->floodval = (i - halfSize) + (halfSize - 1 - j) ;
    } else {
		thisNode->floodval = (i - halfSize) + (j - halfSize) ;
    }
    
	return thisNode;

}

// delete node
void deleteNode(Node ** npp) {
    free(*npp);
    *npp = 0;
}

// Create maze
Maze * newMaze(){
    
    Maze * thisMaze = (Maze *)malloc(sizeof(Maze));

    // Node creation for each cell in the maze
    for (short i = 0; i < SIZE; i++) {
        for (short j = 0; j < SIZE; i++) {
            thisMaze->map[i][j] = newNode(i,j);
        } 
    }

    // set neighbours
    for (short i = 0; i < SIZE; i++) {
        for (short j = 0; j < SIZE; i++) {
            thisMaze->map[i][j]->left = (j == 0) ? NULL : (thisMaze->map[i][j-1]);
			thisMaze->map[i][j]->right = (j == SIZE-1) ? NULL : (thisMaze->map[i][j+1]);
			thisMaze->map[i][j]->up = (i == 0) ? NULL : (thisMaze->map[i-1][j]);
			thisMaze->map[i][j]->down = (i == SIZE-1) ? NULL : (thisMaze->map[i+1][j]);
        } 
    }
    
    return thisMaze;
}

void delete_Maze (Maze ** mpp) {

	for (short i = 0; i < SIZE; i++) {
		for (short j = 0; j < SIZE; j++) {
			deleteNode (&((*mpp)->map[i][j])); 
        }
    }
	free(*mpp);
	*mpp = 0;
}

// Stack Constructor
Stack * newStack() {

	Stack * thisStack = (Stack *) malloc(sizeof(Stack));
	thisStack->properties[SPI] = 0;
	thisStack->properties[SSI] = STACKSIZE;
	return thisStack;
}

// Stack Destructor
void delete_Stack (Stack ** spp) {
	if (spp == 0 || *spp == 0) {
		fprintf(stderr, "NULL POINTER\n");
		return;
	}
	free(*spp);
	*spp = 0;

}


// Checks if thisStack is empty
int is_empty_Stack (Stack * thisStack) {

	if (thisStack->properties[SPI] == 0)
		return 1;
	else return 0;
}

// Pops the top element of thisStack
void pop (Stack * thisStack, Node ** npp) {

	short index;
	index = thisStack->properties[SPI] - 1;
	*npp = thisStack->theStack[index];
	thisStack->properties[SPI] -= 1;

}

// Pushes an element to the top of thisStack
void push (Stack * thisStack, Node * this_node) {

	short index;
	index = thisStack->properties[SPI];
	thisStack->theStack[index] = this_node;
	thisStack->properties[SPI] += 1;
}

short get_smallest_neighbor (Node * thisNode) {

	// The Node's floodval will be 1 higher than the neigboring cell
	short smallestneighbor = LARGEVAL;

	if (thisNode->left != NULL && (thisNode->left->right != NULL) && (thisNode->left->floodval) < smallestneighbor)
		smallestneighbor = thisNode->left->floodval;

	if (thisNode->right != NULL && (thisNode->right->left != NULL) && (thisNode->right->floodval) < smallestneighbor)
		smallestneighbor = thisNode->right->floodval;	

	if (thisNode->up != NULL && (thisNode->up->down != NULL) && (thisNode->up->floodval) < smallestneighbor)
		smallestneighbor = thisNode->up->floodval;

	if (thisNode->up != NULL && (thisNode->up->up != NULL) && (thisNode->up->floodval) < smallestneighbor)
		smallestneighbor = thisNode->up->floodval;

	return smallestneighbor;

}

short get_smallest_neighbor_dir (Node * thisNode, const short preferred_dir) {

	short smallestval;		// smallest neighbor value 
	short pathcount;			// number of available paths 
	
	//get the smallest neighboring flood_val
	smallestval = get_smallest_neighbor(thisNode);
	
	// clear pathcount
	pathcount = 0;

    // count the number of available paths
	if ((thisNode->up != NULL) && (thisNode->up->floodval == smallestval)) 
    	pathcount++;
    
  	if ((thisNode->right != NULL) && (thisNode->right->floodval == smallestval)) 
    	pathcount++;
  	
  	if ((thisNode->down != NULL) && (thisNode->down->floodval == smallestval)) 
    	pathcount++;
    
  	if ((thisNode->left != NULL) && (thisNode->left->floodval == smallestval)) 
    	pathcount++;

    switch (preferred_dir){

    	case NORTH: 
    		if ((thisNode->up != NULL) && (thisNode->up->floodval == smallestval))
    			return NORTH;
    		break;
    	case EAST: 
    		if ((thisNode->right != NULL) && (thisNode->right->floodval == smallestval))
    			return EAST;
    		break;
    	case SOUTH: 
			if ((thisNode->down != NULL) && (thisNode->down->floodval == smallestval))
				return SOUTH;
    		break;
    	case WEST:  
    		if ((thisNode->left != NULL) && (thisNode->left->floodval == smallestval))
    			return WEST;
    		break;
    }

    	if ((thisNode->up != NULL) && (thisNode->up->floodval == smallestval) && (thisNode->up->visited == FALSE)){
		   return NORTH;
        } else if ((thisNode->right != NULL) && (thisNode->right->floodval == smallestval) && (thisNode->right->visited == FALSE)) {
    		return EAST;
        } else if ((thisNode->down != NULL) && (thisNode->down->floodval == smallestval) && (thisNode->down->visited == FALSE)) {
    		return SOUTH;
        } else if ((thisNode->left != NULL) && (thisNode->left->floodval == smallestval) && (thisNode->left->visited == FALSE)) {
    		return WEST;
        }

		if ((thisNode->up != NULL) && (thisNode->up->floodval == smallestval)){
		   return NORTH;
        } else if ((thisNode->right != NULL) && (thisNode->right->floodval == smallestval)){ 
    		return EAST;
        } else if ((thisNode->down != NULL) && (thisNode->down->floodval == smallestval)){
    		return SOUTH;
        } else {
    		return WEST;
        }
}

short floodval_check(Node * thisNode) {

	//return a flag determining wheter this node should be updated / is this Node 1 + min open adj cell?
	if (get_smallest_neighbor (thisNode) + 1 == thisNode->floodval)
		return TRUE;

	return FALSE;
}

void update_floodval (Node * thisNode) {

	// set this node's value to 1 + min open adjascent cell //
	thisNode->floodval = get_smallest_neighbor (thisNode) + 1;

}

void push_open_neighbors (Node * thisNode, Stack * thisStack) {

	//A NULL neighbor represents a wall. if neighbor is accessible, push it onto stack!
	if (thisNode->left != NULL && thisNode->right != NULL) 
		push (thisStack, thisNode->left);

	if (thisNode->right != NULL && thisNode->right->left != NULL) 
		push (thisStack, thisNode->right);

	if (thisNode->up != NULL && thisNode->up->down != NULL) 
		push (thisStack, thisNode->up);

	if (thisNode->down != NULL && thisNode->down->up != NULL) 
		push (thisStack, thisNode->down);

}

void flood_fill (Node * thisNode, Stack * thisStack, const short refloodFlag) {

	short status;  /* Flag for updating the flood value or not */

	/* we want to avoid flooding the goal values - this is for non-reverse */
	if (!refloodFlag) 
		if (thisNode->row == SIZE / 2 || thisNode->row == SIZE / 2 - 1) 
    		if (thisNode->col == SIZE / 2 || thisNode->col == SIZE / 2 - 1) 
    			return;

    /* we want to avoid flooding the goal values - this is reverse */
	if (refloodFlag) 
		if (thisNode->row == START_X && thisNode->col == START_Y)
    		return;
	
	/* is the cell (1 + minumum OPEN adjascent cell) ? */
	status = floodval_check (thisNode);

	/* if no, update curent cell's flood values, 
	   then push open adjascent neighbors to stack. */
	if (!status) {
		update_floodval(thisNode); /* Update floodval to 1 + min open neighbor */
		push_open_neighbors(thisNode, thisStack); /* pushed, to be called later */
	}
	
}

void set_value (Node * thisNode, const short value) {
	thisNode->floodval = value;
}

void set_visited (Node * thisNode) {
	thisNode->visited = TRUE;
}

void set_wall (Node * thisNode, const short dir) {

	/* set a wall, of this node, of the specified direction 
	switch (dir) {

		case NORTH :
			if (thisNode->row != 0) {
				thisNode->up = NULL;
			} break;

		case SOUTH :
			if (thisNode->row != SIZE -1) {
				thisNode->down = NULL;
			} break; 

		case EAST : 
			if (thisNode->col != SIZE - 1) {
				thisNode->right = NULL;
			} break;

		case WEST :
			if (thisNode->col != 0) { 
				thisNode->left = NULL;
			} break;
	}                                                       */

    switch (dir) {
        case NORTH:
            if (API_wallFront() && thisNode->row != 0) {
                thisNode->up = NULL;
                API_setWall(thisNode->col, thisNode->row, 'n');
            } 
            break;
        case SOUTH:
            if (thisNode->row != SIZE-1) {
                thisNode->down = NULL;
                API_setWall(thisNode->col, thisNode->row, 's');
            } 
            break;
        case EAST:
            if (API_wallRight() && thisNode->col != SIZE-1) {
                thisNode->right = NULL;
                API_setWall(thisNode->col, thisNode->row, 'e');
            } 
            break;
        case WEST:
            if (API_wallLeft() && thisNode->col != 0) {
                thisNode->left = NULL;
                API_setWall(thisNode->col, thisNode->row, 'w');
            } 
            break;
    }
}


void print_map (const Maze * thisMaze) {

    /*
	printf("\n%s\n\n", "CURRENT MAP VALUES: ");
	for (short i = 0; i < SIZE; ++i) {
		for (short j = 0; j < SIZE; ++j) {
			printf("%s%3hd", "  ", thisMaze->map[i][j]->floodval);
		} 
		printf("\n\n");
	}
	printf("\n");
    */
    
    for (short i = 0; i < SIZE; ++i) {
        for (short j = 0; j < SIZE; ++j) {
            char text[10];
            sprintf(text, "%d", thisMaze->map[i][j]->floodval);
            API_setText(j, i, text);
        }
    }
}



void check_start_reached(short* x, short* y, short* found_start) {
    if (*x == START_X && *y == START_Y) {
        *found_start = TRUE;
        API_setColor(*x, *y, 'Y');  // Mark start cell yellow
        API_setText(*x, *y, "START");
    }
}


void check_goal_reached(short* x, short* y, short* found_goal) {
    if (*x == SIZE/2 || *x == SIZE/2-1) {
        if (*y == SIZE/2 || *y == SIZE/2-1) {
            *found_goal = TRUE;
            API_setColor(*x, *y, 'R');  // Mark goal cell red
            API_setText(*x, *y, "GOAL");
        }
    }
}

/* function for updating the location and direction of mouse 
   the actual "move" function */
void move_dir (Maze * thisMaze, short * x, short * y, short * dir) {

    /* x, y are current positions, dir is current directions
     these output params may be updated at the exit of this function */ 

    Node* thisNode = thisMaze->map[(*x)][(*y)];  
    short next_dir = get_smallest_neighbor_dir(thisNode, *dir);

    // Turn to face the correct direction
    while (*dir != next_dir) {
        if ((*dir + 1) % 4 == next_dir) {
            API_turnRight();
            *dir = (*dir + 1) % 4;
        } else {
            API_turnRight();
            *dir = (*dir + 3) % 4;
        }
    }
    
    // Move forward
    if (!API_wallFront()) {
        // Update position based on direction
        switch (next_dir) {
            case NORTH: (*x)--; break;
            case EAST:  (*y)++; break;
            case SOUTH: (*x)++; break;
            case WEST:  (*y)--; break;
        }
        API_setColor(*x, *y, 'G');  // Mark visited cells
    }
}


/* the function that represents a visiting of a node 
   walls will be checked, and flood fill called apprepriately */
void visit_Node (Maze * thisMaze, Stack * thisStack, short x, short y, short wallval, char flag) {

    Node* thisNode = thisMaze->map[x][y];

    // Check walls using simulator API
    bool northwall = API_wallFront();
    bool eastwall = API_wallRight();
    bool southwall = false;
    bool westwall = API_wallLeft();
    

    /* push to stack the cell on other side of wall if valid 
        sets the walls as specified by the values checked above */
    if (northwall) {
        if (thisNode->row != 0) {
            push (thisStack, thisMaze->map[thisNode->row-1][thisNode->col]);
            API_setWall(x, y, 'n');

        }
        set_wall(thisNode, NORTH);
    }
    if (eastwall) {
        if (thisNode->col != SIZE-1) {
            push (thisStack, thisMaze->map[thisNode->row][thisNode->col+1]);
            API_setWall(x, y, 'e');
        }
        set_wall(thisNode, EAST);
    }
    if (southwall) {
        if (thisNode->row != SIZE-1) {
            push (thisStack, thisMaze->map[thisNode->row+1][thisNode->col]);
            API_setWall(x, y, 's');

        }
        set_wall(thisNode, SOUTH);
    }
    if (westwall) {
        if (thisNode->col != 0) {
            push (thisStack, thisMaze->map[thisNode->row][thisNode->col-1]);
            API_setWall(x, y, 'w');
        }
        set_wall(thisNode, WEST);
    }

    /* push this node itself, as it was updated */
    push(thisStack, thisNode);
        
    
    /* pop until the stack is empty, and call flood_fill on that node */  
    while (!is_empty_Stack(thisStack)) {
        pop(thisStack, &thisNode);
        /* NOTE: the flag parameter determines wheter to update goal cells or not */
        flood_fill(thisNode, thisStack, flag);
    }
    
    set_visited (thisNode);
    API_setColor(x, y, 'B');

}


/* subroutine used for reading in characters for solver simulator */
int getVal(char c){

  int rtVal = 0;
  if(c >= '0' && c <= '9') {
      rtVal = c - '0';
  } else {
    rtVal = c - 'a' + 10;
  }
  return rtVal;

}

void press_enter_to_continue(){
  printf("press enter to continue...\n");
  while(getchar() == 13);
}

int main() {
    // Remove Walls[SIZE][SIZE] array since we'll use API functions
    int exit_solver_loop = FALSE;
    int trip_count = 0;
    
    Maze* my_maze = newMaze();
    Stack* my_stack = newStack();
    
    short found_dest = FALSE;
    short direction = NORTH;
    short x = START_X;
    short y = START_Y;
    short goal_x, goal_y;
    Node* temp;
    
    /* Initialize variables */
    x = START_X;
    y = START_Y;
    direction = NORTH;
    found_dest = FALSE;
    exit_solver_loop = FALSE;
    trip_count = 0;
    

    // Initial visualization
    API_clearAllColor();
    API_clearAllText();
    
    // Set initial position color
    API_setColor(x, y, 'G');  // Green for current position
    print_map(my_maze);  // This will now use API_setText for flood values

    /* print the initial state of the maze flood values */
    print_map(my_maze);

    /* Solver Loop */
    while (!exit_solver_loop) {
        found_dest = FALSE;
        direction = NORTH;

        // START TO GOAL TRIP
        printf("Begin Forward Trip %d\n", ++trip_count);

        while (!found_dest) {
            // Use API wall detection instead of Walls array
            visit_Node(my_maze, my_stack, x, y, 0, FALSE);  // 0 since we don't need wallval anymore
            move_dir(my_maze, &x, &y, &direction);
            
            // Update visualization
            API_setColor(x, y, 'G');
            print_map(my_maze);
            
            check_goal_reached(&x, &y, &found_dest);
            if (x < 0 || y < 0) {
                printf("NEGATIVE COORD: ERROR\n");
                return FALSE;
            }
        }

        goal_x = x;
        goal_y = y;

        // Check center cells walls
        for (int i = 0; i < 4; i++) {
            visit_Node(my_maze, my_stack, x, y, 0, FALSE);
            
            // Move around center cells
            if (x == SIZE/2-1 && y == SIZE/2-1) x++;
            else if (x == SIZE/2 && y == SIZE/2-1) y++;
            else if (x == SIZE/2 && y == SIZE/2) x--;
            else y--;
            
            API_setColor(x, y, 'B');  // Mark as explored
            print_map(my_maze);
        }

        // Reflood from center
        for (int i = 0; i < SIZE; i++) 
            for (int j = 0; j < SIZE; j++)
                my_maze->map[i][j]->floodval = LARGEVAL;
        
        set_value(my_maze->map[START_X][START_Y], 0);
        push_open_neighbors(my_maze->map[START_X][START_Y], my_stack);
        
        while(!is_empty_Stack(my_stack)) {
            pop(my_stack, &temp);
            if (!(temp->row == START_X && temp->col == START_Y))
                flood_fill(temp, my_stack, TRUE);
        }

        // Reset for return trip
        found_dest = FALSE;

        // GOAL TO START TRIP
        while (!found_dest) {
            visit_Node(my_maze, my_stack, x, y, 0, TRUE);
            move_dir(my_maze, &x, &y, &direction);
            
            API_setColor(x, y, 'Y');  // Different color for return trip
            print_map(my_maze);
            
            check_start_reached(&x, &y, &found_dest);
            if (x < 0 || y < 0) {
                printf("NEGATIVE COORD: ERROR\n");
                return FALSE;
            }
        }

        // Reflood from start
        for (int i = 0; i < SIZE; i++) 
            for (int j = 0; j < SIZE; j++)
                my_maze->map[i][j]->floodval = 0;
        
        push_open_neighbors(my_maze->map[goal_x][goal_y], my_stack);
        while(!is_empty_Stack(my_stack)) {
            pop(my_stack, &temp);
            flood_fill(temp, my_stack, FALSE);
        }

        // Check for reset from simulator
        if (API_wasReset()) {
            API_ackReset();
            x = START_X;
            y = START_Y;
            direction = NORTH;
            API_clearAllColor();
            API_clearAllText();
        }
        
        // Optional: Check for manual exit condition
        // You might want to remove this and rely on simulator reset
        exit_solver_loop = TRUE;  // Or implement your own exit condition
    }

    delete_Maze(&my_maze);
    delete_Stack(&my_stack);

    return TRUE;
}
