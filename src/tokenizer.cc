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

        while (*ptr != '*' || *(ptr + 1) != '/')
            ptr++;
    }

    return ptr;
}

char *gcc::tokenizer::get_number(char *ptr)
{
    int value = 0;

    while (*ptr && isdigit(*ptr))
        value = (value * 10) + *ptr++ - '0';

    token_t tok;
    tok.type = TT_DIGIT;
    tok.data.value = value;
    tokens_.add(tok);

    return ptr;
}

bool gcc::tokenizer::get_keyword(char **ptr)
{
    struct {
        const char *s;
        token_type_t type;
    } keywords[] = {
        { "break",    TT_BREAK },    { "case",     TT_CASE },
        { "char",     TT_CHAR },     { "const",    TT_CONST },
        { "continue", TT_CONTINUE }, { "default",  TT_DEFAULT },
        { "do",       TT_DO },       { "double",   TT_DOUBLE },
        { "else",     TT_ELSE },     { "enum",     TT_ENUM },
        { "extern",   TT_EXTERN },   { "float",    TT_FLOAT },
        { "for",      TT_FOR },      { "goto",     TT_GOTO },
        { "if",       TT_IF },       { "inline",   TT_INLINE },
        { "int",      TT_INT },      { "long",     TT_LONG },
        { "register", TT_REGISTER }, { "restrict", TT_RESTRICT },
        { "return",   TT_RETURN },   { "short",    TT_SHORT },
        { "signed",   TT_SIGNED },   { "sizeof",   TT_SIZEOF },
        { "static",   TT_STATIC },   { "struct",   TT_STRUCT },
        { "switch",   TT_SWITCH },   { "typedef",  TT_TYPEDEF },
        { "union",    TT_UNION },    { "unsigned", TT_UNSIGNED },
        { "void",     TT_VOID },     { "volatile", TT_VOLATILE },
        { "while",    TT_WHILE },    { "uint8_t",  TT_U8 },
        { "uint16_t", TT_U16 },      { "uint32_t", TT_U32 },
        { "uint64_t", TT_U64 },      { "int8_t",   TT_I8 },
        { "int16_t",  TT_I16 },      { "int32_t",  TT_I32 },
        { "int64_t",  TT_I64 },      { "size_t",   TT_SIZET }
    };

    for (int i = 0; i < 42; ++i) {
        if (!strncmp(*ptr, keywords[i].s, strlen(keywords[i].s))) {
            if (!isalnum(*(*ptr + strlen(keywords[i].s))) && *(*ptr + strlen(keywords[i].s)) != '_') {
                tokens_.add({ keywords[i].type, 0 });
                *ptr = *ptr + strlen(keywords[i].s);
                return true;
            }
        }
    }

    return false;
}

bool gcc::tokenizer::get_operator(char **ptr)
{
    struct {
        const char *s;
        token_type_t type;
    } operators[] = {
        { "*=",  TT_MUL_ASSIGN },    { "-=",  TT_SUB_ASSIGN },
        { "/=",  TT_DIV_ASSIGN },    { "+=",  TT_ADD_ASSIGN},
        { "&&",  TT_AND_EXP },       { "||",  TT_OR_EXP },
        { "==",  TT_EQUAL },         { ">=",  TT_EQ_LARGER },
        { "<=",  TT_EQ_SMALLER },    { "<<",  TT_LSHIFT },
        { ">>",  TT_RSHIFT },        { "<<=", TT_LSHIFT_ASSIGN },
        { ">>=", TT_RSHIFT_ASSIGN }, { "^=",  TT_XOR_ASSIGN },
        { "++",  TT_INCR },          { "--",  TT_DECR },
        { "&",   TT_AND },           { "|",   TT_OR },
        { "^",   TT_XOR },           { "~",   TT_ANOT },
        { "*",   TT_MUL },           { "-",   TT_SUB },
        { "/",   TT_DIV },           { "+",   TT_ADD },
        { "(",   TT_LPAREN },        { ")",   TT_RPAREN },
        { "[",   TT_LSQUARE },       { "]",   TT_RSQUARE },
        { "{",   TT_LCURLY },        { "}",   TT_RCURLY },
        { "?",   TT_QMARK },         { ":",   TT_COLON },
        { "!",   TT_EXCLAMATION },   { ";",   TT_SEMICOLON },
        { ",",   TT_COMMA },         { ".",   TT_DOT },
        { "<",   TT_LTHAN },         { ">",   TT_GTHAN },
        { "=",   TT_ASSIGN },        { "\"",  TT_QUOTE },
        { "\'",  TT_SQUOTE }
    };

    for (int i = 0; i < 41; ++i) {
        if (!strncmp(*ptr, operators[i].s, strlen(operators[i].s))) {
            tokens_.add({ operators[i].type, 0 });
            *ptr = *ptr + strlen(operators[i].s);
            return true;
        }
    }

    return false;
}

bool gcc::tokenizer::get_identifier(char **ptr)
{
    size_t len    = 0;
    char *tok     = nullptr;
    char *saveptr = *ptr;
    char *uptr    = *ptr;

    if (!isalpha(*uptr) && *uptr != '_')
        return false;

    while (isalpha(*uptr) || *uptr == '_')
        len++, (uptr)++;

    if (!(tok = (char *)malloc(len + 1))) {
        ERROR("Failed to allocate memory for identifier!");
        return false;
    }

    memcpy(tok, saveptr, len);
    tok[len] = 0;

    tokens_.add({ TT_IDENTIFIER, tok });

    *ptr = saveptr + len;
    return true;
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
        } else if (get_operator(&ptr)) {
            continue;
        } else if (get_keyword(&ptr)) {
            continue;
        } else if (get_identifier(&ptr)) {
            continue;
        } else {
            ERROR("nothing above!\n");
        }

        ERROR("invalid token stream: '%c' '%s'!\n", *ptr, ptr);
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
