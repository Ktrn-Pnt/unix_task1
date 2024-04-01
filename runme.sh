#!/bin/bash

make 1>>/dev/null

python3 gen_fileA.py

./myprogram fileA fileB

gzip -k fileA fileB

gzip -cd fileB.gz | ./myprogram fileC

./myprogram -b 100 fileA fileD

stat fileA > result.txt
echo >> result.txt
stat fileB >> result.txt
echo >> result.txt
stat fileC >> result.txt
echo >> result.txt
stat fileD >> result.txt
echo >> result.txt
stat fileA.gz >> result.txt
echo >> result.txt
stat fileB.gz >> result.txt
