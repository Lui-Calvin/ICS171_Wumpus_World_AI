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
#include <iomanip>
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
	prevNode = std::vector<int>(49,-1);
	//std::cout << "prevNode size: " << prevNode.size() << std::endl;
	agentRow = 6;
	agentCol = 0;
	agentDir = 1;
	rowWall = -1;
	colWall = 7;
	pfCounter = 1;
	score = 0;
	goldFound = false;
	wumpusFound = false;
	Moves = new Agent::Action[200];
	moveIndex = 0;
	moveSet = false;
	goalRow = -1;
	goalCol = -1;
	findGoal();
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
	
	score--;
	if(score < -200)
		goldFound = true;
	if(agentRow == 6 && agentCol == 0 && (goldFound||breeze))
	{
		cout<< "RETURNED HOME RETURNED HOME RETURNED HOME" << endl;
		delete [] Moves;
		for(int i =0;i < 7; ++i)
			delete [] map[i];
		delete [] map;
		return CLIMB;
	}
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
		moveSet = false;
		
	}
	if(glitter && !goldFound)
	{
		goldFound = true;
		moveSet = false;
		cout << "GOLD FOUND GOLD FOUND GOLD FOUND GOLD FOUND" << endl;
		return GRAB;
	}
	if(agentRow == goalRow && agentCol == goalCol)
		moveSet = false;
	//setMoves();
	//iagentBoard();
	calculateCost();
	//costBoard();
	findGoal();
	//cin.get();
	//djik(42);
	//getMove();
	


	//`return CLIMB;
	return getMove();	
	// ======================================================================
	// YOUR CODE ENDS
	// ======================================================================
}

// ======================================================================
// YOUR CODE BEGINS
// ======================================================================
//MyAI::~MyAgent()
//{
	
//}
void MyAI::updateRoom(int row, int col,bool stench,bool breeze)
{
	//if this room we arrived to this room that means there is no wumpus or pit here
	//check precepts to flag potential pits or wumpus in the vicinity
	map[row][col].searched = true;
	zeroPitFlags(row,col);
	map[row][col].noPit = true;
	map[row][col].wumpusFlag = false;
	map[row][col].noWumpus = true;
	if(breeze)
		flagPits(row,col);
		markPits();
	if(!breeze)
	{
		zeroPitFlags(row-1,col);
		zeroPitFlags(row+1,col);
		zeroPitFlags(row,col-1);
		zeroPitFlags(row,col+1);
	}
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
	flagPit(row-1,col,row,col);
	flagPit(row+1,col,row,col);
	flagPit(row,col-1,row,col);
	flagPit(row,col+1,row,col);
	pfCounter++;
}
void MyAI::flagPit(int row,int col,int origRow,int origCol)
{
	if(inBounds(row,col)&& !map[row][col].searched && !map[row][col].noPit)
		map[row][col].pitFlags[pfDir(row,col,origRow,origCol)] = pfCounter;	
}
void MyAI::zeroPitFlags(int row,int col)
{
	if(inBounds(row,col))
	{
		for(int i = 0;i < 4;++i)
		{
			map[row][col].pitFlags[i] = 0;
		}
		map[row][col].noPit = true;
	}
}
int MyAI::pfDir(int row,int col, int origRow,int origCol)
{
	//if we flag a pit, determine the direction of the square that marks the potential pit. so we can mark which direction in our pflag is recieved
	if(row == origRow)
	{
		switch(col-origCol)
		{
			case -1:
				return 2;
			case 1: 
				return 0;
		}
		
	} else {

		switch(row-origRow)
		{
			case -1:
				return 1;
			case 1:
				return 3;
		}
	}
}
void MyAI::markPits()
{
	//if a room has more than 2 pitflags it is for sure a pit, so we remove all pit flags from the same group
	for(int i = 0;i < 7; ++i)
	{
		for(int j = 0;j < 7;++j)
		{
			if(countPFlags(i,j) > 2)
			{
				map[i][j].pit = true;
				clearFlagGroupsIn(i,j);
			}
		}
	} 

}
int MyAI::countPFlags(int row,int col)
{
	if(inBounds(row,col))
	{
		int sum = 0;
		for(int i = 0;i < 4;++i)
		{
			if(map[row][col].pitFlags[i] != 0)
				++sum;
		}
		return sum;
	}
}
void MyAI::clearFlagGroupsIn(int row,int col)
{
	for(int i = 0;i < 7; ++i)
	{
		for(int j = 0;j < 7;++j)
		{
			if(i != row || j != col)
			{
				for(int k = 0; k < 4;++k)
				{	
					if(flagIn(row,col,map[i][j].pitFlags[k]))
						map[i][j].pitFlags[k] = 0;
				}
			}
		}
	}
}
bool MyAI::flagIn(int row,int col, int x)
{
	for(int i = 0; i < 4;++i)
	{
		if(map[row][col].pitFlags[i] == x)
			return true;
	}
	return false;
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
void MyAI::turnLeft(int& x)
{
	if(x == 4)
	{
		x = 1;
	}else {
		x += 1;
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
	cout << "agent location: (" << agentRow << ',' << agentCol << ')' << endl;
	cout << "agentDir: " << agentDir << endl;
	cout << "Boundaries: " << colWall << "," << rowWall << endl;
	cout << "Goal: " << goalRow << "," << goalCol << endl;
	cout << "Gold Found : " << goldFound <<endl;
	cout << "score: " << score << endl;
	
	for(int i = 0;i < 7;++i)
	{
		for(int j = 0; j < 7;++j)
		{
			cout << (map[i][j].searched?'s':'.');
			//cout << (map[i][j].pit?'O':'.');
			cout << countPFlags(i,j)<<"{" << map[i][j].pitFlags[0] << map[i][j].pitFlags[1] << map[i][j].pitFlags[2] << map[i][j].pitFlags[3] << "}";
			cout << (map[i][j].wumpusFlag?'f':'.');
			cout << (map[i][j].wumpus?'w':'.');
			cout << ' ';
		}
		cout << endl;
	}

}
void MyAI::costBoard()
{
	for(int i = 0; i < 7;++i)
	{
		for(int j = 0; j < 7;++j)
		{
			cout << setw(4)  <<costMap[i][j] << ' ';
		}
		cout << endl;
	}
}
Agent::Action MyAI::getMove()
{
	//std::cout << "moveIndex: " << moveIndex<<std::endl;
	/*
	int bestDir = 0;
	int bestCost = 3000;
	bool equalCost = true;
	findGoal(); 
	int moveCost[4] = {3000,3000,3000,3000};
	int goalDistance[4] = {100,100,100,100};
	if(inBounds(agentRow,agentCol+1) )
	{
		moveCost[0] = costMap[agentRow][agentCol+1];
		goalDistance[0] = goalDist(agentRow,agentCol-1);
		if(moveCost[0] < bestCost)
		{ 
			bestDir = 1;
			bestCost = costMap[agentRow][agentCol+1];
		}
	}
	if(inBounds(agentRow-1,agentCol))
	{
		moveCost[1] =  costMap[agentRow-1][agentCol];
		goalDistance[1] = goalDist(agentRow-1,agentCol-1); 
		if(moveCost[1] < bestCost)
		{
			bestDir = 2;
			bestCost = costMap[agentRow-1][agentCol];
		}
	}
	
	if(inBounds(agentRow,agentCol-1))
	{
		moveCost[2] = costMap[agentRow][agentCol-1];
		goalDistance[2] = goalDist(agentRow,agentCol-1);
		if(moveCost[2] < bestCost)
		{
			bestDir = 3;
			bestCost = costMap[agentRow][agentCol-1];
		}
	}	
	if(inBounds(agentRow+1,agentCol) )
	{
		moveCost[3] = costMap[agentRow+1][agentCol];
		goalDistance[3] = goalDist(agentRow+1,agentCol);
		if(moveCost[3] < bestCost)
		{
			bestDir = 4;
			bestCost = costMap[agentRow+1][agentCol];
		}
	}
	if(numPossibleMoves(agentRow,agentCol) == countEquals(moveCost)&& countEquals(moveCost) >= 2)
	{
		//cout << "smallest Index" <<smallestIndex(goalDistance) << endl;;
		bestDir = smallestIndex(goalDistance) + 1;
	}
	if(agentDir == bestDir)
	{
		agentForward();	
		return FORWARD;
	} else {
		agentTurnLeft();
		return TURN_LEFT;
	}
	*/
	//std::cout << "MOVESET: " << moveSet << std::endl;	
	if(moveSet)
	{
		//std::cout << "made move" << std::endl;
		if(Moves[moveIndex] == TURN_LEFT)
			turnLeft(agentDir);
		if(Moves[moveIndex] == FORWARD)
			agentForward();
		moveIndex++;
		//printMove(Moves[moveIndex-1]);
		return Moves[moveIndex-1];	
	} else {
		//std::cout << "set move" << std::endl;
		setMoves();
	}
	if(Moves[moveIndex] == TURN_LEFT)
		turnLeft(agentDir);
	if(Moves[moveIndex] == FORWARD)
		agentForward();
	moveIndex++;
	//printMove(Moves[moveIndex-1]);
	return Moves[moveIndex-1];
}
int MyAI::goalDist(int row,int col)
{
	if(inBounds(row,col))
		return abs(goalRow - row) + abs(goalCol-col);
	return 1000;
}
int MyAI::numPossibleMoves(int row,int col)
{
	int sum = 0;
	if(inBounds(row+1,col))
		sum++;
	if(inBounds(row-1,col)&& (row-1 > rowWall))
		sum++;
	if(inBounds(row,col+1) && (col+1 < colWall))
		sum++;
	if(inBounds(row,col-1))
		sum++;
	return sum;
}
int MyAI::countEquals(int* possMoves)
{
	int highestCount = 0;
	for(int i = 0;i < 4;++i)
	{
		int count = 0;
		for(int j = 0; j < 4;++j)
		{
			if (possMoves[i] == possMoves[j]) 
				++count;
		}
		if (count > highestCount)
			highestCount = count;	
	}
}
int MyAI::smallestIndex(int* goalDist)
{
	int smallestValue = 100;
	int index = 0;
	for(int i = 0;i < 4; ++i)
	{
		if(goalDist[i] < smallestValue)
		{
			index = i;
			smallestValue = goalDist[i];
		}
	}
	return index;
}
void MyAI::setMoves()
{
	//find goal room
	//std::cout << "SETMOVES" << std::endl;
	calculateCost();	
	djik(coordToInt(agentRow,agentCol));
	findGoal();
/*	
	for(int i = 0;i < prevNode.size();++i)
	{
		std::cout << setw(7) << i << ':'<< prevNode[i] << " ";
		if((i-6)%7 == 0&& prevNode[i]!=-1)
			std::cout << std::endl;
	}
*/	
	int currDir = agentDir;
	int currRow = agentRow;
	int currCol = agentCol;
	//std::cout << "CurrDir: " << currDir << std::endl;
	//std::cout << "CurrRow: " << currRow << std::endl;
	//std::cout << "currCol: " << currCol << std::endl;
	//std::cout << "GoalRow: " << goalRow << std::endl;
	//std::cout << "goalCol: " << goalCol << std::endl;
	moveIndex = 0;
	std::vector<int>shortestPath;
	/*
	if(prevNode[coordToInt(goalRow,goalCol)] == -1)
	{
		std::cout << "setMove: goal home" << std::endl;
		goldFound = true;
		goalRow = 6;
		goalCol = 0;
	}
	*/
	//std::cout << "setMoves checkpoint 2" << std::endl;
	int x = 0;
	for(int i = coordToInt(goalRow,goalCol);i != -1 && x < 49; i = prevNode[i])
	{
		shortestPath.insert(shortestPath.begin(),i);
		++x;
	}
	if(x > 47)
	{
		goldFound = true;
		goalRow = 6;
		goalCol = 0;
		shortestPath.clear();
		for(int i = coordToInt(goalRow,goalCol);i != -1; i = prevNode[i])
			shortestPath.insert(shortestPath.begin(),i);
 
	}
	//std::cout << goalDist(agentRow,agentCol) << ','<<shortestPath.size()<< std::endl;
	//std::cout << "setMoves checkpoint 1" << std::endl;		
	//shortestPath.insert(shortestPath.begin(),coordToInt(goalRow,goalCol));
	/*
	std::cout << coordToInt(agentRow,agentCol);
	std::cout <<shortestPath[0];
	int curr = shortestPath[0];
	while(curr != coordToInt(agentRow,agentCol))
	{
		curr = shortestPath[0];
		if(prevNode[curr] != -1)
			shortestPath.insert(shortestPath.begin(),prevNode[curr]);
		std::cout << shortestPath[0];
	}
	std::cout <<std::endl;
	*/
	//std::cout << "size of shortestPath: " << shortestPath.size() << std::endl;
	//for(int i = 0; i < shortestPath.size();++i)
	//	std::cout << shortestPath[i] << std::endl;
	//we set to one because we dont try to move to the node we started at
	for(int i = 1;i < shortestPath.size();++i)
	{
		//std::cout << "pfDir: " << (pfDir(intToRow(shortestPath[i]),intToCol(shortestPath[i]),currRow,currCol)+1)<<std::endl; 
		while(pfDir(intToRow(shortestPath[i]),intToCol(shortestPath[i]),currRow,currCol)+1 != currDir)
		{
			//std::cout << "added Left" << std::endl;
			Moves[moveIndex] = TURN_LEFT;
			turnLeft(currDir);
			++moveIndex;
		}
		//std::cout << "added forward" << std::endl;
		Moves[moveIndex] = FORWARD;
		++moveIndex;
		switch(currDir)
		{
		case 1:
			++currCol;
			break;
		case 2:
			--currRow;
			break;
		case 3: 
			--currCol;
			break;
		case 4: 
			++currRow;
			break;
		}	
	}
	moveSet = true;
	moveIndex = 0;
	//for(int i = 0;i < shortestPath.size();++i)
	//	std::cout << shortestPath[i] << std::endl;	
	 
}
void MyAI::findGoal()
{
	calculateCost();
	goalRow = 0;
	goalCol = 0;
	if(goldFound)
	{
		goalRow = 6;
		goalCol = 0;
	} else {
		for(int i = 0;i < 7;++i)
		{
			for(int j = 0;j < 7;++j)
			{		
				if(costMap[i][j] != 0 && costMap[i][j] < costMap[goalRow][goalCol])
				{
					goalRow = i;
					goalCol = j;
				}
			}	
		}
	}
	//std::cout << "goalRow: " << goalRow <<std::endl;
	//std::cout << "goalCol: " << goalCol << std::endl;
}
void MyAI::calculateCost()
{
	for(int i = 0; i < 7;++i)
	{
		for(int j = 0;j < 7; ++j)
		{
			if(goldFound)
			{
				costMap[i][j] = abs(6-i) + abs(0-j) + abs(abs(i-6)-j);
			} else {
				costMap[i][j] = 0;
				if(map[i][j].searched)
					costMap[i][j] += 50;
				costMap[i][j] += distanceFromAgent(i,j);
			}
			//costMap[i][j] += distanceFromAgent(i,j);
			//}
			if(i <= rowWall || j >= colWall)
				costMap[i][j] += 1000;
			if(map[i][j].wumpus)
				costMap[i][j] += 1000;
			if(map[i][j].wumpusFlag)
				costMap[i][j] += 200;
			if(map[i][j].pit)
				costMap[i][j] += 1000;
			if(countPFlags(i,j) > 0)
				costMap[i][j] += 200*countPFlags(i,j);
			
		}
	}
}
int MyAI::distanceFromAgent(int row,int col)
{
	return abs(row-agentRow) + abs(col-agentCol);
}
void MyAI::djik( int source)
{
	createAdjList();
	std::vector<bool> visited(49);
	std::vector<int> dist(49,INT_MAX);
	//for(int i = 0;i < 49;++i)
	//	std::cout << dist[i] << std::endl;
	//for(int i = 0;i < 49;++i)
		//dist[i] = INT_MAX;
	dist[source] = 0;
	for(int i = 0;i < 49;++i)
		prevNode[i] = -1;
	
	for(int i = 0;i < 49;++i)
	{
		int curr = -1;
		for(int j = 0;j<49;++j)
		{
			if(visited[j])
				continue;
			if(curr == -1 || dist[j] < dist[curr])
				curr = j;
		}
		//std::cout << "curr " << curr << "size: " <<  adjList[curr].size() << std::endl;
		visited[curr] = true;
		for(int k = 0;k < adjList[curr].size();++k)
		{
			int path = dist[curr] + 1;
			if(path < dist[adjList[curr][k]])
			{
				dist[adjList[curr][k]] = path;
				//std::cout << "DING" << std::endl;		
				prevNode[adjList[curr][k]] = curr;
				
			}
		}
	}
}
void MyAI::createAdjList()
{
	for(int i = 0; i < 7;++i)
	{
		for(int j = 0; j < 7; ++j)
		{
			adjList[coordToInt(i,j)].clear();
			if(inBounds(i-1,j) && costMap[i][j] < 200 && costMap[i-1][j] < 200)
			{
				adjList[coordToInt(i,j)].push_back(coordToInt(i-1,j));
			}
			
			if(inBounds(i+1,j) && costMap[i][j] < 200 && costMap[i+1][j] < 200)
			{
				adjList[coordToInt(i,j)].push_back(coordToInt(i+1,j));
			}	
			if(inBounds(i,j-1) && costMap[i][j] < 200 && costMap[i][j-1] < 200)
			{
				adjList[coordToInt(i,j)].push_back(coordToInt(i,j-1));
			}
			if(inBounds(i,j+1) && costMap[i][j] < 200 && costMap[i][j+1] < 200)
			{
				adjList[coordToInt(i,j)].push_back(coordToInt(i,j+1));
			}			
		}
	}
/*	
	for(int i = 0;i < 7;++i)
	{
		for(int j = 0;j < 7;++j)
		{
			cout << "{";
			for(int k = 0; k < adjList[coordToInt(i,j)].size();++k)
				cout << adjList[coordToInt(i,j)][k] << ',';
			cout << "} ";
		}
		cout << endl;
	}
*/	
}
int MyAI::coordToInt(int row,int col)
{
	return (row*7)+col;
}
int MyAI::intToRow(int x)
{
	return x/7;
}
int MyAI::intToCol(int x)
{
	return x%7;
}
void MyAI::printMove(Agent::Action a)
{
	if(a == TURN_LEFT)
		std::cout << "==========turn left==========" << std::endl;
	if(a == FORWARD)
		std::cout << "========move forward=========" << std::endl;
	if(a == TURN_RIGHT)
		std::cout << "turn right" << std::endl;
}
// ======================================================================
// YOUR CODE ENDS
// ======================================================================
