#!/bin/bash

#Для каждого файла из заданного списка, сценарий должен определить тип архиватора, которым был создан тот или иной файл 
#(с помощью утилиты file). Затем сценарий должен выполнить соответствующую команду разархивации (gunzip, bunzip2, unzip, 
#uncompress или что-то иное). Если файл не является архивом, то сценарий должен оповестить пользователя об этом и ничего не делать с этим файлом.

for file in "$@"; do
# Получаем тип файла с помощью утилиты file
file_type=$(file -b "$file")


case "$file_type" in
*gzip\ compressed\ data*) # gzip архив
echo "Разархивация файла $file с помощью gunzip..."
gunzip "$file"
;;
*bzip2\ compressed\ data*) # bzip2 архив
echo "Разархивация файла $file с помощью bunzip2..."
bunzip2 "$file"
;;
*Zip\ archive*) # Zip архив
echo
 "Разархивация файла $file с помощью unzip..."
unzip "$file"
;;
*compress\ data*) # compress архив
echo "Разархивация файла $file с помощью uncompress..."
uncompress "$file"
;;
*) # Файл не является архивом
echo "Файл $file не является архивом."
;;
esac
done
