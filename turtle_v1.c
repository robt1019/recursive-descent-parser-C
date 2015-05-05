#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAXNUMTOKENS 1000
#define MAXTOKENSIZE 10
#define MAXVARNUM 150
#define STACKSIZE 200
#define MAXNUMVARS 24
#define CAPITALCONVERSION 65

typedef struct prog{
    char words[MAXNUMTOKENS][MAXTOKENSIZE];
    int current_word;
} Program;

typedef struct element{
    float value;
} Element;

typedef struct stack{
    Element stack[STACKSIZE];
    int top;
} Stack;

typedef struct var_array{
    float value;
} var_array;


int string_same(char *str1, char *str2);
void error (char *error_message);
var_array * make_2d_var_array(int size);

void Main(Program *p, var_array **v);
void Instruction_list(Program *p, var_array **v);
void Instruction(Program *p, var_array **v);

void Varnum(Program *p, var_array **v);
void Number(Program *p, var_array **v);
void Var(Program *p, var_array **v);

int var_key(char *var);
void assign_var(var_array **v, int key, float val);

void Do(Program *p, var_array **v);
void Set(Program *p, var_array **v);

float Polish(Program *p, Stack *s, var_array **v);
void Op(Program *p);

float evaluate_expression(float a, float b, char *operand);
void Initialise_Stack(Stack *s);
float Push(Stack *s, Program *p, var_array **v);
float Pop(Stack *s, var_array **v);

int main(int argc, char *argv[])
{
    int i;
    char *prog_name = argv[1];
    FILE *fp;
    Program prog;
    prog.current_word = 0;
    var_array *var_table;
    var_table = make_2d_var_array(MAXNUMTOKENS);

    for(int i = 0; i < MAXNUMTOKENS; ++i){
        prog.words[i][0] = '\0';
    }

    if(!(fp = fopen (prog_name, "r"))){
        printf("couldn't open file, %s\n", prog_name);
        exit (1);
    }

    i = 0;
    while(fscanf (fp, "%s", prog.words[i]) == 1 && i < MAXNUMTOKENS){
        i++;
    }

    Main(&prog, &var_table);

    printf ("Parsed OK\n");
    free(var_table);
    return 0;
}

//returns a 1 if two strings entered are the same, 0 otherwise
int string_same(char *str1, char *str2){
    if(strcmp (str1, str2) == 0){
        return 1;
    }
    else{
        return 0;
    }
}

//prints a specific error message, shows line/file of error and exits program
void error(char *error_message){
    fprintf(stderr, "\n%s It happened on line %d in %s\n", error_message, __LINE__, __FILE__);
    exit(2);
}

var_array * make_2d_var_array(int size){
    var_array *var_array_pointer;
    var_array_pointer = malloc(sizeof(var_array) * size);
    return var_array_pointer;
}

void Main(Program *p, var_array **v){
    if(!string_same (p->words[p->current_word], "{")){
    error("Missing start brace?");
    }
    p->current_word++;
    Instruction_list(p, v);
}

void Instruction_list(Program *p, var_array **v){
    if (string_same(p->words[p->current_word], "}")){
        return;
    }
    Instruction(p, v);
    p->current_word++;
    Instruction_list(p, v);
}

void Instruction(Program *p, var_array **v){
    if(string_same(p->words[p->current_word], "FD")){
        p->current_word++;
        Varnum(p, v);
        return;
    }
    if(string_same(p->words[p->current_word], "LT")){
        p->current_word++;
        Varnum(p, v);
        return;
    }
    if(string_same(p->words[p->current_word], "RT")){
        p->current_word++;
        Varnum(p, v);
        return;
    }
    if(string_same(p->words[p->current_word], "DO")){
        p->current_word++;
        Do(p, v);
        return;
    }
    if(string_same(p->words[p->current_word], "SET")){
        p->current_word++;
        Set(p, v);
        return;
    }
    error("Invalid INSTRUCTION?");
}

void Varnum(Program *p, var_array **v){
    float varnum = 0;
    //if Varnum is an integer
    if(sscanf(p->words[p->current_word], "%f", &varnum) == 1){
        Number(p, v);
        return;
    }
    else{
        Var(p, v);
        return;
    }
}

void Number(Program *p, var_array **v){
    float varnum = 0;
    if(sscanf(p->words[p->current_word], "%f", &varnum) == 1 && varnum > 0 && varnum < MAXVARNUM){
        return;
    }
    Var(p, v);
    error("Invalid VARNUM?");
}

void Var(Program *p, var_array **v){
    char var = 0;
    //checks if Var is a letter from A to Z and if it's 1 character long
    if(sscanf(p->words[p->current_word], "%[A-Z]c", &var) == 1 && strlen(p->words[p->current_word]) == 1){
        return;
    }
    printf("current_word = %s\n", p->words[p->current_word]);
    error("Invalid VAR?");
}

void Do(Program *p, var_array **v){
    Var(p, v);
    p->current_word++;
    if(string_same(p->words[p->current_word], "FROM")){
        p->current_word++;
        Varnum(p, v);
        p->current_word++;
        if(string_same(p->words[p->current_word], "TO")){
            p->current_word++;
            Varnum(p, v);
            p->current_word++;
            if(string_same(p->words[p->current_word], "{")){
                p->current_word++;
                Instruction_list(p, v);
                return;
            }
        }
    }
    error("Incorrect format for DO command?");
}

void Set(Program *p, var_array **v){
    char *var = p->words[p->current_word];
    int key = var_key(var);
    float val;
    Var(p, v);
    Stack s;
    Initialise_Stack(&s);
    p->current_word++;
    if(string_same(p->words[p->current_word], ":=")){
        printf("got heeeeeeeere\n");
        p->current_word++;
        val = Polish(p, &s, v);
        printf("val = %f\n", val);
        printf("got here...\n");
        assign_var(v, key, val);
        return;
    }
    error("Invalid SET syntax?");
}

int var_key(char *var){
    int key = *var - CAPITALCONVERSION;
    printf("*var = %d\n", *var);
    printf("key = %d\n", key);
    return key;
}

void assign_var(var_array **v, int key, float val){
    printf("made it this far\n");
    printf("key = %d\n", key);
    printf("val = %f\n", val);
    printf("v[key]->value = %f\n", v[key]->value);
    v[key]->value = val;
    printf("assigned variable %f to var_array %d\n", val, key);
}

float Polish(Program *p, Stack *s, var_array **v){
    char instruction = 0;
    float a, b, result = 0;
    printf("s->top = %d\n", s->top);

    //checks if next instruction is an operand
    printf("current_word = %s\n", p->words[p->current_word]);
    if(sscanf(p->words[p->current_word], "%[+,-,*,/]s", &instruction) == 1){
        printf("here 1\n");
        printf("instruction = %c\n", instruction);
        Op(p);
        printf("here 2\n");
        b = Pop(s, v);
        printf("b = %f\n", b);
        a = Pop(s, v);
        printf("a = %f\n", a);
        result = evaluate_expression(a, b, &instruction);
        Push(s, p, v);
    }
    else{
        Varnum(p, v);
        printf("current_word = %s\n", p->words[p->current_word]);
        result = Push(s, p, v);
    }
    p->current_word++;
    if(string_same(p->words[p->current_word], ";")){
        printf("returning result %f\n", result);
        return result;
    }
    result = Polish(p, s, v);
    return result;
}

void Op(Program *p){
    if(string_same(p->words[p->current_word], "+")){
        return;
    }
    if(string_same(p->words[p->current_word], "-")){
        return;
    }
    if(string_same(p->words[p->current_word], "*")){
        return;
    }
    if(string_same(p->words[p->current_word], "/")){
        return;
    }
    error("Invalid OPERATOR?");
}

float evaluate_expression(float a, float b, char *operand){
    float result = 0;
    printf("operand = %c\n", *operand);
    if(string_same(operand, "+")){
        result = a + b;
    }
    if(string_same(operand, "-")){
        result = a - b;
    }
    if(string_same(operand, "*")){
        result = a * b;
        printf("a * b = %f\n", result);
    }
    if(string_same(operand, "/")){
        result = a / b;
    }
    return result;
}

void Initialise_Stack(Stack *s){
    s->top = 0;
}

float Push(Stack *s, Program *p, var_array **v){
    char var;
    int key;
    float num, val = 0;
    assert(s->top < STACKSIZE);
    if(sscanf(p->words[p->current_word], "%[A-Z]s", &var) == 1){
        printf("got here!!!\n");
        key = var_key(&var);
        s->stack[s->top].value = v[key]->value;
        printf("v[key]->value = %f\n", v[key]->value);
        val = v[key]->value;
    }
    else if(sscanf(p->words[p->current_word], "%f", &num) == 1){
        s->stack[s->top].value = num;
        val = num;       
    }
    s->top = s->top + 1;
    return val;
}

float Pop(Stack *s, var_array **v){
    assert(s->top > 0);
    s->top = s->top - 1;
    printf("stack value = %f\n", s->stack[s->top].value);
    // if(sscanf((s->stack[s->top].value, "%f", &variable_value) != 1))
    return s->stack[s->top].value;
}

