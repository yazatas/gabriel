#include <iostream>

#include "parser.hh"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <input file>\n", __FILE__);
        exit(EXIT_FAILURE);
    }

    gcc_error_t ret;
    gcc::parser parser;
    gcc::tokenizer tokenizer;

    if ((ret = tokenizer.tokenize(argv[1])) != GCC_SUCCESS) {
        fprintf(stderr, "Failed to tokenize file %s: %s\n", argv[1], gcc_error(ret));
        exit(EXIT_FAILURE);
    }

    if ((ret = parser.parse(tokenizer.get_token_stream())) != GCC_SUCCESS) {
        fprintf(stderr, "Failed to parse tokens!\n");
        exit(EXIT_FAILURE);
    }
}
