/**********************************************************************************
* Program Name: SmallShell - C portion
* Author: Lily Shellhammer
* Date: Feb 19th, 2017
* Description: 
***********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>


/*****************************************************************
* Struct: 
*****************************************************************/
struct userInput{
	char** argv; //double array of words entered
	int argc;	//amount of words user entered
	int count;
};

struct strippedUserInput{
	char** argv; //double array of words entered
	char* firstArgv;
	int argc;	//amount of words user entered
	int count;
};

/*****************************************************************
* Function name: catchSIGINT
* Description: 
* Input: 
* Output: 
*****************************************************************/
void catchSIGINT(int signo){
	char* message = "Caught SIGINT, sleeping for 2 seconds\n";
	write(STDOUT_FILENO, message, 38);
	raise(SIGUSR2);
	sleep(2);
}
void catchSIGUSR2(int signo){
	char* message = "Caught SIGUSR2, exiting!\n";
	write(STDOUT_FILENO, message, 25);
	exit(0);
}


/*****************************************************************
* Function name: cd_command
* Description: 
* Input: 
* Output: 
*****************************************************************/
void status_command(){
	printf("In status function.\n");
}

void cd_command(){
	printf("In cd command function\n");
}

void get_answer(char* answer, struct userInput *u){
	//printf("answer is: %s\n", answer);
	int i = 0;

	/*ALLOCATE SPACE TO HOLD WORDS*/
	int num_letters = 15;
	u->count = 20;
	u->argv = malloc(u->count * sizeof(char*));
	for(i = 0; i < u->count; i++){
		u->argv[i] = malloc(30 * sizeof(char));
		memset(u->argv[i], '\0', sizeof(u->argv[i]));
	}

	/*STRTOK to save in char** array */
	char* token = strtok(answer, " ");
	strcpy(u->argv[0], token);
	u->argc = 1;
	while(token!= NULL){
		token = strtok(NULL, " ");
		if(token != NULL){
			strcpy(u->argv[u->argc] , token);
			u->argc++;
		}
	}

	/*PRINT WHOLE STRUCT*/
	/*for(i = 0; i < u->argc; i++){
		printf("%s\n", u->argv[i]);
	}*/
	
}

/*****************************************************************
* Function name: filter_answer
* Description: 
* Input: 
* Output: 
*****************************************************************/
void exec_command(struct userInput *stripped){

}

/*****************************************************************
* Function name: filter_answer
* Description: 
* Input: 
* Output: 
*****************************************************************/
void run_answer(struct userInput *stripped, int* flag){
	/*NEED TO STRTOK TO GET EACH ARGUMENT INDIVIDUALLY IN A CHAR** */

	char cd_ans[3]="cd"; 
	char exit_ans[5] = "exit";
	char status_ans[7] = "status";
	char quit_inc_ans[5] = "quit";

	if(stripped->argc == 1){
		/*CHECK FOR CD, EXIT, STATUS, ELSE RUN EXEC COMMAND*/
		if (!strcmp(stripped->argv[0], cd_ans)){
			/*CALL CD ANSWER*/
			printf("gonna run cd command\n");
			cd_command();
		}
		else if (!strcmp(stripped->argv[0], exit_ans)){
			printf("still need to exit all outside processes creating atexit()\n");
			printf("gonna exit\n");
			exit(0);
		}
		else if (!strcmp(stripped->argv[0], status_ans)){
			printf("gonna run status\n");
			status_command();
		}
		else if (!strcmp(stripped->argv[0], quit_inc_ans)){
			printf("Did you mean 'exit'?\n");
		}
		else{
			printf("gonna run exec on %s\n", stripped->argv[0]);
			exec_command(stripped);
		}
	}
}

void write_to(){

		/*CREATE OUTPUT FILE
	file_descriptor = open(u.argv[i+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (file_descriptor == -1)
	{
		printf("Hull breach - open() failed on \"%s\"\n", argv[i+1]); perror("In main()");
		exit(1);
	}
	*/
}

void strip_struct(struct userInput *u, struct strippedUserInput* stripped, int* placesToIgnore){
	int i, j;
	stripped->argc = u->argc;

	for(i = 0; i < 5; i++){
		printf("%d, ", placesToIgnore[i]);
	}
	printf("\n and stripped->argc == %d\n", stripped->argc);

	stripped->argv = malloc(stripped->argc * sizeof(char*));
	for(i = 0; i < u->argc; i++){
		stripped->argv[i] = malloc(30 * sizeof(char));
		memset(stripped->argv[i], '\0', sizeof(stripped->argv[i]));
	}
	stripped->firstArgv= malloc(30 * sizeof(char));
	memset(stripped->firstArgv, '\0', sizeof(stripped->firstArgv));

	int flag = 0;
	int count= 0;
	strcpy(stripped->firstArgv, u->argv[0]);
	for(i = 0; i < u->argc; i++){
		flag = 0;
		for(j = 0; j < 20; j++){
			if(i == placesToIgnore[j]){
			flag = 1;
			stripped->argc--;
			}
		}
		if(flag == 0){
			strcpy(stripped->argv[count++], u->argv[i]);
		}
	}

	for(i = 0; i < stripped->argc; i++){
		printf("%s", stripped->argv[i]);
	}
}


/*****************************************************************
* Function name: accept_input
* Description: 
* Input: 
* Output: 
*****************************************************************/
int accept_input(struct userInput *u, struct strippedUserInput *stripped){
	int i;
	/*PREP FOR POSSIBLE INPUT OUTPUT FILES AND BACKGROUND EXEC. FLAG*/
	char inputFile[20], outputFile[20];
	memset(inputFile, '\0', sizeof(inputFile));
	memset(outputFile, '\0', sizeof(outputFile));
	
	int backgroundFlag = 0;

	int* placesToIgnore = malloc(10*sizeof(int));
	memset(placesToIgnore, '\0', sizeof(placesToIgnore));
	int count = 0;
	/*CHECK ALL INPUT POSSIBILITIES*/
	if(u->argv[u->argc-1][0] == '&'){
			backgroundFlag = 1;
			printf("Executing in background.\n");
			placesToIgnore[count++] = u->argc-1;
			//this means executed in the background
	}
	if(u->argv[0][0] == '#'){
		printf("this is a comment line, do nothing \n");
	}
	else{
		for(i=0; i< u->argc; i++){
			if(u->argv[i][0] == '<'){
				if(u->argc >= i+1){
					if((u->argv[i+1][0] !='>') && u->argv[i+1][0] != '&'){
						strcpy(outputFile, u->argv[i+1]);
						printf("Output file is: %s\n", outputFile);
						placesToIgnore[count++] = i; placesToIgnore[count++] = i+1;

					}
					else{
						printf("ERROR: Incorrect command arguments \n");
						printf("do some sort of exit with status\n");
					}
				}
				else{
					printf("ERROR: Incorrect command  arguments\n");
					exit(1);  //check that this is a valid file!!
				}
			}
			else if(u->argv[i][0] == '>'){
				if(u->argc >= i+1){
					if((u->argv[i+1][0] !='<') && u->argv[i+1][0] != '&'){
						strcpy(inputFile, u->argv[i+1]);
						printf("Input file is: %s\n", inputFile);
						placesToIgnore[count++] = i; placesToIgnore[count++] = i+1;
					}
					else{
						printf("ERROR: Incorrect command arguments \n");
					}
				}
				else{
					printf("ERROR: Incorrect command arguments\n");
					exit(1);  //check that this is a valid file!
				}

			}
		}
	}
	strip_struct(u, stripped, placesToIgnore);
	printf("we are after strip struct\n");
	free(placesToIgnore);
}

/*****************************************************************
* Function name: reset
* Description: Clears out argv in user_input struct, sets argc to 0
* Input: userInput struct filled with last command
* Output: none
*****************************************************************/
int reset(struct userInput *u){
	int i;
	for (i = 0; i < u->argc; i++){
		free(u->argv[i]);
	}
	free(u->argv);
	u->argc = 0;
}

/*****************************************************************
* Function name: main
* Description: 
* Input: 
* Output: 
*****************************************************************/
int main(){
	/*MAIN SIGNAL HANDLING STUFF*/
	struct sigaction SIGINT_action = {0}, SIGUSR2_action = {0}, ignore_action = {0};
	SIGINT_action.sa_handler = catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;

	SIGUSR2_action.sa_handler = catchSIGUSR2;
	sigfillset(&SIGUSR2_action.sa_mask);
	SIGUSR2_action.sa_flags = 0;

	ignore_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGUSR2, &SIGUSR2_action, NULL);
	sigaction(SIGTERM, &ignore_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);
	
	printf("SIGTERM, SIGHUP, and SIGQUIT are disabled.\n");

	/*CREATE TERMINAL PROMPT*/
	char* answer = malloc(256*sizeof(char));
	char exit[5] = "exit";
	int flag = 0;
	struct userInput u = {0};
	struct strippedUserInput stripped = {0};
	/************* TO FIX!
	/************CURRENTLY DO WHILE FOR PROMPTING USER ": " ONLY HANDLES ONE INPUT ARG*/
	do{
		memset(answer, '\0', sizeof(answer));
		printf(": ");
		fflush(stdout);
		fgets(answer, 256, stdin);
		strtok(answer, "\n"); 
		get_answer(answer, &u);
		accept_input(&u, &stripped);
		run_answer(&u, &flag);
		reset(&u);
		//flag = 1;
	}while(flag == 0);


	//free(answer);
}
   