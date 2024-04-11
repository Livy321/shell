#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <limits.h>
#include <fcntl.h>
#define NEXT 2000
#define PNEXT 5


// функция ввода динамической строки
char *strget(void){
    char *s=malloc(NEXT);
    char PathName[PATH_MAX];

    int k=0,n=0;
    if (!s){
        fprintf(stderr,"ERROR!");
        exit(1);
    }
    getcwd(PathName,sizeof(PathName));
    printf("%s: ", PathName);
    while (fgets(s+k,NEXT,stdin)){
        n=strlen(s);
        if (s[n-1]!='\n'){
            k+=NEXT-1;
            s=realloc(s,NEXT+k);
            if (!s){
                fprintf(stderr,"ERROR!");
                exit(1);
            }
        }
        else{
            s[n-1]='\0';
            return s;
        }
    }
    free(s);
    return NULL;
}


int words(char ***mas,int *nlen){
    char *s=NULL,*s1=NULL;
    int lens=0,i=0,j=0,k=0,p=0,k1=0,fl=1;

    if((s=strget())==NULL)
        return 0;

    *mas=malloc(NEXT*sizeof(char*));

    s1=malloc(sizeof(char)*NEXT);
    k=lens+NEXT;
    k1=NEXT;
    j=lens;
    while (s[i]!='\0'){
        if (j>=k){
            k+=NEXT;
            *mas=realloc(*mas,(k)*sizeof(char*));
        }
        while (s[i]==' ')
            i++;
        if((s[i]=='&' && s[i+1]=='&') || (s[i]=='|' && s[i+1]=='|') || (s[i]=='>' && s[i+1]=='>')){
            s1[0]=s[i];
            s1[1]=s[i+1];
            s1[2]='\0';
            i+=2;
            (*mas)[j]=s1;
            j++;
            s1=malloc(sizeof(char)*NEXT);

        }
        else if(s[i]=='&' || s[i]=='|' || s[i]=='>' || s[i]=='<' || s[i]==';' || s[i]=='(' || s[i]==')'){
            s1[0]=s[i];
            s1[1]='\0';
            (*mas)[j]=s1;
            i++;
            j++;
            s1=malloc(sizeof(char)*NEXT);
        }

        else{
            while(s[i]!=' ' && s[i]!='|'  && s[i]!='&' && s[i]!='<' && s[i]!='\0' && s[i]!=';' && s[i]!='(' && s[i]!=')' && s[i]!='>' ){
                if (s[i]=='"'){
                    i++;
                    while(s[i]!='"' && s[i]!= '\0'){
                        if (p<k1-1){
                            s1[p]=s[i];
                            p++;
                            i++;
                        }
                        else{
                            k1+=NEXT;
                            s1=realloc(s1,k1*sizeof(char));
                            s1[p]=s[i];
                            i++;
                            p++;
                        }
                    }
                    if (s[i]!='\0')
                        i++;
                    else{
                        fl=0;
                        break;
                    }
                }
                else{
                    if (p<k1-1){
                            s1[p]=s[i];
                            p++;
                            i++;
                    }
                    else{
                        k1+=NEXT;
                        s1=realloc(s1,k1*sizeof(char));
                        s1[p]=s[i];
                        i++;
                        p++;
                    }
                }
            }
            if(p!=0){
                s1[p]='\0';
                (*mas)[j]=s1;
                j++;
                s1=malloc(sizeof(char)*NEXT);
                p=0;
                k1=NEXT;

            }
        }


    }
    if (fl)
        *nlen=j;

    else{

        *nlen=0;
        for(int i=0;i<j;i++)
            free((*mas)[i]);
        free(*mas);
        *mas=NULL;
    }
    free(s1);
    free(s);
    return 1;

}
void printwords(char **s,int len){
    int i=0;
    for(i=0;i<len;i++)
        printf("%s \n",s[i]);
}
void com(char **s1,int beg,int end,int pal,int *stat,pid_t *pid1){
    char **s2=NULL;
    int status;
    pid_t pid;
    int i=0,j=end,k=0,p=0,fd1,fd[2];
    i=beg;
    while(i<end){
        if (i<end &&( strcmp(s1[i],">")==0)){
            if (i+1>=end){
               fprintf(stderr,"ERROR: не указан файл\n");
               exit(1);
            }
            fd1=open(s1[i+1],O_WRONLY|O_TRUNC|O_CREAT,0666);
            if (fd1==-1){
                fprintf(stderr,"ERROR: не удалось открыть файл\n");
                exit(1);
            }
            dup2(fd1,1);
            close(fd1);
            i+=2;
        }
        else if (i<end &&(strcmp(s1[i],">>")==0 )){
            if (i+1>=end){
               fprintf(stderr,"ERROR: не указан файл\n");
               exit(1);
            }
            fd1=open(s1[i+1],O_WRONLY|O_APPEND|O_CREAT,0666);
            if (fd1==-1){
                fprintf(stderr,"ERROR: не удалось открыть файл\n");
                exit(1);
            }
            dup2(fd1,1);
            close(fd1);
            i+=2;
        }
        else if (i<end &&(strcmp(s1[i],"<")==0)){
            if (i+1>=end){
               fprintf(stderr,"ERROR: не указан файл\n");
               exit(1);
            }
            fd1=open(s1[i+1],O_RDONLY,0666);
            if (fd1==-1){
                fprintf(stderr,"ERROR: не удалось открыть файл\n");
                exit(1);
            }
            dup2(fd1,0);
            close(fd1);
            i+=2;
        }
        else
            break;
    }
    j=end-1;
    while(j>=beg){
        if (j>=beg &&(strcmp(s1[j],">")==0)){
            if (j+1>=end){
               fprintf(stderr,"ERROR: не указан файл\n");
               exit(1);
            }
            fd1=open(s1[j+1],O_WRONLY|O_TRUNC|O_CREAT,0666);
            if (fd1==-1){
                fprintf(stderr,"ERROR: не удалось открыть файл\n");
                exit(1);
            }
            dup2(fd1,1);
            close(fd1);
            j--;
        }
        else if (j>=beg &&(strcmp(s1[j],">>")==0 )){
            if (j+1>=end){
               fprintf(stderr,"ERROR: не указан файл\n");
               exit(1);
            }
            fd1=open(s1[j+1],O_WRONLY|O_APPEND|O_CREAT,0666);
            if (fd1==-1){
                fprintf(stderr,"ERROR: не удалось открыть файл\n");
                exit(1);
            }
            dup2(fd1,1);
            close(fd1);
            j--;
        }
        else if (j>=beg &&(strcmp(s1[j],"<")==0)){
            if (j+1>=end){
               fprintf(stderr,"ERROR: не указан файл\n");
               exit(1);
            }
            fd1=open(s1[j+1],O_RDONLY,0666);
            if (fd1==-1){
                fprintf(stderr,"ERROR: не удалось открыть файл\n");
                exit(1);
            }
            dup2(fd1,0);
            close(fd1);
            j--;
        }
        else if(j>beg&& ((strcmp(s1[j-1],">>")==0 || strcmp(s1[j-1],">")==0)||strcmp(s1[j-1],"<")==0))
            j--;
       else
           break;
    }
    if(i==end){
        fprintf(stderr,"ERROR: нет файла\n");
        exit(1);

    }
    fd1=dup(0);
    s2=malloc((end-beg+1)*sizeof(char*));
    s2[0]=NULL;
    while(k<=pal){
        p=0;

        while(i<end&&strcmp(s1[i],"|")!=0){
            if(k<pal&&(strcmp(s1[i],"<")==0||strcmp(s1[i],">")==0||strcmp(s1[i],">>")==0)){
                fprintf(stderr,"ERROR: команды перенаправления не могут стоять внутри конвейера!\n");
                exit(1);
            }
            if(k==pal&&(strcmp(s1[i],"<")==0||strcmp(s1[i],">")==0||strcmp(s1[i],">>")==0))
                break;
            s2[p]=s1[i];
            i++;
            p++;
        }
        s2[p]=NULL;
        pipe(fd);
        if((pid=fork())==0){
            if (k<pal)
                dup2(fd[1],1);
            close(fd[0]);
            close(fd[1]);
            close(fd1);
            execvp(s2[0],s2);
            perror("ERROR: не удалось найти файл ");
            exit(1);
        }
        dup2(fd[0],0);
        k++;
        i++;
        close(fd[0]);
        close(fd[1]);
    }
    dup2(fd1,0);
    close(fd1);
    free(s2);
    waitpid(pid,&status,0);
    while(wait(NULL)!=-1);
    *stat=status;
    *pid1=pid;
}
int main(){
    signal(SIGINT,SIG_IGN);
    signal(SIGUSR1,SIG_IGN);
    char **s1=NULL;
    int len=0;
    int status;
    int i=0,fd1=0,pal=0,beg=0;
    pid_t pid,pid1;
    while(words(&s1,&len)){
        if(s1!=NULL){

            if (len>0 && strcmp(s1[0],"cd")!=0){

                beg=0;
                pal=0;
                i=0;
                while(i<len){

                    if(strcmp(s1[i],"&")==0 ){
                        if (i-beg>0){
                            if ((fork())==0){
                                signal(SIGINT,SIG_IGN);
                                signal(SIGUSR1,SIG_DFL);
                                fd1=open("/dev/null",O_RDONLY,0666);
                                dup2(fd1,0);
                                close(fd1);
                                pal=0;
                                for(int j=beg;j<i;j++){
                                    if(strcmp(s1[j],"|")==0)
                                        pal++;
                                    if(strcmp(s1[j],"&&")==0 ){
                                        if(j==beg){
                                            printf("ERROR:пустая команда\n");
                                            exit(1);
                                        }

                                        com(s1,beg,j,pal,&status,&pid1);
                                        pal=0;
                                        beg=j+1;
                                        if (WIFEXITED(status)==0 || WEXITSTATUS(status)!=0)
                                            break;

                                    }
                                    if(strcmp(s1[j],"||")==0 ){
                                        if(j==beg){
                                            printf("ERROR:пустая команда\n");
                                            exit(1);
                                        }

                                        com(s1,beg,j,pal,&status,&pid);
                                        pal=0;
                                        beg=j+1;
                                        if (WIFEXITED(status)!=0 && WEXITSTATUS(status)==0)
                                            break;

                                    }
                                    if(j==i-1){
                                        if(j+1==beg){
                                            printf("ERROR:пустая команда\n");
                                            exit(1);
                                        }
                                        com(s1,beg,j+1,pal,&status,&pid);
                                    }
                                }
                                //com(s1,beg,i,pal,&status,&pid);
                                if (WIFEXITED(status))
                                    printf("Process %d exited with code %d \n",pid,(WEXITSTATUS(status)));
                                else
                                    fprintf(stderr,"Process %d aborted by sygnal %d \n",pid,(WTERMSIG(status)));

                                exit(0);
                            }
                        }
                        else{
                            fprintf(stderr,"ERROR: перед символом & ничего нет \n");
                            break;
                        }
                        pal=0;
                        beg=i+1;
                    }
                    else if(strcmp(s1[i],";")==0 || i==len-1){
                        if(i==len-1)
                            i++;
                        if (i-beg>0){
                            if ((pid=fork())==0){
                                signal(SIGINT,SIG_DFL);
                                pal=0;
                                for(int j=beg;j<i;j++){
                                    if(strcmp(s1[j],"|")==0)
                                        pal++;
                                    if(strcmp(s1[j],"&&")==0 ){
                                        if(j==beg){
                                            printf("ERROR:пустая команда\n");
                                            exit(1);
                                        }

                                        com(s1,beg,j,pal,&status,&pid1);
                                        pal=0;
                                        beg=j+1;
                                        if (WIFEXITED(status)==0 || WEXITSTATUS(status)!=0)
                                            break;

                                    }
                                    if(strcmp(s1[j],"||")==0 ){
                                        if(j==beg){
                                            printf("ERROR:пустая команда\n");
                                            exit(1);
                                        }

                                        com(s1,beg,j,pal,&status,&pid1);
                                        pal=0;
                                        beg=j+1;
                                        if (WIFEXITED(status)!=0 && WEXITSTATUS(status)==0)
                                            break;

                                    }
                                    if(j==i-1){
                                        if(j+1==beg){
                                            printf("ERROR:пустая команда\n");
                                            exit(1);
                                        }
                                        com(s1,beg,j+1,pal,&status,&pid1);
                                    }
                                }
                                //com(s1,beg,i,pal,&status,&pid);
                                if (WIFEXITED(status)==0)
                                    fprintf(stderr,"ERROR: программа завершилась с ошибкой \n");
                                exit(0);
                            }
                            waitpid(pid,NULL,0);
                        }

                        else{
                            if(i==len){
                                fprintf(stderr,"ERROR: пустая команда \n");
                                break;
                            }

                            fprintf(stderr,"ERROR: перед символом ; ничего нет \n");
                            break;
                        }
                        pal=0;
                        beg=i+1;

                           // if (WIFEXITED(status)==0)
                             //   fprintf(stderr,"ERROR: программа завершилась с ошибкой \n");
                        //exit(0);
                    }
                i++;
                }
            }
            else if (len>0){
                if (len<2)
                    chdir((getenv("HOME")));
                else
                    if(chdir(s1[1])!=0)
                        fprintf(stderr,"ERROR: нет такой директории \n");
            }
            for(int i=0;i<len;i++)
                free(s1[i]);
            free(s1);
            pal=0;
            i=0;
            while((pid=waitpid(-1,&status,WNOHANG))>0){
                if (WIFEXITED(status))
                    fprintf(stderr,"Process %d exited with code %d \n",pid,(WEXITSTATUS(status)));
                else
                    fprintf(stderr,"Process %d aborted by sygnal %d \n",pid,(WTERMSIG(status)));

            }

        }
        else
            fprintf(stderr,"ERROR \n");
    }
    kill(0,SIGUSR1);
}



