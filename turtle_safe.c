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

typedef struct prog{
    char words[MAXNUMTOKENS][MAXTOKENSIZE];
    double var_array[MAXNUMVARS][2];
    int current_word;
    int line_id[MAXNUMTOKENS];
    int variable_key;
    char *file_name;
    int loop_start;
    int loop_end;
    int loop_start_address;
    double a;
    double b;
    Comparator comp;
    double result;
    char *operator;

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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////PROTOTYPES//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////TESTS////////////////////////////////////////////////

void run_tests();
void initialise_program_test(FILE *test_fp, Program *test_program);
void set_default_start_test(FILE *fp);
void initialise_variable_array_test(FILE *fp, Program *test_program);
void initialise_words_array_test(FILE *fp, Program *test_program);
void open_file_test(FILE *test_fp);
void scan_file_test(FILE *test_fp, Program *test_program);
void word_found_test(FILE *test_fp, Program *test_program);
void string_same_test(FILE *test_fp);
// void error_test(FILE *test_fp, Program *test_program);

// void Launch_test(FILE *fp);

// void Instruction_list_test(FILE *test_fp);
// void Instruction_test(FILE *test_fp);
// void movement_instruction_test(FILE *test_fp);

void Varnum_test(FILE *test_fp, Program *test_program);
void Var_test(FILE *test_fp, Program *test_program);
void Number_test(FILE *test_fp, Program *test_program);
void assign_var_test(FILE *test_fp, Program *test_program);
void interpret_varnum_test(FILE *test_fp, Program *test_program);

void Do_test(FILE *test_fp, Program *test_program);
void set_loop_start_val_test(FILE *test_fp, Program *test_program);
void set_loop_end_val_test(FILE *test_fp, Program *test_program);
void execute_loop_test(FILE *test_fp, Program *test_program);

// void Set_test(FILE *test_fp, Program *test_program);
void Set_draw_test(FILE *test_fp, Program *test_program);

// void If_test(FILE *test_fp, Program *test_program);
void set_if_statement_test(FILE *test_fp, Program *test_program);
// void evaluate_if_statement_test(FILE *test_fp, Program *test_program);
void interpret_comparator_test(FILE *test_fp, Program *test_program);
void evaluate_comparison_test(FILE *test_fp, Program *test_program);

void Lt_test(FILE *test_fp);
void Rt_test(FILE *test_fp);

// void Polish_test(FILE *test_fp, Program *test_program);
void operator_found_test(FILE *test_fp, Program *test_program);
void push_result_to_stack_test(FILE *test_fp, Program *test_program, Stack *test_stack);
void push_varnum_to_stack_test(FILE *test_fp, Program *test_program, Stack *test_stack);
void evaluate_expression_test(FILE *test_fp);
void Initialise_Stack_test(FILE *test_fp);
void Push_test(FILE *test_fp, Stack *test_stack);
void Pop_test(FILE *test_fp, Stack *test_stack, Program *test_program);

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
void set_if_statement(Program *p);
void evaluate_if_statement(Program *p, Co_ordinates *c, window *w);
Comparator interpret_comparator(Program *p);
int evaluate_comparison(Program *p);

/////////////////////////////////////MOVEMENT FUNCTIONS/////////////////////////////////////////////

void Lt(Co_ordinates *c, double turn_angle);
void Rt(Co_ordinates *c, double turn_angle);
void Fd(double current_angle, double distance, Co_ordinates *c, window *w);

////////////////////////////////////////POLISH FUNCTIONS////////////////////////////////////////////

double Polish(Program *p, Stack *s);
int operator_found(Program *p);
void push_result_to_stack(Program *p, Stack *s);
void push_varnum_to_stack(Program *p, Stack *s);
double evaluate_expression(double a, double b, char *operator);
void Initialise_Stack(Stack *s);
double Push(Stack *s, double value);
double Pop(Stack *s, Program *p);

////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{   
    FILE *fp;
    Program p;
    char *prog_name = argv[1];
    initialise_program(&p, prog_name);

    if(argc > 2){
        if(string_same(argv[2], "testing")){
            run_tests();
        }
    }

    initialise_variable_array(&p);
    initialise_words_array(&p);

    fp = open_file(prog_name, "r");
    if(fp == NULL){
        exit(3);
    }

    scan_file(&p, fp);
    fclose(fp);
    Launch(&p);

    printf ("Parsed OK\n");
    return 0;
}

/////////////////////////////////////TESTING FUNCTIONS//////////////////////////////////////////////

//tests functions listed below and outputs results to "test_results.txt" file
void run_tests(){
    FILE *test_fp = open_file("test_results.txt", "w");
    Program test_program;
    Stack test_stack;
    Initialise_Stack(&test_stack);

    initialise_program_test(test_fp, &test_program);
    set_default_start_test(test_fp);
    initialise_variable_array_test(test_fp, &test_program);
    initialise_words_array_test(test_fp, &test_program);
    open_file_test(test_fp);
    scan_file_test(test_fp, &test_program);    
    string_same_test(test_fp);
    word_found_test(test_fp, &test_program);
    Var_test(test_fp, &test_program);
    Number_test(test_fp, &test_program);
    Varnum_test(test_fp, &test_program);
    assign_var_test(test_fp, &test_program);
    interpret_varnum_test(test_fp, &test_program);
    set_loop_start_val_test(test_fp, &test_program);
    set_loop_end_val_test(test_fp, &test_program);
    execute_loop_test(test_fp, &test_program);
    Do_test(test_fp, &test_program);
    Set_draw_test(test_fp, &test_program);
    set_if_statement_test(test_fp, &test_program);
    interpret_comparator_test(test_fp, &test_program);
    evaluate_comparison_test(test_fp, &test_program);
    Lt_test(test_fp);
    Rt_test(test_fp);
    operator_found_test(test_fp, &test_program);
    push_varnum_to_stack_test(test_fp, &test_program, &test_stack);
    push_result_to_stack_test(test_fp, &test_program, &test_stack);
    evaluate_expression_test(test_fp);
    Initialise_Stack_test(test_fp);
    Push_test(test_fp, &test_stack);
    Pop_test(test_fp, &test_stack, &test_program);

    fclose(test_fp);
}

void initialise_program_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\ninitialise_program_test checks that initialise_program function\n");
    fprintf(test_fp, "sets current_word counter to zero and sends filename to the filename\n");
    fprintf(test_fp, "variable in the program struct for error reporting\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    char *dummy_file = "made_up.txt";
    initialise_program(test_program, dummy_file);
    //True tests
    if(string_same(test_program->file_name, dummy_file)){
        if(test_program->current_word == 0){
            fprintf(test_fp, "initialise_program_test passed\n");
            test_program->file_name = NULL;
            return;
        }
    }
    fprintf(test_fp, "initialise_program_test failed\n");
}

void set_default_start_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nset_default_start_test checks that turtle direction is set to NORTH\n");
    fprintf(test_fp, "and that turtle start point is set to the centre of the SDL window\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    Co_ordinates c;
    set_default_start(&c);
    if(c.current_angle != NORTH || c.x_start != BOARD_WIDTH/2 || c.y_start != BOARD_HEIGHT/2){
        fprintf(test_fp, "set_default_start_test failed\n");
    }
    fprintf(test_fp, "set_default_start_test passed\n");
}

void initialise_variable_array_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\ninitialise_variable_array_test checks that all elements in 2D var_array \n");
    fprintf(test_fp, "are correctly set to zero\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");    
    initialise_variable_array(test_program);
    for(int i = 0; i < MAXNUMVARS; i++){
        if(test_program->var_array[i][0] != 0 || test_program->var_array[i][1] != 0){
            fprintf(test_fp, "initialise_variable_array_test failed\n");
        }
    }
    fprintf(test_fp, "initialise_variable_array_test passed\n");
}

void initialise_words_array_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\ninitialise_words_array_test checks that the first element of each \n");
    fprintf(test_fp, "word in the 2D words array is set to the terminal character \n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    initialise_words_array(test_program);
    for(int i = 0; i < MAXNUMTOKENS; ++i){
        if(test_program->words[i][0] != '\0'){
            fprintf(test_fp, "initialise_words_array_test failed\n");
        }
    }
    fprintf(test_fp, "initialise_words_array_test passed\n");
}

void open_file_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nopen_file_test checks that open_file function returns NULL when trying \n");
    fprintf(test_fp, "to open a non-existent file for reading and a file pointer for a valid file \n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    FILE *fp = NULL;
    //false test
    fp = open_file("made_up.txt", "r");
    if(fp == NULL){
        //true test
        fp = open_file("turtle_test1.txt", "r");
        if (fp != NULL){
            fprintf(test_fp, "open_file_test passed\n");
            fclose(fp);
            return;
        }
    }
    fclose(fp);
    fprintf(test_fp, "open_file_test failed\n");
}

void scan_file_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nscan_file_test checks that line_ids are correctly assigned to words\n");
    fprintf(test_fp, "in a file and that all words in test file are scanned into the words array\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");    
    FILE *fp;
    test_program->current_word = 0;
    fp = open_file("scan_test_fileA.txt", "r");
    if(fp == NULL){
        fprintf(test_fp, "scan_file_test failed. Scan test file missing?\n");
    }
    scan_file(test_program, fp);
    fclose(fp);
    //tests whether line numbers have been correctly assigned and words scanned correctly
    if(test_program->line_id[test_program->current_word] == 1 
        && string_same(test_program->words[test_program->current_word], "I")){
        if(test_program->line_id[test_program->current_word + 1] == 2 
            && string_same(test_program->words[test_program->current_word + 1], "like")){
            if(test_program->line_id[test_program->current_word + 3] == 3 
                && string_same(test_program->words[test_program->current_word + 3], "and")){
                if(test_program->line_id[test_program->current_word + 6] == 4 
                    && string_same(test_program->words[test_program->current_word + 6], "much")){
                    fprintf(test_fp, "scan_file_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "scan_file_test failed\n");
}

void word_found_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nword_found-test checks that word_found function correctly identifies\n");
    fprintf(test_fp, "specific words and moves the program counter on the correct amount\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");    
    test_program->current_word = 0;
    strcpy(test_program->words[test_program->current_word], "EGG");
    strcpy(test_program->words[test_program->current_word + 1], "3");
    //true test
    if(word_found("EGG", test_program)){
        //false test
        if(!word_found("HAM", test_program)){
        fprintf(test_fp, "word_found_test passed\n");
        return;
        }
    }
    fprintf(test_fp, "word_found_test failed\n");
}

void string_same_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nstring_same_test checks that string_same function correctly identifies \n");
    fprintf(test_fp, "specific words\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
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

void Varnum_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nVarnum_test checks that a testing version of the Varnum function\n");
    fprintf(test_fp, "follows the correct route for specific inputs\n");
    fprintf(test_fp, "I.E. that numbers are sent to the Number function\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    strcpy(test_program->words[test_program->current_word], "A");
    //False tests
    if(Varnum_for_test(test_program) == 0){
        //True tests
        strcpy(test_program->words[test_program->current_word], "1");
        if(Varnum_for_test(test_program) == 1){
            fprintf(test_fp, "Varnum_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "Varnum_test failed\n");
}

void Var_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nVar_test checks that a testing version of the Var function\n");
    fprintf(test_fp, "only returns true for valid vars (A-Z)\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    //true tests
    strcpy(test_program->words[test_program->current_word], "A");
    if(Var_for_test(test_program) == 1){
        strcpy(test_program->words[test_program->current_word], "Z");
        if(Var_for_test(test_program) == 1){
            //false tests
            strcpy(test_program->words[test_program->current_word], "thirty");
            if(Var_for_test(test_program) == 0){
                strcpy(test_program->words[test_program->current_word], "c");
                if(Var_for_test(test_program) == 0){
                    strcpy(test_program->words[test_program->current_word], "100000");
                    if(Var_for_test(test_program) == 0){
                        strcpy(test_program->words[test_program->current_word], "-1");
                        if(Var_for_test(test_program) == 0){
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

void Number_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nNumber_test checks that a testing version of the Number function\n");
    fprintf(test_fp, "only returns true for valid numbers (1 to 10000)\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    //true tests
    strcpy(test_program->words[test_program->current_word], "1");
    if(Number_for_test(test_program) == 1){
        strcpy(test_program->words[test_program->current_word], "40");
        if(Number_for_test(test_program) == 1){
            //false tests
            strcpy(test_program->words[test_program->current_word], "thirty");
            if(Number_for_test(test_program) == 0){
                strcpy(test_program->words[test_program->current_word], "c");
                if(Number_for_test(test_program) == 0){
                    strcpy(test_program->words[test_program->current_word], "1000001");
                    if(Number_for_test(test_program) == 0){
                        strcpy(test_program->words[test_program->current_word], "-1");
                        if(Number_for_test(test_program) == 0){
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

void assign_var_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nassign_var_test checks that a variable 'D'\n");
    fprintf(test_fp, "is correctly assigned the value 20 in the var_array\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    strcpy(test_program->words[test_program->current_word], "D");
    assign_var(test_program, 3, 20);
    Varnum(test_program);
    if(interpret_varnum(test_program) == 20){
        fprintf(test_fp, "assign_var_test passed\n");
        return;
    }
    fprintf(test_fp, "assign_var_test failed\n");
}

void interpret_varnum_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\ninterpret_varnum_test checks that a variable D previously set to 20\n");
    fprintf(test_fp, "is correctly interpreted by interpret_varnum and the value 20 is returned\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    strcpy(test_program->words[test_program->current_word], "D");
    assign_var(test_program, 3, 20);
    Varnum(test_program);
    if(interpret_varnum(test_program) == 20){
        fprintf(test_fp, "interpret_varnum_test passed\n");
        return;
    }
    fprintf(test_fp, "interpret_varnum_test failed\n");        
}

void set_loop_start_val_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nset_loop_start_val_test checks that loop start variable is correctly\n");
    fprintf(test_fp, "set in Program struct based on current_word and that the specified variable\n");
    fprintf(test_fp, "is set to the correct value in Var_array\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    test_program->variable_key = 2;
    //True test
    strcpy(test_program->words[test_program->current_word], "1");
    set_loop_start_val(test_program);
    if(test_program->current_word == 1){
        if(test_program->loop_start == 1){
            if(test_program->var_array[test_program->variable_key][0] == 1){
                fprintf(test_fp, "set_loop_start_val_test passed\n");
                return;
            }
        }
    }
    fprintf(test_fp, "set_loop_start_val_test failed\n");
}

void set_loop_end_val_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nset_loop_end_val_test checks that loop end variable is correctly\n");
    fprintf(test_fp, "set in Program struct when both variables and numbers are input\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    test_program->variable_key = 2;
    assign_var(test_program, test_program->variable_key, 50);
    strcpy(test_program->words[test_program->current_word], "C");
    set_loop_end_val(test_program);
    //True test with letter variable
    if(test_program->loop_end == 50){
        if(test_program->current_word == 1){
            //True test with number variable
            test_program->current_word = 0;
            strcpy(test_program->words[test_program->current_word], "50");
            set_loop_end_val(test_program);
            if(test_program->loop_end == 50){
                if(test_program->current_word == 1){
                fprintf(test_fp, "set_loop_end_val_test passed\n");
                return;
                }
            }
        }
    }
    fprintf(test_fp, "set_loop_end_val_test failed\n");
}

void execute_loop_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nexecute_loop_test checks that DO loop executes correct number of times\n");
    fprintf(test_fp, "and that final value for the specified variable is correct\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    test_program->variable_key = 2;
    test_program->var_array[test_program->variable_key][0] = 0;
    test_program->loop_start = 1;
    test_program->loop_end = 50;
    test_program->loop_start_address = 0;
    strcpy(test_program->words[test_program->current_word], "}");
    // checks loop is executing correct number of times
    if(execute_loop_for_test(test_program) == 50){
        if(test_program->var_array[test_program->variable_key][0] == 50){
            //False test
            test_program->current_word = 0;
            strcpy(test_program->words[test_program->current_word], "FD");
            if(execute_loop_for_test(test_program) == 0){
                fprintf(test_fp, "execute_loop_test passed\n");
                return;                
            }
        }
    }
    fprintf(test_fp, "execute_loop_test failed\n");
}

void Do_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nDo test checks a testing version of the Do function which returns ints\n");
    fprintf(test_fp, "to check that Do function correctly routes to follow on functions\n");
    fprintf(test_fp, "and only accepts valid grammar\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    FILE *fp;
    test_program->current_word = 0;
    fp = open_file("Do_test1.txt", "r");
    initialise_words_array(test_program);
    scan_file(test_program, fp);
    //True test
    if(Do_for_test(test_program) == 1){
        if(test_program->loop_start_address == 7){
            //False tests
            fp = open_file("Do_test2.txt", "r");
            initialise_words_array(test_program);
            scan_file(test_program, fp);
            fclose(fp);
            if(Do_for_test(test_program) == 0){
                fp = open_file("Do_test3.txt", "r");
                initialise_words_array(test_program);
                scan_file(test_program, fp);
                if(Do_for_test(test_program) == 0){
                    fprintf(test_fp, "Do_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "Do_test failed\n");
}


//Checks that co-ordinates properly set by Set_draw function and rounded correctly
void Set_draw_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nSet_draw_test checks that Set_draw correctly updates the turtle\n");
    fprintf(test_fp, "draw point in the Co_ordinates struct, including rounding floating\n");
    fprintf(test_fp, "correctly\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    Co_ordinates c;
    test_program->current_word = 0;
    strcpy(test_program->words[test_program->current_word], "30.2");
    strcpy(test_program->words[test_program->current_word + 1], "300.7");
    Set_draw(test_program, &c);
    if(c.x_start == 30){
        if(c.y_start == 301){
            fprintf(test_fp, "Set_draw_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "Set_draw_test failed\n");
}

void set_if_statement_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nset_if_statement_test checks that the statement 'A < 30.3'\n");
    fprintf(test_fp, "is properly updated in the program struct when A is equal to 20.6\n");
    fprintf(test_fp, "And that the program counter is correctly updated\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    assign_var(test_program, 0, 20.6);
    strcpy(test_program->words[test_program->current_word], "A");
    strcpy(test_program->words[test_program->current_word + 1], "<");
    strcpy(test_program->words[test_program->current_word + 2], "30.3");
    set_if_statement(test_program);
    if(test_program->a == 20.6){
        if(test_program->comp == less_than){
            if(test_program->b == 30.3){
                if(test_program->current_word == 3){
                    fprintf(test_fp, "set_if_statement_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "set_if_statement_test failed\n");
}

void interpret_comparator_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\ninterpret_comparator_test checks that the correct enumerated Comparator\n");
    fprintf(test_fp, "values are returned when fed the strings '==', '<' and '>'\n");
    fprintf(test_fp, "(That it returns equals, less_than and greater_than respectively)\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    strcpy(test_program->words[test_program->current_word], "==");
    if(interpret_comparator(test_program) == equals){
        strcpy(test_program->words[test_program->current_word], "<");
        if(interpret_comparator(test_program) == less_than){
            strcpy(test_program->words[test_program->current_word], ">");
            if(interpret_comparator(test_program) == greater_than){
                fprintf(test_fp, "interpret_comparator_test passed\n");
                return;
            }
        }
    }
    fprintf(test_fp, "interpret_comparator_test failed\n");
}

void evaluate_comparison_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\ninterpret_comparator_test checks that the statement '30 comparator 30.1'\n");
    fprintf(test_fp, "returns 1 when the comparator is 'less_than', 0 when comparator is\n");
    fprintf(test_fp, "'greater_than', and 0 when the comparator is 'equals'\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    test_program->a = 30;
    test_program->b = 30.1;
    test_program->comp = less_than;
    if(evaluate_comparison(test_program) == 1){
        test_program->comp = greater_than;
        if(evaluate_comparison(test_program) == 0){
            test_program->comp = equals;
            if(evaluate_comparison(test_program) == 0){
                fprintf(test_fp, "evaluate_comparison_test passed\n");
                return;
            }
        }
    }
    fprintf(test_fp, "evaluate_comparison_test failed\n");
}

void Lt_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nLt_test checks that the current angle in the Co_ordinates struct\n");
    fprintf(test_fp, "is updated correctly after Rt is called with a number of values\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    double turn_angle;
    Co_ordinates c;
    c.current_angle = 90;
    turn_angle = 90;
    Lt(&c, turn_angle);
    if(c.current_angle == 180){
        turn_angle = 360;
        Lt(&c, turn_angle);
        if(c.current_angle == 180){
            turn_angle = 180;
            Lt(&c, turn_angle);
            if(c.current_angle == 0){
                fprintf(test_fp, "Lt_test passed\n");
                return;
            }
        }
    }
    fprintf(test_fp, "Lt_test failed\n");
}

void Rt_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nRt_test checks that the current angle in the Co_ordinates struct\n");
    fprintf(test_fp, "is updated correctly after Rt is called with a number of values\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    double turn_angle;
    Co_ordinates c;
    c.current_angle = 0;
    turn_angle = 90;
    Rt(&c, turn_angle);
    if(c.current_angle == 270){
        turn_angle = 360;
        Rt(&c, turn_angle);
        if(c.current_angle == 270){
            turn_angle = 180;
            Rt(&c, turn_angle);
            if(c.current_angle == 90){
                fprintf(test_fp, "Rt_test passed\n");
                return;
            }
        }
    }
    fprintf(test_fp, "Lt_test failed\n");
}

void operator_found_test(FILE *test_fp, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\noperator_found_test checks that only operators '+' '-' '*' and '/'\n");
    fprintf(test_fp, "are recognised, returning 1. The values '3' and '&' are input\n");
    fprintf(test_fp, "as invalid tests, returning 0\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    //True tests
    strcpy(test_program->words[test_program->current_word], "+");
    if(operator_found(test_program)){
        strcpy(test_program->words[test_program->current_word], "-");
        if(operator_found(test_program)){
            strcpy(test_program->words[test_program->current_word], "*");
            if(operator_found(test_program)){
                strcpy(test_program->words[test_program->current_word], "/");
                if(operator_found(test_program)){
                    //False tests
                    strcpy(test_program->words[test_program->current_word], "3");
                    if(!operator_found(test_program)){
                        strcpy(test_program->words[test_program->current_word], "&");
                        if(!operator_found(test_program)){
                            fprintf(test_fp, "operator_found_test passed\n");
                            return;
                        }
                    }
                }
            }
        }    
    }
    fprintf(test_fp, "operator_found_test failed\n");
}

void push_result_to_stack_test(FILE *test_fp, Program *test_program, Stack *test_stack){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\npush_result_to_stack_test checks that when the expression '31.3 2 *'\n");
    fprintf(test_fp, "is pushed to the stack, the value 62.6 is assigned to stack->top.value\n");
    fprintf(test_fp, "and the result variable in the program struct is also assigned 62.6\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    Push(test_stack, 31.3);
    Push(test_stack, 2);
    test_program->operator = "*";
    push_result_to_stack(test_program, test_stack);
    if(test_program->result == 62.6){
        if(test_stack->top->value == 62.6){
            fprintf(test_fp, "push_result_to_stack_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "push_result_to_stack_test failed\n");
}

void push_varnum_to_stack_test(FILE *test_fp, Program *test_program, Stack *test_stack){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\npush_varnum_to_stack_test checks that the correct value is being\n");
    fprintf(test_fp, "assigned to the polish result variable in the program struct\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    assign_var(test_program, 3, 31.3);
    strcpy(test_program->words[test_program->current_word], "D");
    push_varnum_to_stack(test_program, test_stack);
    if(test_program->result == 31.3){
        if(test_stack->top->value == 31.3){
            fprintf(test_fp, "push_varnum_to_stack_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "push_varnum_to_stack_test failed\n");
}

void evaluate_expression_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nevaluate expression test inputs a series of values and operators for\n");
    fprintf(test_fp, "the expression 'a operator b' and ensures that the correct result is returned\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    double a, b;
    char *operator;
    a = 5;
    b = 2;
    operator = "*";
    if(evaluate_expression(a, b, operator) == 10){
        operator = "+";
        if(evaluate_expression(a, b, operator) == 7){
            a = 3;
            b = 5;
            operator = "-";
            if(evaluate_expression(a, b, operator) == -2){
                a = 100;
                b = 4;
                operator = "/";
                if(evaluate_expression(a, b, operator) == 25){
                    fprintf(test_fp, "evaluate_expression_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "evaluate_expression_test failed\n");
}

void Initialise_Stack_test(FILE *test_fp){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nInitialise_Stack_test\n");
    fprintf(test_fp, "\nchecks that Initialise_Stack function correctly sets prev pointer\n");
    fprintf(test_fp, "to NULL and top pointer is not NULL\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    Stack s;
    Initialise_Stack(&s);
    if(s.top->prev == NULL){
        if(s.top == NULL){
            fprintf(test_fp, "Initialise_Stack_test failed\n");
            return;
        }
    fprintf(test_fp, "Initialise_Stack_test passed\n");
    }
}

void Push_test(FILE *test_fp, Stack *test_stack){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nPush_test checks that new value is correctly assigned to top->value,\n");
    fprintf(test_fp, "function returns correct top->value and that prev pointer is not NULL\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    double return_value = Push(test_stack, 34);
    if(test_stack->top != NULL){
        if(test_stack->top->value == 34){
            if(test_stack->top->prev != NULL){
                if(return_value == 34){
                    fprintf(test_fp, "Push_test passed\n");
                    return;
                }
            }
        }
    }
    fprintf(test_fp, "Push_test failed\n");
}

void Pop_test(FILE *test_fp, Stack *test_stack, Program *test_program){
    fprintf(test_fp, "_________________________________________________________________________\n");
    fprintf(test_fp, "\nPop_test checks that top->value is properly updated and that\n");
    fprintf(test_fp, "function returns correct value for popped value\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    Push(test_stack, 34);
    Push(test_stack, 2);
    double return_value = Pop(test_stack, test_program);
    if(test_stack->top->value == 34){
        if(return_value == 2){
            fprintf(test_fp, "Pop_test passed\n");
            return;
        }
    }
    fprintf(test_fp, "Pop_test failed\n");
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
void initialise_variable_array(Program *p){
    for (int i = 0; i < MAXNUMVARS; ++i){
        p->var_array[i][1] = unassigned;
        p->var_array[i][0] = unassigned;
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

void Launch(Program *p){
    Co_ordinates co_ordinates;
    set_default_start(&co_ordinates);
    window w;
    setup_SDL(&w, BOARD_HEIGHT, BOARD_WIDTH);
    if(! word_found("{", p)){
    error("Missing start brace?", p);
    }
    Instruction_list(p, &co_ordinates, &w);
    SDL_Delay(5000);
}

void Instruction_list(Program *p, Co_ordinates *c, window *w){
    if (string_same(p->words[p->current_word], "}")){
        return;
    }
    Instruction(p, c, w);
    p->current_word++;
    Instruction_list(p, c, w);
}

void Instruction(Program *p, Co_ordinates *c, window *w){
    
    if(movement_instruction(p, c, w)){
        return;
    }
    if(word_found ("DO", p)){
        Do(p, c, w);
        return;
    }
    if(word_found ("SET", p)){
        Set(p);
        return;
    }
    if(word_found ("SET_DRAW", p)){
        Set_draw(p, c);
        return;
    }
    if(word_found ("IF", p)){
        If(p, c, w);
        return;
    }
    error("Invalid INSTRUCTION?", p);
}

int movement_instruction(Program *p, Co_ordinates *c, window *w){
    double distance, turn_angle;
    
    if(word_found("FD", p)){
        Varnum(p);
        distance = interpret_varnum(p);
        Fd(c->current_angle, distance, c, w);
        return 1;
    }    
    if(word_found ("LT", p)){
        Varnum(p);
        turn_angle = interpret_varnum(p);
        Lt(c, turn_angle);
        return 1;
    }
    if(word_found ("RT", p)){
        Varnum(p);
        turn_angle = interpret_varnum(p);
        Rt(c, turn_angle);
        return 1;
    }
    return 0;
}

void Varnum(Program *p){
    double varnum = 0;
    //if Varnum is an integer
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1){
        Number(p);
        return;
    }
    else{
        Var(p);
        return;
    }
}

//checks whether varnums are diverted to correct function
//returns ints instead of function calls/errors, for testing purposes
int Varnum_for_test(Program *p){
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
int Number_for_test(Program *p){
    double varnum = 0;
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1 && varnum > 0 && varnum < MAXVARNUM){
        if(varnum >= 1 && varnum <= 10000){
            return 1;
        }
    }
    Var_for_test(p);
    return 0;
}

void Number(Program *p){
    double varnum = 0;
    if(sscanf(p->words[p->current_word], "%lf", &varnum) == 1 && varnum > 0 && varnum < MAXVARNUM){
        if(varnum >= 1 && varnum <= 10000){
            return;
        }
    }
    Var(p);
    error("Invalid VARNUM? must be between 1 and 10000", p);
}

//checks if Var is a letter from A to Z and if it's 1 character long. Returns var as a character
void Var(Program *p){
    char var = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]c", &var) == 1 && strlen(p->words[p->current_word]) == 1){
        return;
    }
    error("Invalid VAR?", p);
    exit(4);
}

//Returns ints for testing instead of error and program exit
int Var_for_test(Program *p){
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

void assign_var(Program *p, int key, double val){
    p->var_array[key][0] = val;
    p->var_array[key][1] = assigned;
}

//evaluates a varnum to see if it's a number or a variable. Returns the value of the varnum as a double
double interpret_varnum(Program *p){
    int key;
    char var;
    double num, val = 0;
    if(sscanf(p->words[p->current_word], "%[A-Z]s", &var) == 1){
        key = var_key(&var);
        val = p->var_array[key][0];
        if(p->var_array[key][1] == unassigned){
            error("unassigned variable?", p);
        }
    }
    else if(sscanf(p->words[p->current_word], "%lf", &num) == 1){
        val = num;       
    }
    return val;
}

void Do(Program *p, Co_ordinates *c, window *w){
    Var(p);
    p->variable_key = var_key(p->words[p->current_word]);
    p->current_word++;
    if(word_found ("FROM", p)){
        set_loop_start_val(p);
        if(word_found ("TO", p)){
            set_loop_end_val(p);
            if(word_found ("{", p)){
                p->loop_start_address = p->current_word;
                execute_loop(p, c, w);
                return;
            }
        }
    }
    error("Incorrect format for DO command?", p);
}

//Do function with function calls stripped out. For testing that function flows correctly
//and doesn't allow invalid syntax
int Do_for_test(Program *p){
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

void set_loop_start_val(Program *p){
    Varnum(p);
    double value = p->loop_start = interpret_varnum(p);
    assign_var(p, p->variable_key, value);
    p->current_word++;
}

void set_loop_end_val(Program *p){
    Varnum(p);
    p->loop_end = interpret_varnum(p);
    p->current_word++;
}

void execute_loop(Program *p, Co_ordinates *c, window *w){
    double value = p->loop_start;
    for(int i = p->loop_start; i <= p->loop_end; i++){
        if(string_same(p->words[p->current_word], "}")){
            if(i == p->loop_end){
                p->current_word = p->loop_start_address;
                Instruction_list(p, c, w);
                return;
            }
            value++;
            assign_var(p, p->variable_key, value);
            p->current_word = p->loop_start_address;
        }
        Instruction_list(p, c, w);
    }
    error("Invalid DO loop syntax?", p);
}

//returns ints for testing instead of function calls or errors
int execute_loop_for_test(Program *p){
    double value = p->loop_start;
    for(int i = p->loop_start; i <= p->loop_end; i++){
        if(string_same(p->words[p->current_word], "}")){
            if(i == p->loop_end){
                p->current_word = p->loop_start_address;
                return value;
            }
            value++;
            assign_var(p, p->variable_key, value);
            p->current_word = p->loop_start_address;
        }
    }
    return 0;
}

void Set(Program *p){
    char *var = p->words[p->current_word];
    int key = var_key(var);
    double val;
    Stack s;
    Var(p);
    Initialise_Stack(&s);
    p->current_word++;
    if(word_found (":=", p)){
        val = Polish(p, &s);
        if(val < 1){
            error("Invalid SET syntax? Variable must be set between 1 and 10000", p);
        }
        assign_var(p, key, val);
        return;
    }
    error("Invalid SET syntax?", p);
}

void Set_draw(Program *p, Co_ordinates *c){
    Varnum(p);
    c->x_start = (int) (interpret_varnum(p) + ROUNDING_CONVERSION);
    p->current_word++;
    Varnum(p);
    c->y_start = (int) (interpret_varnum(p) + ROUNDING_CONVERSION);
    return;
}

void If(Program *p, Co_ordinates *c, window *w){
    if(word_found ("(", p)){
        set_if_statement(p);
        if(word_found(")", p)){
            evaluate_if_statement(p, c, w);
            return;
        }
    }
    error("Invalid IF syntax?", p);
}

void set_if_statement(Program *p){
    Varnum(p);
    p->a = interpret_varnum(p);
    p->current_word++;
    p->comp = interpret_comparator(p);
    p->current_word++;
    Varnum(p);
    p->b = interpret_varnum(p);
    p->current_word++;
}

void evaluate_if_statement(Program *p, Co_ordinates *c, window *w){
    if(evaluate_comparison(p) == 1){
        if(string_same(p->words[p->current_word], "{")){
            p->current_word++;
            Instruction_list(p, c, w);
            return;
        }
    }
    else{
        while(!string_same(p->words[p->current_word], "}")){
            if(string_same(p->words[p->current_word], "")){
                error("IF statement missing closing bracket?", p);
            }
            p->current_word++;
        }
    }
    return;
}

Comparator interpret_comparator(Program *p){
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

int evaluate_comparison(Program *p){
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

void Lt(Co_ordinates *c, double turn_angle){
    c->current_angle = c->current_angle + turn_angle;
    if(c->current_angle >= FULLTURN){
        c->current_angle = c->current_angle - FULLTURN;
    }
}

void Rt(Co_ordinates *c, double turn_angle){
    c->current_angle = c->current_angle - turn_angle;
    //if new angle is less than 0, wrap around and start at 360 degrees again
    if(c->current_angle < 0){
        c->current_angle = FULLTURN - - c->current_angle;
    }
}

void Fd(double current_angle, double distance, Co_ordinates *c, window *w){

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

double Polish(Program *p, Stack *s){

    if(string_same(p->words[p->current_word], ";")){
        return p->result;
    }
    if(operator_found(p)){
        push_result_to_stack(p, s);
    }
    else{
        push_varnum_to_stack(p, s);
    }
    p->result = Polish(p, s);
    return p->result;
}

double Polish_for_test(Program *p, Stack *s){

    if(string_same(p->words[p->current_word], ";")){
        return p->result;
    }
    if(operator_found(p)){
        push_result_to_stack(p, s);
    }
    else{
        push_varnum_to_stack(p, s);
    }
    p->result = Polish(p, s);
    return p->result;
}

int operator_found(Program *p){
    if(sscanf(p->words[p->current_word], "%[-,+,*,/]s", p->operator) == 1){
        return 1;
    }
    return 0;
}

void push_result_to_stack(Program *p, Stack *s){
    double a, b;
    b = Pop(s, p);
    a = Pop(s, p);
    p->result = evaluate_expression(a, b, p->operator);
    Push(s, p->result);
    p->current_word++;
}

void push_varnum_to_stack(Program *p, Stack *s){
    double value;
    Varnum(p);
    value = interpret_varnum(p);
    p->result = Push(s, value);
    p->current_word++;
}

double evaluate_expression(double a, double b, char *operator){
    double result = 0;
    if(string_same(operator, "+")){
        result = a + b;
    }
    if(string_same(operator, "-")){
        result = a - b;
    }
    if(string_same(operator, "*")){
        result = a * b;
    }
    if(string_same(operator, "/")){
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
    s->top = e;
    s->top->value = value;
    s->top = e;
    return value;
}

double Pop(Stack *s, Program *p){
    double value = s->top->value;
    Element *temp = s->top;
    s->top = s->top->prev;
    free(temp);
    if(s->top == NULL){
        error("Incorrect number of variables?", p);
    }
    return value;
}
