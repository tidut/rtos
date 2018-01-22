import sys
import time
import shutil
import os
import subprocess

print "Do you want to change the SW version\n"
print "Enter yes or no : \n"

def ChangeNumValue (Value):
	print "ChangeNumValue =  %c\n" % Value
	
	if (Value == "0") :
		print "returning 1 \r\n"
		return '1'
	elif (Value == "1") :
		print "returning 2 \r\n"
		return '2'
	elif (Value == "2") :
		print "returning 3 \r\n"
		return '3'
	elif (Value == "3") :
		print "returning 4 \r\n"
		return '4'
	elif (Value == "4") :
		print "returning 5 \r\n"
		return '5'
	elif (Value == "5") :
		print "returning 6 \r\n"
		return '6'
	elif (Value == "6") :
		print "returning 7 \r\n"
		return '7'
	elif(Value == "7") :
		print "returning 8 \r\n"
		return '8'
	elif (Value == "8") :
		print "returning 9 \r\n"
		return '9'
	elif (Value == "9") :
		print "returning 0 \r\n"
		return '0'
		
line = sys.stdin.readline()

if line == "yes\n":
	print "user has selected yes\n"
		
	arg1 = sys.argv[1]
	arg2 = sys.argv[2]	
	cmdargs = str(sys.argv)
	
	print ("Args list: %s \n" % cmdargs)
	print ("Args list 1: %s \n" % arg1)
	print ("Args list 2: %s \n" % arg2)
	
	dir_name = arg2
	DIR1 = arg1 
	DIR2 = arg2
	output_filename = arg2

	arg1 = sys.argv[1]	
	cmdargs = str(sys.argv)
	
	print ("Args list: %s \n" % cmdargs)
	print ("Args list 1: %s \n" % arg1)
	
	from subprocess import Popen,PIPE
	
	os.chdir("./" + arg2 + "/")	
	print("Path at terminal when executing this file\n")
	print(os.getcwd() + "\n")

	python_script = './date.py'
	subprocess.Popen("python date.py 1", shell=True)
	
	print("1 for board 1 Current sensor \n")
	print("2 for board 2 Pot sensor \n")
	print("3 for board 3 Energy sensor \n")
	print("4 for board 4 Air sensor \n")
	print("5 for board 5 Relay sensor \n")
	print("6 for board 6 Thermocouple sensor \n")
	
	strpath = './tech.sh' + ' 1'
	subprocess.call(strpath, shell=True)

	strpath = './tech.sh' + ' 2'		
	subprocess.call(strpath, shell=True)
	
	strpath = './tech.sh' + ' 3'
	subprocess.call(strpath, shell=True)
	
	strpath = './tech.sh' + ' 4'
	subprocess.call(strpath, shell=True)
	
	strpath = './tech.sh' + ' 5'
	subprocess.call(strpath, shell=True)
	
	strpath = './tech.sh' + ' 6'
	subprocess.call(strpath, shell=True)
		
	print "end"
			
	os.chdir("../")	
	print("Path at terminal when executing this file\n")
	print(os.getcwd() + "\n")
	
	Filename = "/compare.txt"
	print("Path %s \n" % ("./"+ dir_name + Filename))

	fo = open("./" + dir_name + Filename, "w")

	fo.write("This below  is comparison between two directories set by user\n\n")
	fo.write("Dir 1 %s\n"  % arg1 )
	fo.write("Dir 2 %s\n"  % arg2 )

	now = time.strftime("%c")
	fo.write("Time of compilation %s\n"  % now )
	fo.write("\n");

	## Display current date and time from now variable 
	print ("Current time %s\n"  % now ) 

	from filecmp import dircmp
	def print_diff_files(dcmp):
		for name in dcmp.diff_files:
			print "diff_file %s found in %s and %s \n" % (name, dcmp.left,dcmp.right)
			fo.write("diff_file %s found in %s and %s \n" % (name, dcmp.left,dcmp.right));
				  
		for sub_dcmp in dcmp.subdirs.values():
			print_diff_files(sub_dcmp)

	dcmp = dircmp(arg1,arg2) 
	print_diff_files(dcmp)	
	
	mystr = "tech-contiki-3.0.00"
	mystr = arg2
	
	print ("Zip file name :  %s \n" % dir_name)
	shutil.make_archive(dir_name, 'zip', dir_name)
	
	shutil.move(dir_name + '.zip', "./ZIP_files/")
		
	#tech-contiki-3.0.1
	print "Length of the string: ", len(mystr)

	if(len(mystr) > 18) :
		print "Version number is two digit \r\n"
		print "Value %c\n" % mystr[18]	
						
		newchar = ChangeNumValue(mystr[18])		
		if( newchar >= '1' and newchar <= '9'):
			index = 18
			mystr = mystr[:index] + newchar
		else :
			index = 17
			char = ChangeNumValue(mystr[17])
			mystr = mystr[:index] + char + newchar
	else:
		print "Version number is One digit \r\n"		
		print "Value %c\n" % mystr[17]	
		char = ChangeNumValue(mystr[17])
		
		if( char >= '1' and char <= '9'):
			index = 17
			mystr = mystr[:index] + char
		else :
			index = 17
			char = '1'
			newchar = '0'
			mystr = mystr[:index] + char + newchar			
							
	for fileName in os.listdir("."):
		os.rename(fileName, fileName.replace(arg2,mystr))
		
	fo.write("Old Version %s \n" % arg2)
	fo.write("New Version %s \n" % mystr)
    
	print ("New name: %s \n" % mystr)	
	
	fo.close()
	
else :	
	from subprocess import Popen,PIPE
	print "user has selected no\n"
	print "start\n"
	
	arg1 = sys.argv[1]	
	cmdargs = str(sys.argv)
	
	print ("Args list: %s \n" % cmdargs)
	print ("Args list 1: %s \n\n" % arg1)
	
	os.chdir("./" + arg1 + "/")	
	print("Path at terminal when executing this file\n")
	print(os.getcwd() + "\n")

	python_script = './date.py'
	subprocess.Popen("python date.py 1", shell=True)
	
	
	time.sleep(1)

	print("Enter 1 for board 1 Current sensor \n")
	print("Enter 2 for board 2 Pot sensor \n")
	print("Enter 3 for board 3 Energy sensor \n")
	print("Enter 4 for board 4 Air sensor \n")
	print("Enter 5 for board 5 Relay sensor \n")
	print("6 for board 6 Thermocouple sensor \n")
		
	line = sys.stdin.readline()
	if line == "1\n":
		strpath = './tech.sh' + ' 1 ' +  arg1
		subprocess.call(strpath, shell=True)
	elif line == "2\n":
		strpath = './tech.sh' + ' 2 ' +  arg1
		subprocess.call(strpath, shell=True)
	elif line == "3\n":
		strpath = './tech.sh' + ' 3 '+  arg1
		subprocess.call(strpath, shell=True)
	elif line == "4\n":
		strpath = './tech.sh' + ' 4 ' +  arg1
		subprocess.call(strpath, shell=True)
	elif line == "5\n":
		strpath = './tech.sh' + ' 5 ' +  arg1
		subprocess.call(strpath, shell=True)
	elif line == "6\n":
		strpath = './tech.sh' + ' 6 ' +  arg1
		subprocess.call(strpath, shell=True)
	else :
		print "No selection \n"
	
	print "end"

print "Good bye!\n"

 


