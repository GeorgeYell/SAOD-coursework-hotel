#pragma once
#include <iostream>
#include <ctype.h>
#include <stdlib.h>
#include <conio.h>
# define NO_OF_CHARS 256
using namespace std;  

struct room
{
	char nom[5] = {};
	int guest_number = 0;
	int room_number = 0;
	bool toilet = false;
	string equipment = {};
};

struct node
{
	room element;
	node* left;
	node* right;
	int height;
};

typedef struct node* nodeptr;

class bstree
{
public:

	friend bool operator>(room& first, nodeptr& second);
	friend bool operator<(room& first, nodeptr& second);
	void insert(room, nodeptr&);
	void del(room, nodeptr&);
	room fill(nodeptr p);
	room deletemin(nodeptr&);
	room find(room x, nodeptr& p, bool* err);
	void search_equip(nodeptr p, string need_equip, static bool* b);
	void makeempty(nodeptr&);
	void preorder(nodeptr);
	int bsheight(nodeptr);
	nodeptr srl(nodeptr&);
	nodeptr drl(nodeptr&);
	nodeptr srr(nodeptr&);
	nodeptr drr(nodeptr&);
	int maximum(int, int);
	void badCharHeuristic(char* str, int size, int badchar[NO_OF_CHARS]);
	bool search(char* txt, char* pat);
};