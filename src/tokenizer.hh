#ifndef __TOKENIZER_HH__
#define __TOKENIZER_HH__

#include <vector>

#include "token.hh"
#include "util/error.hh"

namespace gcc {

    class tokenizer {
        public:
            tokenizer();
            ~tokenizer();

            /* tokenize the input file into a vector of tokens */
            gcc_error_t tokenize(char *file);

            /* get reference to tokenized stream */
            gcc::token_stream_t& get_token_stream();

        private:
            /* extract token from the stream */
            token_t create_token(union TOKEN type, char *ptr);

            /* read file contents to a buffer */
            char *read_file(char *file);

            /* skip whitespace characters */
            char *skip_ws(char *ptr);

            /* skip comments */
            char *skip_comments(char *ptr);

            /* extract number from the stream into a token */
            bool get_digit(char **ptr);

            /* extract operand from the stream into a token */
            bool get_operator(char **ptr);

            /* extract keyword from the stream into a token */
            bool get_keyword(char **ptr);

            /* extract identifier from the stream into a token */
            bool get_identifier(char **ptr);

            gcc::token_stream_t tokens_;
    };
};

#endif /* __TOKENIZER_HH__ */
