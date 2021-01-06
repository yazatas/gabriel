#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "ast.hh"
#include "parser.hh"
#include "util/log.hh"

#define CHANNEL "parser"

#define EXPECT(token, err) \
    do { \
        if (!tokens_.get(token)) { \
            ERROR(err); \
            return nullptr; \
        } \
    } while (0)

gcc::parser::parser():
    ast_(nullptr),
    tokens_()
{
}

gcc::parser::~parser()
{
}

gcc::prog_t *gcc::parser::get_prog()
{
    return prog_;
}

gcc::node_t *gcc::parser::make_node(token_type_t type, gcc::node_t *l, gcc::node_t *r)
{
    gcc::node_t *node = new gcc::node_t;

    node->l    = l;
    node->r    = r;
    node->type = type;

    return node;
}

gcc::node_t *gcc::parser::types()
{
    if (tokens_.get(TT_IDENTIFIER)) {
        if (tokens_.get(TT_LPAREN)) {
            EXPECT(TT_RPAREN, "Invalid token encountered: %d!\n");
            WARN("function\n");
        } else {
            DEBUG("ELSE variable %d\n", tokens_.get_current().type);
            gcc::node_t *node = new gcc::node_t;

            node->l    = nullptr;
            node->r    = nullptr;
            node->type = TT_VAR;

            return node;
        }
    }

    ERROR("What else could it be!\n\n");
}

gcc::node_t *gcc::parser::postfix()
{
    gcc::node_t *node = types();

    DEBUG("parse postfix\n");

    return nullptr;
}

gcc::node_t *gcc::parser::prefix()
{
    gcc::node_t *l = postfix();

    DEBUG("parse prefix\n");

    return l;
}

gcc::node_t *gcc::parser::mul_div()
{
    gcc::node_t *l = prefix();

    ERROR("unimplemented!\n");

    /* TODO: parse mul/div */

    return l;
}

gcc::node_t *gcc::parser::add_sub()
{
    gcc::node_t *l = mul_div();

    ERROR("unimplemented!\n");

    /* TODO: parse add/sub */

    return l;
}

gcc::node_t *gcc::parser::bitwise_shift()
{
    gcc::node_t *l = add_sub();

    ERROR("unimplemented!\n");

    /* TODO: parse <</>> */

    return l;
}

gcc::node_t *gcc::parser::relational()
{
    gcc::node_t *l = bitwise_shift();

    if (tokens_.get(TT_EQ_LARGER)) {
        /* TODO:  */
    } else if (tokens_.get(TT_GTHAN)) {
        /* TODO:  */
    } else if (tokens_.get(TT_EQ_SMALLER)) {
        /* TODO:  */
    } else if (tokens_.get(TT_LTHAN)) {
        return make_node(TT_LTHAN, l, bitwise_shift());
    }

    return l;
}

gcc::node_t *gcc::parser::equality()
{
    gcc::node_t *l = relational();

    if (tokens_.get(TT_EQUAL)) {
        DEBUG("parse equality\n");
        return make_node(TT_EQUAL, l, relational());
    } else if (tokens_.get(TT_NOT_EQUAL)) {
        DEBUG("parse unequality\n");
        /* TODO: parse inequality */
    }

    return l;
}

gcc::node_t *gcc::parser::bitwise_and()
{
    gcc::node_t *l = equality();

    if (!tokens_.get(TT_AND))
        return l;

    DEBUG("parse bitwise and\n");

    /* TODO: parse bitwise and */

    return l;
}

gcc::node_t *gcc::parser::bitwise_xor()
{
    gcc::node_t *l = bitwise_and();

    if (!tokens_.get(TT_XOR))
        return l;

    DEBUG("parse bitwise xor\n");

    /* TODO: parse bitwise xor */

    return l;
}

gcc::node_t *gcc::parser::bitwise_or()
{
    gcc::node_t *l = bitwise_xor();

    /* TODO: parse bitwise or */
    if (!tokens_.get(TT_OR))
        return l;

    DEBUG("parse bitwise or\n");

    return l;
}

gcc::node_t *gcc::parser::logical_and()
{
    gcc::node_t *l = bitwise_or();

    if (!tokens_.get(TT_AND_EXP))
        return l;

    DEBUG("parse logical and\n");

    /* TODO: parse logical or */
    return l;
}

gcc::node_t *gcc::parser::logical_or()
{
    gcc::node_t *l = logical_and();

    if (!tokens_.get(TT_OR_EXP))
        return l;

    DEBUG("parse logical or\n");

    /* TODO: parse logical or */
    return l;
}

gcc::node_t *gcc::parser::conditional()
{
    gcc::node_t *l = logical_or();

    if (!tokens_.get(TT_QMARK))
        return l;

    DEBUG("parse conditional\n");

    /* TODO: parse ternary operator */
    return l;
}

gcc::node_t *gcc::parser::assignment()
{
    DEBUG("parse assignment\n");

    gcc::node_t *l = conditional();

    /* TODO:  */
    if (tokens_.get(TT_ASSIGN)) {

    } else if (tokens_.get(TT_ADD_ASSIGN)) {

    } else if (tokens_.get(TT_SUB_ASSIGN)) {
    }

    return l;
}

gcc::node_t *gcc::parser::expression()
{
    DEBUG("parse expression\n");

    gcc::node_t *l = assignment();

    if (tokens_.get(TT_COMMA))
        return make_node(TT_COMMA, l, expression());
    return l;
}

gcc::node_t *gcc::parser::statement()
{
    DEBUG("parse statement\n");

    token_t tok = tokens_.get();

    switch (tok.type) {
        case TT_IF:
        {
            DEBUG("parse if\n");
            EXPECT(TT_LPAREN, "Missing left parenthesis!\n");
            gcc::node_t *node = new gcc::node_t;

            node->cond = expression();
            EXPECT(TT_RPAREN, "Missing left parenthesis!\n");

            ERROR("successfully parsed?\n");

            if (tokens_.get(TT_ELSE))
                node->els = expression();
        }
        break;

        case TT_FOR:
            ERROR("parse for\n");
            return nullptr;
            break;

        case TT_IDENTIFIER:
            /* ERROR("parse function call\n"); */
            break;

        default:
            /* ERROR("NOT SURE WHAT TO DO HERE!\n\n"); */
            break;
    }

    /* TODO: assignments */
    /* TODO: while */
    /* TODO: do */
    /* TODO: identifier (function call) */
    return nullptr;
}

gcc::node_t *gcc::parser::compound_statement()
{
    ERROR("parse compound statement\n");

    gcc::node_t *node = new gcc::node_t;

    while (!tokens_.get(TT_RCURLY))
        node->statements.push_back(statement());

    return node;
}

gcc::type_t gcc::parser::declaration_specifiers()
{
    DEBUG("parsing declaration specifiers\n");

    token_t tok = tokens_.get();
    gcc::type_t ret{TT_INVALID, false, false, false, false};

    if (tok.type == TT_END)
        return {TT_END, false, false, false, false};

    if (tok.type == TT_INVALID) {
        ERROR("Invalid token stream!\n");
        return ret;
    }

    while (tok.type == TT_EXTERN || tok.type == TT_VOLATILE ||
           tok.type == TT_STATIC || tok.type == TT_REGISTER)
    {
        switch (tok.type) {
            case TT_EXTERN:
                DEBUG("extern\n");
                if (ret.xtrn) {
                    ERROR("Extern given more than once\n");
                    return ret;
                }
                ret.xtrn = true;
                break;

            case TT_VOLATILE:
                DEBUG("volatile\n");
                if (ret.vltl) {
                    ERROR("Volatile given more than once\n");
                    return ret;
                }
                ret.vltl = true;
                break;

            case TT_STATIC:
                DEBUG("static\n");
                if (ret.sttc) {
                    ERROR("Static given more than once\n");
                    return ret;
                }
                ret.sttc = true;
                break;

            case TT_REGISTER:
                DEBUG("register\n");
                if (ret.rgstr) {
                    ERROR("Register given more than once\n");
                    return ret;
                }
                ret.sttc = true;
                break;
        }

        tok = tokens_.get();
    }

    switch (tok.type) {
        case TT_U8:
            DEBUG("uint8_t\n");
            ret.type = TT_U8;
            break;
        case TT_SIZET:
            DEBUG("size_t\n");
            ret.type = TT_SIZET;
            break;
        case TT_VOID:
            DEBUG("void\n");
            ret.type = TT_VOID;
            break;
    }

    if (tok.type == TT_STRUCT)
        ret.type = TT_STRUCT;

    if (tokens_.get(TT_STAR)) {
        if (ret.type != TT_INVALID && ret.type != TT_STAR) {
            ret.ptr = true;
            return ret;
        }

        ERROR("Stray * found!\n");
        return ret;
    }

    return ret;
}

gcc::prog_t *gcc::parser::build_ast()
{
    gcc::prog_t *prog = new gcc::prog_t;
    token_t tok;

    while (!tokens_.get(TT_END)) {
        type_t type = declaration_specifiers();

        if (type.type == TT_END)
            return prog;

        if (type.type == TT_INVALID) {
            WARN("invalid token\n");
            return nullptr;
        }

        if ((tok = tokens_.get()).type != TT_IDENTIFIER) {
            ERROR("invalid token! %d\n", tokens_.get().type);
            break;
        }

        /* struct declaration */
        if (type.type == TT_STRUCT) {
            WARN("parse struct\n");
            continue;
        }

        /* function */
        if (tokens_.get(TT_LPAREN)) {
            gcc::func_t func;

            if (prog->functions.find(tok.str) != prog->functions.end()) {
                ERROR("Duplicate function %s() found!\n", tok.str.c_str());
                return nullptr;
            }

            func.name     = tok.str;
            func.ret_type = { TT_FUNC };

            do {
                type_t type = declaration_specifiers();

                if (type.type == TT_END)
                    return prog;

                if (type.type == TT_INVALID) {
                    ERROR("Failed to parse declaration specifiers, invalid token encountered!\n");
                    return nullptr;
                }

                if ((tok = tokens_.get()).type != TT_IDENTIFIER) {
                    ERROR("invalid token! %d\n", tokens_.get().type);
                    return nullptr;
                }

                if (func.args.find(tok.str) != func.args.end()) {
                    ERROR("Duplicate variable %s for %s() found!\n", tok.str.c_str(), func.name.c_str());
                    return nullptr;
                }

                gcc::var_t var;
                var.name = tok.str;
                var.type = { TT_VAR };

                func.args.insert(std::make_pair(tok.str, var));

                DEBUG("add parameter %s for %s()\n", tok.str.c_str(), func.name.c_str());

                /* consume comma if there are multiple arguments */
                tokens_.get(TT_COMMA);

            } while (!tokens_.get(TT_RPAREN));

            if (tokens_.get(TT_SEMICOLON)) {
                DEBUG("function prototype\n");
                continue;
            }
            EXPECT(TT_LCURLY, "Expected function body!\n");

            func.node.body = compound_statement();
            prog->functions.insert(std::make_pair(func.name, func));

            continue;
        }

        /* global variable */
        if (prog->globals.find(tok.str) != prog->globals.end()) {
            ERROR("Duplicate global variable '%s'\n", tok.str.c_str());
            return nullptr;
        }

        gcc::var_t var = { { TT_VAR, 0 } , tok.str };
        prog->globals.insert(std::make_pair(tok.str, var));

        while (tokens_.get(TT_COMMA)) {
            tok = tokens_.get();

            if (tok.type != TT_IDENTIFIER) {
                ERROR("Expected identifer, got %d\n", tok.type);
                return nullptr;
            }

            if (prog->globals.find(tok.str) != prog->globals.end()) {
                ERROR("Duplicate global variable '%s'\n", tok.str.c_str());
                return nullptr;
            }

            gcc::var_t var = { { TT_VAR, 0 }, tok.str };
            prog->globals.insert(std::make_pair(tok.str, var));
        }

        EXPECT(TT_SEMICOLON, "Missing semicolon!\n");
    }

    return prog;
}

gcc_error_t gcc::parser::parse(gcc::token_stream_t& tokens)
{
    DEBUG("parsing %zu tokens\n", tokens.size());

    if (!tokens.size()) {
        ERROR("cannot parse empty token stream!\n");
        return GCC_INVALID_VALUE;
    }

    tokens_ = tokens;

    if (!(prog_ = build_ast()))
        return GCC_INVALID_VALUE;
    return GCC_SUCCESS;
}
