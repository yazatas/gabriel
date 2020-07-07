#include <iostream>
#include <fstream>
#include <vector>

#include "parser.hh"
#include "util/log.hh"

#define CHANNEL "parser"

gcc::parser::parser()
{
}

gcc::parser::~parser()
{
}

gcc_error_t gcc::parser::parse(gcc::token_stream_t& tokens)
{
    token_t token;

    DEBUG("parsing %zu tokens\n", tokens.size());

    if (!tokens.size()) {
        ERROR("cannot parse empty token stream!\n");
        return GCC_INVALID_VALUE;
    }

    while ((token = tokens.get()).type != TT_END) {
        if (token.type == TT_DIGIT)
            printf("%d ", token.value);
        else
            printf("%c ", token.value);
    }
    puts("");

    return GCC_SUCCESS;
}
