#ifndef __TOKEN_HH__
#define __TOKEN_HH__

#include <deque>

namespace gcc {

    typedef enum {
        TT_INVALID,
        TT_END,
        TT_DIGIT,
        TT_MUL,
        TT_ADD,
        TT_SUB,
        TT_DIV,
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
        TT_COMMA,
        TT_SEMICOLON,
        TT_IDENTIFIER,
        TT_LAST
    } token_type_t;

    typedef struct token {
        token_type_t type;
        union {
            char *str;
            int value;
            int id;
        } data;
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
            return token;
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
