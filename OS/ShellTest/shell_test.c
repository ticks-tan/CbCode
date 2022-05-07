/**
* FileName:   shell_test.c
* CreateDate: 2022-04-30 12:28:28
* Author:     ticks
* Email:      2938384958@qq.com
* Des:        Linux 简单 Shell模拟
*/

#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define READ_BUF_MAX_LENGTH 1024
#define FILE_MAX_LENGTH  128
#define ARGS_MAX_LENGTH     64
#define WORKSPACE_MAX_LENGTH 256

typedef struct Command
{
    char* args[ARGS_MAX_LENGTH];
    // 重定向文件
    char* re[2];
}Command;

// Shell结构
typedef struct Shell
{
    // 工作目录
    char work_dir[WORKSPACE_MAX_LENGTH];
    // 临时文件
    char tmp_file[FILE_MAX_LENGTH][2];
    // 子进程pid
    pid_t pid;
    // 命令列表
    char* command[FILE_MAX_LENGTH];
    // 分割后命令长度
    int command_length;
    // 读取的数据
    char read_buf[READ_BUF_MAX_LENGTH];
    // 命令
    Command cmd[ARGS_MAX_LENGTH];
    // 命令管道数量
    int gd;
}Shell;

// 全局静态Shell
static Shell s_shell;

// 输出错误信息
void shell_error(const char* format, ...);
// 读取一行字符
bool shell_get_line();
// 初始化 Shell
bool shell_init();
// 退出Shell的清理工作
bool shell_clear();
// 读取数据
void shell_read_command();
// 解析数据
bool shell_parse_command();
// 执行命令
int shell_run_command();
// 显示工作目录
void shell_show_workdir();

void sig_exit(int sig){
    if (sig == SIGINT){
        kill(s_shell.pid, SIGINT);
    }
}

int main(void)
{
    // 初始化shell
    if (!shell_init()){
        shell_error("初始化shell错误");
    }
    // 注册信号
    signal(SIGINT, sig_exit);
    while (true){
        shell_show_workdir();
        if (!shell_get_line()){
            break;
        }
        shell_read_command();
        // 解析命令
        if (!shell_parse_command()){
            shell_error("命令解析错误");
            continue;
        }
        // 执行命令
        shell_run_command();
    }
    shell_clear();
    return 0;
}

// 输出错误信息
void shell_error(const char* format, ...)
{
    va_list vl;
    printf("\033[38;2;255;0;0m");
    vprintf(format, vl);
    printf("\033[0m");
}

// 显示工作目录
void shell_show_workdir()
{
    printf("[%s %s]: ", "ticks", s_shell.work_dir);
}

// 读取一行字符
bool shell_get_line()
{
    {
        int len = 0, tmp;
        while (len < READ_BUF_MAX_LENGTH && (tmp = getchar()) != '\n'){
            s_shell.read_buf[len++] = (char)tmp;
        }
        s_shell.read_buf[len] = '\0';
    }
    if (strcmp(s_shell.read_buf, "exit") == 0 || strcmp(s_shell.read_buf, "EXIT") == 0){
        return false;
    }
    return true;
}

// 初始化 Shell
bool shell_init()
{
    if (getcwd(s_shell.work_dir, 255) == NULL){
        return false;
    }
    strcpy(s_shell.tmp_file[0], "shell_tmp_0");
    strcpy(s_shell.tmp_file[1], "shell_tmp_1");
    return true;
}

// 退出Shell的清理工作
bool shell_clear()
{
    return true;
}

// 读取数据
void shell_read_command()
{
    int pos = 0;
    char* substr = strtok(s_shell.read_buf, " ");
    while (substr){
        s_shell.command[pos++] = substr;
        substr = strtok(NULL, " ");
    }
    s_shell.command_length = pos;
    s_shell.command[pos] = NULL;
}

// 解析数据
bool shell_parse_command()
{
    s_shell.gd = 0;
    int pos = 0;
    for (int i = 0; i < s_shell.command_length; ++i){
        s_shell.cmd[0].args[0] = s_shell.command[i];
    }
    s_shell.cmd[0].args[s_shell.command_length] = NULL;
    s_shell.cmd[0].re[0] = s_shell.cmd[0].re[1] = NULL;
    for (int i = 0; i < s_shell.command_length; ++i){
        // 管道
        if (strcmp(s_shell.command[i], "|") == 0){
            s_shell.cmd[s_shell.gd].args[pos] = NULL;
            ++s_shell.gd;
            s_shell.cmd[s_shell.gd].re[0] = NULL;
            s_shell.cmd[s_shell.gd].re[1] = NULL;
            pos = 0;
        }else if (strcmp(s_shell.command[i], "<") == 0){
            // 输入重定向
            if (i + 1 < s_shell.command_length) {
                s_shell.cmd[s_shell.gd].args[pos++] = NULL;
                s_shell.cmd[s_shell.gd].re[0] = s_shell.command[i + 1];
            }else{
                // 语法错误
                return false;
            }
        }else if (strcmp(s_shell.command[i], ">") == 0){
            // 输出重定向
            if (i + 1 < s_shell.command_length){
                s_shell.cmd[s_shell.gd].args[pos++] = NULL;
                s_shell.cmd[s_shell.gd].re[1] = s_shell.command[i + 1];
            }
            else{
                // 语法错误
                return false;
            }
        }else{
            s_shell.cmd[s_shell.gd].args[pos++] = s_shell.command[i];
        }
    }
    return true;
}

// 执行命令
int shell_run_command()
{
    s_shell.pid = fork();
    if (s_shell.pid < 0){
        shell_error("执行命令错误");
        return 1;
    }else if (s_shell.pid == 0){
        // 子进程
        if (s_shell.gd == 0){
            // 没有管道
            // 判断是否有重定向输入和输出
            if (s_shell.cmd[0].re[0] != NULL){
                close(STDIN_FILENO);
                open(s_shell.cmd[0].re[0], O_RDONLY);
            }
            if (s_shell.cmd[0].re[1] != NULL){
                close(STDOUT_FILENO);
                open(s_shell.cmd[0].re[1], O_WRONLY | O_CREAT | O_TRUNC);
            }
            // 执行程序
            execvp(s_shell.cmd[0].args[0], s_shell.cmd[0].args);
        }else{
            // 有管道
            int exec_cnt, in_pos = 0;
            // 遍历命令并执行
            for (exec_cnt = 0; exec_cnt <= s_shell.gd; ++exec_cnt){
                pid_t pid2 = fork();
                if (pid2 < 0){
                    shell_error("fork error");
                    exit(1);
                }else if (pid2 == 0){
                    // 子进程
                    if (exec_cnt > 0){
                        close(STDIN_FILENO);
                        open(s_shell.tmp_file[in_pos], O_RDONLY);
                    }
                    if (s_shell.cmd[exec_cnt].re[0] != NULL){
                        close(STDIN_FILENO);
                        open(s_shell.cmd[exec_cnt].re[0], O_RDONLY);
                    }
                    if (s_shell.cmd[exec_cnt].re[1] != NULL){
                        close(STDOUT_FILENO);
                        open(s_shell.cmd[exec_cnt].re[1], O_WRONLY | O_CREAT | O_TRUNC);
                    }
                    if (exec_cnt < s_shell.gd) {
                        close(STDOUT_FILENO);
                        open(s_shell.tmp_file[1 - in_pos], O_WRONLY | O_CREAT | O_TRUNC);
                    }
                    // remove(s_shell.tmp_file[in_pos]);
                    if ((execvp(s_shell.cmd[exec_cnt].args[0], s_shell.cmd[exec_cnt].args)) < 0){
                        shell_error("exec error");
                        exit(1);
                    }
                }else{
                    // 父进程
                    waitpid(pid2, NULL, 0);
                }
                remove(s_shell.tmp_file[in_pos]);
                in_pos = 1 - in_pos;
            }
            remove(s_shell.tmp_file[0]);
            remove(s_shell.tmp_file[1]);
            exit(0);
        }
    }else{
        // 父进程
        waitpid(s_shell.pid, NULL, 0);
    }
    return 0;
}