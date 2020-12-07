#include <inttypes.h>

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

struct ColorNode {
  char color;
  char *lex;
  int type;
  char *profile;
  struct ColorNode *up;
  struct ColorNode *down;
};

struct StackNode {
  uintptr_t addr;
  struct StackNode *next;
};
void insert_node(char color, char *lex, int type, char *profile);
void prune_list();
int search_green_nodes(char *lex);
int search_blue(char *lex);
void push_green(struct ColorNode *GreenNode);
void pop_green();
uintptr_t get_tail_address();
void check_add_green_node(Token t);
void check_add_blue(char *lex, int type);
