#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "display.h"

#define MAXNUMTOKENS 10000
#define MAXTOKENSIZE 100
#define MAXVARNUM 10000
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
typedef enum comparator Comparator;
enum bool {True = 1, False = 2};
typedef enum bool bool;

typedef struct element{
    double value;
    bool If_status;
    double a;
    double b;
    Comparator comp;
    struct element *prev;
} Element;

typedef struct stack{
    Element *top;
} Stack;

typedef struct prog{
    char words[MAXNUMTOKENS][MAXTOKENSIZE];
    double var_array[MAXNUMVARS][2];
    int current_word;
    int line_id[MAXNUMTOKENS];
    int variable_key;
    char *file_name;
    double result;
    char op[MAXTOKENSIZE];
} Program;

typedef struct co_ordinates{
    double current_angle;
    int x_start;
    int y_start;
    int x_finish;
    int y_finish;
} Co_ordinates;

typedef double Var_array[MAXNUMVARS][2];

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////PROTOTYPES//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////TESTING////////////////////////////////////////////////

void run_tests();

///////////////////////////////////////////FILE SET UP//////////////////////////////////////////////

void initialise_program(Program *p, char *file_name);
void set_default_start(Co_ordinates *c);
void initialise_variable_array(Program *p);
void initialise_words_array(Program *p);
FILE * open_file(char *file_name, char *read_write);
void scan_file(Program *p, FILE *fp);

////////////////////////////////////////GENERAL FUNCTIONS///////////////////////////////////////////

int word_found(char *word, Program *p);
int string_same(char *str1, char *str2);
void error(char *error_message, Program *p);

/////////////////////////////////////////TURTLE FUNCTIONS///////////////////////////////////////////

void Launch(Program *p);

//////////////////////////////////////INSTRUCTION FUNCTIONS/////////////////////////////////////////

void Instruction_list(Program *p, Co_ordinates *c, window *w);
void Instruction(Program *p, Co_ordinates *c, window *w);
int movement_instruction(Program *p, Co_ordinates *c, window *w);

/////////////////////////////////////////VARNUM FUNCITONS///////////////////////////////////////////

void Varnum(Program *p);
int Varnum_for_test(Program*p);
void Number(Program *p);
int Number_for_test(Program *p);
void Var(Program *p);
int Var_for_test(Program *p);
int var_key(char *var);
void assign_var(Program *p, int key, double val);
double interpret_varnum(Program *p);

/////////////////////////////////////////DO FUNCTIONS///////////////////////////////////////////////

void Do(Program *p, Co_ordinates *c, window *w);
int Do_for_test(Program *p);
void set_loop_start_val(Program *p);
void set_loop_end_val(Program *p);
void execute_loop(Program *p, Co_ordinates *c, window *w);
int execute_loop_for_test(Program *p);

////////////////////////////////////////SET FUNCTIONS///////////////////////////////////////////////

void Set(Program *p);
void Set_draw(Program *p, Co_ordinates *c);

/////////////////////////////////////////IF FUNCTIONS///////////////////////////////////////////////

void If(Program *p, Co_ordinates *c, window *w);
// void set_if_statement(Program *p);
// void evaluate_if_statement(Program *p, Co_ordinates *c, window *w);
Comparator interpret_comparator(Program *p);
int evaluate_comparison(double a, double b, Comparator comp);
// void Push_If_status(Stack *s, bool IF_status);
// bool Pop_If_status(Stack *s, Program *p);


/////////////////////////////////////MOVEMENT FUNCTIONS/////////////////////////////////////////////

void Lt(Co_ordinates *c, double turn_angle);
void Rt(Co_ordinates *c, double turn_angle);
void Fd(double current_angle, double distance, Co_ordinates *c, window *w);

////////////////////////////////////////POLISH FUNCTIONS////////////////////////////////////////////

double Polish(Program *p, Stack *s);
int operator_found(Program *p);
void perform_operation(Program *p, Stack *s);
void push_result_to_stack(Program *p, Stack *s);
void push_varnum_to_stack(Program *p, Stack *s);
double evaluate_expression(double a, double b, char *operator);
void Initialise_Stack(Stack *s);
double Push(Stack *s, double value);
double Pop(Stack *s, Program *p);

////////////////////////////////////////////////////////////////////////////////////////////////////