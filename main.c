#include <ctype.h>
#include <stdio.h>
#include <string.h> //for space

#define DS_IMPLEMENTATION
#define DS_IO_IMPLEMENTATION
#include "ds.h"


enum token_kind {
    IDENT, //variables
    LABEL,
    INT,
    INPUT,
    OUTPUT,
    GOTO,
    IF,
    THEN,
    EQUAL,
    PLUS,
    LESS_THAN,
    INVALID,
    END
};

struct token{
    enum token_kind kind;
    char *value; //actual string
};

const char *show_token_kind(enum token_kind kind) {
    switch (kind) {
    case IDENT:
        return "ident";
    case LABEL:
        return "label";
    case INT:
        return "int";
    case INPUT:
        return "input";
    case OUTPUT:
        return "output";
    case GOTO:
        return "goto";
    case IF:
        return "if";
    case THEN:
        return "then";
    case EQUAL:
        return "equal";
    case PLUS:
        return "plus";
    case LESS_THAN:
        return "less_than";
    case INVALID:
        return "invalid";
    case END:
        return "end";
    }
}

void print_token(struct token tok){
    const char *kind = show_token_kind(tok.kind);
    printf("%s", kind);
    if (tok.value != NULL)
    {
        printf("(%s)", tok.value);
    }
    printf("\n");
}

struct lexer { //like an iterator
    char *buffer; //thats gonna read, full soirce doe
    unsigned int buffer_len; //length
    unsigned int pos;
    unsigned int read_pos;
    char ch; //just to keep the current position in the current char
};

static char lexer_peek_char(struct lexer *l) { //looks at next character without moving forward
    if (l->read_pos >= l->buffer_len) {
    return EOF;
    }
    return l->buffer[l->read_pos];
}

static char lexer_read_char(struct lexer *l){ //reads next character/moves forward one charater
    l->ch = lexer_peek_char(l);

    l->pos = l->read_pos;
    l->read_pos += 1;

    return l->ch;
}

static void skip_whitespace(struct lexer *l) { //removes significance of white spaces
    while (isspace(l->ch)) {
        lexer_read_char(l);
    }
}
static void lexer_init(struct lexer *l, char *buffer, unsigned int buffer_len) {
    l->buffer = buffer;
    l->buffer_len = buffer_len;
    l->pos = 0;
    l->read_pos = 0;
    l->ch = 0;

    lexer_read_char(l);  //make sure we read the fisrt char
} 

static struct token lexer_next_token(struct lexer *l){
    skip_whitespace(l);

    printf("%c\n",l->ch);

    if(l->ch == EOF){ //if file ends return end
        lexer_read_char(l);
        return(struct token){.kind = END, .value = NULL};
    }


    else if(l->ch == '='){
        lexer_read_char(l);
        return(struct token){.kind = EQUAL, .value = NULL};
    } else if(l->ch == '+'){
        lexer_read_char(l); 
        return(struct token){.kind = PLUS, .value = NULL};
    } else if(l->ch == '<'){
        lexer_read_char(l);
        return(struct token){.kind = LESS_THAN, .value = NULL};
    } else if(l->ch == ':'){
        lexer_read_char(l);
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 0};
        
        while (isalnum(l->ch)||l->ch =='_'){
            slice.len += 1;
            lexer_read_char(l);
        }
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        return(struct token){.kind = LABEL, .value = value};
        
    }else if (isdigit(l->ch)){
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 0};
        while (isdigit(l->ch)){
            slice.len += 1;
            lexer_read_char(l);
        }
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        return(struct token){.kind = INT, .value = value};
    }else if(isalnum(l->ch) || l->ch == '_'){
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 0};
        while (isalnum(l->ch) || l->ch == '_'){
            slice.len += 1;
            lexer_read_char(l);
        }
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        if(strcmp(value, "input") == 0){
            return(struct token){.kind = INPUT, .value = NULL};
        }else if(strcmp(value, "output") == 0){
            return(struct token){.kind = OUTPUT, .value = NULL};
        }else if(strcmp(value, "goto") == 0){
            return(struct token){.kind = GOTO, .value = NULL}; 
        }else if(strcmp(value, "if") == 0){
            return(struct token){.kind = IF, .value = NULL};
        }else if(strcmp(value, "then") == 0){
            return(struct token){.kind = THEN, .value = NULL};  
        }else{
            return(struct token){.kind = IDENT, .value = value}; 
        } 

    }else{
        
        ds_string_slice slice = {.str = l->buffer + l->pos, .len = 1};
        char *value = NULL;
        ds_string_slice_to_owned(&slice, &value);
        lexer_read_char(l);
        return(struct token){.kind = INVALID, .value = value};
    }
}

int lexer_tokenize(char *buffer, unsigned int length, ds_dynamic_array *tokens){
    struct lexer lexer;
    lexer_init(&lexer,(char *)buffer, length);

    struct token tok;
    do{
        tok = lexer_next_token(&lexer);
        if (ds_dynamic_array_append(tokens, &tok) != 0){
            DS_PANIC("Failed to append token to array");
        }
    } while (tok.kind != END);
    return 0;
}

// What Parser Actually Does- Reads tokens, Checks grammar rules
// Makes sure syntax is correct, Builds structured tree representation

enum term_kind{
    TERM_INPUT,
    TERM_INT,
    TERM_IDENT
};

struct term_node{
    enum term_kind kind;
    union{
        char *value;
    };
};

enum expr_kind {
    EXPR_TERM,
    EXPR_PLUS,
    
};

struct term_binary_node{
    struct term_node lhs;
    struct term_node rhs;
};

struct expr_node{
    enum expr_kind kind;
    union{
        struct term_node term;
        struct expr_binary_node add;
            
    };
};

enum rel_kind {
    REL_LESS_THAN,
};

struct rel_node{
    enum rel_kind kind;
    union{
        struct rel_binary_node less_than;
            
    };
};



enum instr_kind {
    INSTR_ASSIGN,
    INSTR_IF,
    INSTR_GOTO,
    INSTR_OUTPUT,
    INSTR_LABEL
};

struct assign_node{
    char *ident;
    struct expr_node expr;
};
struct if_node{
    struct rel_node rel;
    struct instr_node *instr;
};

struct goto_node{
     char *label;
};

struct output_node{
    struct term_node term;
};

struct label_node{
    char *label;
};

struct instr_node {
    enum instr_kind kind;
    union{
        struct assign_node assign;
        struct if_node if_;
        struct goto_node goto_;
        struct output_node output;
        struct label_node label;
    };
};

struct program_node{
    ds_dynamic_array instrs; 
};

struct parser {
    ds_dynamic_array tokens;
    unsigned int index;
    unsigned int read_pos;
    struct token tok;
    
};

void parser_init(ds_dynamic_array tokens, struct parser *p){
    p->tokens = tokens;
    p->index = 0;
}

void parser_current(struct parser *p, struct token *token) { //peek
    ds_dynamic_array_get(&p->tokens, p->index, token);
}
void parser_advance(struct parser *p) { 
    p->index++;
}

parse_term(struct parser *p, struct term_node *term){
    struct token token;
    parser_current(p, &token);
    if (token.kind == INPUT){
        term->kind = TERM_INPUT;
    } else if (token.kind == INT){
        term->kind = TERM_INT;
        term->value = token.value;
    } else if (token.kind == IDENT){
        term->kind = TERM_IDENT;
        term->value = token.value;
    } else {
        DS_PANIC("Expected a term(input, int or ident) but found %s", show_token_kind(token.kind));
    }
}

void parse_expr(struct parser *p,struct expr_node *expr){
    struct token token;
    struct term_node lhs, rhs;

    parse_term(p, &lhs);
    
    parser_current(p, &token);
    if (token.kind != PLUS){
        parser_advance(p);
        parse_term(p,&rhs);
        expr->kind = EXPR_PLUS;
        expr->add.lhs = lhs;
        expr->add.rhs = rhs;
    } else{
        expr->kind = EXPR_TERM;
        expr -> term = lhs;
    }
    
}

void parse_assign(struct parser *p, struct instr_node *instr){
    struct token token;

    instr -> kind = INSTR_ASSIGN;

    parser_current(p, &token);
    instr->assign.ident = token.value;
    parser_advance(p);

    parser_current(p, &token);
    if (token.kind != EQUAL){
        DS_PANIC("Expected '=', found %s", show_token_kind(token.kind));
    }
    parser_advance(p);

    parse_expr(p, &instr->assign.expr);
}

void parse_if(struct parser *p, struct instr_node *instr){
    struct token token;

    instr -> kind = INSTR_IF;
    parser_advance(p);

    parse_rel(p, &instr -> if_.rel);
    parser_current(p, &token);
    if (token.kind != THEN){
        DS_PANIC("Expected 'THEN', found %s", show_token_kind(token.kind));
    }
    parser_advance(p);


    parse_instr(p, &instr->if_.instr);
}

void parse_instr(){
    struct instr_node instr;
        
        parser_current(p, &token);
        if (token.kind == IDENT)
        {
            parse_assign(p, &instr);  
        } else if (token.kind == IF)
        {
            parse_if(p, &instr);
        } else if (token.kind == GOTO)
        {
            parse_goto(p, &instr);
        } else if (token.kind == OUTPUT)
        {
            parse_output(p, &instr);
        } else if (token.kind == LABEL)
        {
            parse_label(p, &instr);
        } else {
            DS_PANIC("Unexpected token: %s", show_token_kind(token.kind));
        }
        parser_advance(p);
        ds_dynamic_array_append(&program->instrs, &instr);
}

void parse_program(struct parser *p, struct program_node *program){
    ds_dynamic_array_init(&program->instrs, sizeof(struct instr_node));

    struct token token;
    do
    {
        parse_instr(p);
        parser_current(p,&token);
    } while (token.kind != END);
    
}

int main(){
    char *buffer = NULL;
    int length = ds_io_read("example.txt", &buffer,"r");
    printf("%d\n", length);
    ds_dynamic_array tokens;
    ds_dynamic_array_init(&tokens, sizeof(struct token));

    lexer_tokenize(buffer, length, &tokens);
    printf("%d\n", tokens.count);
    for (unsigned i=0; i< tokens.count; i++)
    {
         struct token tok;
         ds_dynamic_array_get(&tokens, i, &tok); 
         print_token(tok); 
    }

    printf("%s", buffer);
}