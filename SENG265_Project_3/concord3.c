/*
 * concord3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2022.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "seng265-list.h"

#define LINE_CHAR_NUM_UPLIMIT LIST_MAXLEN_TEXT
#define WORD_START_NUM 30
#define WORD_RIGHT_LIMIT 60
#define WORD_LEFT_LIMIT 10


void print_key(node_t *node, void *arg)
{
    char *format = (char *)arg;
    printf(format, node->key);
}

void print_text(node_t *node, void *arg)
{
    char *format = (char *)arg;
    printf(format, node->text);
}

int my_getline(char* line, int max_size) {
    int c;
    int len = -1;
    while ((c = getchar()) != EOF && len < max_size) {
    	if(len < 0) {
    		len = 0;
		}
        if ('\n' == c) {
            break;
        } else {
            line[len++] = c;
        }
    }
    line[len] = '\0';
    return len;
}


void upper_keyword_list(node_t *keyword_list) {
    node_t *temp = keyword_list;
    while(temp != NULL) {
        for(int i=0;i<KEY_MAXLEN_TEXT && temp->key[i] != '\0';i++) {
            if(temp->key[i] >= 'a' && temp->key[i] <= 'z') {
                temp->key[i] = temp->key[i] + 'A' - 'a';
            }
        }
        temp = temp->next;
    }
}

int init_two_list(node_t *keyword_list, node_t *text_list) {
    char str[LINE_CHAR_NUM_UPLIMIT];
    char key_word_start[] = "''''";
    char key_word_end[] = "\"\"\"\"";
    int count = 0;
    int  is_first_line = 1;
    int is_keyword_start = 0;
    int is_keyword_end = 0;
    int versionNum = -1;
    while(my_getline(str, LINE_CHAR_NUM_UPLIMIT) >= 0) {
        if(is_first_line) {
            versionNum = atoi(str);
            if(versionNum != 2) {
                printf("Input is version %d, concord3 expected version 2\n", versionNum);
                return -1;
            }
            is_first_line = 0;
        } else {
            if(!is_keyword_start) {
                if(strcmp(key_word_start, str) == 0) {
                    is_keyword_start = 1;
                }
            } else {
                if(!is_keyword_end) {
                    if(strcmp(key_word_end, str) == 0) {
                        is_keyword_end = 1;
                    } else {
                        if(strlen(str) > 0) {
                            node_t  *temp_node = new_node(str, NULL);
                            keyword_list = add_end(keyword_list, temp_node);
                        }
                    }
                } else {
                    if(strlen(str) > 0) {
                        node_t  *temp_node = new_node(NULL, str);
                        text_list = add_end(text_list, temp_node);
                    }
                }
            }
        }
        count++;
    }
    return 0;
}

int is_word_in_keyword_list(node_t *keyword_list, char word[]) {
	node_t *temp_list = keyword_list;
	while(temp_list != NULL) {
		if(strncmp(temp_list->key, word, KEY_MAXLEN_TEXT) == 0) {
			return 1;
		}
		temp_list = temp_list->next;
	}
	return 0;
}



void init_result_node(node_t *keyword_list, node_t *text_list, node_t *result_list) {
	node_t *temp_text_list = text_list;
	while(temp_text_list != NULL) {
		int start = -1;
		int end = -1;
		int word_found = 0;
		char line[LIST_MAXLEN_TEXT];
		strncpy(line, temp_text_list->text, LIST_MAXLEN_TEXT);
		temp_text_list = temp_text_list->next; 
		char word[KEY_MAXLEN_TEXT];
		int len = strlen(line);
		start = 0;
		while(start < len && end < len) {
			while(start<len) {
				if(line[start] != ' ') {
					word_found = 1;
					break;
				} else {
					start++;
				}
			}
			if(word_found) {
				for(end=start+1;end<len;) {
					if(line[end] == ' ' || line[end] == '\0') {
						break;
					} else {
						end++;
					}
				}
				int i=start, j=0;
				for(;i<end;i++, j++) {
					word[j] = line[i];
                    if(word[j] >= 'a' && word[j] <= 'z') {
                        word[j] = word[j] + 'A' - 'a';
                    }
				}
				word[j] = '\0';
				int is_keyword = is_word_in_keyword_list(keyword_list, word);
				if(!is_keyword) {
					int word_len = end - start;
					if(WORD_START_NUM + word_len <= WORD_RIGHT_LIMIT) {
						for(i=0;i<word_len;i++) {
							if(word[i] >= 'a' && word[i] <= 'z') {
	                          word[i] = word[i] + 'A' - 'a';
	                       }
						}
						char text[WORD_RIGHT_LIMIT+1];
						text[WORD_RIGHT_LIMIT] = '\0';
						for(i=0;i < WORD_RIGHT_LIMIT;i++) {
							text[i] = ' ';
						}
						for(i = WORD_START_NUM - 1, j = 0; j < word_len; i++, j++ ) {
							text[i] = word[j];
						}
                        int target;
                        if(len-1-start <= WORD_RIGHT_LIMIT - WORD_START_NUM) {
                            target = len-1;
                            while(line[target] == ' ') {
                                target--;
                            }
                        } else {
                            target = start + WORD_RIGHT_LIMIT - WORD_START_NUM;
                            if(line[target] == ' ') {
                                while(line[target] == ' ') {
                                    target--;
                                }
                            } else {
                                if(line[target+1] != ' ') {
                                    while(line[target] != ' ' && target >= end -1) {
                                        target--;
                                    }
                                    while(line[target] == ' ' && target >= end -1) {
                                        target--;
                                    }
                                }
                            }
                        }
                        for(i = WORD_START_NUM + word_len - 1, j=end;j<=target;i++, j++) {
                            text[i] = line[j];
                        }
                        text[i] = '\0';
						target = 0;
                        if(start <= WORD_START_NUM - WORD_LEFT_LIMIT) {
                            target = 0;
                            while(target < start && line[target] == ' ') {
                                target++;
                            }
                        } else {
                            target = start - WORD_START_NUM + WORD_LEFT_LIMIT;
                            if(line[target] == ' ') {
                                while(line[target] == ' ') {
                                    target++;
                                }
                            } else {
                                if(line[target-1] != ' ') {
                                    while(line[target] != ' ' && target < start) {
                                        target++;
                                    }
                                    while(line[target] == ' ' && target < start) {
                                        target++;
                                    }
                                }
                            }
                        }
                        for(i=target;i<start;i++) {
                            text[WORD_START_NUM-1+i-start] = line[i];
                        }
						node_t *temp_node = new_node(word, text);
                        result_list = add_end(result_list, temp_node);
					}
				}
				start = end + 1;
                word_found = 0;
			} 
		}
	}
}



int key_compare(void const *a, void const *b) {
    return strcmp(((node_t*)a)->key, ((node_t*)b)->key);
}

int main(int argc, char *argv[])
{

    node_t *keyword_list = new_node("","");
    node_t *text_list = new_node("","");
    int res = init_two_list(keyword_list, text_list);
    if(res < 0) {
        exit(-1);
    } 
    node_t *temp;
    temp = keyword_list;
    keyword_list = remove_front(keyword_list);
    if(temp != NULL) {
    	free(temp);
	}
	temp = text_list;
    text_list = remove_front(text_list);
    if(temp != NULL) {
    	free(temp);
	}
    if(keyword_list == NULL && text_list == NULL) {
        exit(-1);
    } else {
        if(text_list == NULL) {
            exit(-1);
        }
        upper_keyword_list(keyword_list);
        node_t *result_node = new_node("", "");
        init_result_node(keyword_list, text_list, result_node);
        temp = result_node;
    	result_node = remove_front(result_node);
    	if(temp != NULL) {
    		free(temp);
		}
        int len = len_node_t(result_node);
        node_t array[len];
        temp = result_node;
        for(int i=0;i<len;i++) {
            array[i] = *temp;
            temp = temp->next;
        }
        qsort(array, len, sizeof (node_t), key_compare);
        for(int i=0;i<len;i++) {
            printf("%s\n", array[i].text);
        }
        while(result_node != NULL) {
        	temp = result_node;
        	result_node = result_node->next;
        	free(temp);
		}
		while(keyword_list != NULL) {
			temp = keyword_list;
        	keyword_list = keyword_list->next;
        	free(temp);
		} 
		while(text_list != NULL) {
			temp = text_list;
        	text_list = text_list->next;
        	free(temp);
		} 
    }
    exit(0);
}
