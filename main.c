
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include <linux/limits.h>



char target_path[] = "/bin/ft_shield";
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


void configure_daemon(){
    if (access(daemon_config_path, W_OK) != 0) {
        printf("oopss we don't have access to it sorry");
    }
    printf("we have access to write our service");


    printf("------------");
    printf("Daemon configuration\n%s", daemon_config);
    printf("system status = %d\n",system("systemctl daemon-reload && systemctl enable ft_shield.service"));
}


int main(){

    // if we are in /bin 
    if (getuid() != 0){
        printf("you don't have root access\n");
    }
    



    if (!in_bin()) {
        printf("we are at bin\n");
    } else {
        configure_daemon();
        printf("ooops we are not at bin\n");

    }





    return 0;
}
