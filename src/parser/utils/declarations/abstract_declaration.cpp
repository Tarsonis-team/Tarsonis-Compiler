#include "category.hpp"
#include "span.cpp"

using namespace std;


class Declaration {
public:
    explicit Declaration(string& name, Category category, Span* span): name(name), category(category), span(span) {
        this->wasUsed = false;
    };
    explicit Declaration(Category category, Span* span): category(category), span(span) {
        this->wasUsed = false;
        this->name = "";
    }
    string name;
    Category category;
    // type
    // initializer
    Span* span;
    bool wasUsed;
};