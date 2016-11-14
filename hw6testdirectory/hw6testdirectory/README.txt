Do your programming in the 'mydirectory' directory, in a directory
lastname_hw0x (all lower case). In my case, I do all my programming
in the directory 'mydirectory/buell_hw0x' for programming 
assignment number x. That is, the 'x' should be replaced by the
actually number of the homework assignment.

Run the script zaZipUpScript.txt to tar and gzip your directory
and copy it to the 'assignment' directory.

Run the script zbFileCopyScript.txt to create directory 'testdirectory'
and unzip and untar your submission from 'assignment' into 'testdirectory'.

Run the script zcCompileScript.txt to compile all the programs in
'testdirectory'.

Run the script zdExecuteScript.txt to execute all the programs that
were compiled in the previous step. Note that this redirects the output
to a file.

If this works, you should have the appropriate output files in your
directory. To determine the names of the appropriate output files,
look at the zdExecuteScript.txt to see where it is sending the
output either by redirecting standard output or by providing command
line arguments that are the file names.
