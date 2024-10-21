#pragma once

#include "parser/AST-node.hpp"

#include <memory>
#include <vector>

/*
 *  This class is responsible for application of various 
 *  checks and optimisation techniques to the program AST.
 *
 *       It is supposed to be used in cascading. 
 *          (our favourite builder pattern)
*/
class Analyzer
{
public:
    explicit Analyzer(std::shared_ptr<parsing::Program> program);

    template <typename Check>
    Analyzer& withCheckOf()
    {
        Check instance(m_program);
        std::vector<std::string> errors = instance.checkErrors();
        if (!errors.empty())
        {
            for (auto& err : errors)
            {
                m_errors.push_back(err);
            }
        }
        return *this;
    }

    template <class Optimization>
    Analyzer& withOptimizationOf()
    {
        /* 
         * No need to apply any operations 
         * as program contains errors
        */
        if (!m_errors.empty())
        {
            return *this;
        }
        Optimization opt(m_program);
        opt.apply();
        return *this;
    }

    void done();

private:
    std::shared_ptr<parsing::Program> m_program;
    std::vector<std::string> m_errors;
};
