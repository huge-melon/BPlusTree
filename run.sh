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
count=5000 # 13小时测试，日志每10s记录一次，因此循环5000次
echo '测试开始时间：'`date`>>log
./test >>log 2>>err_info & # 运行B+树并发读写，将错误信息保存到err_info中

while [ $count -ge 0 ]
do
    let count--
    sys_mess=`pidstat -uIr -h -G "test" 10 1|sed -n '4p'|awk '{print $1,$3,$7,$12,$13}'` # 每隔十秒进行一次采样
    # sys[0] 获取时间信息| sys[1]获得pid| sys[2]获取CPU的平均占用率| sys[3] 已占用内存大小KB| sys[4]内存占用率
    i=0
    for element in $sys_mess
    do
        sys[i]=$element
        let i++
    done
    output_mess=`date -d @${sys[0]} "+%H:%M:%S"`' 进程id:'${sys[1]}'  CPU占用率:'${sys[2]}'%  已占用内存:'${sys[3]}'KB  内存占用率:'${sys[4]}'%'
    echo $output_mess>>log # 将系统资源信息输出到log文件中
done
kill ${sys[1]}
echo '测试结束时间：'`date`>>log
