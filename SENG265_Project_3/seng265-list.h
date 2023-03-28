#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#define LIST_MAXLEN_TEXT 100
#define KEY_MAXLEN_TEXT 40
typedef struct node_t {
    char key[KEY_MAXLEN_TEXT+1];
    char text[LIST_MAXLEN_TEXT+1];
    struct node_t  *next;
} node_t;

node_t *new_node(char *, char*);
node_t *add_front(node_t *, node_t *);
node_t *add_end(node_t *, node_t *);
node_t *peek_front(node_t *);
node_t *remove_front(node_t *);
int len_node_t(node_t *);
void apply(node_t *, void(*fn)(node_t *, void *), void *arg);

#endif
