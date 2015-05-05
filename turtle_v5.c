#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "display.h"

#define MAXNUMTOKENS 10000
#define MAXTOKENSIZE 100
#define MAXVARNUM 1000000
#define STACKSIZE 2000
#define MAXNUMVARS 24
#define CAPITAL_CONVERSION 65
#define PI 3.141592653
#define RADIAN_CONVERSION PI / 180
#define FULLTURN 360
#define ROUNDING_CONVERSION 0.5
#define BOARD_WIDTH 1000
#define BOARD_HEIGHT 600

enum status {assigned = 1, unnasigned = 0};
typedef enum status status;

typedef struct prog{
    char words[MAXNUMTOKENS][MAXTOKENSIZE];
    int current_word;
    int line_id[MAXNUMTOKENS];
    char *file_name;
} Program;

typedef struct element{
    double value;
    struct element *prev;
} Element;

typedef struct stack{
    Element stack[STACKSIZE];
    Element *top;
} Stack;

typedef struct co_ordinates{
    double current_angle;
    int x_start;
    int y_start;
    int x_finish;
    int y_finish;
} Co_ordinates;

typedef double Var_array[MAXNUMVARS][2];

void initialise_variable_array(Var_array v);
void initialise_words_array(Program *p);
FILE * open_file(char *file_name);
void scan_file(Program *p, FILE *fp);

int string_same(char *str1, char *str2);
void error (char *error_message, Program *p);

void Main(Program *p, Var_array v);
void Instruction_list(Program *p, Var_array v, Co_ordinates *c, window *w);
void Instruction(Program *p, Var_array v, Co_ordinates *c, window *w);

void Varnum(Program *p, Var_array v);
void Number(Program *p, Var_array v);
char Var(Program *p, Var_array v);

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
double Pop(Stack *s, Var_array v, Program *p);

int main(int argc, char *argv[])
{
    char *prog_name = argv[1];
    FILE *fp;
    Program prog;
    prog.current_word = 0;
    prog.file_name = argv[1];
    Var_array var_array;

    initialise_variable_array(var_array);

    initialise_words_array(&prog);

    fp = open_file(prog_name);

    scan_file(&prog, fp);

    Main(&prog, var_array);

    printf ("Parsed OK\n");
    return 0;
}

//set all variables in array to zero
void initialise_variable_array(Var_array v){
    for (int i = 0; i < MAXNUMVARS; ++i){
        v[i][1] = unnasigned;
        v[i][0] = unnasigned;
    }
}

//insert null escape codes to allow fscanf to properly scan in turtle tokens
void initialise_words_array(Program *p){
    for(int i = 0; i < MAXNUMTOKENS; ++i){
        p->words[i][0] = '\0';
    }
}

FILE * open_file(char *file_name){
    FILE *pointer;
     if(!(pointer = fopen (file_name, "r"))){
        printf("couldn't open file, %s\n", file_name);
        exit (1);
    }
    return pointer;
}

//scans in turtle file to array of words
void scan_file(Program *p, FILE *fp){
    p->current_word = 0;
    int line_number = 1;
    char c;
    while(fscanf (fp, "%s", p->words[p->current_word]) == 1 && p->current_word < MAXNUMTOKENS){
        //assigns each point in the word counter a line variable
        p->line_id[p->current_word] = line_number;
        //if a newline escape code is encountered after a line is scanned in then the line counter goes up one
        if (( c = fgetc(fp)) == '\n'){
            line_number++;
        }
        p->current_word++;
    }
    p->current_word = 0;
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
void error(char *error_message, Program *p){
    fprintf(stderr, "\n%s It happened on line %d in %s\n", error_message, p->line_id[p->current_word], p->file_name);
    printf("current word = %s\n", p->words[p->current_word]);
    printf("previous word = %s\n", p->words[p->current_word - 1]);
    SDL_Quit();
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
    error("Missing start brace?", p);
    }
    p->current_word++;
    Instruction_list(p, v, &co_ordinates, &w);
    SDL_Delay(5000);
}

void Instruction_list(Program *p, Var_array v, Co_ordinates *c, window *w){
    if (string_same(p->words[p->current_word], "}")){
        return;
    }
    Instruction(p, v, c, w);
    printf("current_word after Instruction call = %s\n", p->words[p->current_word]);
    p->current_word++;
    printf("current_word before Instruction_list call = %s\n", p->words[p->current_word]);
    Instruction_list(p, v, c, w);
}

void Instruction(Program *p, Var_array v, Co_ordinates *c, window *w){
    printf("p->current_word = %s\n", p->words[p->current_word]);
    printf("p->next_word = %s\n", p->words[p->current_word + 1]);
    double distance, turn_angle;
    if(string_same(p->words[p->current_word], "FD")){
        p->current_word++;
        Varnum(p, v);
        distance = interpret_varnum(p, v);
        Fd(c->current_angle, distance, p, c, w);
        return;
    }    
    if(string_same(p->words[p->current_word], "LT")){
        p->current_word++;
        Varnum(p, v);
        turn_angle = interpret_varnum(p, v);
        Lt(c, turn_angle, p);
        return;
    }
    if(string_same(p->words[p->current_word], "RT")){
        p->current_word++;
        Varnum(p, v);
        turn_angle = interpret_varnum(p, v);
        Rt(c, turn_angle, p);
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
    error("Invalid INSTRUCTION?", p);
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
    error("Invalid VARNUM?", p);
}

//checks if Var is a letter from A to Z and if it's 1 character long. Returns var as a character
char Var(Program *p, Var_array v){
    char var = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]c", &var) == 1 && strlen(p->words[p->current_word]) == 1){
        return var;
    }
    error("Invalid VAR?", p);
    exit(4);
}

void Do(Program *p, Var_array v, Co_ordinates *c, window *w){
    int loop_start, loop_end, loop_count = 0, loop_start_position, key;
    char var; 
    var = Var(p, v);
    p->current_word++;
    if(string_same(p->words[p->current_word], "FROM")){
        p->current_word++;
        Varnum(p, v);
        loop_start = interpret_varnum(p, v);
        key = var_key(&var);
        //set variable to initial value at start of loop
        assign_var(v, key, loop_start);
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
                        if(i == loop_end){
                            printf("p->current_word inside loop end condition = %s\n", p->words[p->current_word]);
                            return;
                        }
                        p->current_word = loop_start_position;
                        printf("i = %d\n", i);
                        printf("p->current_word in for loop = %s\n", p->words[p->current_word]);
                        printf("p->next_word in for loop = %s\n", p->words[p->current_word + 1]);
                    }
                    Instruction_list(p, v, c, w);
                    p->current_word++;
                    loop_count++;
                    //increment loop variable value
                    assign_var(v, key, loop_count);
                    printf("A = %lf\n", *v[0]);
                    printf("p->current_word after for loop = %s\n", p->words[p->current_word]);
                }
                printf("previous->word after do loop = %s\n", p->words[p->current_word - 1]);
                printf("current->word after do loop = %s\n", p->words[p->current_word]);
                return;
            }
        }
    }
    error("Incorrect format for DO command?", p);
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
    error("Invalid SET syntax?", p);
}

void Lt(Co_ordinates *c, double turn_angle, Program *p){
    c->current_angle = c->current_angle + turn_angle;
    if(c->current_angle >= FULLTURN){
        c->current_angle = c->current_angle - FULLTURN;
    }
}

void Rt(Co_ordinates *c, double turn_angle, Program *p){
    c->current_angle = c->current_angle - turn_angle;
    //if new angle is less than 0, wrap around and start at 360 degrees again
    if(c->current_angle < 0){
        c->current_angle = FULLTURN - - c->current_angle;
    }
}

void Fd(double current_angle, double distance, Program *p, Co_ordinates *c, window *w){

    c->x_finish = c->x_start + ROUNDING_CONVERSION + (distance * cos(current_angle * RADIAN_CONVERSION));
    c->y_finish = c->y_start + ROUNDING_CONVERSION - (distance * sin(current_angle * RADIAN_CONVERSION));

    SDL_SetRenderDrawColor(w->renderer, 248, 248, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(w->renderer, (int)c->x_start, (int)c->y_start, (int)c->x_finish, (int)c->y_finish);
    SDL_RenderPresent(w->renderer);
    SDL_UpdateWindowSurface(w->win);
    SDL_Delay(5);

    c->x_start = c->x_finish;
    c->y_start = c->y_finish;
}

int var_key(char *var){
    int key = *var - CAPITAL_CONVERSION;
    return key;
}

void assign_var(Var_array v, int key, double val){
    v[key][0] = val;
    v[key][1] = assigned;
}

double Polish(Program *p, Stack *s, Var_array v){
    char operand = 0;
    double a, b, value, result = 0;

    //checks if next instruction is an operand
    if(sscanf(p->words[p->current_word], "%[-,+,*,/]s", &operand) == 1){
        Op(p);
        b = Pop(s, v, p);
        a = Pop(s, v, p);
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
    error("Invalid OPERATOR?", p);
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
    s->top = (Element *) calloc(1, sizeof(Element));
    s->top->prev = NULL;
}

//evaluates a varnum to see if it's a number or a variable. Returns the value of the varnum as a double
double interpret_varnum(Program *p, Var_array v){
    int key;
    char var;
    double num, val = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]s", &var) == 1){
        key = var_key(&var);
        val = v[key][0];
        if(v[key][1] == unnasigned){
            error("Unnasigned variable?", p);
        }
    }
    else if(sscanf(p->words[p->current_word], "%lf", &num) == 1){
        val = num;       
    }
    return val;
}

double Push(Stack *s, double value){
    Element *e;
    e = (Element *) calloc(1, sizeof(Element));
    e->prev = s->top;
    s->top->value = value;
    s->top = e;
    return value;
}

double Pop(Stack *s, Var_array v, Program *p){
    s->top = s->top->prev;      
    if(s->top == NULL){
        error("Incorrect number of variables?", p);
    }
    assert(s->top != NULL);
    return s->top->value;
}
