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
#include <fcntl.h>

int parentPID;
/*****************************************************************
* Struct: 
*****************************************************************/
struct flags{
	int backgroundFlag;
	int foregroundMode;
	int exitStatus;
	char inputFile[20], outputFile[20];
	int fileInFlag;
	int fileOutFlag;
};

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
	char* message = "\nCaught SIGINT, sleeping for 2 seconds\n";
	write(STDOUT_FILENO, message, 38);

}
void catchSIGTSTP(int signo){
	char* message = "\nCaught SIGTSTP, exiting!\n";
	write(STDOUT_FILENO, message, 25);
	exit(0);
}


/*****************************************************************
* Function name: cd_command
* Description: 
* Input: 
* Output: 
*****************************************************************/
void status_command(struct flags *f){
	printf("status is: %d\n", f->exitStatus);
	fflush(stdout);
}


/*****************************************************************
* Function name: replacePid
* Description: 
* Input: 
* Output: 
*****************************************************************/
void replacePid(struct strippedUserInput* stripped, struct flags *f){
	int i, j, k;
	char pidThing[20], pidDone[20];
	memset(pidThing, '\0', sizeof(pidThing));
	memset(pidDone, '\0', sizeof(pidDone));
	//check if $$ appears in the args, replaces it with pid
		for(i = 0; i < stripped->argc; i++){
			for(k = 0; k < 20; k++){
				if(stripped->argv[i][k] == '$'){
					if(stripped->argv[i][k+1] == '$'){
						strncpy(pidThing,stripped->argv[i],k);
						memset(stripped->argv[i], '\0', sizeof(stripped->argv[i]));
						sprintf(pidDone, "%s%d", pidThing, getpid());
						strcpy(stripped->argv[i], pidDone);
					}
				}
			}
		}/*
		for(k = 0; k < 20; k++){ 
			if(f->outputFile[k] == '$'){
				if(f->outputFile[k+1] == '$'){
					strncpy(pidThing,f->outputFile,k);
					memset(f->outputFile, '\0', sizeof(f->outputFile));
					sprintf(pidDone, "%s%d", pidThing, getpid());
					strcpy(f->outputFile, pidDone);
					printf("We foudn the $$! its here:   %s\n", f->outputFile);
				}
			}
		}
		for(k = 0; k < 20; k++){ 
			if(f->inputFile[k] == '$'){
				if(f->inputFile[k+1] == '$'){
					strncpy(pidThing,f->inputFile,k);
					memset(f->inputFile, '\0', sizeof(f->inputFile));
					sprintf(pidDone, "%s%d", pidThing, getpid());
					strcpy(f->inputFile, pidDone);
					printf("We foudn the $$! its here:   %s\n", f->inputFile);
				}
			}
		}*/
}

/*****************************************************************
* Function name: cd_command
* Description: 
* Input: 
* Output: 
*****************************************************************/
void cd_command(struct strippedUserInput *stripped, struct flags *f){
	replacePid(stripped, f);
	int i;
	/*MAKE ARRAY OF EXACT SIZE NEEDED*/
	char** arr= malloc(stripped->argc+1*sizeof(char*));
	for(i = 0; i < stripped->argc; i++){
		arr[i] = malloc(sizeof(stripped->argv[i])*sizeof(char));
		arr[i] = stripped->argv[i];
		printf("arr[%d]: %s\n", i, arr[i]);
		//printf("arr[%d]: %s\n",i, arr[i]);
	}
	arr[stripped->argc] = NULL;
	printf("In cd command function\n");
	if(stripped->argc > 1){
		if(execvp(*arr, arr) < 0){
				perror("Error: incorrect arguments\n");
				fflush(stdout);
				f->exitStatus = 2;
			} 
	}
	else{
		execlp("cd", "cd", "~", NULL);
		f->exitStatus = 0;
	}

	for(i = 0; i < stripped->argc; i++){
		free(arr[i]);
	}
	free(arr);

}

/*****************************************************************
* Function name: get_answer
* Description: 
* Input: 
* Output: 
*****************************************************************/
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
* Function name: exec_command
* Description: 
* Input: 
* Output: 
*****************************************************************/
void exec_command(struct strippedUserInput *stripped, struct flags *f){
	pid_t spawnPid = -5;
	int childExitMethod = -5;
	int i, j, k;
	int file_descriptor_in = -5;
	int file_descriptor_out = -5;
	int result;
	int dontCont = 0;

	/*OPEN OUTWARD AND INWARD FILES*/
	if(f->outputFile[0] != '\0'){	
		f->fileOutFlag = 1;
		file_descriptor_out = open(f->outputFile, O_RDONLY);
		if (file_descriptor_out == -1){
			printf("Hull breach - open() failed on \"%s\"\n", f->outputFile); fflush(stdout); perror("In main()");
			f->exitStatus = 1;
			dontCont= 1;
		}
		fcntl(file_descriptor_out, F_SETFD, FD_CLOEXEC);
	}

	if(f->inputFile[0] != '\0'){
		f->fileInFlag = 1;
		file_descriptor_in = open(f->inputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (file_descriptor_in == -1){
			printf("Hull breach - open() failed on \"%s\"\n", f->inputFile); fflush(stdout); perror("In main()");
			f->exitStatus = 1;
			dontCont = 1;
		}
		fcntl(file_descriptor_in, F_SETFD, FD_CLOEXEC);
	}

	//MAKE ARRAY OF EXACT SIZE NEEDED
	char** arr= malloc(stripped->argc+1*sizeof(char*));
	for(i = 0; i < stripped->argc; i++){
		arr[i] = malloc(sizeof(stripped->argv[i])*sizeof(char));
		arr[i] = stripped->argv[i];
		//printf("arr[%d]: %s\n",i, arr[i]);
	}
	arr[stripped->argc] = NULL;

	if(dontCont == 0){
		spawnPid = fork();
		if (spawnPid == -1) //
		{
			perror("Hull Breach!\n");
			exit(1);
		}
		else if (spawnPid == 0) // Terminate the child process immediately
		{
			replacePid(stripped, f);

			if(f->fileInFlag == 1){
				result = dup2(file_descriptor_in, 1); //write is one
				if (result == -1) { 
					perror("Write/in dup2()");
					exit(2);
				}
			}
			if(f->fileOutFlag == 1){
				result = dup2(file_descriptor_out, 0); //read is zero
				if (result == -1) { 
					perror("Read/out dup2()"); 
					exit(2); 
				}
			}
			fflush(stdout);
			if(execvp(*arr, arr) < 0){
				perror("Error: incorrect arguments\n");
				fflush(stdout);
				f->exitStatus = 2;
				exit(2);
			} 
		}
		pid_t childPID;
		if(f->backgroundFlag == 0)
			childPID = waitpid(spawnPid, &childExitMethod, 0); 
		else
			childPID = waitpid(spawnPid, &childExitMethod, WNOHANG); 

		if (WIFEXITED(childExitMethod)) {
			f->exitStatus = WEXITSTATUS(childExitMethod);
			printf("exit status was %d\n", f->exitStatus);
			fflush(stdout);
		}
		else{
			printf("Child terminated by a signal\n");
			fflush(stdout);
		}
	}
	/*FREE MEMEORY*/
	for(i = 0; i < stripped->argc; i++){
		free(arr[i]);
	}
	free(arr);
}

/*****************************************************************
* Function name: filter_answer
* Description: 
* Input: 
* Output: 
*****************************************************************/
void run_answer(struct strippedUserInput *stripped, struct flags *f){
	char cd_ans[3]="cd"; 
	char exit_ans[5] = "exit";
	char status_ans[7] = "status";
	char quit_inc_ans[5] = "quit";

	if(stripped->argc == 1){
		/*CHECK FOR CD, EXIT, STATUS, ELSE RUN EXEC COMMAND*/
		if (!strcmp(stripped->argv[0], cd_ans)){
			cd_command(stripped, f);
		}
		else if (!strcmp(stripped->argv[0], exit_ans)){
			printf("still need to exit all outside processes creating atexit()\n");
			printf("gonna exit\n");
			fflush(stdout);
			exit(0);
		}
		else if (!strcmp(stripped->argv[0], status_ans)){
			/*STATUS COMMAND IS RUN*/
			status_command(f);
		}
		else if (!strcmp(stripped->argv[0], quit_inc_ans)){
			printf("Did you mean 'exit'?\n");
			fflush(stdout);
		}
		else{
			/*RUN EXEC COMMAND WITH STRIPPED ARGS*/
			exec_command(stripped, f);
		}
	}
	else{
		if(!strcmp(stripped->argv[0], cd_ans)){
			cd_command(stripped, f);
		}
		else /*RUN EXEC COMMAND WITH STRIPPED ARGS*/
			exec_command(stripped, f);
		}
}

/*****************************************************************
* Function name: strip_struct
* Description: 
* Input: 
* Output: 
*****************************************************************/
void strip_struct(struct userInput *u, struct strippedUserInput* stripped, int* placesToIgnore){
	int i, j;
	stripped->argc = u->argc;

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
	strcpy(stripped->argv[count++], u->argv[0]);
	for(i = 0; i < u->argc; i++){
		flag = 0;
		for(j = 0; j < 20; j++){
			if(i == placesToIgnore[j]){
			flag = 1;
			}
		}
		if(flag == 0){
			strcpy(stripped->argv[count++], u->argv[i]);
		}
		else
			stripped->argc--;
	}
	stripped->argc++;

	/*PRINT STRIPPED ARRAY*/
	/*for(i = 0; i < stripped->argc; i++){
		printf("%s\n", stripped->argv[i]);
	}*/
}


/*****************************************************************
* Function name: accept_input
* Description: 
* Input: 
* Output: 
*****************************************************************/
int accept_input(struct userInput *u, struct strippedUserInput *stripped, struct flags *f){
	int i, k, m;
	/*PREP FOR POSSIBLE INPUT OUTPUT FILES AND BACKGROUND EXEC. FLAG*/
	char pidThing[20], pidDone[20];
	memset(f->inputFile, '\0', sizeof(f->inputFile));
	memset(f->outputFile, '\0', sizeof(f->outputFile));
	memset(pidThing, '\0', sizeof(pidThing));
	memset(pidDone, '\0', sizeof(pidDone));
	

	int* placesToIgnore = malloc(10*sizeof(int));
	memset(placesToIgnore, '\0', sizeof(placesToIgnore));
	int count = 0;
	/*CHECK ALL INPUT POSSIBILITIES*/
	if(u->argv[u->argc-1][0] == '&'){
			f->backgroundFlag = 1;
			printf("Executing in background.\n");
			fflush(stdout);
			placesToIgnore[count++] = u->argc-1;
	}
	if(u->argv[0][0] != '#'){
			for(i=0; i< u->argc; i++){
			/*check if < (output file) appears in the args*/
			if(u->argv[i][0] == '<'){
				if(u->argc >= i+1){
					if((u->argv[i+1][0] !='>') && u->argv[i+1][0] != '&'){
						strcpy(f->outputFile, u->argv[i+1]);
						//printf("Output file is: %s\n", f->outputFile);
						//fflush(stdout);
						f->fileOutFlag =1;
						placesToIgnore[count++] = i; placesToIgnore[count++] = i+1;
					}
					else{
						printf("ERROR: Incorrect command arguments \n");
						printf("do some sort of exit with status\n");
						fflush(stdout);
					}
				}
				else{
					printf("ERROR: Incorrect command  arguments\n");
					fflush(stdout);
					exit(1); 
				}
			}
			/*check if > (input file) appears in the args*/
			if(u->argv[i][0] == '>'){
				if(u->argc >= i+1){
					if((u->argv[i+1][0] !='<') && u->argv[i+1][0] != '&'){
						strcpy(f->inputFile, u->argv[i+1]);
						//printf("Input file is: %s\n", f->inputFile);
						//fflush(stdout);
						f->fileInFlag;
						placesToIgnore[count++] = i; placesToIgnore[count++] = i+1;
					}
					else{
						printf("ERROR: Incorrect command arguments \n");
						fflush(stdout);
					}
				}
				else{
					printf("ERROR: Incorrect command arguments\n");
					fflush(stdout);
					exit(1);  
				}

			}
		}
	}
	strip_struct(u, stripped, placesToIgnore);
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
* Function name: resetFlags
* Description: RESETS ALL THE FLAGS TO ZERO THAT WAY THERE IS NO 
* CARRY OVER
*****************************************************************/
int resetFlags(struct flags *f){
	f->backgroundFlag = 0;
	f->exitStatus = 0;
	f->fileInFlag = 0;
	f->fileOutFlag = 0;
}

/*****************************************************************
* Function name: main
* Description: CLEANS UP ZOMBIES
*****************************************************************/
void apocalypse(){
	pid_t init;
	int exitMethod = 0;
	do{
		init = -1;
		init = waitpid(init, &exitMethod, 0);
	}while(init != -1);
}
/*****************************************************************
* Function name: main
* Description: SIGNAL HANDLING, DO WHILE FOR PROMPT, CALLS FUNCTIONS 
* TO PARCE INPUT AND STRIP OF INPUT/OUTPUT FILES
*****************************************************************/
int main(){
	parentPID = getpid();
	/*MAIN SIGNAL HANDLING STUFF*/
	struct sigaction SIGINT_action = {0}, SIGTSTP_action = {0}, ignore_action = {0};
	SIGINT_action.sa_handler = catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags = 0;

	SIGTSTP_action.sa_handler = catchSIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = 0;

	ignore_action.sa_handler = SIG_IGN;
	//sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGINT, &ignore_action, NULL);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);
	sigaction(SIGTERM, &ignore_action, NULL);
	sigaction(SIGHUP, &ignore_action, NULL);
	sigaction(SIGQUIT, &ignore_action, NULL);


	/*CREATE TERMINAL PROMPT*/
	char* answer = malloc(256*sizeof(char));
	char exit[5] = "exit";
	int flag = 0;
	struct userInput u = {0};
	struct strippedUserInput stripped = {0};
	struct flags f = {0};
	do{
		apocalypse();
		memset(answer, '\0', sizeof(answer));
		printf(": ");
		fflush(stdout);
		fgets(answer, 256, stdin);
		strtok(answer, "\n"); 
		get_answer(answer, &u);
		accept_input(&u, &stripped,&f);
		run_answer(&stripped, &f);
		reset(&u);
		resetFlags(&f);
		//flag = 1;
	}while(flag == 0);

	return 0;
	//free(answer);
}
                                                               