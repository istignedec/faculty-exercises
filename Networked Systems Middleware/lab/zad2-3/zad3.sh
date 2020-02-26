#!/usr/bin/bash

# ZADATAK 3

javac -d ./ *.java
jar -cvf VideoCountSort.jar -C ./ .

hdfs dfs -rm -r input1
hdfs dfs -put input input1

hadoop jar VideoCountSort.jar VideoCountSort input1 output1
rm part-00000
hdfs dfs -cat output1/part-00000
hdfs dfs -get output1/part-00000

hdfs dfs -rm -r output1
