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
ull* readCPUMaxTemp();
//read now cpu temps (size = numCPU)
ull* readCPUNowTemp();

typedef struct{
    ull PL1;
    ull TW1;
    ull PL2;
    ull TW2;
} package_power;


//read msr's 8bytes at 'offset',and calculate the value from 'from' to 'to'(not include)
ull* readmsr(int offset,int from_bit,int to_bit);
//used by readmsr:calculate bits to ull
ull get_value_for_bits(ull  val, int from_bit,int  to_bit);
void loadModMsr();


int numCPU;
char** paths;

void temps(){
    ull* maxCpuTemp=readCPUMaxTemp();
    printf("%scpu max temp= %lld\n%s",RESET,*maxCpuTemp,RESET);
    free(maxCpuTemp);
    maxCpuTemp=NULL;
    for (; ;) {
        ull* nowCpuTemps=readCPUNowTemp();
        printf("%scpu temps= %s",RESET,RESET);
        for (int i = 0; i <numCPU ; ++i) {
            if(nowCpuTemps[i]>=70){
                printf("%s%lld %s",RED,nowCpuTemps[i],RESET);
            } else if(nowCpuTemps[i]<70&&nowCpuTemps[i]>=55){
                printf("%s%lld %s",GREEN,nowCpuTemps[i],RESET);
            } else{
                printf("%s%lld %s",RESET,nowCpuTemps[i],RESET);
            }
        }
        free(nowCpuTemps);
        nowCpuTemps=NULL;
        printf("%s\n%s",GREEN,RESET);
        sleep(2);
    }
}



int main() {
    init();
    temps();
    return 0;
}

void init(){
    //check sudo
    if(geteuid()!=0){
        printf("%s== please run with sudo! ==%s\n",RED,RESET);
        exit(1);
    }
    numCPU=(int)sysconf(_SC_NPROCESSORS_ONLN);
    int pathsNum=numCPU;
    paths=(char**)calloc(sizeof(char *),pathsNum);
    for (int i = 0; i <pathsNum ; ++i) {
        char* path=(char*)calloc(30, sizeof(char));
        sprintf(path,"%s%d%s","/dev/cpu/",i,"/msr");
        paths[i]=path;
    }
    //check msr mod loaded
    int fd = open(paths[0], O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if(fd==-1){
        loadModMsr();
    }else{
        close(fd);
    }
}

ull* readCPUMaxTemp(){
    ull* values= readmsr(0x1A2, 16, 23);
    ull* value=(ull*)calloc(sizeof(ull),1);
    *value=values[0];
    free(values);
    values=NULL;
    return value;
}

ull* readCPUNowTemp(){
    ull* nowTemps=(ull*)calloc(sizeof(ull),numCPU);
    ull* maxTemp=readCPUMaxTemp();
    ull* values= readmsr(0x19C, 16, 23);
    for (int i = 0; i < numCPU; ++i) {
        nowTemps[i]=*maxTemp-values[i];
    }
    free(maxTemp);
    maxTemp=NULL;
    return nowTemps;
}

ull* readmsr(int offset,int from_bit,int to_bit){
    //open offset files, return fds.
    int pathsNum=numCPU;
    int fds[pathsNum];
    for (int j = 0; j <pathsNum ; ++j) {
        int fd = open(paths[j], O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if(fd==-1){//if not exist ,reload msr mod
            loadModMsr();
            fd = open(paths[j], O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        }
        fds[j]=fd;
    }
    ull* values=(ull*)calloc(sizeof(ull),pathsNum);
    for (int i = 0; i < pathsNum; ++i) {
        lseek(fds[i],offset,SEEK_SET);
        ull* value=values+i;
        ssize_t count=read(fds[i],value, sizeof(ull));
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

void loadModMsr(){
    printf("%sload kernel mod 'msr'%s\n",YELLOW,RESET);
    int result=system("modprobe msr");
    if(result!=0){
        printf("%sload kernel mod 'msr' failed%s\n",RED,RESET);
        exit(-1);
    }
}
