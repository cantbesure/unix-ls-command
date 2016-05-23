# unix-ls-command
ls command with most of the args

包含：lsls.c lsls.h
第一个ls代表（Li Shuo）
第二个ls代表命令

使用git进行版本控制，虽然只commit了一次
数据结构：线性表，根据获取文件数动态申请空间
排序方式：采用在加入list时进行插入排序

实现功能如下（并没有实现默认列优先输出格式，默认按照字母排序）：
-A   default don.t list . .. //默认不显示. ..文件
-a   list all//显示所有
-c   last changed//按照最后改变时间排序和显示
-d   plain//只对参数中路径或文件进行显示
-F   list "info->type"//显示文件类型标识
-f   no sort//强制不排序
-h   using KB	//用KB
-k   using B default//默认用B
-l   long format//显示长格式，详细信息
-i   print inode//打印inode
-n   numeric grp,owner//数字化所属用户及组
-r   reverse order//逆序输出
-S   size//按照大小排序
-t   modify//按照最后修改时间排序和显示
-s   print block_cnt//打印块计数
-x   force print line fisrt//行优先
-u   access//按照最后使用时间排序和显示
-R   //递归显示所有文件夹
-1   one thing perline//强制每行打印一个文件信息

存在问题：
1.没有实现默认列优先输出
2.没有实现-l参数会显示total文件块数
