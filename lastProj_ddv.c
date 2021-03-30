#include <getopt.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<readline/readline.h>
#include<readline/history.h>
#define N 100
#define len 256

extern char **environ;


int argc=0;


int catSh(char **args);
int envSh(char **args);
int headSh(char **args);
int helpSh(char **args);
int historySh();
int exitSh(char **args);



char *builtin_str[] = {
    "cat",
    "env",
    "head",
    "help",
    "history",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &catSh,
    &envSh,
    &headSh,
    &helpSh,
    &historySh,
    &exitSh
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

void cat_(int, char **, FILE *fout);
void cat_withoutS(int, int, int, int, char **, FILE *fout);
void cat_s(int, int, int, int, char **, FILE *fout);

void head_c(int, int, int, int, char **, FILE *fout);
void head_n(int, int, int, int, char **, FILE *fout);
void head_simple(int, int, char **, FILE *fout);


int envSh(char** argv) {
    int count = 0;
    int ch;

    if(argc == 1) {
        while(environ[count] != NULL)
        {
            printf("[%s] \n ", environ[count]);
            count++;
        }
    } else
        while((ch=getopt(argc,argv,"u:"))!=-1)
        {

            switch(ch)
            {
            case 'u':

                if(unsetenv(argv[2]) == -1)
                    printf("%s","Error");
                break;

            }
        }


    return 1;
}


int headSh(char** args) {

    int ch,nBytes,nline=0;
    char fileName[N];
    int contor=0;
    int v_flag = 0;
    int c_flag = 0;
    int n_flag = 0;

    int q_flag = 0;
    int n_index_fileNames;
    int c_index_fileNames;

    int index;
    int opt;
    int  numberOfArgs=1;
    optind = 1;
    int n_option[10];
    int c_option[10];
    int n_number_Files[10], c_number_Files[10]; 
    char *n_LinesVector[10][10];
    char *c_LinesVector[10][10];

    int nLines_contor=0;
    int cLines_contor=0;


    int n_contor_fisiere,c_contor_fisiere;

    while( args[numberOfArgs] != NULL )
        numberOfArgs++;

    char* filename =   args[argc-1];
    int option=-1;

    while((ch=getopt(numberOfArgs,args,"vn:c:q"))!=-1)
    {
        switch(ch)
        {

        case 'v':
            v_flag = 1;
            q_flag = 0;
            break;

        case 'n':
            n_flag = 1;
            n_index_fileNames = optind;

            n_contor_fisiere=0;
            for( ; n_index_fileNames < argc && *args[n_index_fileNames] != '-'; n_index_fileNames++) {
                n_LinesVector[nLines_contor][n_contor_fisiere]= args[n_index_fileNames];
                n_contor_fisiere++;
            }
            n_number_Files[nLines_contor] = n_contor_fisiere;
            n_option[nLines_contor] = atoi(optarg); 
            nLines_contor++; 
            break;

        case 'c':
            c_flag = 1;
            c_index_fileNames = optind;
            c_contor_fisiere=0;
            for( ; c_index_fileNames < argc && *args[c_index_fileNames] != '-'; c_index_fileNames++) {
                c_LinesVector[cLines_contor][c_contor_fisiere]= args[c_index_fileNames];
                c_contor_fisiere++;
            }
            c_number_Files[cLines_contor] = c_contor_fisiere; 
            c_option[cLines_contor] = atoi(optarg); 
            cLines_contor++; 
            break;

        case 'q':
            q_flag = 1;
            v_flag = 0;
            break;
        }
    }


    if((c_flag == 0)&&(n_flag == 0)) {
        int nrFiles= argc-optind;
        char *strs[nrFiles];
        int contor=0;
        for (index = optind; index < argc; index++) {
            strs[contor] = args[index];
            contor++;
        }
        
        head_simple(q_flag,nrFiles,strs, stdout);
      
    }


    for(int i=0; i<nLines_contor ; i++) {
        head_n(n_option[i], v_flag, q_flag, n_number_Files[i], n_LinesVector[i], stdout);
    }

    for(int i=0; i<cLines_contor ; i++) {
        head_c(c_option[i], v_flag, q_flag, c_number_Files[i], c_LinesVector[i], stdout);
    }

    return 1;

}

void head_n(int nLines, int v_flag, int q_flag,int nrFiles, char **filename, FILE *fout) {

    char line[len];


    for (int i=0; i<nrFiles; i++) {

        FILE* fp=fopen(filename[i],"r");
        if (fp == NULL)
        {
            printf("head: cannot open %s for reading: No such file or directory",filename[i] );
            continue;
        }
        if((v_flag==1)||((nrFiles > 1)&&(q_flag==0)))
            printf("==> %s <==\n",filename[i]);
        int contor=0;
        while (fgets(line,len,fp))
        {
            if(contor<nLines) {
                for(int i=0; i<len; i++) {
                    if(line[i] == '\n') {
                        printf("\n");
                        break;
                    }
                    printf("%c",line[i]);
                }
                contor++;
            } else
                break;
        }
        fclose(fp);
    }

}

void head_c(int byteAmount, int v_flag, int q_flag,int nrFiles, char **filename, FILE *fout) {


    for (int i=0; i<nrFiles; i++) {
        int ch;
        int count=0;


        char buffer[byteAmount+1];

        int r;

        FILE* fp=fopen(filename[i],"r");
        if (fp == NULL)
        {
            printf("head: cannot open %s for reading: No such file or directory",filename[i] );
            continue;
        }

        if((v_flag==1)||((nrFiles > 1)&&(q_flag==0)))
            printf("==> %s <==\n",filename[i]);

        r = fread( buffer, 1, byteAmount, fp );
        buffer[byteAmount] = '\0';

        if(i<nrFiles-1)
        printf("%s\n",buffer);
       else 
       if(i==nrFiles-1)
          printf("%s",buffer);

        fclose(fp);
    }
}

void head_simple(int q_flag,int nrFiles, char **filename, FILE *fout) {
  
  char line[len];


    for (int i=0; i<nrFiles; i++) {

        FILE* fp=fopen(filename[i],"r");
        if (fp == NULL)
        {
            printf("head: cannot open %s for reading: No such file or directory",filename[i] );
            continue;
        }
        if(q_flag==0)
            printf("==> %s <==\n",filename[i]);
       
        int count=0;
        while (fgets(line,len,fp))
        {    if(count==10)
                        break;
                    else if(count<10){
            
                for(int i=0; i<len; i++) {
                    if(line[i] == '\n') {
                        printf("\n");
                        count++;
                        break;
                    }
                    printf("%c",line[i]);
                }
        }
        }
        fclose(fp);
    }

}



int catSh( char **args)
{

    int b_flag = 0;
    int E_flag = 0;
    int n_flag = 0;
    int s_flag = 0;
    int index;

    int opt,i;
    int  numberOfArgs=1;
    optind = 1;
    while( args[numberOfArgs] != NULL )
        numberOfArgs++;

    char* filename =   args[argc-1];

    while((opt=getopt(numberOfArgs, args, "bEns"))!=-1 ) {
        switch (opt)
        {
        case 'b':
            b_flag=1;
            break;
        case 'E':
            E_flag=1;
            break;
        case 'n':
            n_flag = 1;
            break;
        case 's':
            s_flag=1;
            break;
        }
    }

    int nrFiles= argc-optind;
    char *strs[nrFiles];
    int contor=0;
    for (index = optind; index < argc; index++) {
        strs[contor] = args[index];
        contor++;
    }
    
    if((s_flag==0)&&(n_flag==0)&&(b_flag==0)&&(E_flag==0))
        cat_(nrFiles,strs,stdout);
    if((s_flag==0)&&((n_flag!=0)||(b_flag!=0)||(E_flag!=0)))
        cat_withoutS(b_flag, n_flag, E_flag, nrFiles, strs, stdout);

    if(s_flag==1)
        cat_s(b_flag, n_flag, E_flag, nrFiles, strs, stdout);

    return 1;


}
void cat_(int nrFiles, char** filename, FILE *fout){
   char line[len];
    int linenumber=1;
for (int i=0; i<nrFiles; i++) {
   

        FILE* fp=fopen(filename[i],"r");
        if (fp == NULL)
        {
            puts("Error while opening file");
        }
        while (fgets(line,len,fp))
        {
            
          
            for(int i=0; i<len; i++) {
                if(line[i] == '\n') {
                        printf("\n");
                    break;
                }/*
                if(line[i] == '\0') {
                    printf("\0");
                    break;
                }*/
                printf("%c",line[i]);
            }

        }
        fclose(fp);
    }
}

void cat_withoutS (int b_flag, int n_flag, int E_flag, int nrFiles, char** filename, FILE *fout)
{

    if(b_flag==1)
        n_flag = 0;

    char line[len];
    int linenumber=1;
    for (int i=0; i<nrFiles; i++) {
        FILE* fp=fopen(filename[i],"r");
        if (fp == NULL)
        {
            puts("Error while opening file");
        }
        while (fgets(line,len,fp))
        {
            if(b_flag == 1) {
                if((line[0]!='\0')&(line[0]!='\n')) {
                    printf("%d",linenumber);
                    ++linenumber;

                }
            }
            else if(n_flag == 1) {
                printf("%d",linenumber);
                ++linenumber;
            }

            for(int i=0; i<len; i++) {
                if(line[i] == '\n') {
                    if(E_flag==1) {
                        printf("$\n");
                    }
                    else
                        printf("\n");
                    break;
                }/*
                if(line[i] == '\0') {
                    printf("\0");
                    break;
                }*/
                printf("%c",line[i]);
            }

        }
        fclose(fp);
    }

}

void cat_s (int b_flag, int n_flag, int E_flag, int nrFiles, char** filename, FILE *fout)
{

    if(b_flag==1)
        n_flag = 0;
    char line[len];
    int linenumber=1;
    for (int i=0; i<nrFiles; i++) {
        FILE* fp=fopen(filename[i],"r");
        if (fp == NULL)
        {
            puts("Error while opening file");
        }

        int emptyLine=0;

        while (fgets(line,len,fp))
        {

            if(line[0]!='\n') {
                emptyLine=0;

                if(b_flag == 1) {
                    if((line[0]!='\0')&(line[0]!='\n')) {
                        printf("%d",linenumber);
                        ++linenumber;

                    }
                }
                else if(n_flag == 1) {
                    printf("%d",linenumber);
                    ++linenumber;
                }

                for(int i=0; i<len; i++) {
                    if(line[i] == '\n') {
                        if(E_flag==1) {
                            printf("$\n");
                        }
                        else
                            printf("\n");
                        break;
                    }/*
                    if(line[i] == '\0') {
                        printf("\0");
                        break;
                    }*/
                    printf("%c",line[i]);
                }
            }
            if((line[0]=='\n')&&(emptyLine==0)) {

                if(b_flag == 1) {
                    if((line[0]!='\0')&(line[0]!='\n')) {
                        printf("%d",linenumber);
                        ++linenumber;

                    }
                }
                else if(n_flag == 1) {
                    printf("%d",linenumber);
                    ++linenumber;
                }

                for(int i=0; i<len; i++) {
                    if(line[i] == '\n') {
                        if(E_flag==1) {
                            printf("$\n");
                        }
                        else
                            printf("\n");
                        break;
                    }/*
                    if(line[i] == '\0') {
                        printf("\0");
                        break;
                    }*/
                    printf("%c",line[i]);
                }
                emptyLine=1;
            }

        }
        fclose(fp);
    }

}

int helpSh(char **args)
{
    int i;
    printf("Welcome :)\n");
    printf("Author: Denisa Deak\n");
    printf("Available commands:\n");

    for (i = 0; i < num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }


    return 1;
}

int historySh(){
         HISTORY_STATE *myhist = history_get_history_state ();
         HIST_ENTRY **mylist = history_list ();
        for (int i = 0; i < myhist->length; i++) { 
                printf ("%d %s %s\n",i+1, mylist[i]->line, mylist[i]->timestamp);
                }
    return 1;

}

int exitSh(char **args)
{
    return 0;
}


int launchSh(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("$> ");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


int execSh(char **args)
{
    int i;

    if (args[0] == NULL) {
        // An empty command was entered
        return 1;
    }

    for (i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launchSh(args);
}

#define RL_BUFSIZE 1024

char *sh_read_line(void)
{


    int bufsize = RL_BUFSIZE;
    int position = 0;
    char *buffer = (char*)malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "$>: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {

        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += RL_BUFSIZE;
            buffer =(char*) realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "$>: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

char **sh_split_line(char *line)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = (char**)malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "$>: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;

        position++;
        argc=position;
        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens =(char**) realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "$>: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


void loopSh(void)
{
    char *line;
    char **args;
    int status;
    using_history();    /* initialize history */
    do {
        printf("$> ");
        line = sh_read_line();
        add_history(line);
        args = sh_split_line(line);
        status = execSh(args);
         free(line);
        free(args);
    } while (status);
        
       
}


int main(int argc, char **argv[])
{
    loopSh();
    return EXIT_SUCCESS;
}