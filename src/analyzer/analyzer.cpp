#include "analyzer.hpp"
#include "parser/AST-node.hpp"
#include <memory>

/*
 * Collect symbol table
*/
Analyzer::Analyzer(std::shared_ptr<parsing::Program> program) : m_program(program)
{
}
