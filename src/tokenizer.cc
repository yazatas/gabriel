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

char *gcc::tokenizer::skip_comments(char *ptr)
{
    if (*ptr == '/' && *(ptr + 1) == '/') {
        while (*ptr != '\n')
            ptr++;
        ptr++;
    } else if (*ptr == '/' && *(ptr + 1) == '*') {
        ptr += 2;

        while (*ptr != '*' && *(ptr + 1) != '/')
            ptr++;
    }

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

bool gcc::tokenizer::iskeyword(char *ptr)
{
    const char *keywords[] = {
        "break",    "case",     "char",     "const",
        "continue", "default",  "do",       "double",
        "else",     "enum",     "extern",   "float",
        "for",      "goto",     "if",       "inline",
        "int",      "long",     "register", "restrict",
        "return",   "short",    "signed",   "sizeof",
        "static",   "struct",   "switch",   "typedef",
        "union",    "unsigned", "void",     "volatile",
        "while",    "uint8_t",  "uint16_t", "uint32_t",
        "uint64_t", "int8_t",   "int16_t",  "int32_t",
        "int64_t",  "size_t"
    };

    for (int i = 0; i < 42; ++i) {
        if (!strncmp(ptr, keywords[i], strlen(keywords[i]))) {
            if (!isalnum(*(ptr + strlen(keywords[i]))) && *(ptr + strlen(keywords[i])) != '_')
                return true;
        }
    }

    return false;
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

char *gcc::tokenizer::get_keyword(char *ptr)
{
    const char *keywords[] = {
        "break",    "case",     "char",     "const",
        "continue", "default",  "do",       "double",
        "else",     "enum",     "extern",   "float",
        "for",      "goto",     "if",       "inline",
        "int",      "long",     "register", "restrict",
        "return",   "short",    "signed",   "sizeof",
        "static",   "struct",   "switch",   "typedef",
        "union",    "unsigned", "void",     "volatile",
        "while",    "uint8_t",  "uint16_t", "uint32_t",
        "uint64_t", "int8_t",   "int16_t",  "int32_t",
        "int64_t",  "size_t"
    };

    for (int i = 0; i < 42; ++i) {
        if (!strncmp(ptr, keywords[i], strlen(keywords[i]))) {
            if (!isalnum(*(ptr + strlen(keywords[i]))) && *(ptr + strlen(keywords[i])) != '_') {
                tokens_.add({ });
                return ptr + strlen(keywords[i]);
            }
        }
    }
}

gcc_error_t gcc::tokenizer::tokenize(char *file)
{
    char *ptr, *saveptr;

    DEBUG("tokenizing %s\n", file);

    if (!(ptr = saveptr = read_file(file))) {
        ERROR("failed to read file %s\n", file);
        return gcc_errno;
    }

    while (*ptr) {
        if (!*(ptr = skip_ws(ptr)))
            break;
        if (!*(ptr = skip_comments(ptr)))
            break;

        if (isdigit(*ptr)) {
            ptr = get_number(ptr);
            continue;
        } else if (isoperand(*ptr)) {
            if (!(ptr = get_operand(ptr)))
                return GCC_INVALID_VALUE;
            continue;
        } else if (iskeyword(ptr)) {
            if (!(ptr = get_keyword(ptr)))
                return GCC_INVALID_VALUE;
            continue;
        }

        ERROR("invalid token stream: '%c'!\n", *ptr);
        return GCC_INVALID_VALUE;
    }

    free(saveptr);
    return GCC_SUCCESS;
}

gcc::token_stream_t& gcc::tokenizer::get_token_stream()
{
    DEBUG("returning tokenizer output\n");

    return tokens_;
}
