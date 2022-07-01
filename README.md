# wish
wish stands for 'Wisconsin Shell" it is a simple UNIX like shell, that I will built as part
of my study for this [OS](https://pages.cs.wisc.edu/~remzi/Classes/537/Spring2018/) course.

## material about wish

## design of "wish"

## program abstraction "struct program"
the whole thing is built around this
```
struct program{
    char* name;
    uint argc;
    char** argv;

    /**
     * @brief 
     * if null output should be written into the standard output stream.
     * other wise the string will be considered as the file name to which the program
     * output should be redirected.
     */
    char* outstream; 
};
```
struct program is an abstraction of what the user of the shell want to do
it could be a built-in function of the shell or a 3rd party binaries.
for example, if the user typed 
```
ls -l -a > out.txt
```
this will be converted into a program struct with 

```
name = ls
argc = 2
argv = {-l, -a}
outstream = out.txt
```
## parser
parse one command line at a time, it builds a list of programs (the abstraction mentioned above) each program will run in a seprate process in parallel to the others, 
it works in major steps
* parse for the special operators supported by the shell ex "& is the parallel execution operator"  and "> output redirection operator"
  * it starts by spliting the command into sub commands around the '&' operator
  * then start looking for the ">" operator if exist within each sub command
* build a program (abstraction mentioned above) from the individual sub commands
  * extract the program name
  * extract the arguments
  
## executer
will carry on the logic to exceute built in functions and 3rd party binaries.
[not implemented yet]

## what "wish" can do ?

## what "wish" should do ?
- [ ] interactive mode.
- [ ] batch mode.
- [ ] support search path.
- [ ] enable the user to concatenate more search directories into the current search path.
- [ ] support built in commands.
- [ ] support redirection for other programs.
- [ ] support redirection for build in commands.
- [ ] support multiple level of redirections [if this is possible].
- [ ] running parallel commands.
- [ ] handling parsing errors and other errors that the shell should be responsible for.
- [ ] provide debug mode for detailed logs.
