#include "sysapi.h"
#include "../../../shared/stdlib.h"

int cmp_chaine_shell(const char* s1, const char* s2) {
    if (s1 == 0 || s2 == 0) {
      return 1;
    }
    const char* c1, *c2;
    for (c1 = s1; *c1=='\t' || *c1 == ' '; c1++);
    for (c2 = s2; *c2 != 0; c2++, c1++) {
      if (*c2 != *c1) {
        return 1;
      }
    }
    if (*c1 != ' ' && *c1 != '\t' && *c1 != 0) {
      return 1;
    }
    return 0;
}

int cmp_arg(const char* s1, const char* s2) {
    if (s1 == 0 || s2 == 0) {
      return 1;
    }
    const char* c1, *c2;
    for (c1 = s1; *c1=='\t' || *c1 == ' '; c1++);
    for (c1 = c1; *c1 != ' ' && *c1 != '\t'; c1++);
    for (c1 = c1; *c1=='\t' || *c1 == ' '; c1++);
    for (c2 = s2; *c2 != 0; c2++, c1++) {
      if (*c2 != *c1) {
        return 1;
      }
    }
    if (*c1 != ' ' && *c1 != '\t' && *c1 != 0) {
      return 1;
    }
    return 0;
}

void get_arg(const char* commande, char* stock) {
    if (stock != 0 && commande != 0) {
      memset((void*)stock,0,strlen(stock));
      const char* c1;
      for (c1 = commande; *c1=='\t' || *c1 == ' '; c1++);
      for (c1 = c1; *c1 != ' ' && *c1 != '\t'; c1++);
      for (c1 = c1; *c1=='\t' || *c1 == ' '; c1++);
      for (int i=0; *c1 != 0 && *c1 != ' ' && *c1 != '\t'; i++, c1++) {
        stock[i] = *c1;
      }
    }
}

void print_help(void) {
    printf("\nUSER COMMANDS\n");
    printf("%-20s%s\n", "ps", "report a snapshot of the current processes");
    printf("%-20s%s\n", "test_user", "run all the user tests except test7");
    printf("%-20s%s\n", "test_user7", "run test 7, beware it takes 12 mins");
    printf("%-20s%s\n", "echo on/off", "enable/disable display in echo mode");
    printf("%-20s%s\n", "clear", "clear the shell screen");
    printf("%-20s%s\n", "time", "get number of IT since kernel start");
    printf("%-20s%s\n", "sleep <x>", "sleep the terminal for x seconds");
    printf("%-20s%s\n", "exit", "exit the shell");
    printf("%-20s%s\n", "help", "display possible user commands");

}

int main(void * arg) {
    (void) arg;
    printf("Bienvenue sur intellOS!\nSaisissez 'help' pour la liste des commandes! \n\n");

    const long unsigned int size = 20;
    char string[20];

    while (1) {
        printf("shell> ");
        memset(string,0,strlen(string));
        cons_read(string, size);
        if (cmp_chaine_shell(string, "ps") == 0) {
            print_all_processes();
            printf("\n");
        } else if (cmp_chaine_shell(string, "clear") == 0) {
            efface_ecran();
        } else if (cmp_chaine_shell(string, "test_user") == 0) {
            int pid = start("autotest", 4096, 64, (void*)0);
            waitpid(pid, 0);
            printf("\n");
        } else if (cmp_chaine_shell(string, "help") == 0) {
            print_help();
            printf("\n");
        } else if (cmp_chaine_shell(string, "echo") == 0) {
            if(cmp_arg(string, "on")==0) {
              cons_echo(1);
            } else if(cmp_arg(string, "off")==0) {
              cons_echo(0);
            } else {
              printf("Argument incorrect");
            }
            printf("\n");
        } else if (cmp_chaine_shell(string, "time") == 0) {
            printf("%lu\n", current_clock());
        } else if (cmp_chaine_shell(string, "sleep") == 0) {
            char s[20];
            get_arg(string, s);
            unsigned long quartz, ticks;
            clock_settings(&quartz, &ticks);
            (void) quartz;
            wait_clock(current_clock()+strtoul(s, 0, 10)*ticks);
            printf("\n");
        } else if (cmp_chaine_shell(string, "exit") == 0) {
            printf("Au revoir!\n");
            exit(0);
	} else if (cmp_chaine_shell(string, "test_user7") == 0) {
	    int pid = start("test7", 4096, 128, NULL);
	    waitpid(pid, 0);
	    printf("\n");
        } else if (cmp_chaine_shell(string, "") == 0) {

        } else {
            printf("Commande introuvable: %s\n", string);
        }
    }
}
