#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#define LEXER_TOKENS "{};"

typedef std::vector<std::pair<int, std::string>> lex_bugs;

void lexar(std::vector<std::pair<int, std::string>> &words, std::ifstream &ifs)
{
	int line_count = 0;
	int pos;
	int pos_end;

	for (std::string line_buf; std::getline(ifs, line_buf);) {
		std::replace(line_buf.begin(), line_buf.end(), '\t', ' ');
		std::replace(line_buf.begin(), line_buf.end(), '\r', ' ');
		line_count++;
		pos = 0;

		while (pos < line_buf.size()) {
				std::cout << pos << std::endl;
			switch (line_buf[pos]) {
				case ' ':
					pos = line_buf.find_first_not_of(" ", pos);
					break;
				case '{':
				case '}':
				case ';':
					words.push_back(std::make_pair(line_count, std::string(1, line_buf[pos])));
					++pos;
					break;
				default:
					pos_end = line_buf.find_first_of("{}; ", pos);
					words.push_back(std::make_pair(line_count, line_buf.substr(pos, pos_end - pos)));
					pos = pos_end;
					break;
			}
		}
	}
} 

int main()
{
	lex_bugs lex;

	std::ifstream ifs("test.txt");
	// std::vector<std::string> contents;
	std::string str;

	if (ifs.fail()) {
		perror(NULL);
		return (1);
	}

	lexar(lex, ifs);
	for (lex_bugs::iterator it = lex.begin(); it != lex.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}
