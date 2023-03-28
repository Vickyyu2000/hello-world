import sys
import re
import fileinput

"""Citation: The algorithm and most codes for following concordance is from my own a2 work"""

class concord:

	def __init__(self, input=None, output=None):
		self.input = input
		self.output = output
		self.inputLines = self.__inputLines()
		self.excludeWords = self.__excludeWords()	
		self.indexLines = self.__indexLines()
		self.indexWords = self.__indexWords()
		if(self.output != None):
			f = open(self.output,mode= "w")
			for line in self.full_concordance():
				line = line + "\n"
				f.writelines(line)
			f.close()
		
	#return: a list of strings corresponding to the outputlines required(without \n at the end)
	def full_concordance(self):
		outputLines = self.__finalOutput()

		return outputLines
	
	#store lines without /n to a list from input and return this list
	def __inputLines(self):
		inputLines = []
		for line in fileinput.input(self.input):
			line = line.strip()
			inputLines.append(line)
		return inputLines

	#store excludeWords into a list and return this list
	def __excludeWords(self):
		excludeWords = []
		start = self.inputLines.index("''''")
		end = self.inputLines.index('""""')
		[excludeWords.append(elem) for elem in self.inputLines if (start < self.inputLines.index(elem) < end)]
		return excludeWords
	
	#store indexLines into a list and return this list
	def __indexLines(self):
		indexLines = []
		start = self.inputLines.index("''''")
		end = self.inputLines.index('""""')

		[indexLines.append(elem) for elem in self.inputLines if (self.inputLines.index(elem) > end)]

		return indexLines
	
	#store indexWords into a list with no case insensitive duplicates and return this list
	#i.e, "Apple" and "apple" could both appear in this list
	def __indexWords(self):
		index_words = []
		
		for line in self.indexLines:
			split_line = line.split()
			index_words.extend(split_line)

		index_words.reverse()
		for word in index_words:
			while index_words.count(word) > 1:
				index_words.remove(word)

		index_words.reverse()
		
		for word in self.excludeWords:	#remove any exclude word from index words
			for elem in index_words:
				if elem.casefold() == word.casefold():
					index_words.remove(elem)
	

		index_words = sorted(index_words, key=str.lower)
		
		return index_words
	
	#organize the final output
	#return:a list of output lines required
	def __finalOutput(self):
		final = []
		for word in self.indexWords:
			
			for line in self.indexLines:
				
				search  = re.search(r"\b"+word+r"\b", line)
				if search:
					newLine = re.sub(r"\b"+word+r"\b", word.upper(), line, count = 1)
					alignedLine = self.__align(newLine, word.upper())
					final.append(alignedLine)
				else:
					continue
		return final
	
	#helper method of __finalOutput: align each index line
	#parameter:a single line to be aligned & indexed Word in this line
	#return:a string of aligned line
	def __align(self, newLine, indexWord):
		mid_list = [1]
		leftRem = 29
		rightRem = 30 - len(indexWord)
		split_line = newLine.split()
		index = split_line.index(indexWord)
		mid_list[0] = indexWord
		left_list = []
		right_list = []
		left_sub = split_line[:index]
		right_sub = split_line[index:]
		right_sub.pop(0)
		
		while(left_sub):
			if leftRem - (len(left_sub[-1]) + 1) >= 9:
				leftRem -= (len(left_sub[-1]) +1)
				left_list.insert(0, left_sub.pop())
			else:
				left_blank = " " * (leftRem - 1)
				left_list.insert(0, left_blank)
				break
		
		if not left_sub:
			left_blank = " " * (leftRem - 1)
			left_list.insert(0, left_blank)

		while(right_sub):
			if rightRem - ((len(right_sub[0])) + 1) >= -1:
				rightRem -= (len(right_sub[0]) + 1)
				right_list.append(right_sub.pop(0))
			else:
				break
		
		alignedLine = " ".join(left_list+mid_list+right_list)

		return alignedLine








