#!/bin/bash
#prepend.sh 在文件arg1开头添加文件arg2
#运行:$ ./prepend.sh arg1 arg2

E_NOSUCHFILE=65

if [ $# != 2 ]
then
	echo "Usage: ./prepend.sh arg1 arg2"
	exit $E_NOSUCHFILE
else
	echo "argument is: " $1 $2
fi

if [ ! -e "$1" ]
then   # 如果这个文件不存在, 就进来 
	echo "arg1 is not found..."
	exit $E_NOSUCHFILE
fi

if [ ! -e "$2" ]
then   # 如果这个文件不存在, 就进来 
	echo "arg2 is not found..."
	exit $E_NOSUCHFILE
fi

cat - $1 < $2 > $1.tmp		#生成新的文件: arg1.tmp

cat $1.tmp	#打印结果
exit 0
