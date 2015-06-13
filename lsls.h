#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int argcnt;
char ** dir_name_list=NULL;


typedef struct lscmd
{
    int A;                    //   -A   default don.t list . ..
    int a;                    //   -a   list all
    int c;                    //   -c   last changed
    int d;                    //   -d   plain
    int F;                    //   -F   list "info->type"
    int f;                    //   -f   no sort
    int h;                    //   -h   using KB
    int k;                    //   -k   using B default
    int l;                    //   -l   long format
    int i;                    //   -i   print inode
    int n;                    //   -n   numeric grp,owner
    int r;                    //   -r   reverse order
    int S;                    //   -S   size
    int t;                    //   -t   modify
    int s;                    //   -s   print block_cnt
    int x;                    //   -x   //force print line fisrt
    int u;                    //   -u   access
    int R;                    //   -R   //
    int one;                  //   -1   one thing perline
    int show_format;
} lscmd;



typedef struct lsinfo
{
    int link_num; //
    long owner_id;
    long grp_id;
    long size_byte;
    long blk_cnt;
    long blk_sz;
    char* owner_name;
    char* grp_name;
    char mode[11];
    char * name;
    char * type;
    long inode;
    struct timespec mtime;//modify
    struct timespec ctime;//change
    struct timespec atime;//access
} lsinfo;

lscmd cmd;
lsinfo *** dir_file_list;

lscmd make_command(int argc,const char  *argv[]);//make commands
lsinfo* make_info(char * file_name, struct stat * info_p);//make lsinfo
void make_list(int argc,const char  *argv[]);//make lists
void show_normal();
void show_list();
void insert_element(lsinfo ** dir_file_list,lsinfo * info_p,int len);
void show_l_file(lsinfo * info);
void show_file(lsinfo * info);
void find_dir(char * dir_name);

//analyse the auth of mode
void mode_to_letters(int mode , lsinfo *info)
{
    //S_IS***测试宏
    strcpy(info->mode,"----------");
    if (info->mode[0]=='-')  info->type="";
    //与 掩码
    if(mode&S_IRUSR)info->mode[1]  = 'r';
    if(mode&S_IWUSR)info->mode[2]  = 'w';
    if(mode&S_IXUSR)
    {
        info->mode[3]  = 'x';
        info->type = "*";
    }
    if(mode&S_IRGRP)info->mode[4] = 'r';
    if(mode&S_IWGRP)info->mode[5] = 'w';
    if(mode&S_IXGRP)
    {
        info->mode[6]  = 'x';
        info->type = "*";
    }

    if(mode&S_IROTH)info->mode[7] = 'r';
    if(mode&S_IWOTH)info->mode[8] = 'w';
    if(mode&S_IXOTH)
    {
        info->mode[9]  = 'x';
        info->type = "*";
    }

    switch (mode&S_IFMT)
    {
    case S_IFREG:
        info->mode[0] = '-';
        break;
    case S_IFBLK:
        info->mode[0] = 'b';
        break;
    case S_IFCHR:
        info->mode[0] = 'c';
        break;
    case S_IFDIR:
        info->mode[0] = 'd';
        info->type = "/";
        break;
    case S_IFLNK:
        info->mode[0] = 'l';
        info->type = "@";
        break;
    case S_IFIFO:
        info->mode[0] = 'p';
        info->type = "|";
        break;
    case S_IFSOCK:
        info->mode[0] = 's';
        info->type = "=";
        break;
    default:
        break;
    }


}
//通过uid找用户
char *uid_to_name(uid_t uid)
{
    struct passwd *getpwuid(),*pw_ptr;
    static char numstr[10];
    if((pw_ptr = getpwuid(uid)) == NULL)
    {
        sprintf(numstr,"%d",uid);
        return numstr;
    }
    else
        return pw_ptr->pw_name;
}
//通过gid找组名
char *gid_to_name(gid_t gid)
{
    struct group *getgrgid(),*grp_ptr;
    static char numstr[10];
    if((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr,"%d",gid);
        return numstr;
    }
    else
        return grp_ptr->gr_name;
}

void find_dir(char * dir_name)
{
    DIR *dir_ptr;
    struct dirent *dirent_p;
    if((dir_ptr = opendir(dir_name)) == NULL) return ;
    else
    {
        argcnt++;
        dir_name_list=(char**)realloc(dir_name_list,argcnt*sizeof(char*));
        dir_name_list[argcnt-1]=(char*)malloc(1000*sizeof (char));
        strcpy(dir_name_list[argcnt-1],dir_name);
        while((dirent_p = readdir(dir_ptr)) != NULL)
        {
            char * file_name=dirent_p->d_name;
            //for "-A and -a" list all or not
            if (cmd.A&&cmd.a==0&&file_name[0]=='.')
                continue;

            //change to definate path name
            char definite_name[1000];

            if(strlen(dir_name) > 0)
            {
                strcpy(definite_name, dir_name);
                strcat(definite_name, "/");
                strcat(definite_name, file_name);
            }
            else
            {
                strcpy(definite_name, file_name);
            }
            find_dir(definite_name);
        }
        //关闭目录流
        closedir(dir_ptr);
    }

}
