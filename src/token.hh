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
        TT_DIV
    } token_type_t;

    typedef struct token {
        token_type_t type;
        int value;
    } token_t;

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
