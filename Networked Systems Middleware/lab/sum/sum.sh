#!/usr/bin/bash

javac -d ./ *.java
jar -cvf JobSum.jar -C ./ .

hdfs dfs -put inmatrices inmatrices1

hdfs dfs -rm -r sum1
hadoop jar JobSum.jar JobSum inmatrices1 sum1
hdfs dfs -cat sum1/part-00000
