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

    // printf("%c\n",l->ch);

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

struct expr_binary_node {
    struct term_node lhs;
    struct term_node rhs;
};

struct rel_binary_node {
    struct term_node lhs;
    struct term_node rhs;
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


void parse_expr(struct parser *p, struct expr_node *expr);
void parse_rel(struct parser *p, struct rel_node *rel);
void parse_assign(struct parser *p, struct instr_node *instr);
void parse_goto(struct parser *p, struct instr_node *instr);
void parse_output(struct parser *p, struct instr_node *instr);
void parse_label(struct parser *p, struct instr_node *instr);
void parse_if(struct parser *p, struct instr_node *instr);
void parse_instr(struct parser *p, struct instr_node *instr);

void parse_term(struct parser *p, struct term_node *term){
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
    parser_advance(p);
}

void parse_expr(struct parser *p,struct expr_node *expr){
    struct token token;
    struct term_node lhs, rhs;

    parse_term(p, &lhs);
    
    parser_current(p, &token);
    if (token.kind == PLUS){
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

void parse_rel(struct parser *p, struct rel_node *rel){
    struct token token;
    struct term_node lhs, rhs;

    parse_term(p, &lhs);

    parser_current(p, &token);

    if (token.kind == LESS_THAN){
        parser_advance(p);
        parse_term(p,&rhs); 

        rel->kind = REL_LESS_THAN;
        rel->less_than.lhs = lhs;
        rel->less_than.rhs = rhs;
    } else{
        DS_PANIC("Expected rel '<', found %s", show_token_kind(token.kind));
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

void parse_goto(struct parser *p, struct instr_node *instr){
    struct token token;
    
    instr->kind = INSTR_GOTO;
    parser_advance(p);

    parser_current(p, &token);
    if (token.kind != LABEL){
        DS_PANIC("Expected 'label', found %s", show_token_kind(token.kind));
    }
    parser_advance(p);
     
    instr->goto_.label = token.value;

}

void parse_output(struct parser *p, struct instr_node *instr){
    struct token token;
    struct term_node lhs;

    instr->kind = INSTR_OUTPUT;
    parser_advance(p);

    parse_term(p,&lhs);
     
    instr->output.term = lhs;

}

void parse_label(struct parser *p, struct instr_node *instr){
    struct token token;

    instr->kind = INSTR_LABEL;

    parser_current(p, &token);
    instr->label.label = token.value;

    parser_advance(p);
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

    instr-> if_.instr = malloc(sizeof(struct instr_node));
    parse_instr(p, instr->if_.instr);
}

void parse_instr(struct parser *p, struct instr_node *instr){
    
    struct token token;
        
        parser_current(p, &token);
        if (token.kind == IDENT)
        {
            parse_assign(p, instr);  
        } else if (token.kind == IF)
        {
            parse_if(p, instr);
        } else if (token.kind == GOTO)
        {
            parse_goto(p, instr);
        } else if (token.kind == OUTPUT)
        {
            parse_output(p, instr);
        } else if (token.kind == LABEL)
        {
            parse_label(p, instr);
        } else {
            DS_PANIC("Unexpected token: %s", show_token_kind(token.kind));
        } 
        
}

int find_variable(ds_dynamic_array *variables, char *ident){
    for(unsigned int i = 0; i < variables->count; i++){
        char *variable = NULL;
        ds_dynamic_array_get(variables, i, &variable);

        if (strcmp(ident, variable) == 0){
            return i; //return index of variable
        }
    }
    return -1;
}

void parse_program(struct parser *p, struct program_node *program){
    ds_dynamic_array_init(&program->instrs, sizeof(struct instr_node));

    struct token token;
    do
    {
        struct instr_node instr;

        parse_instr(p, &instr);

        ds_dynamic_array_append(&program->instrs, &instr);

        parser_current(p, &token);
    } while (token.kind != END);
    
}


void print_term(struct term_node *term){
    switch (term->kind){
        case TERM_INPUT: printf("input\n"); break;
        case TERM_INT: printf("%s\n", term->value); break;
        case TERM_IDENT: printf("%s\n", term->value); break;
    }

}

void print_instr(struct instr_node instr){
    switch (instr.kind) {

            case INSTR_ASSIGN: {
                printf("assign %s\n", instr.assign.ident);

                switch (instr.assign.expr.kind) {
                    case EXPR_TERM:
                        print_term(&instr.assign.expr.term);
                        break;
                    case EXPR_PLUS:
                        print_term(&instr.assign.expr.add.lhs);
                        printf("+\n"); 
                        print_term(&instr.assign.expr.add.rhs);
                        break;
                }
                break;
            }

            case INSTR_IF:
                printf("if\n");
                switch(instr.if_.rel.kind){
                    case REL_LESS_THAN:
                        print_term(&instr.if_.rel.less_than.lhs);
                        printf("<\n");
                        print_term(&instr.if_.rel.less_than.rhs);
                        break;
                }
                print_instr(*instr.if_.instr);   
                break;

            case INSTR_GOTO:
                printf("goto %s\n", instr.goto_.label);
                break;

            case INSTR_OUTPUT:
                printf("output ");
                print_term(&instr.output.term);
                break;

            case INSTR_LABEL:
                printf("label %s:\n", instr.label.label);
                break;

            default:
                break;
        }
}

void print_program(struct program_node *program) {
    for (unsigned int i = 0; i < program->instrs.count; i++) {
        struct instr_node instr;
        ds_dynamic_array_get(&program->instrs, i, &instr);
        print_instr(instr);
    }
}

void expr_asm(struct expr_node expr, ds_dynamic_array *variables);
void term_declare_variables(struct term_node *term , ds_dynamic_array *variables);
void instr_asm(struct instr_node *instr, ds_dynamic_array *variables);


void instr_asm(struct instr_node *instr, ds_dynamic_array *variables){
    switch (instr->kind) {
    case INSTR_ASSIGN: {
        expr_asm(instr->assign.expr, variables); 
        //the result is int in rax, rax is like the default register 
        // for storing results of expressions
        int index = find_variable(variables, instr->assign.ident);
        printf("%d\n", index);
        break;        
    }
    case INSTR_IF:{
        break;
    }
    case INSTR_GOTO:{
        printf("    jmp %s\n", instr->goto_.label); 
        break;
    }
    case INSTR_OUTPUT:{

        break;
    }
    case INSTR_LABEL:{
        printf("%s:\n", instr->label.label); 
        break;
    }
    default:
         break;
    }
}

void expr_asm(struct expr_node expr, ds_dynamic_array *variables){
    switch(expr.kind){

        case EXPR_TERM:
            if(expr.term.kind == TERM_INT){
            }
            else if(expr.term.kind == TERM_IDENT){
                int index = find_variable(variables, expr.term.value);
                printf("     %s\n", expr.term.value);
            }
            break;

        case EXPR_PLUS:
            break;
    }
}

void term_declare_variables(struct term_node *term , ds_dynamic_array *variables){
    switch (term->kind){
        case TERM_INPUT:
            break;
        case TERM_INT:
            break;
        case TERM_IDENT: 
            for(unsigned int i = 0; i < variables->count; i++){
                char *variable = NULL;
                ds_dynamic_array_get(variables, i, &variable);

                if (strcmp(term->value, variable) == 0){
                    return; //variable already declared
                }
            }
            DS_PANIC("Identifier is not defined: %s", term->value);
            break;
    }
}
void expr_declare_variables(struct expr_node *expr , ds_dynamic_array *variables){
    switch (expr->kind) {
        case EXPR_TERM:
            term_declare_variables(&expr->term, variables);
            
            break;
        case EXPR_PLUS:
            term_declare_variables(&expr->add.lhs, variables);
            term_declare_variables(&expr->add.rhs, variables);
            break;
    }
}

void rel_declare_variables(struct rel_node *rel, ds_dynamic_array *variables){
    switch (rel->kind) {
        case REL_LESS_THAN:
            term_declare_variables(&rel->less_than.lhs, variables);
            term_declare_variables(&rel->less_than.rhs, variables);
            break;
    }
}

void instr_declare_variables(struct instr_node *instr , ds_dynamic_array *variables){
    switch (instr->kind) {
    case INSTR_ASSIGN: {
        expr_declare_variables(&instr->assign.expr, variables);
        
        for(unsigned int i = 0; i < variables->count; i++){
                char *variable = NULL;
                ds_dynamic_array_get(variables, i, &variable);

                if (strcmp(instr->assign.ident, variable) == 0){
                    return;
                }  
            }
        ds_dynamic_array_append(variables, &instr->assign.ident);
        break;      
    }
    case INSTR_IF:{
        rel_declare_variables(&instr->if_.rel, variables);
        instr_declare_variables(instr->if_.instr, variables);
        break;
    }
    case INSTR_GOTO:{
        
        break;
    }
    case INSTR_OUTPUT:{
        term_declare_variables(&instr->output.term, variables);
        break;
    }
    case INSTR_LABEL:{
         
        break;
    }
    default:
         break;
    }
}
void program_asm(struct program_node *program){
    ds_dynamic_array variables;
    ds_dynamic_array_init(&variables, sizeof(char*));

    for (unsigned int i = 0; i < program->instrs.count; i++) {
        struct instr_node instr;
        ds_dynamic_array_get(&program->instrs, i, &instr);

        instr_declare_variables(&instr, &variables);
    }
    printf("    sub rsp, %d\n", variables.count * 8); //allocate space for variables on stack
    for (unsigned int i = 0; i < program->instrs.count; i++) {
        struct instr_node instr;
        ds_dynamic_array_get(&program->instrs, i, &instr);
        instr_asm(&instr, &variables);
    }
    printf("    add rsp, %d\n", variables.count * 8); //deallocate space for variables on stack
}

int main(){
    char *buffer = NULL;
    int length = ds_io_read("example.txt", &buffer,"r");
    // printf("%d\n", length);
    ds_dynamic_array tokens;
    ds_dynamic_array_init(&tokens, sizeof(struct token));

    lexer_tokenize(buffer, length, &tokens);
    // printf("%d\n", tokens.count);
    // for (unsigned i=0; i< tokens.count; i++)
    // {
    //      struct token tok;
    //      ds_dynamic_array_get(&tokens, i, &tok); 
    //      print_token(tok); 
    // }

    struct parser p;
    parser_init(tokens, &p);

    struct program_node program;
    parse_program(&p, &program);
    // print_program(&program);
    program_asm(&program);
}