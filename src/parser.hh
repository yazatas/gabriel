#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <vector>
#include <unordered_map>

#include "ast.hh"
#include "token.hh"
#include "tokenizer.hh"
#include "util/error.hh"

namespace gcc {

    typedef struct node node_t;

    typedef struct type {
        token_type_t type; /* int, short, bool etc. */
        bool xtrn;         /* extern  */
        bool vltl;         /* volatile */
        bool sttc;         /* static */
        bool rgstr;        /* register */
        bool ptr;          /* pointer */
    } type_t;

    typedef struct var {
        type_t type;
        std::string name;
    } var_t;

    struct node {
        node_t *body;
    };

    typedef struct func {
        type_t ret_type;
        node_t node;
        std::string name;
        std::unordered_map<std::string, gcc::var_t> args;
    } func_t;

    typedef struct prog {
        std::unordered_map<std::string, gcc::func_t> functions;
        std::unordered_map<std::string, gcc::var_t>  globals;
    } prog_t;

    class parser {
        public:
            parser();
            ~parser();

            /* parse the token stream into an abstract syntax tree */
            gcc_error_t parse(gcc::token_stream_t& tokens);

            /* return the program built by the parser */
            gcc::prog_t *get_prog();

        private:
            gcc::prog_t *build_ast();
            gcc::type_t declaration_specifiers();
            gcc::node_t *compound_statement();
            void statement();

            gcc::ast_node *ast_;
            gcc::prog_t *prog_;
            token_stream_t tokens_;
    };
};

#endif /* __PARSER_HH__ */
