#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <map>

namespace lexical
{

/*
* I'm planning to move the usage of this class to compile time.
* So lets not bother creating a .cpp file yet.
*/
class SequenceBreaker
{
    struct clause {
        std::function<bool(char)> condition_before;
        std::function<bool(char)> condition_after;
    };
public:
    explicit SequenceBreaker(std::string to_break) : m_to_break(std::move(to_break))
    {
    }

    SequenceBreaker& breakBy(std::string by)
    {
        m_break_by.push_back(std::move(by));
        return *this;
    }
    
    /*
     * This method allows to specify what sequence of characters you want to except.
     * What means "except"? It means not to break the sequence in case a certain 
     * criterion is met, e.g. a sequence is between two characters, satisfying a 
     * provided predicate (e.g. it is a digit!) 
    */
    SequenceBreaker& except(std::string what) {
        m_excepted = std::move(what);
        return *this;
    }

    /*
     * This method allows to specify predicates that should check characters 
     * between a sequence. (see the except method)
    */
    SequenceBreaker& between(auto before, auto after) {
        m_clauses.insert({m_excepted, {before, after}});
        return *this;
    }

    std::vector<std::string> done() const
    {
        std::vector<std::string> result;

        /*
         * For now I'm writing an inefficient algorithm
         * that will break stuff in O(k * n * m),
         * k - number of signatures to be broken by
         * m - maximum length of signature
         * n - size of sequence to break
         * 
         * But we know that reserved keywords are merely
         * 1-2 characters, so we do have O(k * n)
        */
        size_t last_token = 0;
        for (size_t i = 0; i < m_to_break.size(); ++i)
        {
            for (const auto& keywd : m_break_by)
            {
                if (m_to_break.size() < i + keywd.size())
                {
                    continue;
                }
                const std::string_view window = std::string_view(m_to_break).substr(i, keywd.size());

                if (window == keywd)
                {
                    if (i != 0 && i + keywd.size() < m_to_break.size() && m_clauses.contains(keywd)) {  // if a certain keyword is excepted in certain conditions
                        bool some_condition_met = false;
                        for (auto it = m_clauses.lower_bound(keywd); it != m_clauses.upper_bound(keywd); it++) {
                            if (it->second.condition_before(m_to_break[i-1]) && it->second.condition_after(m_to_break[i + keywd.size()])) {
                                // both conditions are met, we do not break by this particular sequence in this window.
                                some_condition_met = true;
                                break;
                            }
                        }
                        if (some_condition_met)
                            continue;
                    }

                    if (i - last_token > 0)
                        result.push_back(m_to_break.substr(last_token, i - last_token));

                    result.push_back(keywd);
                    last_token = i + keywd.size();
                    i += keywd.size() - 1;
                }
            }
        }
        if (last_token != m_to_break.size())
            result.push_back(m_to_break.substr(last_token, m_to_break.size() - last_token));

        return result;
    }

private:
    std::vector<std::string> m_break_by;
    const std::string m_to_break;
    std::multimap<std::string, clause> m_clauses;
    std::string m_excepted;
};

} // namespace lexical
