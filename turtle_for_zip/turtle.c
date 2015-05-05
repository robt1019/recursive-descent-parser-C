#include "turtle.h"

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
    printf("The current word is %s\n", p->words[p->current_word]);
    printf("previous word = %s\n", p->words[p->current_word - 1]);
    SDL_Quit();
    exit(2);
}

//This is the MAIN part of the turtle grammar
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
        // if(varnum >= 1 && varnum <= 10000){
            return;
        // }
    }
    Var(p);
    error("Invalid number? Must be between 1 and 10000", p);
}

//checks if Var is a letter from A to Z and if it's 1 character long. Returns var as a character
void Var(Program *p){
    char var[MAXNUMTOKENS];
    if(sscanf(p->words[p->current_word], "%[A-Z]c", var) == 1 && strlen(p->words[p->current_word]) == 1){
        return;
    }
    error("Invalid VAR?", p);
    exit(4);
}

//Returns ints for testing instead of error and program exit
int Var_for_test(Program *p){
    char var[MAXTOKENSIZE];
    if(sscanf(p->words[p->current_word], "%[A-Z]s", var) == 1 && strlen(p->words[p->current_word]) == 1){
        return 1;
    }
    return 0;
}

//Returns a key for accessing correct element in Var_array. 0 corresponds to A, 1 to B etc.
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
    double loop_start, loop_end, val;
    int loop_start_address, key;
    Var(p);
    key = var_key(p->words[p->current_word]);
    p->current_word++;
    if(word_found ("FROM", p)){
        Varnum(p);
        val = loop_start = interpret_varnum(p);
        assign_var(p, key, val);
        p->current_word++;
        if(word_found ("TO", p)){
            Varnum(p);
            loop_end = interpret_varnum(p) + 1;
            p->current_word++;
            if(word_found ("{", p)){
                loop_start_address = p->current_word;
                for (int i = loop_start; i <= loop_end; ++i){
                    if(string_same(p->words[p->current_word], "}")){
                        val++;
                        assign_var(p, key, val);
                        if(i == loop_end){
                            return;
                        }
                        p->current_word = loop_start_address;
                    }
                    Instruction_list(p, c, w);
                }
            }
        }
    }
    error("Incorrect format for DO command?", p);
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
    double a, b;
    Comparator comp;
    if(word_found ("(", p)){
        Varnum(p);
        a = interpret_varnum(p);
        p->current_word++;
        comp = interpret_comparator(p);
        p->current_word++;
        Varnum(p);
        b = interpret_varnum(p);
        p->current_word++;
        if(string_same(p->words[p->current_word], ")")){
            p->current_word++;
            if(evaluate_comparison(a, b, comp) == 1){
                if(string_same(p->words[p->current_word], "{")){
                    if(evaluate_comparison(a, b, comp) == 1){
                        p->current_word++;
                        Instruction_list(p, c, w);
                        return;                        
                    }
                }
            }
            while(!string_same(p->words[p->current_word], "}")){
                if(string_same(p->words[p->current_word], "")){
                    error("Invalid IF syntax?", p);
                }
                p->current_word++;
            }
            return;
        }
    }
    error("Invalid IF syntax?", p);
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

int evaluate_comparison(double a, double b, Comparator comp){
  if(comp == equals){
        if(a == b){
            return 1;
        }
    }
    if(comp == greater_than){
        if(a > b){
            return 1;
        }
    }
    if(comp == less_than){
        if(a < b){
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

//Set new turtle position and draw line from old position to new position to SDL window
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
        perform_operation(p, s);
        push_result_to_stack(p, s);
    }
    else{
        push_varnum_to_stack(p, s);
    }
    p->result = Polish(p, s);
    return p->result;
}

int operator_found(Program *p){
    if(string_same(p->words[p->current_word], "+")){
        strcpy(p->op, "+");
        return 1;
    }
    if(string_same(p->words[p->current_word], "-")){
        strcpy(p->op, "-");
        return 1;
    }
    if(string_same(p->words[p->current_word], "*")){
        strcpy(p->op, "*");
        return 1;
    }
    if(string_same(p->words[p->current_word], "/")){
        strcpy(p->op, "/");
        return 1;
    }
    return 0;
}

void perform_operation(Program *p, Stack *s){
    double a, b;
    b = Pop(s, p);
    a = Pop(s, p);
    p->result = evaluate_expression(a, b, p->op);
}

void push_result_to_stack(Program *p, Stack *s){
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
    s->top->If_status = True;
    s->top->prev = NULL;
}

double Push(Stack *s, double value){
    Element *e;
    e = (Element *) calloc(1, sizeof(Element));
    e->prev = s->top;
    s->top = e;
    s->top->value = value;
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
