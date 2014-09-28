#!/bin/sh

for ((i = 2; i < 20; ++i))
do
    echo "insert into blog values('$i', 'www.baidu$i.com');"
done
