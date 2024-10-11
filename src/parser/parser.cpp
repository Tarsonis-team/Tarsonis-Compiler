#include <vector>
#include "parser.hpp"
#include "token.hpp"
#include "utils/declarations/AST-node.hpp"
#include "utils/declarations/declaration.hpp"

namespace parsing {

ASTNode Parser::parse() {
    
    int cur_token = 0;
    while (true) {
        auto token = m_tokens[cur_token];

        switch (token.m_id) {
            case 1: // routine
                Routine::
        }

    }
    
    return {};
}

}
