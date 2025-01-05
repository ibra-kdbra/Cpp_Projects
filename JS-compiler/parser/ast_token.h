#pragma once
#include <iostream>
#include "bind_power.h"
#include "parser.h"
#include <vector>
#include <assert.h>
using namespace std;

namespace simple_parser {

enum AstTokenType {
    RESERVE = 0,
    NUM,
    STRING,
    IDENTIFIER,
    STATEMENTS,
    FUNCTION,
    END,

    INVALID,
};

static std::string token_type_to_string(AstTokenType type)
{
    switch (type) {
        case RESERVE: return "RESERVE";
        case NUM: return "NUM";
        case STRING: return "STRING";
        case IDENTIFIER: return "IDENTIFIER";
        case STATEMENTS: return "STATEMENTS";
        case FUNCTION: return "FUNCTION";
        case INVALID: return "INVALID";
        default:
            printf("unknown AstTokenType");
            exit(-1);
    }
}

struct TokenMessager
{
    int line;
    int index;
    string value;
    AstTokenType type;
    TokenMessager(): line(0), index(0), value(""), type(INVALID) {}
};

class AstToken {
    private:
    int line;
    int index;
    string value;
    AstTokenType type;
    string private_data;

    public:
    Parser& parser_instance;

    public:
    AstToken(TokenMessager& messager, Parser& p):parser_instance(p) {
        line = messager.line;
        index = messager.index;
        value = messager.value;
        type = messager.type;
    }

    string peek_value() {
        return this->value;
    }

    AstTokenType peek_type() {
        return this->type;
    }

    void set_type(AstTokenType type) {
        this->type = type;
    }

    int peek_line() {
        return this->line;
    }

    int peek_index() {
        return this->index;
    }

    vector<AstToken *> child_list;
    virtual AstToken* nud() {
        printf("syntax error: no nud at line %d, token index %d\n",
            line, index);
        exit(-1);
    }
    virtual AstToken* led(AstToken* left) {
        printf("syntax error: no led at line %d, token index %d\n",
            line, index);
        exit(-1);
    }
    virtual AstToken* std() {
        printf("syntax error: no std at line %d, token index %d\n",
            line, index);
        exit(-1);
    }
};

//child_list.size = 0, literal token
class LiteralAstToken : public AstToken {
    public:
    LiteralAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p){}
    AstToken* nud() {
        return this;
    }
};

class PrefixAstToken : public AstToken {
    public:
    PrefixAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p){}
    virtual AstToken* nud() {
        child_list.push_back(parser_instance.expression(static_cast<int>(BP_UNARY)));
        return this;
    }
};

class InfixAstToken : public AstToken {
    public:
    int bp;

    InfixAstToken(TokenMessager& messager, Parser& p, int bp): 
        AstToken(messager, p), bp(bp){}
    virtual AstToken* led(AstToken* left) {
        child_list.push_back(left);
        child_list.push_back(parser_instance.expression(bp));
        return this;
    }
};

// should inheritent from both infix and prefix
class PreAndInfixAstToken : public InfixAstToken {
    public:
    PreAndInfixAstToken(TokenMessager& messager, Parser& p, int bp): 
        InfixAstToken(messager, p, bp) {}
    virtual AstToken* nud() {
        child_list.push_back(parser_instance.expression(static_cast<int>(BP_UNARY)));
        return this;
    }    
};

class EndAstToken : public AstToken {
    public:
    EndAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {
        this->set_type(END);
    }
};

// "("
class ParentAstToken : public PreAndInfixAstToken {
    public:
    ParentAstToken(TokenMessager& messager, Parser& p, int bp):PreAndInfixAstToken(messager, p, bp) {}
    AstToken* nud() {
        AstToken* token = parser_instance.expression(static_cast<int>(BP_NONE));
        parser_instance.advance(")");
        delete(this);
        return token;
    }
    AstToken* led(AstToken* left) {
        assert(left->peek_type() == IDENTIFIER);
        child_list.push_back(left);
        while (parser_instance.peek_current_value() != ")") {
            child_list.push_back(parser_instance.expression(0));
            if (parser_instance.peek_current_value() == ")")
                break;
            parser_instance.advance(",");
        }
        parser_instance.advance(")");
        this->set_type(FUNCTION);
        return this;
    }
};

// right affinity such as "=" ">" "<" "&&" "||" ">=" "<=" "==" "!="
class RAffinityInfixAstToken : public InfixAstToken {
    public:
    RAffinityInfixAstToken(TokenMessager& messager, Parser& p, int bp): 
        InfixAstToken(messager, p, bp) {}
    AstToken* led(AstToken* left) {
        if (this->peek_value() == "=") // assgin
            assert(left->peek_type() == IDENTIFIER);
        child_list.push_back(left);
        child_list.push_back(parser_instance.expression(bp - 1)); // right affinity happens
        return this;        
    }    
};

// "{"
class BracketAstToken : public AstToken {
    public:
    BracketAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {} 
    AstToken* std() {
        AstToken *token = parser_instance.statements();
        parser_instance.advance("}");
        delete(this);
        return token;
    }
};

// "if"
class IfAstToken : public AstToken {
    public:
    IfAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {} 
    AstToken* std() {
        parser_instance.advance("(");
        this->child_list.push_back(parser_instance.expression(0));
        parser_instance.advance(")");
        this->child_list.push_back(parser_instance.block());
        if (parser_instance.peek_current_value() == "else" ) {
            parser_instance.advance("else");
            this->child_list.push_back(parser_instance.block());
        }

        return this;
    }
};

// "while"
class WhileAstToken : public AstToken {
    public:
    WhileAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {}
    AstToken *std() {
        parser_instance.advance("(");
        this->child_list.push_back(parser_instance.expression(0));
        parser_instance.advance(")");
        this->child_list.push_back(parser_instance.block());
        return this;
    }
};

// "return"
class ReturnAstToken : public AstToken {
    public:
    ReturnAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {}
    AstToken *std() {
        if (parser_instance.peek_current_value() != ";") {
            this->child_list.push_back(parser_instance.expression(0));
        }
        parser_instance.advance(";");
        return this;
    }
};

// "break"
class BreakAstToken : public AstToken {
    public:
    BreakAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {}
    AstToken *std() {
        parser_instance.advance(";");
        return this;
    }
};

// "function" define
class FuncAstToken : public AstToken {
    public:
    FuncAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {}
    AstToken *std() {
        AstToken *token = parser_instance.function();
        delete(this);
        return token;
    }
};

// "var"
class VarAstToken : public AstToken {
    public:
    VarAstToken(TokenMessager& messager, Parser& p): AstToken(messager, p) {}
    AstToken *std() {
        this->child_list.push_back(parser_instance.expression(0));
        parser_instance.advance(";");
        return this;
    }
};



}