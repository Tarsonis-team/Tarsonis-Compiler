#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "lexer/sequence-breaker.hpp"

using namespace lexical;

TEST(SeqBreakerTest, ConstantParsingTest)
{
    std::string int_const = "420";
    std::string real_const = "420.69";
    std::string int_const_cmplx = ";420\n";
    std::string real_const_cmplx = ":=420.69;";

    auto broken = SequenceBreaker(int_const)
                      .breakBy(";")
                      .breakBy(".")
                      .breakBy("\n")
                      .except(".")
                      .between(isdigit, isdigit)
                      .done();
    EXPECT_EQ(broken.size(), 1);
    EXPECT_EQ(broken.at(0), "420");

    broken = SequenceBreaker(real_const)
                 .breakBy(";")
                 .breakBy(".")
                 .breakBy("\n")
                 .except(".")
                 .between(isdigit, isdigit)
                 .done();
    EXPECT_EQ(broken.size(), 1);
    EXPECT_EQ(broken.at(0), "420.69");

    broken = SequenceBreaker(int_const_cmplx)
                 .breakBy(";")
                 .breakBy(".")
                 .breakBy("\n")
                 .except(".")
                 .between(isdigit, isdigit)
                 .done();
    EXPECT_EQ(broken.size(), 3);
    EXPECT_EQ(broken.at(0), ";");
    EXPECT_EQ(broken.at(1), "420");
    EXPECT_EQ(broken.at(2), "\n");

    broken = SequenceBreaker(real_const_cmplx)
                 .breakBy(";")
                 .breakBy(".")
                 .breakBy(":=")
                 .breakBy("\n")
                 .except(".")
                 .between(isdigit, isdigit)
                 .done();
    EXPECT_EQ(broken.size(), 3);
    EXPECT_EQ(broken.at(0), ":=");
    EXPECT_EQ(broken.at(1), "420.69");
    EXPECT_EQ(broken.at(2), ";");
}

TEST(SeqBreakerTest, Arithmetics)
{
    std::string statement = "arr[13+2]:=(323-a+12/3);\n";

    auto broken = SequenceBreaker(statement)
                      .breakBy(";")
                      .breakBy(".")
                      .breakBy("[")
                      .breakBy("]")
                      .breakBy(")")
                      .breakBy("(")
                      .breakBy("+")
                      .breakBy("-")
                      .breakBy("/")
                      .breakBy(":=")
                      .breakBy("\n")
                      .except(".")
                      .between(isdigit, isdigit)
                      .done();
    std::vector<std::string> expected{ "arr", "[", "13", "+",  "2", "]", ":=", "(", "323",
                                       "-",   "a", "+",  "12", "/", "3", ")",  ";", "\n" };

    EXPECT_EQ(broken.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(broken.at(i), expected.at(i));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
