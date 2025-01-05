#pragma once
#include "../lexser/token.h"
#include "ast_token.h"

class TokenConventor {
    public:

    static simple_parser::AstToken* convert(simple_lexser::Token_t& token, 
        simple_parser::Parser& parser) {
        struct simple_parser::TokenMessager messager;
        messager.index = token.index;
        messager.line = token.line;
        messager.value = token.value;
        messager.type = (simple_parser::AstTokenType)token.type;

        switch(token.type) {
            case simple_lexser::token_type::NUM:
            case simple_lexser::token_type::STRING:
            case simple_lexser::token_type::IDENTIFIER: {
                return new simple_parser::LiteralAstToken(messager, parser);
            }
            case simple_lexser::token_type::END: {
                // can act as expression's ending
                return new simple_parser::EndAstToken(messager, parser);
            }
            case simple_lexser::token_type::RESERVE: {
                if ((messager.value == "*") || (messager.value == "/") || (messager.value == "%")) {
                    return new simple_parser::InfixAstToken(messager, parser, simple_parser::BP_FACTOR);
                }
                if ((messager.value == "+") || (messager.value == "-")) {
                    return new simple_parser::PreAndInfixAstToken(messager, parser, simple_parser::BP_TERM);
                }
                if (messager.value == "(") {
                    return new simple_parser::ParentAstToken(messager, parser, simple_parser::BP_CALL);
                }
                if (messager.value == ")" || messager.value == "}" || messager.value == ";" || messager.value == ",") {
                    // can act as expression's ending
                    return new simple_parser::InfixAstToken(messager, parser, simple_parser::BP_NONE);
                }
                if ((messager.value == "!") || (messager.value == "~")) {
                    return new simple_parser::PrefixAstToken(messager, parser);
                }
                if (messager.value == "=") {
                    return new simple_parser::RAffinityInfixAstToken(messager, parser, simple_parser::BP_ASSIGN);
                }
                if ((messager.value == "<") || (messager.value == ">") || 
                    (messager.value == "<=") || (messager.value == ">=")) {
                    return new simple_parser::RAffinityInfixAstToken(messager, parser, simple_parser::BP_CMP);
                }
                if ((messager.value == "==") || (messager.value == "!=")) {
                    return new simple_parser::RAffinityInfixAstToken(messager, parser, simple_parser::BP_EQUAL);
                }
                if (messager.value == "||") {
                    return new simple_parser::RAffinityInfixAstToken(messager, parser, simple_parser::BP_LOGIC_OR);
                }
                if (messager.value == "&&") {
                    return new simple_parser::RAffinityInfixAstToken(messager, parser, simple_parser::BP_LOGIC_AND);
                }
                if (messager.value == "if") {
                    return new simple_parser::IfAstToken(messager, parser);
                }
                if (messager.value == "{") {
                    return new simple_parser::BracketAstToken(messager, parser);
                }
                if (messager.value == "while") {
                    return new simple_parser::WhileAstToken(messager, parser);
                }
                if (messager.value == "return") {
                    return new simple_parser::ReturnAstToken(messager, parser);
                }
                if (messager.value == "break") {
                    return new simple_parser::ReturnAstToken(messager, parser);
                }
                if (messager.value == "function") {
                    return new simple_parser::FuncAstToken(messager, parser);
                }
                if (messager.value == "var") {
                    return new simple_parser::VarAstToken(messager, parser);
                }                             
            }
            default: {
                printf("unable parse token < %s >, line %d index %d\n", 
                    token.value.c_str(), token.line, token.index);
                exit(-1);
            }
        }
    }

};