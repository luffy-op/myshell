#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>

#define MAX_CMD  1024
#define MAX_PATH 255

char buff[MAX_CMD];
//命令行的缓冲区大小1024个字节


//从终端获取数据，同时需要对终端进行模拟
int get_cmd()
{
    char buff_path[MAX_PATH];
    getcwd(buff_path, MAX_PATH);
    printf("[\033[1;33mmyShell@localhost\033[0m]:\033[1;36m%s\033[0m$ ", buff_path);

    memset(buff, 0x00, MAX_CMD);
    fflush(stdout);
    //模拟终端显示

    fgets(buff, MAX_CMD - 1, stdin);
    //输入
    buff[strlen(buff) - 1] = '\0';
    //最后一位是\0
    return 0;
}

//解析输入的命令
//我们需要将输入的字符串中的所有命令都保存在字符串数组argv中，以NULL结尾
char **do_parse(char *buff)
{
    char *ptr = buff;
    int argc = 0;
    static char *argv[32];//字符串数组长度32
    //将所有提取出来的命令保存进argv中，因为在程序整个生命域中都要使用，       声明为静态

    while('\0' != *ptr)
    {
        if(' ' != *ptr)
        {
            argv[argc++] = ptr;
            while('\0' != *ptr && ' ' != *ptr)
            {
                ptr++;
            }
            *ptr = '\0';
        }
        ptr++;
    }
    //去掉空格，提取命令

    argv[argc] = NULL;
    //最后一位必须是NULL
    return argv;
}

//输出重定向
//这里实现的主要是输出重定向中的>和>>
void do_redirect(char *buff)
{
    int redirect_flag = 0;
    //记录>出现的次数
    char *redirect_file = NULL;
    char *ptr = buff;

    while(*ptr != '\0')
    {
        if(*ptr == '>')
        {
            *ptr++ = '\0';
            ++redirect_flag;
            //如果这一位是>,则计数加一，并将这一位改写为\0,防止被当成命令解析

            if(*ptr == '>')
            {
                ++redirect_flag;
                ptr++;
            }

            while(*ptr == ' ' && *ptr != '\0')
            {
                ptr++;
            }

            redirect_file = ptr;
            //检测完>>之后,后面的就是重定向的文件名,将其解析出来

            while(*ptr != ' ' && *ptr != '\0')
            {
                ptr++;
            }
            *ptr = '\0';
        }
        ptr++;
    }

    if(redirect_flag == 1)
    {
        int fd = open(redirect_file, O_WRONLY|O_CREAT|O_TRUNC, 0664);
        dup2(fd, 1);
        //将标准输出重定向到文件
        //如果只出现了一个>，则说明是覆盖重定向，以O_TRUNC覆盖模式打开
    }
    else if(redirect_flag == 2)
    {
        int fd = open(redirect_file, O_WRONLY|O_CREAT|O_APPEND, 0664);
        dup2(fd, 1);
        //>出现了两次，说明是追加重定向，以O_APPEND追加模式打开
    }
}

//内置函数
int build_in_command(char* argv[])
{
//pwd
    if(strcmp(argv[0], "pwd") == 0)
    {
        char buff_path[MAX_PATH];
        getcwd(buff_path, MAX_PATH);
        printf("%s\n",getcwd(buff_path,MAX_PATH));
        return 1;
    }
//cd
    else if(strcmp(argv[0], "cd") == 0)
    {
        if(chdir(argv[1]) == 0)
            return 1;
    }

    return 0;
}

//程序替换
int do_exec(char *buff)
{
    char **argv = { NULL };
    char path[] = "./comd/";

    int pid = fork();
    //创建子进程， 在子进程中进行程序替换
    if(0 == pid)
    {
        do_redirect(buff);
        //重定向
        argv = do_parse(buff);
        //解析命令
        //printf("%s", argv);

        if(NULL != argv[0])
        {
            int incmd = build_in_command(argv);
            if(incmd == 0)
            {

                if(execv(strcat(path, argv[0]), argv)<0)
                {
                    printf("comand not found\n");
                    exit(-1);
                }
                //替换进程

            }
        }
        else
        {
            exit(-1);
            //输入命令错误则退出进程
        }


    }
    else
    {
        waitpid(pid, NULL, 0);
        //等待子进程退出,防止僵尸进程
    }
    return 0;
}

int main(int argc, char*argv[])
{
    while(1)
    {
        //如果命令输入正确则启用程序替换。
        if(!get_cmd())
            //printf("%sa\n", buff);
            if(strcmp(buff, "exit")==0)
            {
                printf("bye~\n");
                exit(0);
            }
            else
                do_exec(buff);
    }

    return 0;
}
