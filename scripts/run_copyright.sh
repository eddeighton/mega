#!/bin/bash

TEMP_FILE=/tmp/temp_cpwrite_file
CP_LEN=19
COPYRIGHT_CPP=/workspace/root/src/mega/src/scripts/copyright.txt
COPYRIGHT_SH=/workspace/root/src/mega/src/scripts/copyright.sh


for f in `find . -iname "*.hpp"`
do
head -$CP_LEN $f | diff ${COPYRIGHT_CPP} - || ( ( cat ${COPYRIGHT_CPP}; echo; cat $f) > ${TEMP_FILE}; mv ${TEMP_FILE} $f )  
done

for f in `find . -iname "*.cpp"`
do
head -$CP_LEN $f | diff ${COPYRIGHT_CPP} - || ( ( cat ${COPYRIGHT_CPP}; echo; cat $f) > ${TEMP_FILE}; mv ${TEMP_FILE} $f )  
done

for f in `find . -iname "CMakeLists.txt"`
do
head -$CP_LEN $f | diff ${COPYRIGHT_SH} - || ( ( cat ${COPYRIGHT_SH}; echo; cat $f) > ${TEMP_FILE}; mv ${TEMP_FILE} $f )  
done

for f in `find . -iname "*.jinja"`
do
head -$CP_LEN $f | diff ${COPYRIGHT_CPP} - || ( ( cat ${COPYRIGHT_CPP}; echo; cat $f) > ${TEMP_FILE}; mv ${TEMP_FILE} $f )  
done

for f in `find . -iname "*.dbs"`
do
head -$CP_LEN $f | diff ${COPYRIGHT_CPP} - || ( ( cat ${COPYRIGHT_CPP}; echo; cat $f) > ${TEMP_FILE}; mv ${TEMP_FILE} $f )  
done

for f in `find . -iname "*.pts"`
do
head -$CP_LEN $f | diff ${COPYRIGHT_CPP} - || ( ( cat ${COPYRIGHT_CPP}; echo; cat $f) > ${TEMP_FILE}; mv ${TEMP_FILE} $f )  
done
