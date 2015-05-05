#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "display.h"

#define MAXNUMTOKENS 10000
#define MAXTOKENSIZE 100
#define MAXVARNUM 10000
#define STACKSIZE 2000
#define MAXNUMVARS 24
#define CAPITAL_CONVERSION 65
#define PI 3.141592653
#define RADIAN_CONVERSION PI / 180
#define FULLTURN 360
#define ROUNDING_CONVERSION 0.5
#define BOARD_WIDTH 1000
#define BOARD_HEIGHT 600
#define NORTH 90

enum status {unassigned = 0, assigned = 1};
typedef enum status status;
enum comparator {equals = 0, less_than = 1, greater_than = 2};
typedef enum comparator comparator;

typedef struct prog{
    char words[MAXNUMTOKENS][MAXTOKENSIZE];
    int current_word;
    int line_id[MAXNUMTOKENS];
    int variable_key;
    char *file_name;
    int loop_start;
    int loop_end;
    int loop_start_address;
    double a;
    double b;
    comparator comp;
} Program;

typedef struct element{
    double value;
    struct element *prev;
} Element;

typedef struct stack{
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

///////////////////////////////////////////////TESTS////////////////////////////////////////////////

void run_tests();
void open_file_test(FILE *test_fp);
void scan_file_test(FILE *test_fp);
void set_default_start_test(FILE *fp);
void initialise_variable_array_test(FILE *fp);
void initialise_words_array_test(FILE *fp);
void word_found_test(FILE *test_fp);
void string_same_test(FILE *test_fp);
void Var_test(FILE *test_fp);
void Number_test(FILE *test_fp);
void Varnum_test(FILE *test_fp);
void set_loop_start_val_test(FILE *test_fp);
void set_loop_end_val_test(FILE *test_fp);
void execute_loop_test(FILE *test_fp);
void Do_test(FILE *test_fp);

////////////////////////////////////////////PROTOTYPES//////////////////////////////////////////////


///////////////////////////////////////////FILE SET UP//////////////////////////////////////////////

void initialise_program(Program *p, char *file_name);
void set_default_start(Co_ordinates *c);
void initialise_variable_array(Var_array v);
void initialise_words_array(Program *p);
FILE * open_file(char *file_name, char *read_write);
void scan_file(Program *p, FILE *fp);

////////////////////////////////////////GENERAL FUNCTIONS///////////////////////////////////////////

int word_found(char *word, Program *p);
int string_same(char *str1, char *str2);
void error (char *error_message, Program *p);

/////////////////////////////////////////TURTLE FUNCTIONS///////////////////////////////////////////

void Launch(Program *p, Var_array v);

//////////////////////////////////////INSTRUCTION FUNCTIONS/////////////////////////////////////////

void Instruction_list(Program *p, Var_array v, Co_ordinates *c, window *w);

void Instruction(Program *p, Var_array v, Co_ordinates *c, window *w);
int movement_instruction(Program *p, Var_array v, Co_ordinates *c, window *w);

/////////////////////////////////////////VARNUM FUNCITONS///////////////////////////////////////////

void Varnum(Program *p, Var_array v);
int Varnum_for_test(Program*p, Var_array v);
void Number(Program *p, Var_array v);
int Number_for_test(Program *p, Var_array v);
void Var(Program *p, Var_array v);
int Var_for_test (Program *p, Var_array v);
int var_key(char *var);
void assign_var(Var_array v, int key, double val);
double interpret_varnum(Program *p, Var_array v);

/////////////////////////////////////////DO FUNCTIONS///////////////////////////////////////////////

void Do(Program *p, Var_array v, Co_ordinates *c, window *w);
int Do_for_test(Program *p, Var_array v);
void set_loop_start_val(Program *p, Var_array v);
void set_loop_end_val(Program *p, Var_array v);
void execute_loop(Program *p, Var_array v, Co_ordinates *c, window *w);
int execute_loop_for_test(Program *p, Var_array v);

////////////////////////////////////////SET FUNCTIONS///////////////////////////////////////////////

void Set(Program *p, Var_array v);

void Set_draw(Program *p, Var_array v, Co_ordinates *c);

/////////////////////////////////////////IF FUNCTIONS///////////////////////////////////////////////

void If(Program *p, Var_array v, Co_ordinates *c, window *w);
void set_if_statement(Program *p, Var_array v);
void evaluate_if_statement(Program *p, Var_array v, Co_ordinates *c, window *w);
comparator Comparator(Program *p, Var_array v);
int evaluate_comparison(Program *p, Var_array v);

/////////////////////////////////////MOVEMENT FUNCTIONS/////////////////////////////////////////////

void Lt(Co_ordinates *c, double turn_angle, Program *p);
void Rt(Co_ordinates *c, double turn_angle, Program *p);
void Fd(double current_angle, double distance, Program *p, Co_ordinates *c, window *w);

////////////////////////////////////////POLISH FUNCTIONS////////////////////////////////////////////

double Polish(Program *p, Stack *s, Var_array v);
void Op(Program *p);
double evaluate_expression(double a, double b, char *operand);
void Initialise_Stack(Stack *s);
double Push(Stack *s, double value);
double Pop(Stack *s, Var_array v, Program *p);

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{   
    FILE *fp;
    Program p;
    Var_array var_array;
    char *prog_name = argv[1];
    
    initialise_program(&p, prog_name);
    
    if(argc > 2){
        if(string_same(argv[2], "testing")){
            run_tests();
        }
    }

    initialise_variable_array(var_array);

    initialise_words_array(&p);

    fp = open_file(prog_name, "r");
    if(fp == NULL){
        exit(3);
    }

    scan_file(&p, fp);

    Launch(&p, var_array);

    printf ("Parsed OK\n");
    return 0;
}


/////////////////////////////////////TESTING FUNCTIONS//////////////////////////////////////////////

//tests functions listed below and outputs results to "test_results.txt" file
void run_tests(){
    FILE *test_fp = open_file("test_results.txt", "w");
    open_file_test(test_fp);
    scan_file_test(test_fp);
    set_default_start_test(test_fp);
    initialise_variable_array_test(test_fp);
    initialise_words_array_test(test_fp);
    string_same_test(test_fp);
    word_found_test(test_fp);
    Var_test(test_fp);
    Number_test(test_fp);
    Varnum_test(test_fp);
    set_loop_start_val_test(test_fp);
    set_loop_end_val_test(test_fp);
    execute_loop_test(test_fp);
    Do_test(test_fp);
}

void open_file_test(FILE *test_fp){
    FILE *fp = NULL;
    //false test
    fp = open_file("Gobbeldegook.king", "r");
    if(fp == NULL){
        //true test
        fp = open_file("turtle_test1.txt", "r");
        if (fp != NULL){
            fprintf(test_fp, "open_file_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "open_file_test failed\n");
}

void scan_file_test(FILE *test_fp){
    Program p;
    FILE *fp;
    p.current_word = 0;
    fp = open_file("scan_test_fileA.txt", "r");
    if(fp == NULL){
        fprintf(test_fp, "scan_file_test failed. Scan test file missing?\n");
    }
    scan_file(&p, fp);
    //tests whether line numbers have been correctly assigned
    if(p.line_id[p.current_word] == 1 
        && string_same(p.words[p.current_word], "I")){
        if(p.line_id[p.current_word + 1] == 2 
            && string_same(p.words[p.current_word + 1], "like")){
            if(p.line_id[p.current_word + 3] == 3 
                && string_same(p.words[p.current_word + 3], "and")){
                if(p.line_id[p.current_word + 6] == 4 
                    && string_same(p.words[p.current_word + 6], "much")){
                    fprintf(test_fp, "scan_file_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "scan_file_test failed\n");
}

void set_default_start_test(FILE *fp){
    Co_ordinates c;
    set_default_start(&c);
    if(c.current_angle != NORTH || c.x_start != BOARD_WIDTH/2 || c.y_start != BOARD_HEIGHT/2){
        fprintf(fp, "set_default_start_test failed\n");
    }
    fprintf(fp, "set_default_start_test passed\n");
}

void initialise_variable_array_test(FILE *fp){
    Var_array v;
    initialise_variable_array(v);
    for(int i = 0; i < MAXNUMVARS; i++){
        if(v[i][0] != 0 || v[i][1] != 0){
            fprintf(fp, "initialise_variable_array_test failed\n");
        }
    }
    fprintf(fp, "initialise_variable_array_test passed\n");
}

void initialise_words_array_test(FILE *fp){
    Program p;
    initialise_words_array(&p);
    for(int i = 0; i < MAXNUMTOKENS; ++i){
        if(p.words[i][0] != '\0'){
            fprintf(fp, "initialise_words_array_test failed\n");
        }
    }
    fprintf(fp, "initialise_words_array_test passed\n");
}

void word_found_test(FILE *test_fp){
    Program p;
    p.current_word = 0;
    strcpy(p.words[p.current_word], "EGG");
    strcpy(p.words[p.current_word + 1], "3");
    //true test
    if(word_found("EGG", &p)){
        //false test
        if(!word_found("HAM", &p)){
        fprintf(test_fp, "word_found_test passed\n");
        return;
        }
    }
    fprintf(test_fp, "word_found_test failed\n");
}

void string_same_test(FILE *test_fp){
    char *str1 = "HAM";
    char *str2 = "EGGS";
    char *str3 = "HAM";
    //false test
    if(!string_same(str1, str2)){
        //true test
        if(string_same(str1, str3)){
            fprintf(test_fp, "string_same_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "string_same_test failed\n");
}

void Varnum_test(FILE *test_fp){
    Program p;
    p.current_word = 0;
    Var_array v;
    strcpy(p.words[p.current_word], "A");
    //False tests
    if(Varnum_for_test(&p, v) == 0){
        //True tests
        strcpy(p.words[p.current_word], "1");
        if(Varnum_for_test(&p, v) == 1){
            fprintf(test_fp, "Varnum_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "Varnum_test failed\n");
}

void Var_test(FILE *test_fp){
    Program p;
    Var_array v;
    p.current_word = 0;
    //true tests
    strcpy(p.words[p.current_word], "A");
    if(Var_for_test(&p, v) == 1){
        strcpy(p.words[p.current_word], "Z");
        if(Var_for_test(&p, v) == 1){
            //false tests
            strcpy(p.words[p.current_word], "thirty");
            if(Var_for_test(&p, v) == 0){
                strcpy(p.words[p.current_word], "c");
                if(Var_for_test(&p, v) == 0){
                    strcpy(p.words[p.current_word], "100000");
                    if(Var_for_test(&p, v) == 0){
                        strcpy(p.words[p.current_word], "-1");
                        if(Var_for_test(&p, v) == 0){
                            fprintf(test_fp, "Var_test passed\n");
                            return;
                        }
                    }
                }
            }
        }
    }
    fprintf(test_fp, "Var_test passed\n");
}

void Number_test(FILE *test_fp){
    Program p;
    Var_array v;
    p.current_word = 0;
    //true tests
    strcpy(p.words[p.current_word], "1");
    if(Number_for_test(&p, v) == 1){
        strcpy(p.words[p.current_word], "40");
        if(Number_for_test(&p, v) == 1){
            //false tests
            strcpy(p.words[p.current_word], "thirty");
            if(Number_for_test(&p, v) == 0){
                strcpy(p.words[p.current_word], "c");
                if(Number_for_test(&p, v) == 0){
                    strcpy(p.words[p.current_word], "1000001");
                    if(Number_for_test(&p, v) == 0){
                        strcpy(p.words[p.current_word], "-1");
                        if(Number_for_test(&p, v) == 0){
                            fprintf(test_fp, "Number_test passed\n");
                            return;
                        }
                    }
                }
            }
        }
    }
    fprintf(test_fp, "Number_test failed\n");
}

void set_loop_start_val_test(FILE *test_fp){
    Program p;
    Var_array v;
    p.current_word = 0;
    p.variable_key = 2;
    //True test
    strcpy(p.words[p.current_word], "1");
    set_loop_start_val(&p, v);
    if(p.current_word == 1){
        if(p.loop_start == 1){
            if(v[p.variable_key][0] == 1){
                fprintf(test_fp, "set_loop_start_val_test passed\n");
                return;
            }
        }
    }
    fprintf(test_fp, "set_loop_start_val_test failed\n");
}

void set_loop_end_val_test(FILE *test_fp){
    Program p;
    Var_array v;
    p.current_word = 0;
    p.variable_key = 2;
    assign_var(v, p.variable_key, 50);
    strcpy(p.words[p.current_word], "C");
    set_loop_end_val(&p, v);
    //True test with letter variable
    if(p.loop_end == 50){
        if(p.current_word == 1){
            //True test with number variable
            p.current_word = 0;
            strcpy(p.words[p.current_word], "50");
            set_loop_end_val(&p, v);
            if(p.loop_end == 50){
                if(p.current_word == 1){
                fprintf(test_fp, "set_loop_end_val_test passed\n");
                return;
                }
            }
        }
    }
    fprintf(test_fp, "set_loop_end_val_test failed\n");
}

void execute_loop_test(FILE *test_fp){
    Program p;
    Var_array v;
    p.current_word = 0;
    p.variable_key = 2;
    v[p.variable_key][0] = 0;
    p.loop_start = 1;
    p.loop_end = 50;
    p.loop_start_address = 0;
    strcpy(p.words[p.current_word], "}");
    // checks loop is executing correct number of times
    if(execute_loop_for_test(&p, v) == 50){
        if(v[p.variable_key][0] == 50){
            //False test
            p.current_word = 0;
            strcpy(p.words[p.current_word], "FD");
            if(execute_loop_for_test(&p, v) == 0){
                fprintf(test_fp, "execute_loop_test passed\n");
                return;                
            }
        }
    }
    fprintf(test_fp, "execute_loop_test failed\n");
}

void Do_test(FILE *test_fp){
    FILE *fp;
    Program p;
    Var_array v;
    p.current_word = 0;
    fp = open_file("Do_test1.txt", "r");
    initialise_words_array(&p);
    scan_file(&p, fp);
    //True test
    if(Do_for_test(&p, v) == 1){
        if(p.loop_start_address == 7){
            //False tests
            fp = open_file("Do_test2.txt", "r");
            initialise_words_array(&p);
            scan_file(&p, fp);
            if(Do_for_test(&p, v) == 0){
                fp = open_file("Do_test3.txt", "r");
                initialise_words_array(&p);
                scan_file(&p, fp);
                if(Do_for_test(&p, v) == 0){
                    fprintf(test_fp, "Do_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "Do_test failed\n");
}

/////////////////////////////////////////PROGRAM CODE///////////////////////////////////////////////

void initialise_program(Program *p, char *file_name){
    p->current_word = 0;
    p->file_name = file_name;
}

//sets turtle start position and direction to default facing north in centre of board
void set_default_start(Co_ordinates *c){
    c->current_angle = NORTH;
    c->x_start = BOARD_WIDTH/2;
    c->y_start = BOARD_HEIGHT/2;
}

//set all variables in array to zero
void initialise_variable_array(Var_array v){
    for (int i = 0; i < MAXNUMVARS; ++i){
        v[i][1] = unassigned;
        v[i][0] = unassigned;
    }
}

//insert null escape codes to allow fscanf to properly scan in turtle tokens
void initialise_words_array(Program *p){
    for(int i = 0; i < MAXNUMTOKENS; ++i){
        p->words[i][0] = '\0';
    }
}

FILE * open_file(char *file_name, char *read_write){
    FILE *pointer;
     if(!(pointer = fopen (file_name, read_write))){
        printf("couldn't open file, %s\n", file_name);
        return NULL;
    }
    return pointer;
}

//scans in turtle file to array of words and assigns each word a line id
void scan_file(Program *p, FILE *fp){
    p->current_word = 0;
    int line_number = 1;
    char c;
    while(fscanf (fp, "%s", p->words[p->current_word]) == 1 && p->current_word < MAXNUMTOKENS){
        p->line_id[p->current_word] = line_number;
        if (( c = fgetc(fp)) == '\n'){
            line_number++;
        }
        p->current_word++;
    }
    p->current_word = 0;
}

//returns a 1 if word found and moves program counter on. Otherwise returns 0
int word_found(char *word, Program *p){
    if (string_same(p->words[p->current_word], word)){
        p->current_word++;
        return 1;
    }
    else{
        return 0;
    }
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
    printf("\n%s It happened on line %d in %s\n", error_message, p->line_id[p->current_word], p->file_name);
    printf("It happened on line %d in %s\n", p->line_id[p->current_word], p->file_name);
    printf("The current word is %s\n", p->words[p->current_word]);
    printf("previous word = %s\n", p->words[p->current_word - 1]);
    SDL_Quit();
    exit(2);
}

void Launch(Program *p, Var_array v){
    Co_ordinates co_ordinates;
    set_default_start(&co_ordinates);
    window w;
    setup_SDL(&w, BOARD_HEIGHT, BOARD_WIDTH);
    if(! word_found("{", p)){
    error("Missing start brace?", p);
    }
    Instruction_list(p, v, &co_ordinates, &w);
    SDL_Delay(5000);
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
    
    if(movement_instruction(p, v, c, w)){
        return;
    }
    if(word_found ("DO", p)){
        Do(p, v, c, w);
        return;
    }
    if(word_found ("SET", p)){
        Set(p, v);
        return;
    }
    if(word_found ("SET_DRAW", p)){
        Set_draw(p, v, c);
        return;
    }
    if(word_found ("IF", p)){
        If(p, v, c, w);
        return;
    }
    error("Invalid INSTRUCTION?", p);
}

int movement_instruction(Program *p, Var_array v, Co_ordinates *c, window *w){
    double distance, turn_angle;
    
    if(word_found("FD", p)){
        Varnum(p, v);
        distance = interpret_varnum(p, v);
        Fd(c->current_angle, distance, p, c, w);
        return 1;
    }    
    if(word_found ("LT", p)){
        Varnum(p, v);
        turn_angle = interpret_varnum(p, v);
        Lt(c, turn_angle, p);
        return 1;
    }
    if(word_found ("RT", p)){
        Varnum(p, v);
        turn_angle = interpret_varnum(p, v);
        Rt(c, turn_angle, p);
        return 1;
    }
    return 0;
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

//checks whether varnums are diverted to correct function
//returns ints instead of function calls/errors, for testing purposes
int Varnum_for_test(Program *p, Var_array v){
    double varnum = 0;
    //if Varnum is an integer
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1){
        return 1;
    }
    else{
        return 0;
    }
}

//Returns ints for testing instead of error and program exit
int Number_for_test(Program *p, Var_array v){
    double varnum = 0;
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1 && varnum > 0 && varnum < MAXVARNUM){
        return 1;
    }
    Var_for_test(p, v);
    return 0;
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
void Var(Program *p, Var_array v){
    char var = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]c", &var) == 1 && strlen(p->words[p->current_word]) == 1){
        return;
    }
    error("Invalid VAR?", p);
    exit(4);
}

//Returns ints for testing instead of error and program exit
int Var_for_test(Program *p, Var_array v){
    char var = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]c", &var) == 1 && strlen(p->words[p->current_word]) == 1){
        return 1;
    }
    return 0;
}

int var_key(char *var){
    int key = *var - CAPITAL_CONVERSION;
    return key;
}

void assign_var(Var_array v, int key, double val){
    v[key][0] = val;
    v[key][1] = assigned;
}

//evaluates a varnum to see if it's a number or a variable. Returns the value of the varnum as a double
double interpret_varnum(Program *p, Var_array v){
    int key;
    char var;
    double num, val = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]s", &var) == 1){
        key = var_key(&var);
        val = v[key][0];
        if(v[key][1] == unassigned){
            error("unassigned variable?", p);
        }
    }
    else if(sscanf(p->words[p->current_word], "%lf", &num) == 1){
        val = num;       
    }
    return val;
}

void Do(Program *p, Var_array v, Co_ordinates *c, window *w){
    Var(p, v);
    p->variable_key = var_key(p->words[p->current_word]);
    p->current_word++;
    if(word_found ("FROM", p)){
        set_loop_start_val(p, v);
        if(word_found ("TO", p)){
            set_loop_end_val(p, v);
            if(word_found ("{", p)){
                p->loop_start_address = p->current_word;
                execute_loop(p, v, c, w);
                return;
            }
        }
    }
    error("Incorrect format for DO command?", p);
}

//Do function with function calls stripped out. For testing that function flows correctly
//and doesn't allow invalid syntax
int Do_for_test(Program *p, Var_array v){
    p->current_word++;
    if(word_found ("FROM", p)){
        p->current_word++;
        if(word_found ("TO", p)){
            p->current_word++;
            if(word_found ("{", p)){
                p->current_word++;
                p->loop_start_address = p->current_word;
                return 1;
            }
        }
    }
    return 0;
}

void set_loop_start_val(Program *p, Var_array v){
    Varnum(p, v);
    double value = p->loop_start = interpret_varnum(p, v);
    assign_var(v, p->variable_key, value);
    p->current_word++;
}

void set_loop_end_val(Program *p, Var_array v){
    Varnum(p, v);
    p->loop_end = interpret_varnum(p, v);
    p->current_word++;
}

void execute_loop(Program *p, Var_array v, Co_ordinates *c, window *w){
    double value = p->loop_start;
    for(int i = p->loop_start; i <= p->loop_end; i++){
        if(string_same(p->words[p->current_word], "}")){
            if(i == p->loop_end){
                p->current_word = p->loop_start_address;
                Instruction_list(p, v, c, w);
                return;
            }
            value++;
            assign_var(v, p->variable_key, value);
            p->current_word = p->loop_start_address;
        }
        Instruction_list(p, v, c, w);
    }
    error("Invalid DO loop syntax?", p);
}

//returns ints for testing instead of function calls or errors
int execute_loop_for_test(Program *p, Var_array v){
    double value = p->loop_start;
    for(int i = p->loop_start; i <= p->loop_end; i++){
        if(string_same(p->words[p->current_word], "}")){
            if(i == p->loop_end){
                p->current_word = p->loop_start_address;
                return value;
            }
            value++;
            assign_var(v, p->variable_key, value);
            p->current_word = p->loop_start_address;
        }
    }
    return 0;
}

void Set(Program *p, Var_array v){
    char *var = p->words[p->current_word];
    int key = var_key(var);
    double val;
    Stack s;
    Var(p, v);
    Initialise_Stack(&s);
    p->current_word++;
    if(word_found (":=", p)){
        val = Polish(p, &s, v);
        assign_var(v, key, val);
        return;
    }
    error("Invalid SET syntax?", p);
}

void Set_draw(Program *p, Var_array v, Co_ordinates *c){
    Varnum(p, v);
    c->x_start = (int) interpret_varnum(p, v);
    p->current_word++;
    Varnum(p, v);
    c->x_start = (int) interpret_varnum(p, v);
    return;
}

void If(Program *p, Var_array v, Co_ordinates *c, window *w){
    if(word_found ("(", p)){
        set_if_statement(p, v);
        if(word_found(")", p)){
            evaluate_if_statement(p, v, c, w);
            return;
        }
    }
    error("Invalid IF syntax?", p);
}

void set_if_statement(Program *p, Var_array v){
    Varnum(p, v);
    p->a = interpret_varnum(p, v);
    p->current_word++;
    p->comp = Comparator(p, v);
    p->current_word++;
    Varnum(p, v);
    p->b = interpret_varnum(p, v);
    p->current_word++;
}

void evaluate_if_statement(Program *p, Var_array v, Co_ordinates *c, window *w){
    if(evaluate_comparison(p, v) == 1){
        if(string_same(p->words[p->current_word], "{")){
            p->current_word++;
            Instruction_list(p, v, c, w);
            return;
        }
    }
    else{
        while(!string_same(p->words[p->current_word], "}")){
            p->current_word++;
        }
    }
    return;
}

comparator Comparator(Program *p, Var_array v){
    if(string_same(p->words[p->current_word], "==")){
        return equals;
    }
    if(string_same(p->words[p->current_word], "<")){
        return less_than;
    }
    if(string_same(p->words[p->current_word], ">")){
        return greater_than;
    }
    error("Invalid COMPARATOR?", p);
    exit(3);
}

int evaluate_comparison(Program *p, Var_array v){
    if(p->comp == equals){
        if(p->a == p->b){
            return 1;
        }
    }
    if(p->comp == greater_than){
        if(p->a > p->b){
            return 1;
        }
    }
    if(p->comp == less_than){
        if(p->a < p->b){
            return 1;
        }
    }
    return 0;
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
    SDL_RenderDrawLine(w->renderer, (int)c->x_start, (int)c->y_start, 
        (int)c->x_finish, (int)c->y_finish);
    SDL_RenderPresent(w->renderer);
    SDL_UpdateWindowSurface(w->win);
    SDL_Delay(5);

    c->x_start = c->x_finish;
    c->y_start = c->y_finish;
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
