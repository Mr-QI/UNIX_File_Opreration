#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void CreateDir(char *dirname);                      //创建文件夹
void CreateSourceFile(char* dirname);               //创建source文件夹下的文件
void CreateContentDir(char* dirname);               //创建content目录及其子目录
char* GetRandomString(int length,int mode);         //产生随机字符串，mode =1,表示小写字母，mode=2,表示大写字母,mode=3表示数字
char* ReadFileList(char *basepath,char* despath);                 //读取某文件夹及其子文件夹下所有文件名：
char* CpSrcFileToDes(char *srcpath,char* despath,char *filename);  //复制文件到指定目录
void DelDir(char* dirname);                      //删除sources/中所有文件及sources/文件夹的操作

//char *sourcedirname;

void CreateDir(char *dirname)
{
    //查看文件是否存在
    if(access(dirname,0) == -1)
    {
        //如果文件不存在，则创建
        if(mkdir(dirname,0777));
        {
            printf("Creat %s successful !\n",dirname);
        }
        return;
    }
    printf("%s dirctory has exist !\n",dirname);
}

char* GetRandomString(int length,int mode)
{
    char* string;
    length += 1;
    //通过时间函数设置随机数种子，使得每次运行结果随机,只需在主函数里面调用一次就行
    //srand((unsigned)time(NULL));
    if((string = (char*)malloc(sizeof(char)*length)) == NULL)
    {
        printf("malloc failed !\n");
        return NULL;
    }
    for(int i = 0; i < length ; i++)
    {
        switch(mode)
        {
            case 1: string[i] = 'a' + rand() % 26; break;
            case 2: string[i] = 'A' + rand() % 26; break;
            case 3: string[i] =     rand() % 10;   break;
            default:string[i] = 'x'+ rand() % 26; break;
        }
    }
    string[length - 1] = '\0';
    return string;
}


void CreateSourceFile(char* dirname)
{
    int fd;
    char pwd[255];      //保存当前路径
    char filename[10];  //文件名
    char content[150];  //文件内容
    memset(pwd,'\0',sizeof(pwd));
    //保存当前路径，用于之后返回
    if(!getcwd(pwd,255))
    {
        exit(0);
    }
    CreateDir(dirname);
    if(chdir(dirname) != -1)
    {
        //创建文件
        for(int i = 1; i <= 1000; i++)
        {
            //接收文件名,创建文件
            memset(filename,'\0',sizeof(filename));
            strcpy(filename,GetRandomString(5,2));
            fd = open(filename,O_WRONLY|O_CREAT);
            if (fd == -1)
            {
                perror("open error...\n");
                exit(-1);
            }
            //向文件中写入内容
            memset(content,'\0',sizeof(content));
            strcpy(content,GetRandomString(100,1));
            write(fd,content,sizeof(content));
            printf("%s create successful !\n",filename);
            close(fd);
        }        
    }
    else
    {
        printf("Can not find dirctory !\n");
    }
    //返回原路径
    chdir(pwd);
}

//创建content目录及其子目录
void CreateContentDir(char* content)
{
    char capdirname[2];   //大写字母文件夹
    char lowdirname[2];   //小写字母文件夹
    char pwd[255];        //当前文件夹路径
    char cap_pwd[255];    //大写字母文件夹路径
    memset(pwd,'\0',sizeof(pwd));
    CreateDir(content);
    if(!getcwd(pwd,255))
    {
        exit(0);
    }
    //进入content目录
    if(chdir(content) != -1)
    {
        for(int i = 0; i < 26; i++)
        {
            //创建文件名为A-Z的目录
            memset(capdirname,'\0',sizeof(capdirname));
            capdirname[0] = 'A'+i;
            getcwd(cap_pwd,255);
            CreateDir(capdirname);
            //进入文件名为A-Z的目录
            if(chdir(capdirname) != -1)
            { 
                for(int j = 0;j < 26; j++)
                {
                    //创建文件名为a-z的子目录
                    memset(lowdirname,'\0',sizeof(lowdirname));
                    lowdirname[0] = 'a' + j;
                    CreateDir(lowdirname);
                }
            }
            else
            {
                exit(0);
            }
            chdir(cap_pwd);
        }
    }
    chdir(pwd);
}

//复制文件到指定目录
void CopyFile(char *src_path,char *des_path)
{
    char buff[1024];
    int len;
    int fd1,fd2;

    fd1 = open(src_path,O_RDONLY);
    fd2 = open(des_path,O_WRONLY|O_CREAT);
    read(fd1,buff,sizeof(buff));
    write(fd2,buff,len);
    printf("\"%s\" copy to \"%s\" successful !\n",src_path,des_path);

    close(fd1);
    close(fd2);
}

//对于/sources/中的每一个文件，如果文件名以X开头，内容以Y开头，则将该文件复制到/content/X/Y/目录下。
//srcpath:源文件路径
//filename:要创建的文件名
char* CpSrcFileToDes(char *srcpath,char* despath,char *filename)
{
    int fd,n;
    char buf[150];
    char temp1[50],temp2[50];
    memset(buf,'\0',sizeof(buf));
    //获取原路径文件名
    memset(temp1,'\0',sizeof(temp1));
    //获取目的路径文件名
    memset(temp2,'\0',sizeof(temp2));

    strcpy(temp1,srcpath);
    strcat(temp1,"/");
    strcat(temp1,filename);

    if((fd = open(temp1,O_RDONLY)) == -1) 
    {
        perror("CpSrcFileToDes open file error...\n");
        exit(0);
    }
    else if((n = read(fd,buf,sizeof(buf))) < 0)
    {
        perror("CpSrcFileToDes read file error...\n");
        exit(0);
    }
    else 
    {
        //拼接目的路径
        strcpy(temp2,despath);
        strcat(temp2,"/");
        temp2[strlen(temp2)] = filename[0]; 
        strcat(temp2,"/");
        temp2[strlen(temp2)] = buf[0];
        strcat(temp2,"/");
        strcat(temp2,filename);
        CopyFile(temp1,temp2);
    }
    close(fd);

}

//读取某文件夹及其子文件夹下所有文件名并且复制到指定目录
//basepath = "source"
char* ReadFileList(char *basepath,char* despath)
{
    DIR   *dir;
    struct  dirent *ptr;
    dir = opendir(basepath);
    if(NULL == dir)
    {
        printf("ReadFilelist opendir error...\n");
        exit(0);
    }
    while( (ptr = readdir(dir))!=NULL)
    {
        if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)
            continue;
        else if(ptr->d_type == 8)   //文件
        {
            CpSrcFileToDes(basepath,despath,ptr->d_name);
            //return ptr->d_name;
        }
  
    }
    closedir(dir);
}

//打印其路径、文件名及内容；将文件名更改为长度为8的随机数字，并打印新名称；
//查找文件名包含字符X（如’A’）的文件
void PrintPathEtc(char *path,char ch)
{
    DIR *dir;
    char *p;
    unsigned int mask = 0000777;    //用来打印文件权限
    struct dirent *ptr;
    struct stat filestat;           //查看文件信息
    char base[255],buf[150],temp[50];
    char changfna[10];
    int fd;

    dir = opendir(path);
    if(NULL == dir)
    {
        perror("PrintPathEtc opendir error...\n");
        exit(0);
    }
    while( (ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)
            continue;
        else if(ptr->d_type == 8)   //文件
        {
            if((p=strchr(ptr->d_name,ch)) != NULL)
            {
                memset(changfna,'\0',sizeof(changfna));
                memset(buf,'\0',sizeof(buf));
                memset(temp,'\0',sizeof(temp));
                strcpy(changfna,GetRandomString(8,3));
                strcat(temp,path);
                strcat(temp,"/");
                strcat(temp,ptr->d_name);
                //此时temp是具有完整文件名路径名的字符串
                if((fd = open(temp,O_RDONLY)) < 0)
                {
                    perror("PrintPathEtc file open error...\n");
                    exit(0);
                }
                else if((stat(temp,&filestat)) < 0)
                {
                    perror("stat error ...\n");
                    exit(0);
                }
                else if((read(fd,buf,sizeof(buf))) < 0)
                {
                    perror("read error ...\n");
                    exit(0);
                }
                printf("The Path : %s\n",path);
                printf("The Name : %s\n",ptr->d_name);
                printf("The content: %s\n",buf);
                printf("The New File Name: %s\n",changfna);
                printf("The uid: %d\n",filestat.st_uid);
                printf("The gid: %d\n",filestat.st_gid);
                if((chown(temp,filestat.st_uid+1,filestat.st_gid+1)) < 0)
                {
                    perror("chown error ...\n");
                    exit(0);
                }
                printf("The new uid: %d\n",filestat.st_uid);
                printf("The new gid: %d\n",filestat.st_gid);
                printf("The mode: %o\n",mask & filestat.st_mode);
                if((chmod(temp,S_IRGRP|S_IWGRP|S_IXGRP)) < 0)
                {
                    perror("chmod error...\n");
                }
                printf("The new mode: %o\n",mask & filestat.st_mode);
                printf("\n");
                close(fd);
            }   
            continue;
        }
        else if(ptr->d_type == 4)
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,path);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            PrintPathEtc(base,ch);
        }
    }
    closedir(dir);
}

//根据条件在Content目录下检索文件
void SearchContentFile(char *path)
{
    char ch;
    char pwd[255];
    memset(pwd,'\0',sizeof(pwd));
    if(!getcwd(pwd,255))
    {
        exit(0);
    }
    if(chdir(path) != -1)
    {
        printf("Please input character to find file name \n");
        scanf("%c",&ch);
        switch(ch)
        {
            //case 'A':
        }
    }

    chdir(pwd);
}

//以实现删除sources/中所有文件及sources/文件夹的操作
void DelDir(char* dirname)
{
    DIR *dir;
    char path[20];
    struct dirent *ptr;
    if((dir = opendir(dirname)) == NULL)
    {
        perror("DelSouce opendir failed...\n");
        exit(0);
    }
    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)
            continue;
        if(ptr->d_type == 8)
        {
            memset(path,'\0',sizeof(path));
            strcat(path,dirname);
            strcat(path,"/");
            strcat(path,ptr->d_name);
            if((remove(path)) < 0)
            {
                perror("remove error...\n");
                exit(0);
            }
            printf("%s remove successful !\n",ptr->d_name);
        }
    }
    closedir(dir);
    if((rmdir(dirname)) < 0)
        perror("rmdir error ...\n");
    printf("%s delete successful !\n",dirname);
}

void MainMenu()
{
    printf("---------------------------------------------------------\n");
    printf("|                      主菜单                             \n");
    printf("|               (请先执行1,2后在执行其他操作)                \n");
    printf("|               1:创建source文件夹                         \n");
    printf("|               2:创建content文件夹                        \n");
    printf("|               3:复制source文件到content目录               \n");
    printf("|               4:在content/目录下实现检索等操作             \n");
    printf("|               5:删除sources/中所有文件及该文件夹           \n");
    printf("---------------------------------------------------------\n");
}

int main()
{
    srand((unsigned)time(NULL));
    int num;
    MainMenu();
    while((scanf("%d",&num)) > 0)
    {
        MainMenu();
        switch(num)
        {
            case 1:CreateSourceFile("source");MainMenu();break;
            case 2:CreateContentDir("content");MainMenu();break;
            case 3:ReadFileList("source","content");MainMenu();break;
            case 4:PrintPathEtc("content",'A');MainMenu();break;
            case 5:DelDir("source");MainMenu();break;
            default:continue;break;
        }
    }
    return 0;
}
