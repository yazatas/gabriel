#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <vector>
#include <unordered_map>

#include "ast.hh"
#include "token.hh"
#include "tokenizer.hh"
#include "util/error.hh"

namespace gcc {

    /* TODO: create node type enum */
    typedef enum node_type {
        NT_INVALID,
        NT_END,
        NT_DIGIT,
        NT_BREAK,
        NT_CASE,
        NT_CHAR,
        NT_CONST,
        NT_CONTINUE,
        NT_DEFAULT,
        NT_DO,
        NT_DOUBLE,
        NT_ELSE,
        NT_ENUM,
        NT_EXTERN,
        NT_FLOAT,
        NT_FOR,
        NT_GOTO,
        NT_IF,
        NT_INLINE,
        NT_INT,
        NT_LONG,
        NT_REGISTER,
        NT_RESTRICT,
        NT_RETURN,
        NT_SHORT,
        NT_SIGNED,
        NT_SIZEOF,
        NT_STATIC,
        NT_STRUCT,
        NT_SWITCH,
        NT_TYPEDEF,
        NT_UNION,
        NT_UNSIGNED,
        NT_VOID,
        NT_VOLATILE,
        NT_WHILE,
        NT_U8,
        NT_U16,
        NT_U32,
        NT_U64,
        NT_I8,
        NT_I16,
        NT_I32,
        NT_I64,
        NT_SIZET,
        NT_AND,
        NT_OR,
        NT_XOR,
        NT_ANOT,
        NT_MUL_ASSIGN,
        NT_SUB_ASSIGN,
        NT_DIV_ASSIGN,
        NT_ADD_ASSIGN,
        NT_AND_EXP,
        NT_OR_EXP,
        NT_EQUAL,
        NT_EQ_LARGER,
        NT_EQ_SMALLER,
        NT_LSHIFT,
        NT_RSHIFT,
        NT_LSHIFT_ASSIGN,
        NT_RSHIFT_ASSIGN,
        NT_XOR_ASSIGN,
        NT_INCR,
        NT_DECR,
        NT_STAR,
        NT_MINUS,
        NT_DIV,
        NT_PLUS,
        NT_LPAREN,
        NT_RPAREN,
        NT_LSQUARE,
        NT_RSQUARE,
        NT_LCURLY,
        NT_RCURLY,
        NT_QMARK,
        NT_COLON,
        NT_EXCLAMATION,
        NT_SEMICOLON,
        NT_COMMA,
        NT_DOT,
        NT_LTHAN,
        NT_GTHAN,
        NT_ASSIGN,
        NT_QUOTE,
        NT_SQUOTE,
        NT_IDENTIFIER,
        NT_VAR,
        NT_FUNC,
        NT_ARROW,
        NT_NOT_EQUAL,
    } node_type_t;

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
        token_type_t type;

        node_t *l;
        node_t *r;
        node_t *body;
        node_t *cond;
        node_t *then;
        node_t *els;

        std::vector<node_t *> statements;
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
            gcc::node_t *statement();
            gcc::node_t *expression();
            gcc::node_t *assignment();
            gcc::node_t *conditional();
            gcc::node_t *make_node(token_type_t type, gcc::node_t *l, gcc::node_t *r);

            gcc::node_t *logical_or();
            gcc::node_t *logical_and();
            gcc::node_t *bitwise_or();
            gcc::node_t *bitwise_xor();
            gcc::node_t *bitwise_and();
            gcc::node_t *equality();
            gcc::node_t *relational();
            gcc::node_t *bitwise_shift();
            gcc::node_t *add_sub();
            gcc::node_t *mul_div();
            gcc::node_t *prefix();
            gcc::node_t *postfix();
            gcc::node_t *types();

            gcc::ast_node *ast_;
            gcc::prog_t *prog_;
            token_stream_t tokens_;
    };
};

#endif /* __PARSER_HH__ */
