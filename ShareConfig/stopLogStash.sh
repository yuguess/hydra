#!/bin/zsh
name=$(lsof -i:24225|tail -1|awk '"("!=""{print $2}')  
echo $name
if [ -z $name ]  
then  
      echo "No process can be used to killed!"  
      exit 0  
fi  
id=$(lsof -i:24225|tail -1|awk '"("!=""{print $2}')  
echo $id
kill -9 $id  
          
echo "Process name=$name($id) kill!"  
exit 0  
