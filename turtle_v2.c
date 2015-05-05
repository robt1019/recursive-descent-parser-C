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

typedef float Var_array[MAXNUMVARS];

int string_same(char *str1, char *str2);
void error (char *error_message);

void Main(Program *p, Var_array v);
void Instruction_list(Program *p, Var_array v);
void Instruction(Program *p, Var_array v);

void Varnum(Program *p, Var_array v);
void Number(Program *p, Var_array v);
void Var(Program *p, Var_array v);

int var_key(char *var);
void assign_var(Var_array v, int key, float val);

void Do(Program *p, Var_array v);
void Set(Program *p, Var_array v);

float Polish(Program *p, Stack *s, Var_array v);
void Op(Program *p);

float evaluate_expression(float a, float b, char *operand);
void Initialise_Stack(Stack *s);
float interpret_varnum(Program *p, Var_array v);
float Push(Stack *s, float value);
float Pop(Stack *s, Var_array v);

int main(int argc, char *argv[])
{
    int i;
    char *prog_name = argv[1];
    FILE *fp;
    Program prog;
    prog.current_word = 0;
    Var_array var_array;

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

    Main(&prog, var_array);

    printf ("Parsed OK\n");
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

void Main(Program *p, Var_array v){
    if(!string_same (p->words[p->current_word], "{")){
    error("Missing start brace?");
    }
    p->current_word++;
    Instruction_list(p, v);
}

void Instruction_list(Program *p, Var_array v){
    if (string_same(p->words[p->current_word], "}")){
        return;
    }
    Instruction(p, v);
    p->current_word++;
    Instruction_list(p, v);
}

void Instruction(Program *p, Var_array v){
    if(string_same(p->words[p->current_word], "FD")){
        printf("moving forward\n");
        p->current_word++;
        Varnum(p, v);
        return;
    }
    if(string_same(p->words[p->current_word], "LT")){
        p->current_word++;
        printf("turning left\n");
        Varnum(p, v);
        return;
    }
    if(string_same(p->words[p->current_word], "RT")){
        p->current_word++;
        printf("turning right\n");
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

void Varnum(Program *p, Var_array v){
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

void Number(Program *p, Var_array v){
    float varnum = 0;
    if(sscanf(p->words[p->current_word], "%f", &varnum) == 1 && varnum > 0 && varnum < MAXVARNUM){
        return;
    }
    Var(p, v);
    error("Invalid VARNUM?");
}

void Var(Program *p, Var_array v){
    char var = 0;
    //checks if Var is a letter from A to Z and if it's 1 character long
    if(sscanf(p->words[p->current_word], "%[A-Z]c", &var) == 1 && strlen(p->words[p->current_word]) == 1){
        return;
    }
    error("Invalid VAR?");
}

void Do(Program *p, Var_array v){
    int loop_start, loop_end, loop_count = 0, loop_start_position;
    Var(p, v);
    p->current_word++;
    if(string_same(p->words[p->current_word], "FROM")){
        p->current_word++;
        Varnum(p, v);
        loop_start = interpret_varnum(p, v);
        p->current_word++;
        if(string_same(p->words[p->current_word], "TO")){
            p->current_word++;
            Varnum(p, v);
            loop_end = interpret_varnum(p, v);
            p->current_word++;
            if(string_same(p->words[p->current_word], "{")){
                p->current_word++;
                loop_start_position = p->current_word;
                //Do loop code
                for (int i = loop_start; i <= loop_end; ++i){
                    //loops back to the start of the code within the DO loop if closing brace found
                    if(string_same(p->words[p->current_word], "}")){
                        p->current_word = loop_start_position;
                    }
                    Instruction_list(p, v);
                    p->current_word++;
                    loop_count++;
                }
                //to stop parser skipping ahead too far
                p->current_word--;
                return;
            }
        }
    }
    error("Incorrect format for DO command?");
}

void Set(Program *p, Var_array v){
    char *var = p->words[p->current_word];
    int key = var_key(var);
    float val;
    Var(p, v);
    Stack s;
    Initialise_Stack(&s);
    p->current_word++;
    if(string_same(p->words[p->current_word], ":=")){
        p->current_word++;
        val = Polish(p, &s, v);
        assign_var(v, key, val);
        return;
    }
    error("Invalid SET syntax?");
}

int var_key(char *var){
    int key = *var - CAPITALCONVERSION;
    return key;
}

void assign_var(Var_array v, int key, float val){
    v[key] = val;
    printf("assigned variable %f to variable %c\n", val, key + CAPITALCONVERSION);
}

float Polish(Program *p, Stack *s, Var_array v){
    char operand = 0;
    float a, b, value, result = 0;

    //checks if next instruction is an operand
    if(sscanf(p->words[p->current_word], "%[-,+,*,/]s", &operand) == 1){
        Op(p);
        b = Pop(s, v);
        a = Pop(s, v);
        result = evaluate_expression(a, b, &operand);
        Push(s, result);
    }
    else{
        Varnum(p, v);
        value = interpret_varnum(p, v);
        result = Push(s, value);
    }
    p->current_word++;
    if(string_same(p->words[p->current_word], ";")){
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
    if(string_same(operand, "+")){
        result = a + b;
    }
    if(string_same(operand, "-")){
        result = a - b;
    }
    if(string_same(operand, "*")){
        result = a * b;
    }
    if(string_same(operand, "/")){
        result = a / b;
    }
    return result;
}

void Initialise_Stack(Stack *s){
    s->top = 0;
}

//evaluates a varnum to see if it's a number or a variable. Returns the value of the varnum as a float
float interpret_varnum(Program *p, Var_array v){
    int key;
    char var;
    float num, val = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]s", &var) == 1){
        key = var_key(&var);
        val = v[key];
    }
    else if(sscanf(p->words[p->current_word], "%f", &num) == 1){
        val = num;       
    }
    return val;
}

float Push(Stack *s, float value){
    assert(s->top < STACKSIZE);
    s->stack[s->top].value = value;
    // printf("value %f pushed to stack\n", value);
    s->top = s->top + 1;
    return value;
}

float Pop(Stack *s, Var_array v){
    assert(s->top > 0);
    s->top = s->top - 1;
    return s->stack[s->top].value;
}

