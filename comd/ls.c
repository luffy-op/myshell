#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

int main(int argc, char *argv[])
{
    myls(argc, argv);
    return 0;
}

void do_ls(char dirname[])
{
    DIR * path = NULL;
    struct dirent *direntp;

    if((path = opendir(dirname)) == NULL)
    {
        fprintf(stderr, "myls error: cannot open %s\n", dirname);
    }
    else
    {
        printf("\033[1;36m%s\033[0m:\n", dirname);

        while((direntp = readdir(path)) != NULL)
        {
            if(strcmp(direntp->d_name, ".")==0 ||
                    strcmp(direntp->d_name, "..")==0)
                continue;
            printf("%s\n", direntp->d_name);
        }
        closedir(path);
    }
}

char* get_mode(mode_t m,char* str)//��ʾ�ļ���дȨ�޺���
{
    if(S_ISREG(m))
        strcpy(str,"-");
    else if(S_ISDIR(m))
        strcpy(str,"d");
    else if(S_ISCHR(m))
        strcpy(str,"c");
    else if(S_ISBLK(m))
        strcpy(str,"b");
    else if(S_ISFIFO(m))
        strcpy(str,"f");
    else if(S_ISLNK(m))
        strcpy(str,"l");
    else if(S_ISSOCK(m))
        strcpy(str,"n");

    // ����Ȩ��
    strcat(str,m&S_IRUSR?"r":"-");
    strcat(str,m&S_IWUSR?"w":"-");
    strcat(str,m&S_IXUSR?"x":"-");

    // ͬ��Ȩ��
    strcat(str,m&S_IRGRP?"r":"-");
    strcat(str,m&S_IWGRP?"w":"-");
    strcat(str,m&S_IXGRP?"x":"-");

    // ����Ȩ��
    strcat(str,m&S_IROTH?"r":"-");
    strcat(str,m&S_IWOTH?"w":"-");
    strcat(str,m&S_IXOTH?"x":"-");

    return str;
}
int _time(int year)//�����Ƿ������꺯��
{
    if(year%4==0 && year%100 !=0 || year%400 == 0)
        return 29;
    return 28;
}
void time_ch(time_t num)//ͨ����������������
{
    int year=1970;
    int month =1;
    int day =1;
    num = num + 8*3600;
    while(num >= 86400)
    {
        num-=86400;
        day++;
        if(month==1 && day == 32)
        {
            month++;
            day =1;
        }
        else if(month == 2 && day ==_time(year)+1)
        {
            month++;
            day =1;
        }
        else if(month == 3 && day == 32)
        {
            month++;
            day =1;
        }
        else if(month == 4 && day == 31)
        {
            month++;
            day=1;
        }
        else if(month == 5 && day == 32)
        {
            month++;
            day=1;
        }
        else if(month == 6 && day == 31)
        {
            month++;
            day=1;
        }
        else if(month == 7 && day == 32)
        {
            month++;
            day=1;
        }
        else if(month == 8 && day == 32)
        {
            month++;
            day=1;
        }
        else if(month == 9 && day == 31)
        {
            month++;
            day=1;
        }
        else if(month == 10 && day == 32)
        {
            month++;
            day=1;
        }
        else if(month == 11 && day == 31)
        {
            month++;
            day=1;
        }
        else if(month == 12 && day == 32)
        {
            month=1;
            day=1;
            year++;
        }

    }
    int hour = num/3600;
    int minute =num/60 -hour*60;
    printf("%2dm %2dd %2d:%2d ",month,day,hour,minute);
}

void do_ls_l(int argc, char *argv[])
{
    for(int i=1; i<argc; i++)
    {
        char* dir_name=NULL;
        if(argc == 2)
        {
            dir_name=".";
        }
        else if(argc >= 3)
        {
            if(i==1) continue;
            dir_name = argv[i];
        }
        else
        {
            puts("user:ls dir");
            return -1;
        }

        printf("\033[1;36m%s\033[0m:\n", dir_name);

        DIR* dp=opendir(dir_name);
        if(NULL == dp)
        {
            perror("opendir");
            return -1;
        }
        struct dirent* de=readdir(dp);
        for(; de; de=readdir(dp))
        {
            if('.'==de->d_name[0]) continue;
            //ͨ���ļ�������ļ���Ϣ
            struct stat s;
            int ret = lstat(de->d_name,&s);
            if(0 > ret)
            {
                perror("stat");
                return -1;
            }

            char str[11] = {};
            printf("%s ",get_mode(s.st_mode,str));//����
            struct passwd *passwd;
            passwd = getpwuid(s.st_uid);
            printf ("%s ", passwd->pw_name);//����
            struct group *group;
            group = getgrgid(passwd->pw_gid);
            printf ("%s ", group->gr_name); //����
            printf("%5lu ",s.st_size);//��С
            time_ch(s.st_mtime);//ʱ��
            printf("%s\t",de->d_name);//�ļ���
            printf("\n");
        }
        printf("\n");
        closedir(dp);
    }

}

void myls(int argc, char *argv[])
{
    if(argc == 1)
    {
        do_ls(".");
    }
    else if(strcmp(argv[1], "-l")==0)
    {
        do_ls_l(argc, argv);

    }
    else
    {
        for( int i = 1; i < argc; i++)
        {
            //printf("\033[1;36m%s:\033[0m\n", argv[i]);
            do_ls(argv[i]);
        }
    }
}
