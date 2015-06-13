#include "lsls.h"



int *dir_len;

int main(int argc,const char * argv[])
{
    cmd=make_command(argc,argv);
    make_list(argc,argv);
    if (cmd.d==0)
        if (cmd.l)
            show_list();
        else show_normal();
    return 0;
}


lscmd make_command(int argc,const char  *argv[])
{
    lscmd cmd= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    cmd.show_format=1;
    cmd.k=1;
    cmd.A=1;
    if (argc==1)
        return cmd;
    int cnt=1;
    while (cnt<argc)
    {
        if (argv[cnt][0]!='-')
        {
            cnt++;
            continue;
        }
        const char * option=argv[cnt];
        int len=strlen(option);
        printf("%d\n",len);
        int i=0;
        for (i=1; i<len; i++)
        {
            switch (option[i])
            {
            case 'a':
                cmd.a = 1;
                break;
            case 'l':
                cmd.l = 1;
                break;
            case 'c':
                cmd.c = 1;
                break;
            case 'd':
                cmd.d = 1;
                cmd.a = 1;
                break;
            case 'F':
                cmd.F = 1;
                break;
            case 'f':
                cmd.f = 1;
                break;
            case 'h':
                cmd.h = 1;
                //cover 'k'
                cmd.k = 0;
                break;
            case 'i':
                cmd.i = 1;
                break;
            case 'n':
                cmd.n = 1;
                break;
            case 'r':
                cmd.r = 1;
                break;
            case 'S':
                cmd.S = 1;
                break;
            case 't':
                cmd.t = 1;
                break;
            case '1':
                cmd.one = 1;
                break;
            case 's':
                cmd.s = 1;
                break;
            case 'k':
                //covered by 'h'
                if(cmd.h != 1)
                    cmd.k = 1;
                break;
            case 'x':
                cmd.x = 1;
                break;
            case 'u':
                cmd.u = 1;
                break;
            case 'R':
                cmd.R = 1;
                break;
            default:
                break;

            }
        }
        cnt++;
    }

    return cmd;

}

lsinfo* make_info(char * file_name, struct stat * info_p)
{
    lsinfo *info;
    info=(lsinfo*)malloc(sizeof(lsinfo));
    //char *uid_to_name(),*ctime(),*gid_to_name(),*filemode();
    info->inode=(long)info_p->st_ino;
//    if (cmd.i) printf("%ld ",info->inode); //for "-i"
    mode_to_letters(info_p->st_mode,info);
    //printf("%s ",info->mode);

    info->blk_cnt=(long)info_p->st_blocks;
    info->blk_sz=(long) info_p->st_blksize;
    //if (cmd.s) printf("%ld ",info->blk_cnt);//for "-s"
    info->link_num=(int)info_p->st_nlink;
    //printf("%4d ",info->link_num);

    info->owner_id=(long) info_p->st_uid;
    info->owner_name=uid_to_name(info_p->st_uid);
//    if (cmd.n) printf("%-8ld ",info->owner_id);//for "-n"
//    else printf("%-8s ",info->owner_name);

    info->grp_id=(long) info_p->st_gid;
    info->grp_name=gid_to_name(info_p->st_gid);
//    if (cmd.n) printf("%-8ld ",info->grp_id);//for "-n"
//    else printf("%-8s ",info->grp_name);

    info->size_byte=(long)info_p->st_size;
//    if (cmd.h&&info->size_byte>=1000)//for "-h"
//        printf("%4.1fK ",1.0*info->size_byte/1000);
//    else
//        printf("%4ld ",info->size_byte);
    info->mtime=info_p->st_mtim;
    info->ctime=info_p->st_ctim;
    info->atime=info_p->st_atim;
//    printf("%.12s",4+ctime(&info_p->st_mtime));//modify
//    printf("%.12s",4+ctime(&info_p->st_ctime));//change time for "-c"

    info->name=file_name;



    return info;
}
void insert_element(lsinfo ** dir_file_list,lsinfo * info_p,int len)
{
    int index=-1,i;
    if (len==0)
    {
        dir_file_list[0]=info_p;
        return ;
    }
    if (cmd.c)//changed time sort
        for (i=0; i<len; i++)
        {
            if (dir_file_list[i]->ctime.tv_sec<info_p->ctime.tv_sec)
            {
                index=i;
                break;
            }
        }

    else if (cmd.u)//access time
    {
        for (i=0; i<len; i++)
            if (dir_file_list[i]->atime.tv_sec<info_p->atime.tv_sec)
            {
                index=i;
                break;
            }
    }

    else if (cmd.t)//modify time
    {
        for (i=0; i<len; i++)
            if (dir_file_list[i]->mtime.tv_sec<info_p->mtime.tv_sec)
            {
                index=i;
                break;
            }
    }

    else if (cmd.S) //size
        for (i=0; i<len; i++)
        {
            if (dir_file_list[i]->size_byte < info_p->size_byte)
            {
                index=i;
                break;
            }
        }
    else//alpha
    {
        for (i=0; i<len; i++)
        {
            if (strcmp(dir_file_list[i]->name,info_p->name)>0)
            {
                index=i;
                break;
            }
        }

    }

    if (index==len||index==-1)
        dir_file_list[len]=info_p;
    else
    {
        int j=index;
        lsinfo * tmp1=dir_file_list[j];
        lsinfo * tmp2;
        dir_file_list[j]=info_p;
        for (; j<len; j++)
        {
            tmp2=dir_file_list[j+1];
            dir_file_list[j+1]=tmp1;
            tmp1=tmp2;
        }

    }


}
void make_list(int argc,const char  *argv[])
{
    DIR *dir_ptr;
    struct dirent *dirent_p;
    int i;
    argcnt=0;
    if (cmd.R&&cmd.d==0)
    {
        for (i=1; i<argc; i++)
            if (argv[i][0]!='-')
                find_dir(argv[i]);
        printf("!%d\n",argcnt);
        for (i=0; i<argcnt; i++)
            printf("%s\n",dir_name_list[i]);
        dir_file_list=(lsinfo***)malloc(argcnt*sizeof(lsinfo**));
        dir_len=(int*)malloc(argcnt*sizeof(int));
    }
    else
    {
        for (i=1; i<argc; i++)
            if (argv[i][0]!='-')
                argcnt++;
        //save dir_name
        if (argcnt==0)
        {
            dir_name_list=(char**)malloc(sizeof (char*));
            dir_file_list=(lsinfo***)malloc(sizeof(lsinfo**));
            dir_len=(int*)malloc(sizeof(int));
            dir_name_list[0]=".";
        }
        else
        {
            dir_name_list=(char**)malloc(argcnt*sizeof (char*));
            dir_file_list=(lsinfo***)malloc(argcnt*sizeof(lsinfo**));
            dir_len=(int*)malloc(argcnt*sizeof(int));
            int cnt=0;
            for (i=1; i<argc&&cnt<argcnt; i++)
                if (argv[i][0]!='-')
                    dir_name_list[cnt++]=argv[i];
        }
    }

    //for "-d"
    if (cmd.d)
    {
        struct stat info;
        if (argcnt==0)
        {
            dir_file_list[0]=(lsinfo**)malloc(sizeof(lsinfo*));
            dir_len[0]=1;
            if(stat(".",&info) == -1)
                perror(".");
            else
                dir_file_list[0][0]=make_info(".",&info);

            if (cmd.l)
                show_l_file(dir_file_list[0][0]);
            else
                show_file(dir_file_list[0][0]);
        }
        else
        {
            dir_file_list[0]=(lsinfo**)malloc(argcnt*sizeof(lsinfo*));
            dir_len[0]=argcnt;
            for (i=0; i<argcnt; i++)
            {
                if (i) dir_len[i]=0;
                char * filename=dir_name_list[i];
                if(stat(".",&info) == -1)
                    perror(".");
                else
                    insert_element(dir_file_list[0],make_info(filename,&info),i);
            }
            if (cmd.l)
                show_list();
            else
                show_normal();
        }

        return ;
    }
    //save dir files
    for (i=0; i<=(argcnt==0?0:argcnt-1); i++)
    {
        char * dir_name=dir_name_list[i];
        int filecnt =0;
        int cnt=0;
        if((dir_ptr = opendir(dir_name)) == NULL)
        {
            fprintf(stderr,"lsls:cannot open %s\n",dir_name);
            continue;
        }
        while((dirent_p = readdir(dir_ptr)) != NULL)
        {
            if (cmd.A&&cmd.a==0&&dirent_p->d_name[0]=='.')
                continue;
            filecnt++;
        }
        closedir(dir_ptr);
        dir_file_list[i]=(lsinfo**)malloc(filecnt*sizeof(lsinfo*));
        dir_len[i]=filecnt;
        dir_ptr = opendir(dir_name);
        while((dirent_p = readdir(dir_ptr)) != NULL)
            //打印结果
        {
            char * file_name=dirent_p->d_name;

            //for "-A and -a" list all or not
            if (cmd.A&&cmd.a==0&&file_name[0]=='.')
                continue;

            struct stat info;
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


            if(stat(definite_name,&info) == -1)
                perror(definite_name);
            else
                //分析stat结构体
                //dir_file_list[i][cnt++]=make_info(file_name,&info);
                insert_element(dir_file_list[i],make_info(file_name,&info),cnt++);
            //make_info(file_name,&info);
            //show_file(make_info(file_name,&info));
            //printf("%s\n",dirent_p->d_name);
        }
        //关闭目录流
        closedir(dir_ptr);

    }
}
//show long file info
void show_l_file(lsinfo* info)
{
    if (cmd.i) printf("%ld ",info->inode); //for "-i"

    printf("%s ",info->mode);

    if (cmd.s) printf("%ld ",info->blk_cnt);//for "-s"

    printf("%4d ",info->link_num);

    if (cmd.n) printf("%-8ld ",info->owner_id);//for "-n"
    else printf("%-8s ",info->owner_name);

    if (cmd.n) printf("%-8ld ",info->grp_id);//for "-n"
    else printf("%-8s ",info->grp_name);

    if (cmd.h&&info->size_byte>=1000)//for "-h"
        printf("%4.1fK ",1.0*info->size_byte/1000);
    else
        printf("%4ld ",info->size_byte);

//    if (cmd.c)
//        printf("%.12s",4+ctime(&info->ctime.tv_sec));//change time for "-c"
//    else if (cmd.u)
//        printf("%.12s",4+ctime(&info->atime.tv_sec));//change time for "-u"
//    else
//        printf("%.12s",4+ctime(&info->mtime.tv_sec));


    if (cmd.F)
        printf(" %s%s\n",info->name,info->type);//for "F"
    else
        printf(" %s\n",info->name);
}

//show short file info
void show_file(lsinfo* info)
{
    if (cmd.one)
    {
        if (cmd.i) printf("%ld ",info->inode); //for "-i"

        if (cmd.s) printf("%ld ",info->blk_cnt);//for "-s"

        if (cmd.F)
            printf(" %s%s\n",info->name,info->type);//for "F"
        else
            printf(" %s\n",info->name);
    }
    else
    {
        if (cmd.i) printf("%ld ",info->inode); //for "-i"

        if (cmd.s) printf("%ld ",info->blk_cnt);//for "-s"

        if (cmd.F)
            printf(" %s%s\n",info->name,info->type);//for "F"
        else
            printf(" %s\n",info->name);
    }
}
//
void show_normal()
{
    int i,j;
    if (cmd.r)
        for (i=0; i<=(argcnt==0?0:argcnt-1); i++)
        {
            if (argcnt!=0&&argcnt!=1)
                printf("%s:\n",dir_name_list[i]);
            for (j=dir_len[i]-1; j>=0; j--)
            {
                lsinfo* info=dir_file_list[i][j];
                show_file(info);
            }

        }
    else
        for (i=(argcnt==0?0:argcnt-1); i>=0; i--)
        {
            if (argcnt!=0&&argcnt!=1)
                printf("%s:\n",dir_name_list[i]);
            for (j=0; j<dir_len[i]; j++)
            {
                lsinfo* info=dir_file_list[i][j];
                show_file(info);
            }

        }

}
//for "-l"
void show_list()
{
    int i,j;
    if (cmd.r)
        for (i=0; i<=(argcnt==0?0:argcnt-1); i++)
        {
            if (argcnt!=0&&argcnt!=1)
                printf("%s:\n",dir_name_list[i]);
            for (j=dir_len[i]-1; j>=0; j--)
            {
                lsinfo* info=dir_file_list[i][j];
                show_l_file(info);
            }

        }
    else
        for (i=(argcnt==0?0:argcnt-1); i>=0; i--)
        {
            if (argcnt!=0&&argcnt!=1)
                printf("%s:\n",dir_name_list[i]);
            for (j=0; j<dir_len[i]; j++)
            {
                lsinfo* info=dir_file_list[i][j];
                show_l_file(info);
            }

        }
    printf("\n");
}
