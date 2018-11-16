// ======================================================================
// FILE:        MyAI.hpp
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

#ifndef MYAI_LOCK
#define MYAI_LOCK

#include "Agent.hpp"

class MyAI : public Agent
{
public:
	MyAI ( void );
	
	Action getAction
	(
		bool stench,
		bool breeze,
		bool glitter,
		bool bump,
		bool scream
	);
	
	// ======================================================================
	// YOUR CODE BEGINS
	// ======================================================================
private:
	struct room{
		bool searched = false;
		int  pitFlags[4] = {0,0,0,0};
		bool pit = false;
		bool noPit = false;
		bool wumpusFlag = false;
		bool wumpus = false;
		bool noWumpus = false;
	};
	int agentRow;
	int agentCol;
	int agentDir;
	int rowWall;
	int colWall;
	int score;
	bool goldFound;
	bool wumpusFound;
	// 1 = right 2 = up 3 = left 4 = down
	room** map;	
	void updateRoom(int row,int col,bool stench,bool breeze);
	bool inBounds(int row,int col);
	void flagPits(int row,int col);
	void flagPit(int row,int col);
	void wumpus(int row,int col);
	bool checkWumpus(int row, int col);
	void markSafeWumpus(int row,int col);
	void clearWumpusFlags();
	void markPits();
	void agentForward();
	void agentTurnLeft();
	void agentTurnRight();
	void agentMoveBack();	
	void agentBoard();
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
};

#endif
