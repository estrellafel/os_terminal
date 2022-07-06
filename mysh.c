/*
 * Felix Estrella
 *
 * CS441/541: Project 3
 *
 */
#include "mysh.h"

int main(int argc, char * argv[]) {
    int ret;
    
    /*
     * Parse Command line arguments to check if this is an interactive or batch
     * mode run.
     */
    if( 0 != (ret = parse_args_main(argc, argv)) ) {
        fprintf(stderr, "Error: Invalid command line!\n");
        return -1;
    }

    return 0;
}

/* Determines whether it is in batch or interactive mode*/
int parse_args_main(int argc, char **argv)
{
    /* Initialize the job_hist array */
    job_hist = (job_t*) malloc(sizeof(job_t) * 1);
    if (job_hist == NULL) {
        return 0;
    }

    /*
     * If no command line arguments were passed then this is an interactive
     * mode or else run batch mode
     */
    if ( argc == 1 ) {
        interactive_mode();
    } else {
        batch_mode(argc, argv);
    }

    return 0;
}

/* Will run the shell from files */
int batch_mode(int argc, char **argv) {

    /*
     * For each file..
     */
    int i = 0;
    char * line = (char *) malloc(1024 *sizeof(char));
    size_t length = 1024;
    FILE * infile = NULL;

    /* Look through every file enetered*/
    for(; i < argc; i++) {

        /* If the file enetered had .txt open it*/
        if (strstr(argv[i],".txt")) {
           
            infile = fopen(argv[i],"r");
            if (infile == NULL) {
                printf("ERROR OPENING THE FILE\n");
                continue;
            }
            
            /* Look through each line of the file and send it down to be parsed and executed*/
            while (getline(&line, &length, infile) != -1) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                parse_command(strdup(line));
            }
            
        }
    }

    builtin_exit();

    return 0;
}

/* Will run the file from user input */
int interactive_mode(void)
{

    do {
        char * user_input = (char *) malloc(1024 *sizeof(char));
        size_t length = 1024;
        int check_contr_d = 0;

        /*
         * Print the prompt
         */
        printf("%s", PROMPT);
       
        /*
         * Read stdin, break out of loop if Ctrl-D
         */
        check_contr_d = getline(&user_input, &length, stdin);
        if (check_contr_d == -1) {
            printf("\n");
            builtin_exit();
        }
        /* Strip off the newline */
        strtok(user_input, "\n");
        
        /* Make sure a blank line is not ran through but ensure that corner case w will still work*/
        if (strlen(strdup(user_input)) == 1 && strcmp(strdup(user_input),"w") != 0) {
            continue;
        }

        /*
         * Parse and execute the command
         */
        parse_command(strdup(user_input));
    } while( 1/* end condition */);

    return 0;
}

/* Will parse the command so it is ready to be executed */
void parse_command(char *user_input) {
    int i = 0;
    int last = 0; /* Used to look at the last i place for the substring method to weed out & and ; */
    char * check_mult = (char *) malloc(1024 *sizeof(char));
    char * partial_string =  (char *) malloc(1024 *sizeof(char));
    char * holder = (char *) malloc(1024 *sizeof(char));
    char temp;
    int leftover = 0;
    int check_file_red = 0;

    /* Store the full user input in check_mult and partial_string */
    check_mult = strdup(user_input);
    partial_string = strdup(user_input);

    /* While check_mult has not reach the end*/
    while ( check_mult[i] != '\0' ) {

        /* Set the char being looked at to holder */
        temp = check_mult[i];
        holder = &temp;

        /* If a & is found parse the line for a substring*/
        if ( strcmp(holder, "&") == 0 ) {
            
            /* Parse the string and set last to new starting index */
            partial_string = substr(strdup(check_mult), last, i);
            last = i + 1;

            /* Check if the parsed string is not built in command otherwise it will get taken care of in isbuiltin */
            if (is_builtin(strdup(partial_string)) == 0) {
                
                /* Check if file redirection is used and call job set up with 1 for in the background */
                check_file_red = file_redirect(strdup(partial_string));
                job_set_up(strdup(partial_string), 1, check_file_red);
            }

        /* If a ; is found parse the line for a substring */
        } else if ( strcmp(holder, ";" ) == 0 ) {

            /* Parse the string and set last to new starting index */
            partial_string = substr(strdup(check_mult), last, i);
            last = i + 1;

            /* Check if the parsed string is not built in command otherwise it will get taken care of in isbuiltin */
            if (is_builtin(strdup(partial_string)) == 0) {

                /* Check if file redirection is used and call job set up with 0 for in the foreground */
                check_file_red = file_redirect(strdup(partial_string));
                job_set_up(strdup(partial_string), 0, check_file_red);
            }
        }
        i++;
    }

    /* Parse for the final substring and then check to see if anything is left to be executed */
    partial_string = substr(strdup(check_mult), last, i);
    leftover = get_Length(strdup(partial_string));

    /* If there is a string to still execute */
    if (leftover != 0) {
        /* Check if the parsed string is not built in command otherwise it will get taken care of in isbuiltin */
        if (is_builtin(strdup(partial_string)) == 0) {

            /* Check if file redirection is used and call job set up with 0 for in the foreground */
            check_file_red = file_redirect(strdup(partial_string));
            job_set_up(strdup(partial_string), 0, check_file_red);
        }
    }

}

/* Create a new job_t job and set the corresponding values and call launch_job */
void job_set_up(char *user_input, int background, int redirection) {
    job_t * job = (job_t*) malloc(sizeof(job_t));
    job->full_command = strdup(user_input);
    job->argc = get_Length(user_input);                                    
    job->binary = strtok(user_input, " ");
    job->is_background = background;
    job->red_num = redirection;
    set_agrv(job);                                                        
    launch_job(job);
    total_jobs++; 
}

/* Uses Strtok to count the number of elements in a line (ex. ls test, would return 2) */
int get_Length(char *str) {
    int count = 0;
    char * token = strtok(str, " ");
    while (token != NULL) {
        count++;
        token = strtok(NULL, " ");
    }
    return count;
}

/* Will set the argv for the job_t that is entered and place the Null terminator in last index */
void set_agrv(job_t * loc_job) {
    int i = 0;
    char * token = strtok(strdup(loc_job->full_command), " ");
    loc_job->argv = (char **) malloc(sizeof(char *) * (loc_job->argc + 1));
    while (token != NULL) {
        loc_job->argv[i] = strdup(token);
        token = strtok(NULL, " ");
        i++;
    }
    loc_job->argv[i] = NULL;
}

/* Will return a substring for a string */
char* substr(const char *src, int start, int end) {
    int len = end - start;
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
    int i = start;
    
    /* Loop through the string and set the correct characters to dest*/
    for (i = start; i < end && (*(src + i) != '\0'); i++) {
        *dest = *(src + i);
        dest++;
    }

    *dest = '\0';
    
    /* Subtract dest from len to return the correct string*/
    return dest - len;
}

/* Checks if file redirection is used. > returns 1, < returns 2, and if it is not there it returns 0 */
int file_redirect(char *input) {
    char *check;
    check = strstr(input, ">");
    if(check) {
        return 1;
    } 
    check = strstr(input, "<");
    if(check) {
        return 2;
    } 
    return 0;
}

/* Adds an element to the global job_t array job_hist and sets it's binary and background status */
void add_to_array(char *str, int app) {
    char *temp = elim_lead_space(strdup(str));
    job_hist = (job_t*) realloc(job_hist, sizeof(job_hist) * (current_total * sizeof(job_t)));    
    job_hist[current_total - 1].binary = strdup(temp);
    job_hist[current_total - 1].is_background = app;
}

/* Sets the is_builtin status of the current - 1 job_hist index to 1 for on */
void set_builtin() {
    job_hist[current_total - 1].is_builtin = 1;
}

/* Eliminates the white space in front of string entered */
char* elim_lead_space(char *str) {
    char *temp = (char *) malloc(1024 *sizeof(char));
    int last = 0;
    int i = strlen(str);
    
    /* Checks for the whitespace in front and keeps track of the index */
    while (str[last] == ' ') {
        last++;
    }

    /* Gets a substring without the whitespace */
    temp = substr(strdup(str), last, i);

    return temp;
}

/*
 * You will want one or more helper functions for parsing the commands 
 * and then call the following functions to execute them
 */
int launch_job(job_t * loc_job) {
    
    pid_t c_pid = 0;
    int status = 0;
    char *binary = NULL;
    int file_ret = 0;

    /* Add to the current total, get the binary, and add it to the job_hist array */ 
    current_total++;
    binary = strdup(loc_job->binary);
    add_to_array(strdup(loc_job->full_command), loc_job->is_background);

    /* Call a fork which returns a pid and save the pid into the job_hist array */
    c_pid = fork();
    job_hist[current_total - 1].my_pid = c_pid;
    
    /*
     * Launch the job in either the foreground or background
     * If the pid is less than 0 do nothing
     */
    if ( c_pid < 0 ) {
    /* If the pid is 0 execute the command */
    } else if ( c_pid == 0 ) {

        /* If red_num is 1 then it is a rediection from STDOUT */
        if (loc_job->red_num == 1){

            /* Open the file to write into it */
            file_ret = open(strdup(loc_job->argv[2]), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
            if (file_ret == -1) {
                printf("ERROR WITH THE FILE NAME");
                return 0;
            }

            /* Redirect from STDOUT, close the file, and execute the command */
            dup2(file_ret, STDOUT_FILENO);
            close(file_ret);
            execvp(binary, loc_job->argv);

        /* If red_num is 2 then it is a redirection from STDIN */    
        } else if (loc_job->red_num == 2) {

            /* Open the file to read */
            file_ret = open(strdup(loc_job->argv[2]), O_RDONLY, S_IRUSR | S_IWUSR);
            if (file_ret == -1) {
                printf("ERROR WITH OPENING THE FILE");
                return 0;
            }

            /* Redirect from STDIN, close the file, and execute the command */
            dup2(file_ret, STDIN_FILENO);
            close(file_ret);
            execvp(binary, loc_job->argv);
        } else {
            /* Execute the file and if it reaches below execvp then it is an invalid command */
            execvp(binary, loc_job->argv);
            printf("INVALID COMMAND\n");
            exit(-1);
        }
    } else {
        /* If the backgroun is 0 call and waitpid to run in the foreground */
        if (loc_job->is_background == 0) {
            waitpid(c_pid, &status, 0);
        }
    }

    return 0;
}

/* Will execute and return 1 if it is a builtin command, otherwise return 0 */
int is_builtin(char *user_input) {
    int num = 0;
    char * token = strtok(strdup(user_input), " ");
    int ret_val = 0;
    
    /* If it is jobs then it will call to execute jobs and add jobs to the job_hist array */
    if (strcmp(token, "jobs") == 0) {
        builtin_jobs(1);
        current_total++;
        add_to_array(strdup(token), 0);
        set_builtin();
        return 1;

    /* If it is history it will add to the job_hist array and execute histort*/
    } else if (strcmp(token, "history") == 0) {
        current_total++;
        add_to_array(strdup(token), 0);
        set_builtin();
        builtin_history();
        return 1;

    /* If it is fg it will check if an id is present, then execute the correct way and add it to the array */
    } else if (strcmp(token, "fg") == 0) {

        /* Will get the number that is followed if there is a number otherwise returns NULL */
        token = strtok(NULL, " ");

        /* If it is Null then it should execute fg that puts the last background command in the foreground */
        if (token == NULL) {

            /* If the number returned is not 0 then it did not move anything to the foreground*/
            ret_val = builtin_fg();
            if (ret_val != 0) {
                printf("NOTHING RUNNING IN BACKGROUND\n");
            }

            /* Add to the job_hist array and update current_total */
            current_total++;
            add_to_array(strdup("fg"), 0);
            set_builtin();

        /* Else there is a number (or something) after the fg which is a job id */
        } else {
            
            /* Convert the token to a number and if 0 the id does not exist*/
            num = strtol(token, NULL, 10);
            if (num == 0) {
                printf("ID DOES NOT EXIST\n");

            /* Else call the builtin_fg_num with the number to execute*/
            } else {

                /* If it does not return 0 nothing was moved to the foreground */
                ret_val = builtin_fg_num(num);
                if (ret_val != 0) {
                    printf("ID DOES NOT EXIST\n");
                }
            }

            /* Add to the job_hist array and update current_total */
            current_total++;
            add_to_array(strdup(user_input), 0);
            set_builtin();
        }
        return 1;

    /* If it is wait execute wait and then add to job_hist */
    } else if (strcmp(token, "wait") == 0) {
        builtin_wait();
        current_total++;
        add_to_array(strdup(token), 0);
        set_builtin();
        return 1;
        
    /* If it is exit the add to the job_hist array and execute exit */
    } else if (strcmp(token, "exit") == 0) {
        current_total++;
        add_to_array(strdup(token), 0);
        set_builtin();
        builtin_exit();
        return 1;

    /* It is not a built in command and return 0 */
    } else {
        return 0;
    }
}

/* Will print the exit info and exit the program */
int builtin_exit(void) {
    int run = 0;
    int back = 0;
    
    /* Gets the number of commands currently running and total that were in the background */
    back = num_background();
    run = num_running();

    /* If there are any running commands let the user know*/
    if (run > 0) {
        printf("SHELL IS WAITING ON %d JOBS\n", run);
    }
    
    /* Call wait to wait for all current commands running and num_jobs to get the total_jobs */
    builtin_wait();
    num_jobs();
    
    printf("-------------------------------\n");
    printf("Total number of jobs               = %d\n", total_jobs);
    printf("Total number of jobs in history    = %d\n", current_total);
    printf("Total number of jobs in background = %d\n", back);

    /* Free up the job_hist array and exit */
    free_up();
    exit(0);

    return 0;
}

/* Will execute the jobs command and set job_id's for use in builtin_fg_num */
int builtin_jobs(int print) {
    int i = 0;
    int status = 0;
    int count = 1;
    
    /* Look through all the jobs in job_hist */
    for (; i < current_total; i++) {

        /* If the job was put into the background */
        if (job_hist[i].is_background == 1) {

            /* If the job is not known to be done */
            if (job_hist[i].is_done != 1) {

                /* Call waitpid with the option WNHANG to either learn it has finshed or not (returns 0) */
                if (waitpid(job_hist[i].my_pid, &status, WNOHANG) == 0) {
                    
                    /* If print is on print and always set is_done to 0 and set to job_id */
                    if (print == 1) {
                        printf("[%d]   Running    %s\n", count, job_hist[i].binary);
                    }
                    job_hist[i].is_done = 0;
                    job_hist[i].job_id = count;
                } else {
                    /* If print is on print and always set is_done to 1 and set to job_id */
                    if (print == 1) {
                        printf("[%d]   Done       %s\n", count, job_hist[i].binary);
                    }
                    job_hist[i].is_done = 1;
                    job_hist[i].job_id = count;
                }
            }
            count++;
        }
    }
    return 0;
}

/* Will print out the history */
int builtin_history(void) {
    int i = 0;

    /* Iterate through job_hist and print everything out */
    for (; i < current_total; i++) {
        if (job_hist[i].is_background == 1) {
            printf("%4d   %s&\n",i + 1, job_hist[i].binary);
        } else {
            printf("%4d   %s\n",i + 1, job_hist[i].binary);
        }
    }
    return 0;
}

/* Will wait for all the currently running commands */
int builtin_wait(void) {
    int status = 0;
    int i = 0;
   
    /* Iterate through job_hist and if the background was on call waitpid to wait for it if necessary*/
    for (i = 0; i < current_total; i++) {
        if (job_hist[i].is_background == 1) {
            waitpid(job_hist[i].my_pid, &status, 0);
        }
    } 
    return 0;
}

/* Put the last command in the background into the foreground */
int builtin_fg(void) {
    int i = 0;
    int status = 0;

    /* Get the last command to return a waitpid with WNOHANG equal to 0 and call waitpid on that command */
    for (i = current_total - 1; i > 0; i--) {
        if (waitpid(job_hist[i].my_pid, &status, WNOHANG) == 0) {
            waitpid(job_hist[i].my_pid, &status, 0);
            return 0;
        }
    }

    return -1;
}

/* Put the command in the background into the foreground with the desinated job_num*/
int builtin_fg_num(int job_num) {
    int status = 0;
    int i = 0;
    
    /* Call jobs without the print to ensure the job_id's are set */
    builtin_jobs(0);

    /* Iterate through job_hist, look for job_id, check if waidpid with WNOHANG is 0, if it is waitpid and return 0*/
    for (i = 0; i < current_total; i++) {
        if (job_hist[i].job_id == job_num && waitpid(job_hist[i].my_pid, &status, WNOHANG) == 0) {
            waitpid(job_hist[i].my_pid, &status, 0);
            return 0;
        }
    }
    /* Negative 1 returned on faluire */
    return -1;
}

/* Counts the number of commands running */
int num_running() {
    int tot = 0;
    int i = 0;
    int status = 0;
    int pid = 0;
    
    /* Iterates through and checks the waitpid with WNOHNAG to see if it is still running */
    for (i = 0; i < current_total - 1; i++) {
        pid = waitpid(job_hist[i].my_pid, &status, WNOHANG);
        if (pid == 0 && job_hist[i].is_builtin != 1) {           
            tot++;
        }
        
    }
    return tot;
}

/* Correctly updates the total_jobs varaible */
void num_jobs() {
    int i = 0;
    total_jobs = 0;
    for(; i < current_total; i++) {
        if(job_hist[i].is_builtin != 1) {
            total_jobs++;
        }
    } 
}

/* Will count the number of jobs that were in the background */
int num_background() {
    int tot = 0;
    int i = 0;

    for (i = 0; i < current_total; i++) {
        if (job_hist[i].is_background == 1) {
            tot++;
        }
    }
    return tot;
}

/* Free up the space of the job_hist array */
void free_up() {
    free(job_hist);
}