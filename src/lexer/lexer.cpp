#include <vector>
#include <string>
#include <fstream>

#include "token-lib.cpp"


using namespace std;


class Lexer {
public:
    ifstream code_file;
    vector<int> code;

    explicit Lexer(const string& file_name) {
        code_file.open(file_name, ios::in);
    }
    string get_word() {


        return "Hello";
    }

    vector<Token> parse() {

        vector<Token> res;

        while (true) {
            string word = get_word();
            int word_id = tokens.at(word);

            switch (word_id) {
                case 1:
                    res.emplace_back(word, word_id);
            }
        }

        return res;
    }
};


