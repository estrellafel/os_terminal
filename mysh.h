/*
 * Felix Estrella
 *
 * CS441/541: Project 3
 *
 */
#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* For fork, exec, sleep */
#include <sys/types.h>
#include <unistd.h>
/* For waitpid */
#include <sys/wait.h>

/* For Open and Close */
#include <fcntl.h>
#include <unistd.h>
/******************************
 * Defines
 ******************************/
#define TRUE  1
#define FALSE 0

#define MAX_COMMAND_LINE 1024

#define PROMPT ("mysh$ ")


/******************************
 * Structures
 ******************************/
/*
 * A job struct.  Feel free to change as needed.
 */
struct job_t {
    char * full_command;
    int argc;
    char **argv;
    int is_background;
    char * binary;
    pid_t my_pid;
    int is_builtin;
    int is_done;
    int job_id;
    int red_num;
};
typedef struct job_t job_t;


/******************************
 * Global Variables
 ******************************/
 
/*
 * Interactive or batch mode
 */
int is_batch = FALSE;

/*
 * Counts
 */
int total_jobs_display_ctr = 0;
int total_jobs    = 0;
int total_jobs_bg = 0;
int total_history = 0;
int current_total = 0;
/*
 * Global Variables
 */
job_t * job_hist = NULL;
/*
 * Debugging mode
 */
int is_debug = TRUE;

/******************************
 * Function declarations
 ******************************/
/*
 * Parse command line arguments passed to myshell upon startup.
 *
 * Parameters:
 *  argc : Number of command line arguments
 *  argv : Array of pointers to strings
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int parse_args_main(int argc, char **argv);

/*
 * Main routine for batch mode
 *
 * Parameters:
 *  argc : Number of command line arguments
 *  argv : Array of pointers to strings 
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int batch_mode(int argc, char **argv);

/*
 * Main routine for interactive mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int interactive_mode(void);

/*
 * Launch a job
 *
 * Parameters:
 *   loc_job : job to execute
 *
 * Returns:
 *   0 on success
 *   Negative value on error 
 */
int launch_job(job_t * loc_job);

/*
 * Built-in 'exit' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_exit(void);

/*
 * Built-in 'jobs' command
 *
 * Parameters:
 *   int print: 1 is for print, 0 is for no print 
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_jobs(int print);

/*
 * Built-in 'history' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_history(void);

/*
 * Built-in 'wait' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_wait(void);

/*
 * Built-in 'fg' command
 *
 * Parameters:
 *   None (use default behavior)
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_fg(void);

/*
 * Built-in 'fg' command
 *
 * Parameters:
 *   Job id
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_fg_num(int job_num);

/*
 * Will get the number of words in a string
 *
 * Paramaters:
 *   char *str
 *
 * Returns:
 *   The number of words in the string
 */
int get_Length(char *str);

/*
 * Will set the argv for a job_t 
 *
 * Parameter:
 *   job_t * loc_job
 *
 * Returns:
 *   None
 */
void set_agrv(job_t * loc_job);

/*
 * Will get a substring.
 *
 * Parameters:
 *   const char *src
 *   int start
 *   int end
 *
 * Returns:
 *   A char*. (string)
 */
char* substr(const char *src, int start, int end);

/*
 * Will create and set up the job_t
 *
 * Parameters:
 *   char *user_input
 *   int background
 *   int redirection
 *
 * Returns:  
 *   None
 */
void job_set_up(char *user_input, int background, int redirection);

/*
 * Will pasre the command looking for ; and &.
 * 
 * Parameters:
 *   char *user_input
 *
 * Returns:
 *   None
 */
void parse_command(char *user_input);

/*
 * Will send a builtin command to execute.
 *
 * Parameters:
 *   char *user_input
 *
 * Returns:
 *   0 for not builtin command
 *   1 for builtin command
 */
int is_builtin(char *user_input);

/*
 * Will add the given info to the job_hist array
 *
 * Parameters:
 *   char *str
 *   int app
 *
 * Returns:
 *   None
 */
void add_to_array(char *str, int app);

/*
 * Will remove the whitespace before a string
 *
 * Parameter:
 *   char *str
 *
 * Returns:
 *   The string without whitespace before it
 */
char* elim_lead_space(char *str);

/*
 * Will return the number of jobs currently running
 *
 * Parameter:
 *  None
 *
 * Returns:
 * The number of running jobs
 */
int num_running();

/*
 * Will return the number of jobs that were placed in the background
 *
 * Parameter:
 *   None
 *
 * Returns:
 *   The number of jobs that were placed in the background  
 */
int num_background();

/*
 * Will set the correct job_hist index's is_builtin to 1
 *
 * Parameter:
 *   None
 *
 * Returns:
 *   None
 */
void set_builtin();

/*
 * Will return a number based on if it file redirection or not
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 - No file redirction
 *   1 - STDOUT
 *   2 - STDIN
 */
int file_redirect(char *input);

/*
 * Will set the number of total_jobs 
 *
 * Parameter:
 *   None
 *
 * Returns:
 *   None
 */
void num_jobs();

/*
 * Will free the space for job_hist
 *
 * Parameter:
 *   None
 *
 * Returns:
 *   None
 */
void free_up();
#endif /* MYSHELL_H */