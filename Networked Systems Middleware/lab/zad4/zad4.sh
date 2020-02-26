#!/usr/bin/bash

# ZADATAK 4

javac -d ./ *.java
jar -cvf Job1.jar -C ./ .
jar -cvf Job2.jar -C ./ .

hdfs dfs -rm -r inmatrices1
hdfs dfs -rm -r outmatrices1
hdfs dfs -rm -r inmatrices1
hdfs dfs -rm -r outmatrices2
hdfs dfs -put inmatrices inmatrices1

hadoop jar Job1.jar Job1 inmatrices1 outmatrices1
rm part-00000
hdfs dfs -cat outmatrices1/part-00000
hdfs dfs -get outmatrices1/part-00000

hdfs dfs -put part-00000 part-000001

hadoop jar Job2.jar Job2 part-000001 outmatrices2
rm part-00000
hdfs dfs -cat outmatrices2/part-00000
hdfs dfs -get outmatrices2/part-00000
