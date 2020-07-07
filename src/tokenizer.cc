#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

#include "tokenizer.hh"
#include "util/log.hh"

#define CHANNEL "tokenizer"

gcc::tokenizer::tokenizer()
{
}

gcc::tokenizer::~tokenizer()
{
}

char *gcc::tokenizer::read_file(char *file)
{
    FILE *fp;
    size_t len;
    char *buffer;

    DEBUG("read file %s to buffer\n", file);

    if (!file || !(fp = fopen(file, "r"))) {
        ERROR("failed to open input file: %s\n", file);
        gcc_errno = GCC_INVALID_VALUE;
        return nullptr;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (!(buffer = (char *)malloc(len))) {
        gcc_errno = GCC_OUT_OF_MEMORY;
        return nullptr;
    }

    fread(buffer, 1, len, fp);
    fclose(fp);

    return buffer;
}

char *gcc::tokenizer::skip_ws(char *ptr)
{
    while (isspace(*ptr))
        ptr++;
    return ptr;
}

bool gcc::tokenizer::isoperand(char c)
{
    switch (c) {
        case '*':
        case '+':
        case '-':
        case '/':
            return true;
        default:
            return false;
    }
}

char *gcc::tokenizer::get_number(char *ptr)
{
    int value = 0;

    while (*ptr && isdigit(*ptr))
        value = (value * 10) + *ptr++ - '0';

    tokens_.add({ TT_DIGIT, value });
    return ptr;
}

char *gcc::tokenizer::get_operand(char *ptr)
{
    switch (*ptr) {
        case '*':
            tokens_.add({ TT_MUL, *ptr });
            break;
        case '+':
            tokens_.add({ TT_ADD, *ptr });
            break;
        case '-':
            tokens_.add({ TT_SUB, *ptr });
            break;
        case '/':
            tokens_.add({ TT_DIV, *ptr });
            break;
        default:
            ERROR("unexpected operand token %c\n", *ptr);
            return nullptr;
    }

    return ptr + 1;
}

gcc_error_t gcc::tokenizer::tokenize(char *file)
{
    char *ptr;

    DEBUG("tokenizing %s\n", file);

    if (!(ptr = read_file(file))) {
        ERROR("failed to read file %s\n", file);
        return gcc_errno;
    }

    while (*ptr) {
        if (!*(ptr = skip_ws(ptr)))
            break;

        if (isdigit(*ptr)) {
            ptr = get_number(ptr);
            continue;
        } else if (isoperand(*ptr)) {
            if (!(ptr = get_operand(ptr)))
                return GCC_INVALID_VALUE;
            continue;
        }

        ERROR("invalid token stream: '%c'!\n", *ptr);
        return GCC_INVALID_VALUE;
    }

    return GCC_SUCCESS;
}

gcc::token_stream_t& gcc::tokenizer::get_token_stream()
{
    DEBUG("returning tokenizer output\n");

    return tokens_;
}
