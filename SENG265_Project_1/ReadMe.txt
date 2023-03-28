
This program is designed to take in a series of lines of text and create an alphabetical index of all the words 
found in the lines. The program also allows for the exclusion of certain words specified by the user.

Functions： The program contains the following functions:

	extract_exclude_words: extracts exclude words from user input
	deal_words: turns index words into a <length><word><line position> format
	tokenize_line: extracts index words from each line of text
	extract_index_words: extracts index words from all lines of text
	compare_words: compares two words for sorting
	final_print: formats and prints the final index output
