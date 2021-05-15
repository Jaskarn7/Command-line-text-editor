Currently this text editor handles the following operations:
    printing a line(s), appending multiple lines, deleting a line(s).

Usage:
First a prompt will appear something like this: "Number>"
Here the Number is the current working line number. You can either press n to move to next line or ":Number" to move to the Number line.

Writing commands: 
While writing commands make sure you don't type "," as in case of original ed text editor instead just type a space.

Example of writing a command:
    line_number> 3 7 p
The following command will print all lines from line 3 to line 7.

To print a single line type the command:
    line_number> 5 p
This will print line number 5

If no number is passed like:
    line_number> p
Then this will print the current working line (line_number).

For printing all the lines type:
    line_number> 1 $ p
'$' here refers to the last line.

Other commands are also similar to this:
    line_number> d
This will delete current working line.

    line_number> a
This will append lines in the file until EOF (CTRL + z for windows) is not pressed.

Use ':' to move to a certain line:
    line_number> :3
This will change current working line to 3.

Use 'q' command to exit.