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


int backgroundFlag;

/*****************************************************************
* Struct: 
*****************************************************************/
struct userInput{
	char** argv; //double array of words entered
	int argc;	//amount of words user entered
	int count;
};

/*****************************************************************
* Function name: cd_command
* Description: 
* Input: 
* Output: 
*****************************************************************/
void cd_command(){

}

void get_answer(char* answer, struct userInput u){
	printf("answer is: %s\n", answer);
	/*STRTOK to find count */
	int i = 0;
	/*char* token = strtok(answer, " "); 
	printf("strtok on original time is: %s\n", token);
	while(token!= NULL){
		token = strtok(NULL, " "); 
		printf("strtok on %dth time is: %s\n", i, token);
		i++;
	}
	printf("count is: %d\n", i);*/

	/*ALLOCATE SPACE TO HOLD WORDS*/
	int num_letters = 10;
	u.count = 40;
	u.argv = malloc(u.argc * sizeof(char*));
	for(i = 0; i < u.argc; i++){
		u.argv[i] = malloc(num_letters * sizeof(char));
		memset(u.argv[i], '\0', sizeof(u.argv[i]));
	}

	/*STRTOK to save in char** array */
	token = strtok(answer, " ");
	printf("answer is: %s", answer);
	strcpy(u.argv[0], token);
	u.argc = 1;
	for(i=1; i< u.argc; i++){
		u.argv[i] = strtok(NULL, " ");
		u.argc++;
	}

	/*PRINT WHOLE STRUCT*/
	for(i = 0; i < u.argc; i++){
		printf("%s\n", u.argv[i]);
	}
}
/*****************************************************************
* Function name: filter_answer
* Description: 
* Input: 
* Output: 
*****************************************************************/
void filter_answer(char* answer, int* flag){
	/*NEED TO STRTOK TO GET EACH ARGUMENT INDIVIDUALLY IN A CHAR** */

	char cd_ans[3]="cd"; 
	char exit_ans[5] = "exit";
	char status_ans[7] = "status";
	/*CHECK FOR CD, EXIT, STATUS, ELSE RUN EXEC COMMAND*/
	if(strcmp(answer, cd_ans)){
		/*CALL CD ANSWER*/
		printf("gonna run cd command");
		cd_command();
	}
	else if(strcmp(answer, exit_ans)){
		printf("gonna exit");
	}
	else if(strcmp(answer, status_ans)){
		printf("gonna run status");
	}
	else
		printf("gonna run exec on %s", answer);

}


/*****************************************************************
* Function name: accept_input
* Description: 
* Input: 
* Output: 
*****************************************************************/
int accept_input(struct userInput u){
	int i;
	/*PREP FOR POSSIBLE INPUT OUTPUT FILES AND BACKGROUND EXEC. FLAG*/
	char inputFile[20], outputFile[20];
	memset(inputFile, '\0', sizeof(inputFile));
	memset(outputFile, '\0', sizeof(outputFile));


	/*CHECK ALL INPUT POSSIBILITIES*/
	if(u.argv[u.argc][0] == '&'){
			backgroundFlag = 1;
			//this means executed in the background
	}
	for(i=0; i< u.argc; i++){
		if(u.argv[i][0] == '<'){
			if(u.argc != i+1)
				strcpy(outputFile, u.argv[i+1]);
			else{
				printf("ERROR: Incorrect command line arguments\n");
				exit(1);  //check that this is a valid file!!
			}
			/*CREATE OUTPUT FILE
			file_descriptor = open(u.argv[i+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if (file_descriptor == -1)
			{
				printf("Hull breach - open() failed on \"%s\"\n", argv[i+1]); perror("In main()");
				exit(1);
			}
			*/
		}
		else if(u.argv[i][0] == '>'){
			if(u.argc != i+1)
				strcpy(outputFile, u.argv[i+1]);
			else{
				printf("ERROR: Incorrect command line arguments\n");
				exit(1);  //check that this is a valid file!
			}
			/*CREATE INPUT FILE
			file_descriptor = open(u.argv[i+1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
			if (file_descriptor == -1)
			{
				printf("Hull breach - open() failed on \"%s\"\n", u.argv[i+1]); perror("In main()");
				exit(1);
			}
			*/

		}
	}

}

/*****************************************************************
* Function name: reset
* Description: Clears out argv in user_input struct, sets argc to 0
* Input: userInput struct filled with last command
* Output: none
*****************************************************************/
int reset(struct userInput u){
	int i;
	for (i = 0; i < u.argc; i++){
		free(u.argv[i]);
	}
	free(u.argv);
	u.argc = 0;
}

/*****************************************************************
* Function name: main
* Description: 
* Input: 
* Output: 
*****************************************************************/
int main(){
	char* answer = malloc(1000*sizeof(char));
	char exit[5] = "exit";
	int flag = 0;
	struct userInput u = {0};
	/************* TO FIX!
	/************CURRENTLY DO WHILE FOR PROMPTING USER ": " ONLY HANDLES ONE INPUT ARG*/
	do{
		memset(answer, '\0', sizeof(answer));
		printf(": ");
		fflush(stdout);
		fgets(answer, 10000, stdin);
		strtok(answer, "\n"); 
		get_answer(answer, u);
		//do stuff with answer
		reset(u);
		flag = 1;
	}while(flag == 0);

	free(answer);
}
