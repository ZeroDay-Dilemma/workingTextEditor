#!/bin/bash

textbody=$(<$1)
#<cheatsheet.html grep -n "h2" | grep -Po "^.*(?=:)"
#<cheatsheet.html grep -n "h2" | grep -Po "(?<=<h2>).+(?=<\/h2)"

headers=($(echo "$textbody" | grep -P "(?<=<h2>).+?(?=<\/h2>)"))
headerPN=($(echo "$textbody" | grep -n "h2" | grep -Po "^.*(?=:)"))

#commands=($(echo "$textbody" | grep -P "(?<=<h2>).+?(?=<\/h2>)"))
#commandsPN=($(echo "$textbody" | grep -n "h2" | grep -Po "^.*(?=:)"))


#echo "$textbody"
echo "${headers[@]}"
echo "${headerPN[@]}"

#echo "${commands@]}"
#echo "${commandsNP[@]}"


#for header in "${headers[@]}"; do
#    echo "$header"
#done
