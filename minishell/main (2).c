#include"header.h"

char *external_commands[200];
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
char input_string[100];
int main()
{
    system("clear");
    char prompt[100] = "Mini_Shell$";
    scan_input(prompt,input_string);
    return 0;
}

