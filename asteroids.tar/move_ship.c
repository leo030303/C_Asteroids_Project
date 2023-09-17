#include "asteroids.h"
#include <string.h>
#define FAIL_VAL MOVE_DOWN+1
/*
gcc -g -o game asteroids.c move_ship.c -lcurses; ./game
*/


/*
takes in: map
returns: index 0 of map
shifts all map values down so index 1 is now 0, sets final value to zero
*/
int iterateMap(int currentMap[FIELD_WIDTH-SHIP_WIDTH]){
    int res = currentMap[0];
    for(int mapIndex=1;mapIndex<FIELD_WIDTH-SHIP_WIDTH;mapIndex++){
        currentMap[mapIndex-1] = currentMap[mapIndex];
    }
    currentMap[FIELD_WIDTH-SHIP_WIDTH-1] = MOVE_NO;
    return res;
}

//takes in: asteroid field
//returns: row ship is on, or -1 if ship can't be found
int findShip(int field[][FIELD_WIDTH]){
    for(int row=0;row<FIELD_HEIGHT;row++){
        if(field[row][0] == SHIP_VAL){
            return row;
        }
    }
    return -1;
}

//takes in: column and row of point to validate
//returns: 1 if point is within bounds of the field, else 0
// could've rewritten this to fit on fewer lines but this is more readable
int validPoint(int col, int row){
    if(col<0){
        printf("Column less than zero\n");
        return 0;
    }
    if(col>FIELD_WIDTH-1){
        printf("Column greater than Field Width\n");
        return 0;
    }
    if(row<0){
        printf("Row less than zero\n");
        return 0;
    }
    if(row>FIELD_HEIGHT-1){
        printf("Row greater than Field Height\n");
        return 0;
    }
    return 1;
}

/*
takes in:
col = column of the head of the ship
row = row the ship is on
field = current asteroid field
moveVal = the desired direction to move in

returns: 0 if the movement would cause a collision or if moveVal doesn't correspond to a movement, 1 if its safe
*/
int collisionDetect(int col, int row, int field[][FIELD_WIDTH], int moveVal){
    if (moveVal == MOVE_NO){
        if (field[row][col+1] == ASTEROID_VAL){
            return 0;
        }
        return 1;
    }
    if(moveVal == MOVE_UP){
        for(int shipPartIndex=0;shipPartIndex<SHIP_WIDTH;shipPartIndex++){
            if(field[row-1][col-shipPartIndex+1]==ASTEROID_VAL){
                return 0;
            }
        }
        return 1;
    }
    else if(moveVal == MOVE_DOWN){
        for(int shipPartIndex=0;shipPartIndex<SHIP_WIDTH;shipPartIndex++){
            if(field[row+1][col-shipPartIndex+1]==ASTEROID_VAL){
                return 0;
            }
        }
        return 1;
    }
    else{
        return 0;
    }
}


/*
takes in:
col = column of the head of the ship
row = row the ship is on
field = current asteroid field
goal = the desired row to move to

returns: the move that moves the ship most in the direction of the goal, or the FAILVAL if no moves are safe
*/
// probably could've rewritten this to fit on fewer lines but this is more readable
int nextSafeMove(int col, int row, int field[][FIELD_WIDTH], int goal){
    if (validPoint(col, row)){
        if(row == goal){
            if (collisionDetect(col, row, field, MOVE_NO)){
                return MOVE_NO;
            }
            else if(collisionDetect(col, row, field, MOVE_DOWN)){
                return MOVE_DOWN;
            }
            else if(collisionDetect(col, row, field, MOVE_UP)){
                return MOVE_UP;
            }
        }
        else if(row>goal){
            if (collisionDetect(col, row, field, MOVE_UP)){
                return MOVE_UP;
            }
            else if(collisionDetect(col, row, field, MOVE_NO)){
                return MOVE_NO;
            }
            else if(collisionDetect(col, row, field, MOVE_DOWN)){
                return MOVE_DOWN;
            }
        }
        else if(row<goal){
            if (collisionDetect(col, row, field, MOVE_DOWN)){
                return MOVE_DOWN;
            }
            else if(collisionDetect(col, row, field, MOVE_NO)){
                return MOVE_NO;
            }
            else if(collisionDetect(col, row, field, MOVE_UP)){
                return MOVE_UP;
            }
        }
    }
    return FAIL_VAL;
}

/*
takes in:
map = an array of movement values that correspond to the moves the ship must take to reach the other end of the field
shipPosition = the row the ship is on
field = current asteroid field
endGoals = the array of rows that don't contain an asteroid in the final column, starting with those at the centre and moving out
endGoalsCount = the number of values in the endGoals array

returns: void

loops through end goals and picks next move until other side of field is reached, else move to next end goal
*/
void computeMap(int map[FIELD_WIDTH-SHIP_WIDTH], int shipPosition, int field[][FIELD_WIDTH], int endGoals[FIELD_HEIGHT], int endGoalsCount){
    for (int goalIndex=0;goalIndex<endGoalsCount;goalIndex++){
        int goal = endGoals[goalIndex];
        int failFlag = FAIL_VAL+1;
        int positionHolder = shipPosition;
        for(int col=0;col<FIELD_WIDTH-SHIP_WIDTH;col++){
            int nextMove = nextSafeMove(col+SHIP_WIDTH-1, positionHolder, field, goal);
            if(nextMove==FAIL_VAL){
                failFlag = FAIL_VAL;
                break;
            }
            positionHolder += nextMove;
            map[col] = nextMove;
        }
        if(failFlag!=FAIL_VAL){
            break;
        }
    }
}

/*
takes in:
map = an array of movement values that correspond to the moves the ship must take to reach the other end of the field
shipPosition = the row the ship is on
field = current asteroid field
endGoals = the array of rows that don't contain an asteroid in the final column, starting with those at the centre and moving out
endGoalsCount = the number of values in the endGoals array

returns: void

runs the ships current row through all the movements in the map, then finds the next safe move towards the goal from the second
last column of the field and adds it to the end of the map, if there is none it recomputes the map
*/

void continueMap(int map[FIELD_WIDTH-SHIP_WIDTH], int shipPosition, int field[][FIELD_WIDTH], int endGoals[FIELD_HEIGHT], int endGoalsCount){
    int mapEnd = shipPosition;
    for (int actionIndex=0;actionIndex<FIELD_WIDTH-SHIP_WIDTH;actionIndex++){
        mapEnd += map[actionIndex];
    }
    //its -2 below cause the end of the array is -1, so if i had -1 the next step is out of bounds
    int nextMove = nextSafeMove(FIELD_WIDTH-2, mapEnd, field, endGoals[0]);
    if (nextMove == FAIL_VAL){
        computeMap(map, shipPosition, field, endGoals, endGoalsCount);
    }else{
        map[FIELD_WIDTH-SHIP_WIDTH-1] = nextMove;
    }
}

/*
takes in:
endGoals = the array of rows that don't contain an asteroid in the final column, starting with those at the centre and moving out
endGoalsCount = the number of values in the endGoals array
field = current asteroid field

returns: void

recomputes the list of endGoals for the current field, starting from the centre and moving out
I feel the ship has more options if it tries to stay near the centre of the field
*/

void getEndGoals(int endGoals[FIELD_HEIGHT], int *endGoalsCount, int field[][FIELD_WIDTH]){
    *endGoalsCount = 0;
    for(int row=0;row<FIELD_HEIGHT/2;row++){
        if(field[(FIELD_HEIGHT/2)+row][FIELD_WIDTH-1] != ASTEROID_VAL){
            endGoals[*endGoalsCount] = (FIELD_HEIGHT/2)+row;
            (*endGoalsCount)++;
        }
        if(field[(FIELD_HEIGHT/2)-row][FIELD_WIDTH-1] != ASTEROID_VAL &&
         field[(FIELD_HEIGHT/2)-row][FIELD_WIDTH-1] != field[(FIELD_HEIGHT/2)+row][FIELD_WIDTH-1]){ 
            // second condition is to stop it from duplicating the centre row
            endGoals[*endGoalsCount] = (FIELD_HEIGHT/2)-row;
            (*endGoalsCount)++;
        }
    }
}


struct ship_action move_ship(int field[][FIELD_WIDTH], void *ship_state){
    // Your code should go here
    int shipPosition = findShip(field);
    int *map;
    int endGoals[FIELD_HEIGHT] = {0};
    int endGoalsCount;
    getEndGoals(endGoals, &endGoalsCount, field);
    if(ship_state!=NULL){
        // if ship_state isn't null, utilise the previous map and update it
        map = ship_state;
        continueMap(map, shipPosition, field, endGoals, endGoalsCount);
    }else{
        // if ship_state is NULL, initialise it
        map = malloc((FIELD_WIDTH-SHIP_WIDTH) * sizeof(int));
        memset(map, 0, (FIELD_WIDTH-SHIP_WIDTH) * sizeof(int));
        computeMap(map, shipPosition, field, endGoals, endGoalsCount);
    }
    struct ship_action res;
    res.move = iterateMap(map);
    res.state = map;
    return res;
}
/*

Kept to help clarify the code
Roughwork:

function get all clear spots at end of field, put in list, in order of proximity to centre
loop through the list
have variable for ship position
function to decide if move is safe from given position
map to other side gets passed on ship state, its an array of 1, 0, and -1 to show which moves to do
at each timestamp run function to find safe next move from end of map
if no safe move, recompute map


int shipPosition

int nextSafeMove(int x, int y)

int[] computeMap(int[] map)

int[] continueMap(int[] currentMap, int shipPosition){
    int mapEnd = shipPosition
    for i in currentMap
        mapEnd += currentMap[i];
    int nextMove = nextSafeMove(FIELD_WIDTH, mapEnd)
    if nextMove == FAIL_VAL{
        computeMap(currentMap)
    }
}

int[] getEndGoals()


*/