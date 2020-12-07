#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "../headers/colornode.h"

#ifndef TOKEN
#define TOKEN
#include "../headers/token.h"
#endif

static struct ColorNode *dllist = NULL;
static struct StackNode *eye_stack = NULL;

void push_green(struct ColorNode *greenNode) {
  struct StackNode *node = malloc(sizeof(struct StackNode));
  node->addr = (uintptr_t)greenNode;

  if (eye_stack == NULL){
    eye_stack = node;
    printf("push 0x%" PRIXPTR ": %s\n", node->addr, greenNode->lex);
    return;
  }

  node->next = eye_stack;
  eye_stack = node;
  printf("push 0x%" PRIXPTR ": %s\n", node->addr, greenNode->lex);
}

void pop_green() {
  if (eye_stack == NULL) {
    return;
  }

  struct StackNode *tmp;
  tmp = eye_stack;
  
  if (eye_stack->next) {
    eye_stack = eye_stack->next;
    printf("pop: 0x%" PRIXPTR "\n", tmp->addr);
  } else {
    printf("pop: 0x%" PRIXPTR "\n", tmp->addr);
    eye_stack = NULL;
  }

  if (eye_stack) {
    while(get_tail_address() != eye_stack->addr) {
      printf("pruning...\n");
      prune_list();
    }
  }

  struct ColorNode *test;
  test = dllist;
  while(test->down) {
    test = test->down;
  }
  printf("pruned to: %s\n", test->lex);
  printf("%s profile: %s\n", test->lex, test->profile);
  
}

/*
  This will push onto 2 stacks: dllist, and eye. dllist
  will keep both green and blue nodes, while eye will keep only green
  nodes.
 */
void insert_node(char color, char *lex, int type, char *profile) {
  struct ColorNode *node = malloc(sizeof(struct ColorNode));
  node->color = color;
  node->lex = lex;
  node->type = type;
  node->profile = profile;

  if (color == 'G') {
    push_green(node);
  }


  if (dllist == NULL) {
    dllist = node;
    //printf("address of %s: 0x%" PRIXPTR "\n", node->lex, (uintptr_t)node);
    return;
  }

  struct ColorNode *tmp;
  tmp = dllist;
  while(tmp->down) {
    tmp = tmp->down;
  }
  node->up = tmp;
  tmp->down = node;
  //printf("address of %s: 0x%" PRIXPTR "\n", node->lex, (uintptr_t)node);
}

uintptr_t get_tail_address() {
  struct ColorNode *tmp;
  tmp = dllist;

  while (tmp->down) {
    tmp = tmp->down;
  }
  return (uintptr_t)tmp;
}

void prune_list() {

  if (dllist == NULL) {
    return;
  } else if (!dllist->down) {
    //printf("POP Green : %s\n", dllist->lex);
    //printf("address of %s: 0x%" PRIXPTR "\n", dllist->lex, (uintptr_t)dllist);
    dllist = NULL;
    return;
  }
  
  struct ColorNode *tmp;
  tmp = dllist;
  // go to end of list
  while(tmp->down->down) {
    tmp = tmp->down;
  }
  //printf("POP Green : %s\n", tmp->down->lex);
  //printf("address of %s: 0x%" PRIXPTR "\n", tmp->down->lex, (uintptr_t)tmp->down);
  tmp->down = NULL;
}

/*
  returns whether or not the lexeme is found in the existing
  green nodes. 

  0 = not found
  1 = found
 */
int search_green_nodes(char *lex) {
  //printf("search: %s\n", lex);
  if (dllist == NULL) {
    return 0;
  }
  struct ColorNode *tmp;
  tmp = dllist;
  // strcmp() == 0 when identical
  if (!strcmp(tmp->lex, lex)) {
    printf("found: %s\n", tmp->lex);
    return 1;
  }

  // while list has more children...
  while(tmp->down) {
    tmp = tmp->down;
    if (!strcmp(tmp->lex, lex)) {
      printf("found: %s\n", tmp->lex);
      return 1;
    }
  }
  return 0;
}

void check_add_green_node(Token t) {
  char *str = t.str;
  int type = t.type;
  char *profile = "";

  /* TODO: check parent green nodes first */
  if(search_green_nodes(str)) {
    printf("SEMERR: Attempt to redefine `%s`\n", str);
    insert_node('G', "SEMERR", type, profile);
    printf("PUSH SEMERR Green : %s\n", str);    
  }

  insert_node('G', str, type, profile);
  //printf("PUSH Green : %s\n", str);
}

void update_profile() {
  return;
}

uintptr_t get_node_addr(struct ColorNode *n) {
  return (uintptr_t) n;
}

int search_blue(char *lex) {
  if (!dllist) {
    return 0;
  }
  struct ColorNode *tmp;
  tmp = dllist;

  while(tmp->down) {
    tmp = tmp->down;
  }

  uintptr_t parent_green_addr = eye_stack->addr;
  while(get_node_addr(tmp) != parent_green_addr) {
    printf("searching for %s...\n", tmp->lex);
    if (tmp->lex == lex) {
      printf("search_blue: found: %s\n", tmp->lex);
      return 1;
    }
    if (tmp->up) {
      tmp = tmp->up;      
    } else {
      break;
    }
  }
  
  return 0;
}

struct ColorNode *get_parent_green() {
  struct ColorNode *tmp;
  tmp = dllist;
  while(tmp->down)
    tmp = tmp->down;

  uintptr_t parent_addr = eye_stack->addr;
  while(get_node_addr(tmp) != parent_addr) {
    if (tmp->up)
      tmp = tmp->up;
  }
  return tmp;
}

void check_add_blue(char *lex, int type) {
  if(search_blue(lex)) {
    printf("SEMERR: (blue): `%s`\n", lex);
    return;
  }
  printf("check_add_blue: %s\n", lex);
  if(type >= 5 && type <= 9) {
    printf("PARAM TYPE\n");
    struct ColorNode *tmp = get_parent_green();
    char buffer [3];
    sprintf(buffer, "%d", type);
    char *old_profile = tmp->profile;
    tmp->profile = malloc((strlen(tmp->profile) + 2) * sizeof(char));
    strcat(tmp->profile, old_profile);
    strcat(tmp->profile, buffer);
    printf("UPDATE PROFILE: %s -> %s\n", tmp->lex, tmp->profile);
  }
  insert_node('B', lex, type, "");
  return;
}

char *profile_type_to_str(int type) {
  char *str;
  switch(type) {
  case 1:
    str = "INT";
    break;
  case 2:
    str = "REAL";
    break;
  case 3:
    str = "AINT";
    break;
  case 4:
    str = "AREAL";
    break;
  case 5:
    str = "PPINT";
    break;
  case 6:
    str = "PPREAL";
    break;
  case 7:
    str = "PPAINT";
    break;
  case 8:
    str = "PPAREAL";
    break;
  case 9:
    str = "PGPARAM";
    break;
  }
  return str;
}
