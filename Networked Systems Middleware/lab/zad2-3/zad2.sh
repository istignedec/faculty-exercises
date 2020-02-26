#!/usr/bin/bash

# ZADATAK 2

export JAVA_HOME=/usr/lib/jvm/default
export HADOOP_HOME=/opt/hadoop-3.0.0
export PATH=$PATH:$HADOOP_HOME/bin:$HADOOP_HOME/sbin
export CLASSPATH=`hadoop classpath`:.:

javac -d ./ *.java
jar -cvf VideoCount.jar -C ./ .

start-dfs.sh
start-yarn.sh
hdfs dfs -rm -r log1
hdfs dfs -put log log1
hadoop jar VideoCount.jar VideoCount log1 out1
rm part-00000
hdfs dfs -cat out1/part-00000
hdfs dfs -get out1/part-00000

hdfs dfs -rm -r out1
cp part-00000 ./sort/input
