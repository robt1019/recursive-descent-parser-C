#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "display.h"

#define MAXNUMTOKENS 1000
#define MAXTOKENSIZE 10
#define MAXVARNUM 100
#define STACKSIZE 200
#define MAXNUMVARS 24
#define CAPITAL_CONVERSION 65
#define PI 3.141592653
#define RADIAN_CONVERSION PI / 180
#define FULLTURN 360
#define ROUNDING_CONVERSION 0.5
#define BOARD_WIDTH 1000
#define BOARD_HEIGHT 600

typedef struct prog{
    char words[MAXNUMTOKENS][MAXTOKENSIZE];
    int current_word;
} Program;

typedef struct element{
    double value;
} Element;

typedef struct stack{
    Element stack[STACKSIZE];
    int top;
} Stack;

typedef struct co_ordinates{
    double current_angle;
    int x_start;
    int y_start;
    int x_finish;
    int y_finish;
} Co_ordinates;

typedef double Var_array[MAXNUMVARS];

int string_same(char *str1, char *str2);
void error (char *error_message);

void Main(Program *p, Var_array v);
void Instruction_list(Program *p, Var_array v, Co_ordinates *c, window *w);
void Instruction(Program *p, Var_array v, Co_ordinates *c, window *w);

void Varnum(Program *p, Var_array v);
void Number(Program *p, Var_array v);
void Var(Program *p, Var_array v);

int var_key(char *var);
void assign_var(Var_array v, int key, double val);

void Do(Program *p, Var_array v, Co_ordinates *c, window *w);
void Set(Program *p, Var_array v);

void Lt(Co_ordinates *c, double turn_angle, Program *p);
void Rt(Co_ordinates *c, double turn_angle, Program *p);
void Fd(double current_angle, double distance, Program *p, Co_ordinates *c, window *w);

double Polish(Program *p, Stack *s, Var_array v);
void Op(Program *p);

double evaluate_expression(double a, double b, char *operand);
void Initialise_Stack(Stack *s);
double interpret_varnum(Program *p, Var_array v);
double Push(Stack *s, double value);
double Pop(Stack *s, Var_array v);

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
    Co_ordinates co_ordinates;
    //Make an initialize co_ordinates function
    co_ordinates.current_angle = 90;
    co_ordinates.x_start = 500;
    co_ordinates.y_start = 350;
    window w;
    setup_SDL(&w, BOARD_HEIGHT, BOARD_WIDTH);
    if(!string_same (p->words[p->current_word], "{")){
    error("Missing start brace?");
    }
    p->current_word++;
    Instruction_list(p, v, &co_ordinates, &w);
    SDL_Delay(1000);
}

void Instruction_list(Program *p, Var_array v, Co_ordinates *c, window *w){
    if (string_same(p->words[p->current_word], "}")){
        return;
    }
    Instruction(p, v, c, w);
    p->current_word++;
    Instruction_list(p, v, c, w);
}

void Instruction(Program *p, Var_array v, Co_ordinates *c, window *w){
    check_for_quit(w);
    double distance, turn_angle;
    if(string_same(p->words[p->current_word], "FD")){
        printf("moving forward\n");
        p->current_word++;
        Varnum(p, v);
        distance = interpret_varnum(p, v);
        Fd(c->current_angle, distance, p, c, w);
        return;
    }    
    if(string_same(p->words[p->current_word], "LT")){
        p->current_word++;
        printf("turning left\n");
        Varnum(p, v);
        turn_angle = interpret_varnum(p, v);
        Lt(c, turn_angle, p);
        printf("new current_angle = %lf\n", c->current_angle);
        return;
    }
    if(string_same(p->words[p->current_word], "RT")){
        p->current_word++;
        printf("turning right\n");
        Varnum(p, v);
        turn_angle = interpret_varnum(p, v);
        Rt(c, turn_angle, p);
        printf("new current_angle = %lf\n", c->current_angle);
        return;
    }
    if(string_same(p->words[p->current_word], "DO")){
        p->current_word++;
        Do(p, v, c, w);
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
    double varnum = 0;
    //if Varnum is an integer
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1){
        Number(p, v);
        return;
    }
    else{
        Var(p, v);
        return;
    }
}

void Number(Program *p, Var_array v){
    double varnum = 0;
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1 && varnum > 0 && varnum < MAXVARNUM){
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

void Do(Program *p, Var_array v, Co_ordinates *c, window *w){
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
                    Instruction_list(p, v, c, w);
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
    double val;
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

void Lt(Co_ordinates *c, double turn_angle, Program *p){
    c->current_angle = c->current_angle + turn_angle;
    if(c->current_angle >= FULLTURN){
        c->current_angle = c->current_angle - FULLTURN;
    }
}

void Rt(Co_ordinates *c, double turn_angle, Program *p){
    c->current_angle = c->current_angle - turn_angle;
    if(c->current_angle < 0){
        c->current_angle = FULLTURN - turn_angle;
    }
}

void Fd(double current_angle, double distance, Program *p, Co_ordinates *c, window *w){
    printf("current angle = %lf\n", c->current_angle);
    printf("x:y start = %d:%d\n", c->x_start, c->y_start);
    printf("cos(%lf) = %lf\n", current_angle, cos(current_angle * RADIAN_CONVERSION));
    printf("sin(%lf) = %lf\n", current_angle, cos(current_angle * RADIAN_CONVERSION));
    c->x_finish = c->x_start + ROUNDING_CONVERSION + (distance * cos(current_angle * RADIAN_CONVERSION));
    c->y_finish = c->y_start + ROUNDING_CONVERSION + (distance * sin(current_angle * RADIAN_CONVERSION));
    printf("x:y finish = %d:%d\n", c->x_finish, c->y_finish);
    SDL_SetRenderDrawColor(w->renderer, 248, 248, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(w->renderer, (int)c->x_start, (int)c->y_start, (int)c->x_finish, (int)c->y_finish);
    printf("SDL_RenderDrawLine returns %d\n", SDL_RenderDrawLine(w->renderer, (int)c->x_start, (int)c->y_start, (int)c->x_finish, (int)c->y_finish));
    SDL_RenderPresent(w->renderer);
    SDL_UpdateWindowSurface(w->win);
    SDL_Delay(50);
    c->x_start = c->x_finish;
    c->y_start = c->y_finish;
    printf("new x:y start = %d:%d\n", c->x_start, c->y_start);
}

int var_key(char *var){
    int key = *var - CAPITAL_CONVERSION;
    return key;
}

void assign_var(Var_array v, int key, double val){
    v[key] = val;
    printf("assigned variable %lf to variable %c\n", val, key + CAPITAL_CONVERSION);
}

double Polish(Program *p, Stack *s, Var_array v){
    char operand = 0;
    double a, b, value, result = 0;

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

double evaluate_expression(double a, double b, char *operand){
    double result = 0;
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

//evaluates a varnum to see if it's a number or a variable. Returns the value of the varnum as a double
double interpret_varnum(Program *p, Var_array v){
    int key;
    char var;
    double num, val = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]s", &var) == 1){
        key = var_key(&var);
        val = v[key];
    }
    else if(sscanf(p->words[p->current_word], "%lf", &num) == 1){
        val = num;       
    }
    return val;
}

double Push(Stack *s, double value){
    assert(s->top < STACKSIZE);
    s->stack[s->top].value = value;
    // printf("value %lf pushed to stack\n", value);
    s->top = s->top + 1;
    return value;
}

double Pop(Stack *s, Var_array v){
    assert(s->top > 0);
    s->top = s->top - 1;
    return s->stack[s->top].value;
}