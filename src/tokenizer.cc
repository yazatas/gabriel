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

bool gcc::tokenizer::isoperator(char *ptr)
{
    const char *operators[] = {
        "&", "|", "^", "~",
        "*", "-", "/", "+",
        "*=", "-=", "/=", "+=",
        "&&", "||", "==", ">=",
        "<=", "<<", ">>", "<<=",
        ">>=", "^=", "++", "--"
        "(", ")", "[", "]", 
        "{", "}", "?", ":", 
        "!", ";", ",", ".", 
        "<", ">"
    };

    for (int i = 0; i < 34; ++i) {
        if (!strncmp(ptr, operators[i], strlen(operators[i])))
            return true;
    }

    return false;
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
            if (!isalnum(*(ptr + strlen(keywords[i]))) && *(ptr + strlen(keywords[i])) != '_') {
                return true;
            }
        }
    }

    return false;
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

char *gcc::tokenizer::get_operator(char *ptr)
{
    switch (*ptr) {
        case '*':
            tokens_.add({ TT_MUL, 0 });
            break;
        case '+':
            tokens_.add({ TT_ADD, 0 });
            break;
        case '-':
            tokens_.add({ TT_SUB, 0 });
            break;
        case '/':
            tokens_.add({ TT_DIV, 0 });
            break;
        case ',':
            tokens_.add({ TT_COMMA, 0 });
            break;
        case ';':
            tokens_.add({ TT_SEMICOLON, 0 });
            break;
        default:
            ERROR("unexpected operand token '%c'\n", *ptr);
            return nullptr;
    }

    return ptr + 1;
}

char *gcc::tokenizer::get_keyword(char *ptr)
{
    struct {
        token_type_t type;
        const char *str;
    } keywords[] = {
        { TT_BREAK,    "break"    }, { TT_CASE,     "case"     }, { TT_CHAR,     "char" },
        { TT_CONST,    "const"    }, { TT_CONTINUE, "continue" }, { TT_DEFAULT,  "default" },
        { TT_DO,       "do"       }, { TT_DOUBLE,   "double"   }, { TT_ELSE,     "else" },
        { TT_ENUM,     "enum"     }, { TT_EXTERN,   "extern"   }, { TT_FLOAT,    "float" },
        { TT_FOR,      "for"      }, { TT_GOTO,     "goto"     }, { TT_IF,       "if" },
        { TT_INLINE,   "inline"   }, { TT_INT,      "int"      }, { TT_LONG,     "long" },
        { TT_REGISTER, "register" }, { TT_RESTRICT, "restrict" }, { TT_RETURN,   "return" },
        { TT_SHORT,    "short"    }, { TT_SIGNED,   "signed"   }, { TT_SIZEOF,   "sizeof" },
        { TT_STATIC,   "static"   }, { TT_STRUCT,   "struct"   }, { TT_SWITCH,   "switch" },
        { TT_TYPEDEF,  "typedef"  }, { TT_UNION,    "union"    }, { TT_UNSIGNED, "unsigned" },
        { TT_VOID,     "void"     }, { TT_VOLATILE, "volatile" }, { TT_WHILE,    "while" },
        { TT_U8,       "uint8_t"  }, { TT_U16,      "uint16_t" }, { TT_U32,      "uint32_t" },
        { TT_U64,      "uint64_t" }, { TT_I8,       "int8_t"   }, { TT_I16,      "int16_t" },
        { TT_I32,      "int32_t"  }, { TT_I64,      "int64_t"  }, { TT_SIZET,    "size_t" },
    };

    for (int i = 0; i < 42; ++i) {
        if (!strncmp(ptr, keywords[i].str, strlen(keywords[i].str))) {
            if (!isalnum(*(ptr + strlen(keywords[i].str))) && *(ptr + strlen(keywords[i].str)) != '_') {
                tokens_.add({ keywords[i].type, 0 });
                return ptr + strlen(keywords[i].str);
            }
        }
    }
}

bool gcc::tokenizer::isidentifier(char *ptr)
{
    if (isalpha(*ptr) || *ptr == '_')
        return true;
    return false;
}

char *gcc::tokenizer::get_identifier(char *ptr)
{
    size_t len    = 0;
    char *tok     = nullptr;
    char *saveptr = ptr;

    while (isalpha(*ptr) || *ptr == '_')
        len++, ptr++;

    if (!(tok = (char *)malloc(len + 1))) {
        ERROR("Failed to allocate memory for identifier!");
        return nullptr;
    }

    memcpy(tok, saveptr, len);
    tok[len] = 0;

    tokens_.add({ TT_IDENTIFIER, tok });

    return saveptr + len;
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
        } else if (isoperator(ptr)) {
            if (!(ptr = get_operator(ptr)))
                return GCC_INVALID_VALUE;
            continue;
        } else if (iskeyword(ptr)) {
            if (!(ptr = get_keyword(ptr)))
                return GCC_INVALID_VALUE;
            continue;
        } else if (isidentifier(ptr)) {
            if (!(ptr = get_identifier(ptr)))
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
