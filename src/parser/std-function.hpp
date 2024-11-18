#pragma once

#include "AST-node.hpp"
#include "statement.hpp"
#include <memory>
#include <unordered_set>
#include <string>

namespace parsing
{

class StdFunction : public RoutineCall
{
public:
    void accept(IVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(IVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor& visitor) override
    {
        visitor.visit(*this);
    }

    void accept(ICompleteVisitor&& visitor) override
    {
        visitor.visit(*this);
    }

    static inline bool is_std_function(const std::string& name) {
        return std_functions.find(name) != std_functions.end();
    }

    explicit StdFunction(std::string name) : RoutineCall(name) {}
private:
    static inline const std::unordered_set<std::string> std_functions = {
        "print"
    };
};

// inline const std::unordered_set<std::string> std_functions = {
//     "print"
// };

// inline bool is_std_function(const std::string& name) {
//     return std_functions.find(name) != std_functions.end();
// }

} // namespace parsing