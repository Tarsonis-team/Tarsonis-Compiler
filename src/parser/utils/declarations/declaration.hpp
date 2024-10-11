#include "AST-node.hpp"
#include <vector>

using namespace parsing;

class Variable : Declaration {

};

class RoutineParameter : Declaration {

};

class Routine : Declaration {

public:
    Body m_body;
    std::vector<RoutineParameter> m_params;
};


class Record : Declaration {

};

class Type : Declaration {

};

class Array : Declaration {
    
};



