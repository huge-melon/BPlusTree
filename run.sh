#!/bin/bash
cd /home/shixin/BPlusTree

make clean # 清除之前编译的结果
make # 重新编译
if [ -f log ] # log文件已存在则删除
then
    rm -r log
fi

if [ -f err_info ] # err_info文件已存在则删除
then
    rm -r err_info
fi

count=4500 # 12小时测试，日志每10s记录一次，因此循环4500次

./test >>log 2>>err_info & # 运行B+树并发读写，将错误信息保存到err_info中
# ./test & 
sleep 1

while [ $count -ge 0 ]
do
    let count--

    cpu_mess=`pidstat -uI -G "test"|sed -n '4p'|awk '{print $1,$3,$7}'`
    mem_mess=`pidstat -r -G "test"|sed -n '4p'|awk '{print $7,$8}'`

    # cpu[0] 获取时间信息 cpu[1]获得pid cup[2]获取CPU的平均占用率 
    i=0
    for element in $cpu_mess
    do 
        cpu[i]=$element
        let i++
    done

    # mem[0] 已占用内存大小KB mem[1]内存占用率
    i=0
    for element in $mem_mess
    do
        mem[i]=$element
        let i++
    done

    output_mess=${cpu[0]}' 进程id:'${cpu[1]}'  CPU占用率:'${cpu[2]}'%  已占用内存:'${mem[0]}'KB  内存占用率:'${mem[1]}'%'
    echo $output_mess>>log # 将系统资源信息输出到log文件中

    sleep 10 # 10s
done

kill ${cpu[1]}

