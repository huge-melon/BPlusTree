#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
    system("top -n 1 |grep Cpu  >>cpu.txt");
    system("top -n 1 |grep Cpu | cut -d \",\" -f 2 >>cpu.txt");
    system("top -n 1 |grep Mem  >>cpu.txt");
    system("top -n 1 |grep Mem | cut -d \",\" -f 2 >>cpu.txt");
    return 0;
}