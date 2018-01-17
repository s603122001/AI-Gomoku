//
//  main.cpp
//  AI_gomoku
//
//  Created by Wei Tsung Lu on 2016/11/6.


#include <time.h>
#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <memory>
#include <iomanip>
#include <thread>
#include <future>
#include <random>
#include <algorithm>


#include "MCST.h"
#include "Board_Function.h"

#define BOARD_SIZE 15
#define SEARCH_RANGE 4
#define COMPUTE_TIME 9.99 // Thinking time of A.I
#define WIN_CON 5 // Gomoku's win condition
#define THREAD_NUMBER 3
using namespace std;

// start A.I Game
void AI_Game(int humen_color);

// A.I moving
//    thread_number: 開幾個thread跑模擬
//    node: 每個thread當前node pointer
pair<int,int>  AI_Move(int player, Node* node[THREAD_NUMBER],int sc_p1_board[BOARD_SIZE][BOARD_SIZE],int sc_p2_board[BOARD_SIZE][BOARD_SIZE]);


//-----------MCST------------//
// Tree
vector<unique_ptr<Node>> Trees;
// Evaluate the positions
//
int AI_evaulate(int r, int c, int board[BOARD_SIZE][BOARD_SIZE], int player);
int score_step(int r, int c,char axis,char p_n, int board[BOARD_SIZE][BOARD_SIZE],int player);
int weighting(int in);
// check if there are points
pair<int,int> Ready_To_Win(int board[BOARD_SIZE][BOARD_SIZE],int player);
// Simulation
//    starting_node:開始計算的點
int Compute_Tree(Node* starting_node);

// Variables for the game
int real_board[BOARD_SIZE][BOARD_SIZE];
int current_player;
int threat_board[BOARD_SIZE][BOARD_SIZE];


int main(int argc, const char * argv[]) {
    string game_mode;
    cout << "Black or White ?\n"; //Player choose sides
    cin >> game_mode;
    if(game_mode == "Black"|| game_mode =="black")
        AI_Game(0);
    else
        AI_Game(1);
    return 0;
}

int score_board_p1[BOARD_SIZE][BOARD_SIZE];
int score_board_p2[BOARD_SIZE][BOARD_SIZE];
int p1_best_score;
int p2_best_score;

void AI_Game(int color){
    memset( (void *)real_board, -1, sizeof(int)*BOARD_SIZE*BOARD_SIZE);
    int gameover = 0;
    int move = 1;
    int player_1 = color;
    int player_2 =(color+1)%2;
    
    current_player = 0;
    Node* current_node[THREAD_NUMBER];
    
    for(int i = 0; i < THREAD_NUMBER;i++){
        Trees.push_back(unique_ptr<Node>(new Node(real_board,current_player)));
        current_node[i] = Trees[i].get();
    }
    
    pair<int,int> rc[2] ;
    // Start the game
    while(!gameover && move < BOARD_SIZE*BOARD_SIZE){
        // Blak
        if(current_player ==  player_1){
            rc[player_1] = Move(real_board,player_1);
// 2 A.I Game
//            if(move==1){
//                board[7][7] = player_1;
//                rc[player_1] = make_pair(7, 7);
//            }
//            else
//                rc[player_1] = AI_Move(player_1,current_node,score_board_p1,score_board_p2);
        }
        // White
        else if(current_player == player_2){
            if(move==1){
                real_board[7][7] = player_2;
                rc[player_2] = make_pair(7, 7);
            }
            else{
                rc[player_2] = AI_Move(player_2,current_node,score_board_p1,score_board_p2);
            }
            
        }
        
        real_board[rc[current_player].first][rc[current_player].second] = current_player;
        
        // Attck or Deffence evaluation
        vector<pair<int,int>> best_points;
        if(current_player == player_1){
            p1_best_score = -20000;
            p2_best_score = -20000;
            for(int i = 0;i < BOARD_SIZE; i++){
                for(int j = 0;j < BOARD_SIZE; j++){
                    if(real_board[i][j] == -1){
                        score_board_p1[i][j] = AI_evaulate(i,j,real_board,player_1);
                        if(score_board_p1[i][j] > p1_best_score)
                            p1_best_score = score_board_p1[i][j];
                        score_board_p2[i][j] = AI_evaulate(i,j,real_board,player_2);
                        if(score_board_p2[i][j] > p2_best_score)
                            p2_best_score = score_board_p2[i][j];
                    }
                    else{
                        score_board_p1[i][j] = -100000;
                        score_board_p2[i][j] = -100000;
                    }
                }
            }
 ////
            int test_board[BOARD_SIZE][BOARD_SIZE];
            int tmp;
            int tmp2;
            for(int i = 0;i < BOARD_SIZE; i++){
                for(int j = 0;j < BOARD_SIZE; j++){
                        threat_board[i][j] = 1;
                }
            }
            for(int i = 0;i < BOARD_SIZE; i++){
                for(int j = 0;j < BOARD_SIZE; j++){
                    if(real_board[i][j] != -1){
                        for(int k = 0;k < BOARD_SIZE; k++){
                            for(int l = 0;l < BOARD_SIZE; l++){
                                test_board[k][l] = real_board[k][l];
                            }
                        }
                        test_board[i][j] = player_2;
                        for(int k = 0;k < BOARD_SIZE; k++){
                            for(int l = 0;l < BOARD_SIZE; l++){
                                if(test_board[k][l] != -1){
                                    tmp = AI_evaulate(k, l, test_board, player_1);
                                    tmp2 = AI_evaulate(k, l, test_board, player_2);
                                    if(tmp  >= 7000)
                                        threat_board[i][j] *= 0.7;
                                    if(tmp2 >= 7000)
                                        threat_board[i][j] *= 1.5;
                                }
                            }
                        }
                    }
                }
            }
            
//            for(int i = 0;i < BOARD_SIZE; i++){
//                for(int j = 0;j < BOARD_SIZE; j++){
//                    score_board_p1[i][j]*=threat_board[i][j];
//                }
//            }

/////
            for(int i = 0;i < BOARD_SIZE; i++){
                for(int j = 0;j < BOARD_SIZE; j++){
                    if(real_board[i][j] == -1){
                        if(color == 0){
                            if(p2_best_score > 200){
                                if(score_board_p1[i][j] >= 6500 || score_board_p2[i][j] >= 6500)
                                    best_points.push_back(make_pair(i,j));
                                else if(score_board_p1[i][j] >= p1_best_score)
                                    best_points.push_back(make_pair(i,j));
                                else if(score_board_p2[i][j] >= p2_best_score)
                                    best_points.push_back(make_pair(i,j));
                            }
                            else
                                if(score_board_p1[i][j] >= p1_best_score)
                                    best_points.push_back(make_pair(i,j));
                        }
                        else{
                            if(score_board_p1[i][j] >= 6500 || score_board_p2[i][j] >= 6500)
                                best_points.push_back(make_pair(i,j));
                            else if(score_board_p1[i][j] >= p1_best_score)
                                best_points.push_back(make_pair(i,j));
                            else if(score_board_p2[i][j] >= p2_best_score)
                                best_points.push_back(make_pair(i,j));
                        }
                    }
                }
            }
        }
        if(move == 1){
            for(int i = 0; i < THREAD_NUMBER; i++){
                current_node[i]->Add_Child(rc[current_player], real_board, current_player);
            }
        }
//        else if(move == 2 && current_player == player_1){
//            for(int i = 0; i < THREAD_NUMBER; i++){
//                current_node[i]->Add_Unexpected_Child(rc[current_player], real_board, current_player);
//            }
//       }
        move++;
        int found;
        int found_best;
        for(int i = 0; i < THREAD_NUMBER; i++){
            found = 0;
            for(auto child: current_node[i]->children){
                if(child->move == rc[current_player]){
                        current_node[i] = child;
                        found = 1;
                    break;
                }
            }
            if(found == 0){
                current_node[i] = current_node[i]->Add_Unexpected_Child(rc[current_player], real_board, current_player);
            }
        }
        
        if(current_player == player_1){
            for(int i = 0; i < THREAD_NUMBER; i++){
                for(auto best: best_points){
                    found_best = 0;
                    for(auto move: current_node[i]->moves){
                        if(move == best){
                            found_best = 1;
                            break;
                        }
                    }
                    if(found_best == 0){
                        current_node[i]->Add_Unexpected_Moves(best);
                    }
                }
            }
        }
        
        gameover = CheckGame(rc[current_player].first,rc[current_player].second,real_board,current_player);
        current_player = (current_player+1)%2;
    }
    cout << "Final Board\n";
    PrintBoard(real_board);
    cout << "***** Game Over ****** Winner was Player " << (current_player+1)%2 << "\n";
}

// Weighting function
// x>=15 ==> x*1000
// 14<=x <15   X*500
// 12<=x <14  X*100
// x<12    X*10

int AI_evaulate(int r, int c, int board[BOARD_SIZE][BOARD_SIZE], int player){
    int x = 5 + score_step(r,c,'x','+',board,player) + score_step(r,c,'x','-',board,player);
    
    int y = 5 + score_step(r,c,'y','+',board,player) + score_step(r,c,'y','-',board,player);
    int z = 5 + score_step(r,c,'z','+',board,player) + score_step(r,c,'z','-',board,player);
    int w = 5 + score_step(r,c,'w','+',board,player) + score_step(r,c,'w','-',board,player);
    return weighting(x)+weighting(y)+weighting(z)+weighting(w);
}

int weighting(int in){
    int result;
    if (in >= 15)
        result = in * 1000;
    else if (in >= 14 && in < 15)
        result = in * 500;
    else if (in >= 12 && in < 14)
        result = in * 100;
    else
        result = in * 10;
    return result;
}

int score_step(int r, int c,char axis,char p_n, int board[BOARD_SIZE][BOARD_SIZE],int player){
    int score = 0;
    int grade;
    int step_r = 0;
    int step_c = 0;
    int tmp;
    int empty = -1;
    
    for(int i = 1; i <= WIN_CON; i++){
        grade = WIN_CON - i;
        int ii = i;
        if(p_n == '-')
            ii *= -1;
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
        
        if(step_r >= 0 && step_r < BOARD_SIZE && step_c >=0 && step_c < BOARD_SIZE){
            tmp = board[step_r][step_c];
            if(tmp == -1){
                if(empty == -1)
                    empty = 1;
                else
                    break;
            }
            if(tmp == (player+1)%2){
                score -= grade;
                break;
            }
            else if (tmp == player)
                score += grade;
        }
    }
    return score;
}

int t,start_time;
int Compute_Tree(Node* starting_node){
    random_engine.seed(rd());
    int sim_board[BOARD_SIZE][BOARD_SIZE];
    int sim_player;
    int sim_gameover;
    pair<int,int> l_rc[2];// Last move 儲存上一次下的點
    
    int iter_time = 0;// Num of complete simulations 計算做了幾次完整模擬
    long long int t = 0;// Num of total moves
    
    clock_t start_t = (double)clock() / CLOCKS_PER_SEC;
    
    while((double)clock() / CLOCKS_PER_SEC - start_t < 2.5*COMPUTE_TIME ){

        for(int i = 0; i < BOARD_SIZE; i++){
            for(int j = 0; j < BOARD_SIZE; j++){
                sim_board[i][j] = real_board[i][j];
            }
        }

        auto node = starting_node;
        while(!node->Has_Untried_Moves() && node->Has_Children()){
            node = node->Select_Child_UCT();
            sim_board[node->move.first][node->move.second] = node->player;
        }
        
        sim_player = (node->player+1)%2;
        l_rc[(sim_player+1)%2] = node->move;
        if(node->Has_Untried_Moves()){
            auto move = node->Get_Untried_Moves();
            l_rc[sim_player] = node->move;
            sim_board[move.first][move.second] = sim_player;
            node = node->Add_Child(move,sim_board,sim_player);
        }
        sim_player = (sim_player+1)%2;
        
        sim_gameover = 0;
        int how_many_turns = 0;
        while(!sim_gameover && !IdentifyNextMove(0, 0, 0, 1, sim_board).empty()){
            auto sim_win_pos = Ready_To_Win(sim_board,sim_player);
            
            if(sim_win_pos.first != -1){
                l_rc[sim_player] = sim_win_pos;
            }
            else{
                int sim_p1_best = -20000;
                int sim_p2_best = -20000;
                int sim_scored_bored_p1[BOARD_SIZE][BOARD_SIZE];
                int sim_scored_bored_p2[BOARD_SIZE][BOARD_SIZE];
                for(int i = 0; i < BOARD_SIZE; i++){
                    for(int j = 0; j < BOARD_SIZE; j++){
                        if(sim_board[i][j] == -1){
                            sim_scored_bored_p1[i][j] = AI_evaulate(i, j, sim_board, sim_player);
                            if(sim_scored_bored_p1[i][j] >= sim_p1_best)
                                sim_p1_best = sim_scored_bored_p1[i][j];
                            sim_scored_bored_p2[i][j] = AI_evaulate(i, j, sim_board, (sim_player+1)%2);
                            if(sim_scored_bored_p2[i][j] >= sim_p2_best)
                                sim_p2_best = sim_scored_bored_p2[i][j];
                        }
                        else{
                            sim_scored_bored_p1[i][j] = -100000;
                            sim_scored_bored_p2[i][j] = -100000;
                        }
                    }
                }
                vector<pair<int,int>> possible_moves;
                for(int i = 0; i < BOARD_SIZE; i++){
                    for(int j = 0; j< BOARD_SIZE; j++){
                        if(sim_scored_bored_p1[i][j] >= 3000 || sim_scored_bored_p2[i][j] >= 3000)
                            possible_moves.push_back(make_pair(i,j));
                        if(sim_scored_bored_p1[i][j] >= sim_p1_best)
                            possible_moves.push_back(make_pair(i,j));
                        if(sim_scored_bored_p2[i][j] >= sim_p2_best)
                            possible_moves.push_back(make_pair(i,j));
                    }
                }
                random_engine.seed(rd());
                uniform_int_distribution<size_t> moves_distribution(0, possible_moves.size() - 1);
                l_rc[sim_player] = possible_moves[moves_distribution(random_engine)];;
            }
            
            sim_board[l_rc[sim_player].first][l_rc[sim_player].second] = sim_player;
            sim_gameover = CheckGame(l_rc[sim_player].first,l_rc[sim_player].second,sim_board,sim_player);
            sim_player = (sim_player+1)%2;
            t++;
            how_many_turns++;
        }
        int sim_winner;
        if(sim_gameover)
            sim_winner = (sim_player+1)%2;
        else
            sim_winner = -1;
        
        //VTD:  gamma = 0.8  alphaTD = 0.4 lambda = 0.8
        double delta;
        if(node->player == sim_winner)
            delta = pow(0.8,how_many_turns) * 1 - node->VTD;
        else if(node->player == (sim_winner+1)%2)
            delta = pow(0.8,how_many_turns) * 0 - node->VTD;
        else
            delta = pow(0.8,how_many_turns) * 0.5 - node->VTD;
        
        double eligi = 1;
        while(node!= nullptr){
            if(node->player == sim_winner)
                node->update(1);
            else if(node->player == (sim_winner+1)%2)
                node->update(0);
            else
                node->update(0.5);
            
            node->VTD += 0.4*eligi*delta;
            eligi = eligi * 0.8 * 0.8;
            
            node = node->parent;
        }
        iter_time++;
        
    }
    //cout <<"time spend: " << (double)clock() / CLOCKS_PER_SEC - start_t << " hands: " << t <<endl;
    cout << "total moves: " << t <<endl;
    cout <<"complete games: " << iter_time <<endl;
    return iter_time;
}

pair<int,int>  AI_Move(int player,Node* node[THREAD_NUMBER],int sc_p1_board[BOARD_SIZE][BOARD_SIZE],int sc_p2_board[BOARD_SIZE][BOARD_SIZE]){
    
    // thread computing

    future<int> re[THREAD_NUMBER];
    int waiter;
    
    cout <<"Thread Perfomance: " <<endl;
    for(int i = 0; i < THREAD_NUMBER; i++)
        re[i] = async(launch::async, Compute_Tree,node[i]);
    for(int i = 0; i < THREAD_NUMBER; i++)
        waiter+=re[i].get();
    
    
    // merge the results
    map<pair<int,int>, int> plays;
    map<pair<int,int>, double> wins;
    
    long long games_played = 0;
    for(int i = 0; i < THREAD_NUMBER;i++){
        auto root =node[i];
        games_played += root->plays;
        for(auto child :root->children){
            plays[child->move] += child->plays;
            wins[child->move]  += child->wins;
        }
        
    }
    double score;
    double best_score = -10000;

    pair<int,int> best_move;
    for(auto itr: plays){
        auto move = itr.first;
        double p = itr.second;
        double w = wins[move];
        score = (double(w) +1) / (double(p) +2); //+ sqrt(2.0 * log(double(games_played) / p));
        score =  8000*score +threat_board[move.first][move.second]*(sc_p1_board[move.first][move.second] + sc_p2_board[move.first][move.second]);
        if(score > best_score){
            best_score = score;
            best_move = move;
        }
    }
    // Check for any lethal point
    auto win_pos = Ready_To_Win(real_board,current_player);
    if(win_pos.first != -1){
        cout << "Must move " << static_cast<char>('A' + win_pos.first) << "," << win_pos.second << endl;
        return win_pos;
    }
    
    
    cout << "Best Move: " << static_cast<char>('A' + best_move.first) << "," << best_move.second << endl;
    cout <<"score: " << best_score <<endl;
    return best_move;
    
    
}

pair<int,int> Ready_To_Win(int board[BOARD_SIZE][BOARD_SIZE],int player){
    auto possible_moves = IdentifyNextMove(0, 0, 0, 1, board);
    int self_win = 0;
    int enemy_win = 0;
    pair<int,int> enemy_win_pos = make_pair(-1, -1);
    for(auto move : possible_moves){
        self_win = CheckGame(move.first, move.second, board, player);
        if(self_win == 1){
            return move;
        }
        enemy_win = CheckGame(move.first, move.second,board,(player+1)%2);
        if(enemy_win == 1)
            enemy_win_pos = move;
    }
    
    if(enemy_win_pos.first != -1)
        return enemy_win_pos;
    else
        return make_pair(-1, -1);
}



