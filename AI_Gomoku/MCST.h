//
//  MCST.h
//  AI_gomoku
//
//  Created by Wei Tsung Lu on 2016/11/6.



#ifndef MCST_h
#define MCST_h

#include <vector>
#include <utility>
#include <random>

#include "Board_Function.h"

#define BOARD_SIZE 15
#define SEARCH_RANGE 4

using namespace std;
//random
random_device rd;
mt19937_64 random_engine;


class Node{
public:
    Node(int board[BOARD_SIZE][BOARD_SIZE],int player);
    ~Node();
    
    Node* Select_Child_UCT() const;
    Node* Add_Child(pair<int,int> move,int board[BOARD_SIZE][BOARD_SIZE],int player);
    Node* Add_Unexpected_Child(pair<int,int> move,int board[BOARD_SIZE][BOARD_SIZE],int player);
    void Add_Unexpected_Moves(pair<int,int> move);
    Node(pair<int,int> _move, Node* _parent,int board[BOARD_SIZE][BOARD_SIZE],int player);
    void update(double result){
        wins+=result;
        plays++;
    }
    
    const pair<int,int> move;// position on the board  這個node動哪一步
    const int player;//whose move 誰動這一步
    Node* const parent;
    
    bool Has_Untried_Moves() const{
        return !moves.empty();
    }
    pair<int,int> Get_Untried_Moves();
    bool Has_Children()const{
        return !children.empty();
    }
    
    double wins;
    int    plays;
    
    bool must_chose;// Lethal? 是否為必選點
    
    
    vector<pair<int,int>> moves;// Unexpanded and availble positions on the board 還沒被擴展的可行點
    vector<Node*> children;
    
    double VTD;
private:
    
    double UCT_score;
    
};

// Root node
Node::Node(int board[BOARD_SIZE][BOARD_SIZE],int player) :
    move(make_pair(-1,-1)),
    player(player),
    parent(nullptr),
    wins(0),
    plays(0),
    moves(IdentifyNextMove(0, 0,SEARCH_RANGE,1,board)),
    UCT_score(0),
    VTD(0.5)
{ }

// Child node
Node::Node(pair<int,int> _move, Node* _parent,int board[BOARD_SIZE][BOARD_SIZE],int player) :
    move(_move),
    player(player),
    parent(_parent),
    wins(0),
    plays(0),
    must_chose(0),
    moves(IdentifyNextMove(_move.first, _move.second,SEARCH_RANGE,0,board)),
    UCT_score(0),
    VTD(0.5)
{}

Node::~Node(){
    for(auto child: children){
        delete child;
    }
}

Node* Node::Select_Child_UCT() const{
    for(auto child: children){
        child ->UCT_score = (0.4 * VTD) + (0.6 * (double(child->wins) / double(child->plays)) +
        0.5*sqrt(2.0 * log(double(this->plays) / child->plays)));
    }
    
    return *max_element(children.begin(), children.end(),[](Node* a, Node* b){
        return a->UCT_score < b ->UCT_score;});
}


void Node::Add_Unexpected_Moves(pair<int,int> move){
    moves.push_back(move);
}
Node* Node::Add_Unexpected_Child(pair<int,int> move,int board[BOARD_SIZE][BOARD_SIZE],int player){
    auto node = new Node(move,this,board,player);
    children.push_back(node);
    return node;
}
Node* Node::Add_Child(pair<int,int> move,int board[BOARD_SIZE][BOARD_SIZE],int player){
    auto node = new Node(move,this,board,player);
    children.push_back(node);
    
    auto itr = moves.begin();
    for(;itr != moves.end() && *itr != move; ++itr);
    moves.erase(itr);
    return node;
}

pair<int,int> Node::Get_Untried_Moves(){
    random_engine.seed(rd());
    uniform_int_distribution<size_t> moves_distribution(0, moves.size() - 1);
    return moves[moves_distribution(random_engine)];
    
}

#endif /* MCST_h */
