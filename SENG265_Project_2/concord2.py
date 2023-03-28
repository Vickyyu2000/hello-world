#!/usr/bin/env python3
import fileinput
import sys

def main():
	
	lines = sys.stdin.readlines()
	
	#exit the program and produce a message if an assignment #1 test file is provided
	if lines[0] == "1\n":
		print("Input is version 1, concord2.py expected version 2")
		exit()

	exclude_words, index_lines = extract_excludeWords_and_lines(lines)
	
	exclude_words.sort()

	unaligned_output, index_words, unaligned_dic  = unaligned_output_lines(exclude_words, index_lines)
		

	final_print = final_output(unaligned_output, unaligned_dic)
	for line in final_print:
		print(line)
		
def final_output(unaligned_output, unaligned_dic):
	"""parameter 1: a list of unleft-aligned output (same result as in assigment #1)
	   parameter 2: a dictionary -key: a string of a line in unleft-aligned output
	   							 -value: the index number of the word to be left-aligned in the split list of key
	
	   return: a list of output which each line with a left-aligned index, Words to the left of the index word
	   		   may appear but only if they do not go further left than column 10. Words to the right of the 
			   index word may appear but only if they do not go further right than column 60"""
	
	final_list = []
	middle_list = [1]
	

	for line in unaligned_output:
		
		pos_of_index = unaligned_dic[line]
		split_line = line.split()
		word = split_line[pos_of_index].upper()

		left_remain = 29
		right_remain = 30 - len(word)
		left_list = []	#modified words to the left of the index word and store in this list
		right_list = []	#modified words to the right of the index word and store in this list
		left_sub = split_line[:pos_of_index]
		right_sub = split_line[pos_of_index:]
		right_sub.pop(0)
		middle_list[0] = word
						
		while(left_sub):	
			if left_remain - (len(left_sub[-1]) + 1) >= 9:
				left_remain -= (len(left_sub[-1]) + 1)
				left_list.insert(0, left_sub.pop())
			else:
				left_blank = " " * (left_remain - 1)
				left_list.insert(0, left_blank)
				break

		if not left_sub:
			left_blank = " " * (left_remain - 1)
			left_list.insert(0, left_blank)

		while(right_sub):

			if right_remain - ((len(right_sub[0])) + 1) >= -1:
				right_remain -= (len(right_sub[0]) + 1)
				right_list.append(right_sub.pop(0))
			else:
				break
				
		new_line = " ".join(left_list+middle_list+right_list)	
		final_list.append(new_line)

	return final_list
	

def unaligned_output_lines(exclude_words, index_lines):
	"""parameter 1: a list of exclude words
	   parameter 2: a list of index lines
	   return: 1. a list of unleft-aligned output (same result as in assginment #1)
	           2. a list of sorted index words
			   3. a dictionary: -key: a string of a line in unleft-aligned output
			   			  		 -value: the index number of the word to be left-aligned in the split list of key"""

	unaligned_output = []
	unaligned_output_with_index = {}
	index_words = []
	
	for line in index_lines:	#split every single word in index lines and collect into list index_words
		
		split_line = line.split()
		index_words.extend(split_line)
	
	#remove case-insensitive duplicates from index_words
	temp_dict = {}
	temp_list = []
	for word in index_words:
		if word.lower() in temp_dict:
			continue
		else:
			temp_dict[word.lower()] = word.lower()
			temp_list.append(word)
	
	index_words = temp_list
	
	for word in exclude_words:	#remove any exclude word from index words
		for elem in index_words:
			if elem.casefold() == word.casefold():
				index_words.remove(elem)
	
	index_words = sorted(index_words, key=str.lower)
	

	for word in index_words:

		temp_word = word.lower()
		
		for line in index_lines:
			
			temp_line = line.lower()
			split_temp = temp_line.split()
			split_line = line.split()
			num_occur =split_temp.count(temp_word)
			
			if num_occur == 0:
		
				continue
			
			else:
				
				index = split_temp.index(temp_word)
				second_half = split_temp[index:]
				
				while(temp_word in second_half):
					
					second_half[0] = temp_word.upper()
					split_line[index] = temp_word.upper()
					new_line = " ".join(split_line)
					unaligned_output.append(new_line)
					unaligned_output_with_index[new_line] = index		
					
					if temp_word in second_half:
						
						index = index + second_half.index(temp_word)
						second_half = split_temp[index:]
					
					else:
						break
			
	return unaligned_output, index_words, unaligned_output_with_index
					
def extract_excludeWords_and_lines(lines):
	"""This function extracts exclude words and index lines 
	   from the input file into list exclude_words and list index_lines.
	   parameter: a list of lines read from input file which with final new lines
	   return: 1. a list of exclude words
	   		   2. a list of index lines which stripped off the final new lines"""
	striped_lines = []

	for elem in lines:
		striped_lines.append(elem.strip())
	
	exclude_words = []
	index_lines = []
	start_exclude = striped_lines.index("''''")
	end_exclude = striped_lines.index('""""')
	
	[exclude_words.append(elem) for elem in striped_lines if (start_exclude < striped_lines.index(elem) < end_exclude)]
	
	[index_lines.append(elem) for elem in striped_lines if (striped_lines.index(elem) > end_exclude)]
	
	return exclude_words, index_lines

if __name__ == "__main__":
	main()


