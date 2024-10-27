#pragma once

enum class GrammarUnit
{
    UNDEFINED,
    PROGRAM,
    BODY,
    RANGE,
    IDENTIFIER,
    VARIABLE,
    ROUTINE,
    PARAMETER,
    RECORD,
    TYPE,
    ARRAY_TYPE,
    RECORD_TYPE,
    ALIAS,
    ARRAY,
    IF,
    FOR,
    WHILE,
    CALL,
    ASSIGNMENT, // is
    INTEGER,
    BOOL,
    REAL,
    ACCESS_ARRAY,
    ACCESS_RECORD,
    PLUS,
    MINUS,
    MULTIPLICATE,
    DIVISION,
    MOD,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,
    EQUAL,
    NOT_EQUAL,
    AND,
    OR,
    XOR,
    TRUE,
    FALSE,
    ROUTINE_CALL
};