//
//  Board_Function.h
//  AI_gomoku
//
//  Created by Wei Tsung Lu on 2016/11/10.


#ifndef Board_Function_h
#define Board_Function_h
#define BOARD_SIZE 15
#define WIN_CON 5

#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
// Check if any player wins
//    r,c: postion of last move
//    return 1: Game over
int CheckGame(int r,int c,int board[BOARD_SIZE][BOARD_SIZE],int player);
int Step(int r,int c,char axis,char p_n,int board[BOARD_SIZE][BOARD_SIZE],int player);

// Find posible moves
//    mode0: Search in certain range.
//    mode1: Search the whole board.
vector <pair<int,int>> IdentifyNextMove(int r,int c,int range,int mode,int board[BOARD_SIZE][BOARD_SIZE]);

// Humen moving
pair<int,int>  Move(int board[BOARD_SIZE][BOARD_SIZE],int player);

// Show the Board
void PrintBoard(int board[BOARD_SIZE][BOARD_SIZE]);

int CheckGame(int r,int c,int board[BOARD_SIZE][BOARD_SIZE],int player){
    int x = 1+ Step(r,c,'x','+',board,player) + Step(r,c,'x','-',board,player);
    int y = 1+ Step(r,c,'y','+',board,player) + Step(r,c,'y','-',board,player);
    int z = 1+ Step(r,c,'z','+',board,player) + Step(r,c,'z','-',board,player);
    int w = 1+ Step(r,c,'w','+',board,player) + Step(r,c,'w','-',board,player);
    
    if(x >= WIN_CON || y >= WIN_CON || z >= WIN_CON || w >= WIN_CON)
        return 1;
    else
        return 0;
}

int Step(int r,int c,char axis,char p_n,int board[BOARD_SIZE][BOARD_SIZE],int player){
    int val = 0;
    int step_r = 0;
    int step_c = 0;
    int ii;
    for(int i = 1;i <= WIN_CON; i++){
        ii = i;
        if(p_n == '-'){
            ii = -1 * i;
        }
        if(axis == 'x'){
            step_r = r + ii;
            step_c = c;
        }
        else if(axis == 'y'){
            step_r = r;
            step_c = c + ii;
        }
        else if(axis == 'z'){
            step_r = r + ii;
            step_c = c + ii;
        }
        else if(axis == 'w'){
            step_r = r + ii;
            step_c = c - ii;
        }
        if(step_r >= 0 && step_r < BOARD_SIZE && step_c >= 0 && step_c < BOARD_SIZE){
            int tmp = board[step_r][step_c];
            if(tmp == -1 || tmp != player)
                break;
            else if(tmp == player)
                val++;
        }
    }
    return val;
}


pair<int,int> Move(int board[BOARD_SIZE][BOARD_SIZE],int player){
    int valid_input = 0;
    string input;
    stringstream ss;
    char dot;
    char row = 0;
    int column = 0;
    
    while(!valid_input){
        PrintBoard(board);
        cout<< "Player " << player << ", select a row,column :\n";
        cin >> input;
        stringstream ss;
        ss << input;
        ss >> row;
        row -=65;
        ss >> dot;
        ss >> column;
        if(board[row][column] == -1){
            valid_input = 1;
        }
        else
            cout << "Invalid Input!\n";
    }
    return make_pair(row,column);
    
}

vector <pair<int,int>> IdentifyNextMove(int r,int c,int range,int mode,int board[BOARD_SIZE][BOARD_SIZE]){
    vector <pair<int,int>> moves;
    if(!mode){
        for(int i = r - range; i < r + range; i++){
            for(int j = c - range; j < c + range; j++){
                if(i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE){
                    if(board[i][j] == -1){
                        moves.push_back(make_pair(i,j));
                    }
                }
            }
        }
    }
    else {
        for(int i = 0; i < BOARD_SIZE; i++){
            for(int j = 0; j < BOARD_SIZE; j++){
                if(board[i][j] == -1){
                    moves.push_back(make_pair(i,j));
                }
            }
        }
    }
    return moves;
}

void PrintBoard(int board[BOARD_SIZE][BOARD_SIZE]){
    char row = 'A';
    cout<<"  ";
    for(int i = 0; i<BOARD_SIZE; i++){
        cout << setw(2) << i <<" ";
    }
    cout <<"\n";
    for(int i = 0; i<BOARD_SIZE; i++){
        cout<< row++ << " ";
        for(int j = 0; j<BOARD_SIZE; j++){
            if(board[i][j] == -1)
                cout <<" + ";
            else
                cout << setw(2) << board[i][j] << " ";
        }
        cout << "\n" ;
    }
}


#endif /* Board_Function_h */
