#!/usr/bin/python
 
import time
import os

now = time.strftime("%c")
## date and time representation
print "Current date & time " + time.strftime("%c")
 
## Only date representation
print "Current date "  + time.strftime("%x")
 
## Only time representation
print "Current time " + time.strftime("%X")
 
## Display current date and time from now variable 
print ("Current time %s"  % now ) 

f = open('./examples/er-rest-example/compile_time.h','w')
f.write('#define COMPILE_TIME "') # python will convert \n to os.linesep
f.write("%s"  % now )
f.write('"')
f.close()


print("Path at terminal when executing this file")
print(os.getcwd() + "\n")

full_path = os.getcwd()
print(full_path + "\n")

str1 = os.path.dirname(full_path)
end = len(str1)
print "Length of the string: ", len(str1)
str2 = "tech-contiki-3.0."


i = full_path.find('tech-contiki-3.0.') # i now contains the value 18
sub1 = full_path[i:i+19]
print(sub1)

print(full_path + "\n")

f = open('./examples/er-rest-example/version.h','w')
f.write('#define VERSION_NAME "') # python will convert \n to os.linesep
f.write("%s"  % sub1 )
f.write('"')
f.close()

#echo "${PWD##*/}" >> version.h
