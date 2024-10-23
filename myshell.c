#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM 1024
#define OPT_NUM 64
char lineCommand[NUM];
char *myargv[OPT_NUM];
int lastCode = 0;
int lastSig = 0;

int main() {
    while (1) {
        //输入提示符
        printf("用户名@主机名 当前路径# ");
        fflush(stdout);

        //获取用户输入，输入的时候，输入\n
        char *s = fgets(lineCommand, sizeof(lineCommand) - 1, stdin);
        assert(s != NULL);
        (void)s;
        //清除最后一个\n，abcd\n
        lineCommand[strlen(lineCommand) - 1] = 0;
        // printf("test: %s\n", lineCommand);

        //"ls -a -l -h" -> "ls" "-a" "-l" "-h" -> 1 -> n
        //字符串切割
        myargv[0] = strtok(lineCommand, " ");
        //如果没有字串了，strtok -> NULL, myargv[end] = NULL
        int i = 1;
        while (myargv[i++] = strtok(NULL, " "))
            ;

        if (myargv[0] != NULL && strcmp(myargv[0], "cd") == 0) {
            if (myargv[1] != NULL) chdir(myargv[1]);
            continue;
        }

        if (myargv[0] != NULL && strcmp(myargv[0], "echo") == 0) {
            if (strcmp(myargv[1], "$?") == 0) {
                printf("%d, %d\n", lastCode, lastSig);
            } else {
                printf("%s\n", myargv[1]);
            }
            continue;
        }

        //测试是否成功,条件编译
#ifdef DEBUG
        for (int i = 0; myargv[i]; i++) {
            printf("myargv[%d]: %s\n", i, myargv[i]);
        }
#endif
        //执行命令
        pid_t id = fork();
        assert(id != -1);

        if (id == 0) {
            execvp(myargv[0], myargv);
            exit(1);
        }

        int status = 0;
        waitpid(id, &status, 0);
        lastCode = ((status >> 8) & 0xFF);
        lastSig = (status & 0x7F);
    }
}
