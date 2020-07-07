#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <vector>

#include "token.hh"
#include "tokenizer.hh"
#include "util/error.hh"

namespace gcc {

    class parser {
        public:
            parser();
            ~parser();

            /* parse the token stream into an abstract syntax tree */
            gcc_error_t parse(gcc::token_stream_t& tokens);

        private:
    };
};

#endif /* __PARSER_HH__ */
