/*
--- Day 4: High-Entropy Passphrases ---

A new system policy has been put in place that requires all accounts to use a 
passphrase instead of simply a password. A passphrase consists of a series of 
words (lowercase letters) separated by spaces.

To ensure security, a valid passphrase must contain no duplicate words.

For example:

aa bb cc dd ee is valid.
aa bb cc dd aa is not valid - the word aa appears more than once.
aa bb cc dd aaa is valid - aa and aaa count as different words.
The system's full passphrase list is available as your puzzle input. How many 
passphrases are valid?
*/

#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, const char** argv) {
	std::fstream input;
	input.open("input");
	int result = 0;
	while(!input.eof()) {
		char buf[4096];
		input.getline(buf, 4096);
		std::stringstream lineStream{std::string(buf)};

		std::set<std::string> words;
		bool valid = true;
		while(!lineStream.eof()) {
			std::string cur_word;
			lineStream >> cur_word;
			std::sort(cur_word.begin(), cur_word.end());
			if (words.find(cur_word) == words.end()) {
				words.insert(cur_word);
			}
			else {
				std::cout << "Rejected: " << buf << std::endl;
				valid = false;
				break;
			}
		}
		if (valid) result++;
	}

	std::cout << result << std::endl;
	
	return 0;
}
