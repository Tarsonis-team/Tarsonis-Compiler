#pragma once

#include "body.hpp"
#include "declaration.hpp"
#include <memory>

namespace parsing
{

class Routine : public Declaration
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

    explicit Routine(std::string name) : Declaration(GrammarUnit::ROUTINE, std::move(name))
    {
        return_type = "";
    }

    std::shared_ptr<Body> m_body;
    std::vector<std::shared_ptr<RoutineParameter>> m_params;
    std::string return_type;
};

} // namespace parsing
