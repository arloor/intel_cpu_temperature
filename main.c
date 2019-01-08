#include <stdio.h>
#include <math.h>
#include <zconf.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define YELLOW  "\033[93m"
#define GREEN  "\033[92m"
#define RED  "\033[91m"
#define RESET  "\033[0m"
#define BOLD  "\033[1m"

typedef unsigned long long ull;
typedef long long ll;

//init
void init();
//read max CPU temp
ull readCPUMaxTemp();
//read now cpu temps (size = numCPU)
ull* readCPUNowTemp();


//read msr's 8bytes at 'offset',and calculate the value from 'from' to 'to'(not include)
ull* readmsr(int offset,int from_bit,int to_bit);
//used by readmsr:calculate bits to ull
ull get_value_for_bits(ull  val, int from_bit,int  to_bit);


int numCPU;
char** paths;

void test(){
    ull maxCpuTemp=readCPUMaxTemp();
    printf("%scpu max temp= %lld\n%s",RESET,maxCpuTemp,RESET);
    for (; ;) {
        ull* nowCpuTemps=readCPUNowTemp();
        printf("%scpu temps= %s",RESET,RESET);
        for (int i = 0; i <numCPU ; ++i) {
            if(nowCpuTemps[i]>=70){
                printf("%s%lld %s",RED,nowCpuTemps[i],RESET);
            } else if(nowCpuTemps[i]<70&&nowCpuTemps[i]>=55){
                printf("%s%lld %s",YELLOW,nowCpuTemps[i],YELLOW);
            } else{
                printf("%s%lld %s",GREEN,nowCpuTemps[i],RESET);
            }

        }
        printf("%s\n%s",GREEN,RESET);
        sleep(2);
    }
}

int main() {
    init();
    test();
    return 0;
}

void init(){
//    printf("== init start! ==\n");
    numCPU=(int)sysconf(_SC_NPROCESSORS_ONLN);
//    printf("set CPU num to %d\n\n",numCPU);
    int pathsNum=numCPU;
    paths=(char**)calloc(sizeof(char *),pathsNum);
    for (int i = 0; i <pathsNum ; ++i) {
        char * path=(char*)calloc(30, sizeof(char));
        sprintf(path,"%s%d%s","/dev/cpu/",i,"/msr");
        paths[i]=path;
    }

    //test the path exists..
//    printf("CPU msr paths:\n");
    for (int j = 0; j <pathsNum ; ++j) {
//        printf("%d %s\n",j,paths[j]);
        int fd = open(paths[j], O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if(fd==-1){
            printf("%sinit failed: no msr file in /dev/cpu/ !\nmaybe no msr mod.\nrun \"lsmod | grep msr\" \nexit -1\n%s",RED,RESET);
            exit(-1);
        }else{
            close(fd);
        }
    }
//    printf("== init end! ==\n");
}

ull readCPUMaxTemp(){
    ull* values= readmsr(0x1A2, 16, 23);
    return values[0];
}

ull* readCPUNowTemp(){
    ull* nowTemps=(ull*)calloc(sizeof(ull),numCPU);
    ull maxTemp=readCPUMaxTemp();
    ull* values= readmsr(0x19C, 16, 23);
    for (int i = 0; i < numCPU; ++i) {
        nowTemps[i]=maxTemp-values[i];
    }
    return nowTemps;
}

ull* readmsr(int offset,int from_bit,int to_bit){
    //open offset files, return fds.
    int pathsNum=numCPU;
    int* fds=(int *)calloc(pathsNum, sizeof(int));
    for (int j = 0; j <pathsNum ; ++j) {
        int fd = open(paths[j], O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if(fd==-1){
            printf("%sreadmsr failed: no offset file in /dev/cpu/ !\nmaybe no offset mod.\nrun \"lsmod | grep offset\" \nexit -1\n%s",RED,RESET);
            exit(-1);
        }else{
            fds[j]=fd;
        }
    }
    ull* values=(ull*)calloc(sizeof(ull),pathsNum);
    for (int i = 0; i < pathsNum; ++i) {
        lseek(fds[i],offset,SEEK_SET);
        ull* value=values+i;
        int count=read(fds[i],value, sizeof(ull));
        close(fds[i]);
        if(errno==-1||count!= sizeof(ull)){
            printf("%sread offset error%s",RED,RESET);
            return NULL;
        }else{
            *value=get_value_for_bits(*value,from_bit,to_bit);
        }
    }
    return values;
}

ull get_value_for_bits(ull  val, int from_bit,int  to_bit){
    ull mask=0;
    for(int i=from_bit;i<to_bit+1;i++){
        mask+=(ull)pow(2,i);
    }
    return (val & mask) >> from_bit;
}
