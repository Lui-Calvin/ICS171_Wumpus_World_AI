// ======================================================================
// FILE:        MyAI.cpp
//
// AUTHOR:      Abdullah Younis
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#include "MyAI.hpp"
//TODO REMOVE
#include <iostream>
using namespace std;
MyAI::MyAI() : Agent()
{
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
	map = new room*[7];
	for(int i = 0; i < 7;++i)
	{
		map[i] = new room[7];
	}
	agentRow = 6;
	agentCol = 0;
	agentDir = 1;
	rowWall = -1;
	colWall = 7;
	score = 0;
	goldFound = false;
	wumpusFound = false;
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}
	
Agent::Action MyAI::getAction
(
	bool stench,
	bool breeze,
	bool glitter,
	bool bump,
	bool scream
)
{
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
	
	/*
	cout << "agentRow: " << agentRow << endl;
	cout << "agentCol: " << agentCol << endl;
	cout << "agentDir: " << agentDir << endl;
	cout << "stench: " << stench << endl;
	cout << "breeze: " << breeze << endl;
	*/
	if(agentRow == 6 && agentCol == 0 && goldFound)
		return CLIMB;
	if(!bump)
	{
		if(!map[agentRow][agentCol].searched)
			updateRoom(agentRow,agentCol,stench,breeze);
	} else {
		//go back if bump
		if(agentDir == 2)
			rowWall = agentRow;
		if(agentDir == 1)
			colWall = agentCol;
		agentMoveBack();
		
	}
	if(glitter && !goldFound)
	{
		goldFound = true;
		return GRAB;
	}
	agentBoard();
	std::cout << "Press 'w' to Move Forward  'a' to 'Turn Left' 'd' to 'Turn Right'" << std::endl;
	std::cout << "Press 's' to Shoot         'g' to 'Grab'      'c' to 'Climb'" << std::endl;	
	// Get Input
	std::cout << "Please input: ";
	char userInput;
	std::cin >> userInput;
	std::cin.ignore(9999, '\n');
	
	// Return Action Associated with Input
	if ( userInput == 'w')
	{
		agentForward();
		return FORWARD;
	}
	if ( userInput == 'a' )
	{
		agentTurnLeft();
		return TURN_LEFT;
	}
	if ( userInput == 'd' )
	{
		agentTurnRight();
		return TURN_RIGHT;
	}
	if ( userInput == 's' )
		return SHOOT;
	
	if ( userInput == 'g' )
		return GRAB;
	
	return CLIMB;	
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}

// ======================================================================
// YOUR CODE BEGINS
// ======================================================================

void MyAI::updateRoom(int row, int col,bool stench,bool breeze)
{
	//if this room we arrived to this room that means there is no wumpus or pit here
	//check precepts to flag potential pits or wumpus in the vicinity
	map[row][col].searched = true;
	//map[row][col].pitFlag = {0,0,0,0};
	map[row][col].noPit = true;
	map[row][col].wumpusFlag = false;
	map[row][col].noWumpus = true;
	//if(breeze)
	//flagPits(row,col);
	if(stench && !wumpusFound)
		wumpus(row,col);
	if(!stench && !wumpusFound)
	{
		markSafeWumpus(row-1,col);
		markSafeWumpus(row+1,col);
		markSafeWumpus(row,col-1);
		markSafeWumpus(row,col+1);
	}
		
}
bool MyAI::inBounds(int row, int col)
{
	if(row >= 0 && row < 7 && col >=0 && col < 7)
		return true;
	return false;
}

void MyAI::flagPits(int row,int col)
{
	//marks agent's map to see potential pits
	flagPit(row-1,col);
	flagPit(row+1,col);
	flagPit(row,col-1);
	flagPit(row,col+1);
}
void MyAI::flagPit(int row,int col)
{
	if(inBounds(row,col)&& !map[row][col].searched && !map[row][col].noPit)
		return;
		//map[row][col].pitFlag = true;	
}
void MyAI::wumpus(int row,int col)
{
	//marks agent's map. if a location hasn't been flagged for wumpus,flag it. if its already been flagged,
	//then the wumpus is there
	if(checkWumpus(row-1,col))
		return;
	if(checkWumpus(row+1,col))
		return;
	if(checkWumpus(row,col-1))
		return;
	if(checkWumpus(row,col+1))
		return;
/*
	if(inBounds(row+1,col)&& !map[row+1][col].searched)
		map[row+1][col].wumpusFlag = true;
	if(inBounds(row,col-1)&& !map[row][col-1].searched)
		map[row][col-1].wumpusFlag = true;	
	if(inBounds(row,col+1)&& !map[row][col+1].searched)
		map[row][col+1].wumpusFlag = true;
*/
}

bool MyAI::checkWumpus(int row,int col)
{
	//checks current square. if there isn't a wumpus flag, flag this location. if this location has already been flagged,
	//then wumpus is here
	//returns true if wumpus is found
	if(inBounds(row,col)&& !map[row][col].searched && !map[row][col].noWumpus)
	{
		if(!map[row][col].wumpusFlag)
		{	
			//if this location hasn't
			map[row][col].wumpusFlag = true;
			return false;
		} else {	
			wumpusFound = true;
			clearWumpusFlags();
			map[row][col].wumpus = true;
			map[row][col].wumpusFlag = true;
			return true;
		}
	}
}
void MyAI::markSafeWumpus(int row,int col)
{
	if(inBounds(row,col))
	{
		map[row][col].noWumpus = true;
		map[row][col].wumpusFlag = false;
	}
}
void MyAI::clearWumpusFlags()
{
	//once we find wumpus we call this function to remove all of our flags, except for the flag on this square.
	for(int i = 0;i < 7;++i)
	{
		for(int j = 0;j < 7;++j)
		{
			map[i][j].wumpusFlag = false;
			map[i][j].noWumpus = true;
		}
	}
	
}

void MyAI::markPits()
{
	for(int i = 0;i < 7; ++i)
	{
		for(int j = 0;j < 7;++j)
		{
			int pFlags = 0;
			/*
			if(inBounds(i-1,j) && map[i-1][j].pitFlag)
				 pFlags += 1; 
			if(inBounds(i+1,j) && map[i+1][j].pitFlag)
				 pFlags += 1;
			if(inBounds(i,j-1) && map[i][j-1].pitFlag)
				 pFlags += 1;
			if(inBounds(i,j+1) && map[i][j+1].pitFlag)
				 pFlags += 1;
			if(pFlags >=2)
				map[i][j].pit= true;
			*/
			
		}
	} 

}
void MyAI::agentForward()
{
	switch(agentDir)
	{
		case 1:
			++agentCol;
			break;
		case 2:
			--agentRow;
			break;
		case 3: 
			--agentCol;
			break;
		case 4: 
			++agentRow;
			break;
	}

}
void MyAI::agentTurnLeft()
{
	if(agentDir == 4)
	{
		agentDir = 1;
	}else {
		agentDir += 1;
	}
		
}
void MyAI::agentTurnRight()
{
	if(agentDir == 1)
	{
		agentDir = 4;
	}else{
		agentDir-=1;
	}
}
void MyAI::agentMoveBack()
{
	//if agent hits a wall, revert coordinates back to the previous location
	switch(agentDir)
	{
		case 1:
			--agentCol;
			break;
		case 2:
			++agentRow;
			break;
		case 3: 
			++agentCol;
			break;
		case 4: 
			--agentRow;
			break;
	}
}
void MyAI::agentBoard()
{
	//prints our percieved board
	cout << "agent location: (" << agentRow << ',' << agentCol << endl;
	cout << "agentDir: " << agentDir << endl;
	cout << "Boundaries: " << colWall << "," << rowWall << endl;
	for(int i = 0;i < 7;++i)
	{
		for(int j = 0; j < 7;++j)
		{
			cout << (map[i][j].searched?'s':'.');
			cout << (map[i][j].pit?'O':'.');
			//cout << (map[i][j].pitFlags != {0,0,0,0}?'P':'.');
			cout << (map[i][j].wumpusFlag?'f':'.');
			cout << (map[i][j].wumpus?'w':'.');
			cout << ' ';
		}
		cout << endl;
	}
}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================
