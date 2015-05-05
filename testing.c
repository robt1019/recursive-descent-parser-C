#include "turtle.h"

/////////////////////////////////////////TEST PROTOTYPES////////////////////////////////////////////

void initialise_program_test(FILE *test_fp, Program *test_program);
void set_default_start_test(FILE *fp);
void initialise_variable_array_test(FILE *fp, Program *test_program);
void initialise_words_array_test(FILE *fp, Program *test_program);
void open_file_test(FILE *test_fp);
void scan_file_test(FILE *test_fp, Program *test_program);
void word_found_test(FILE *test_fp, Program *test_program);
void string_same_test(FILE *test_fp);

void Varnum_test(FILE *test_fp, Program *test_program);
void Var_test(FILE *test_fp, Program *test_program);
void Number_test(FILE *test_fp, Program *test_program);
void assign_var_test(FILE *test_fp, Program *test_program);
void interpret_varnum_test(FILE *test_fp, Program *test_program);

// void Do_test(FILE *test_fp, Program *test_program);

void Set_draw_test(FILE *test_fp, Program *test_program);

void set_if_statement_test(FILE *test_fp, Program *test_program);
void interpret_comparator_test(FILE *test_fp, Program *test_program);
void evaluate_comparison_test(FILE *test_fp, Program *test_program);

void Lt_test(FILE *test_fp);
void Rt_test(FILE *test_fp);

void operator_found_test(FILE *test_fp, Program *test_program);
void push_result_to_stack_test(FILE *test_fp, Program *test_program, Stack *test_stack);
void push_varnum_to_stack_test(FILE *test_fp, Program *test_program, Stack *test_stack);
void evaluate_expression_test(FILE *test_fp);
void Initialise_Stack_test(FILE *test_fp);
void Push_test(FILE *test_fp, Stack *test_stack);
void Pop_test(FILE *test_fp, Stack *test_stack, Program *test_program);

////////////////////////////////////////////////////////////////////////////////////////////////////


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
    // Do_test(test_fp, &test_program);
    Set_draw_test(test_fp, &test_program);
    // set_if_statement_test(test_fp, &test_program);
    // interpret_comparator_test(test_fp, &test_program);
    // evaluate_comparison_test(test_fp, &test_program);
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
    fprintf(test_fp, "\npush_result_to_stack_test checks that the correct value is pushed to'\n");
    fprintf(test_fp, "the stack when the result 62.6 is assigned to stack->top->value\n");
    fprintf(test_fp, "_________________________________________________________________________\n\n");
    test_program->current_word = 0;
    test_program->result = 62.6;
    strcpy(test_program->op, "*");
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