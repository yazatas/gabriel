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
    gcc::destroy_ast(ast_);
    free(ast_);
}

gcc::ast_node *gcc::parser::get_ast()
{
    return ast_;
}

void gcc::parser::statement()
{
}

void gcc::parser::compound_statement()
{
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

    while (true) {
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
            ERROR("parse function arguments\n");
            continue;
        }

        /* global variable */
        prog->globals.push_back({ type, strdup(tok.data.str) });

        while (tokens_.get(TT_COMMA)) {
            tok = tokens_.get();
            prog->globals.push_back({ type, strdup(tok.data.str) });
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
