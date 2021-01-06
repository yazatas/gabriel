#ifndef __TOKEN_HH__
#define __TOKEN_HH__

#include <deque>
#include <string>

namespace gcc {

    typedef enum {
        TT_INVALID,
        TT_END,
        TT_DIGIT,
        TT_BREAK,
        TT_CASE,
        TT_CHAR,
        TT_CONST,
        TT_CONTINUE,
        TT_DEFAULT,
        TT_DO,
        TT_DOUBLE,
        TT_ELSE,
        TT_ENUM,
        TT_EXTERN,
        TT_FLOAT,
        TT_FOR,
        TT_GOTO,
        TT_IF,
        TT_INLINE,
        TT_INT,
        TT_LONG,
        TT_REGISTER,
        TT_RESTRICT,
        TT_RETURN,
        TT_SHORT,
        TT_SIGNED,
        TT_SIZEOF,
        TT_STATIC,
        TT_STRUCT,
        TT_SWITCH,
        TT_TYPEDEF,
        TT_UNION,
        TT_UNSIGNED,
        TT_VOID,
        TT_VOLATILE,
        TT_WHILE,
        TT_U8,
        TT_U16,
        TT_U32,
        TT_U64,
        TT_I8,
        TT_I16,
        TT_I32,
        TT_I64,
        TT_SIZET,
        TT_AND,
        TT_OR,
        TT_XOR,
        TT_ANOT,
        TT_MUL_ASSIGN,
        TT_SUB_ASSIGN,
        TT_DIV_ASSIGN,
        TT_ADD_ASSIGN,
        TT_AND_EXP,
        TT_OR_EXP,
        TT_EQUAL,
        TT_EQ_LARGER,
        TT_EQ_SMALLER,
        TT_LSHIFT,
        TT_RSHIFT,
        TT_LSHIFT_ASSIGN,
        TT_RSHIFT_ASSIGN,
        TT_XOR_ASSIGN,
        TT_INCR,
        TT_DECR,
        TT_STAR,
        TT_MINUS,
        TT_DIV,
        TT_PLUS,
        TT_LPAREN,
        TT_RPAREN,
        TT_LSQUARE,
        TT_RSQUARE,
        TT_LCURLY,
        TT_RCURLY,
        TT_QMARK,
        TT_COLON,
        TT_EXCLAMATION,
        TT_SEMICOLON,
        TT_COMMA,
        TT_DOT,
        TT_LTHAN,
        TT_GTHAN,
        TT_ASSIGN,
        TT_QUOTE,
        TT_SQUOTE,
        TT_IDENTIFIER,
        TT_VAR,
        TT_FUNC,
        TT_LAST
    } token_type_t;

    typedef struct token {
        token_type_t type;
        int value;
        std::string str;
    } token_t;

    static inline const char *test_func(token_type_t type)
    {
        static const char *names[] = {
            "invalid",
            "end",
            "digit",
            "mul",
            "add",
            "sub",
            "div",
        };

        if (type < TT_INVALID || type >= TT_LAST)
            return names[0];
        return names[type];
    }

    typedef struct token_stream {
        std::deque<token_t> tokens_;
        token_t curr_;

        void add(token_t token)
        {
            tokens_.push_back(token);
        }

        token_t get()
        {
            if (!tokens_.size())
                return { TT_END, 0 };

            token_t token = tokens_.front();
            tokens_.pop_front();
            return (curr_ = token);
        }

        bool get(token_type_t type)
        {
            if (!tokens_.size() || tokens_.front().type != type)
                return false;

            curr_ = tokens_.front();
            tokens_.pop_front();
            return true;
        }

        token_t get_current()
        {
            return curr_;
        }

        void put(token_t token)
        {
            tokens_.push_front(token);
        }

        size_t size()
        {
            return tokens_.size();
        }

        token_t& at(size_t index)
        {
            return tokens_.at(index);
        }

    } token_stream_t;
};

#endif /* __TOKEN_HH__ */
