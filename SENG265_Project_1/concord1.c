#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFLEN 100
#define MAXWORDS 100
#define LINEMAXCHAR 70
#define WORDMAXCHAR 22

void print_line_upper(char *line, char *word);
int is_in_exclude_words(char exclude_words[][20], int num_exclude_words, char *t);
void final_print(char index_lines[][70], char index_words[][22], int num_index_line, char exclude_words[][20],int num_exclude_words );
static int num_words = 0;
int extract_exclude_words(char *buffer, char exclude_words[][20]){
    fgets(buffer, sizeof(char) *BUFLEN, stdin);
    int count = 0;//# of exclude word - 1

    while(strncmp(buffer, "\"\"\"\"\n", 20) != 0){
		int len = strlen(buffer);
		if (buffer[len-1] == '\n'){
		    buffer[len-1] = '\0';
		}
	    	int repeat_flag = 0;
		for(int i = 0; i < count; i++){
		    if (strncmp(exclude_words[i], buffer, 20) == 0){
			repeat_flag = 1;
			break;
		    }
		}
	
		if (!repeat_flag){
		    strncpy(exclude_words[count], buffer, strlen(buffer));
		    count++;
		}
		fgets(buffer, sizeof(char) *BUFLEN, stdin);
    }
    return count;
}
/*This function is turning every index word into a <length><word><line position> format. 
example: 
index_line[0]: aaa bbb ccc
index_words[0] = "3aaa0"*/
void deal_words(char *word, int word_size, int line_num){
	word[0] = word_size;
	if (word[word_size]=='\n'){
		word[0] = (char)(word_size-1);
		word[word_size] = line_num+1;	
		word[word_size+1] = '\0';
	}else{
		word[word_size+1] = line_num+1;
		word[word_size+2] = '\0';
	}
	
}
//part of following lines are from Dr.Zastre's in-class code
void tokenize_line (char *input_line, char index_words[][22] , int line_num, char exclude_words[][20], int num_exclude_word){
    char *t;
    int word_size = 0;
    t = strtok (input_line, " ");
    while (t) {
	int exclude_flag = is_in_exclude_words(exclude_words, num_exclude_word, t);
	if(exclude_flag == 1){
		t = strtok(NULL, " ");
	}else{
        	strncpy (index_words[num_words]+1, t, WORDMAXCHAR);
		word_size = strlen(t);
		deal_words(index_words[num_words], word_size, line_num);	
	        num_words++;
		t = strtok(NULL, " ");        
    	}
    }
    return;
}
//end of citation
void extract_index_words(char index_lines[][70], int num_index_line, char index_words[][22] ,char exclude_words[][20], int num_exclude_word){
	for( int i = 0; i < num_index_line; i++){
		char temp_line[70];
		strncpy(temp_line,index_lines[i],70);
		tokenize_line(temp_line, index_words, i, exclude_words, num_exclude_word);		
	}

}



int compare_words(const void *wa, const void *wb) {
    char *p_wa = (char *)wa;
    char *p_wb = (char *)wb;
    char wa_dest[20];
    char wb_dest[20];
    int len_a = p_wa[0];
    int len_b = p_wb[0];
    strncpy(wa_dest, p_wa+1, len_a+1);
    strncpy(wb_dest, p_wb+1, len_b+1);
    
    return strcmp(wa_dest, wb_dest);
}

int main(int argc, char *argv[]) {
    char buffer[BUFLEN];
    int num_exclude_words=0;// # of exclude words

    char exclude_words[20][20];
    char index_lines[100][70]; 
    char index_words[100][22];
    //part of following lines are from Dr.Zastre's in-class code.
    while (fgets(buffer, sizeof(char) * BUFLEN, stdin)) {
	if (strncmp(buffer, "\'\'\'\'\n", 20) == 0){
	    num_exclude_words =  extract_exclude_words(buffer, exclude_words);
	    break;
	}
        num_exclude_words++;// # of exclude words
    } 
    //at this time, buffer is at """"
    int num_index_line = 0;//position of index lines
    while (fgets(buffer, sizeof(char) * BUFLEN, stdin)!=NULL) {
	strncpy(index_lines[num_index_line], buffer, strlen(buffer)-1);
        num_index_line++;
    }
    
    extract_index_words(index_lines, num_index_line, index_words, exclude_words, num_exclude_words);

    qsort(index_words, num_words, sizeof(index_words[0]), compare_words);   

    final_print(index_lines,index_words,num_index_line,exclude_words,num_exclude_words); 
    return 0;
}

void final_print(char index_lines[][70], char index_words[][22], int num_index_line, char exclude_words[][20],int num_exclude_words ){
	for(int i = 0; i< num_words; i++){
		int word_len = (int)index_words[i][0];	
		int temp_line_num = (int) index_words[i][word_len+1]-1;
		char temp_word[word_len];
		strncpy(temp_word, index_words[i]+1,word_len);
		temp_word[word_len] = '\0';
		print_line_upper(index_lines[temp_line_num], temp_word);
	}
}
void word_upper(char *word){
	for(int i = 0; i< strlen(word); i++){
		if(word[i]<='z'&&word[i]>='a'){
			word[i] += 'A' - 'a';
		}
	}
}

void print_line_upper(char *line, char *word){
    char *t;
    int word_size = 0;
    char new_line[70];
    strncpy(new_line, line, 70);
    t = strtok (new_line, " ");
    char print_line[70];
    int print_line_idx = 0;
    while (t) {
	if(strncmp(t,word,20)==0){
		word_upper(t);
	}
	for(int i = 0; i < strlen(t); i++){
		print_line[print_line_idx] = t[i];
		print_line_idx++;
	}
	
	print_line[print_line_idx] = ' ';
	print_line_idx++;
	t = strtok(NULL, " ");
    }
    print_line[print_line_idx - 1] = '\n';
    print_line[print_line_idx] = '\0';
    printf("%s",print_line);
    return;
}
int is_in_exclude_words(char exclude_words[][20], int num_exclude_words, char *t){
	
	int len = strlen(t);
	if (t[len-1] == '\n'){
		t[len-1] = '\0';
	}
	for(int i = 0; i < num_exclude_words; i++){
		if(strncmp(t, exclude_words[i], 20) == 0){
			
			return 1;
		}

	}
	return 0;
}
