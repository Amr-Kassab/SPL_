#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 1000
#define CAP 8

typedef struct {
	char *name;
	char *val;
} Var;

Var *vars = NULL;
int var_count = 0;

char pwdbuf[SIZE];
char *buf;
char **argv;
int argc;

int parse(void);
int detect_args(char *arg);
char *get_var(char *name);
void set_var(char *name, char *val);
char *expand(char *str);

int main(void)
{
	while (1) {
		printf("Nano Shell Prompt > ");
		fflush(stdout);

		argc = parse();
		if (argc == -1)
			exit(-2);

		if (argc == 0 || argv[0] == NULL) {
			free(argv);
			continue;
		}

		int has_eq = 0;
		for (int i = 0; i < argc; i++) {
			if (strchr(argv[i], '=') != NULL)
				has_eq = 1;
		}

		if (has_eq) {
			if (argc == 1) {
				char *eq = strchr(argv[0], '=');
				if (eq != NULL && eq != argv[0]) {
					*eq = '\0';
					char *name = argv[0];
					char *val = eq + 1;
					char *exp_val = expand(val);
					set_var(name, exp_val);
					free(exp_val);
					free(argv);
					free(buf);
					continue;
				}
			}
			printf("Invalid command\n");
			free(argv);
			free(buf);
			continue;
		}

		for (int i = 0; i < argc; i++) {
			argv[i] = expand(argv[i]);
		}

		if (detect_args("echo")) {
			for (int i = 1; i < argc; i++) {
				printf("%s ", argv[i]);
			}
			printf("\n");
		} 
		else if (detect_args("export")) {
			if (argc > 1) {
				char *val = get_var(argv[1]);
				if (val != NULL)
					setenv(argv[1], val, 1);
			}
		}
		else if (detect_args("cd"))
		{
			if (argc < 2)
			{
				printf("missing arguments \n");
			}
			else if (chdir(argv[1]) != 0)
			{
				printf("%s : no such file or directory \n", argv[1]);
			}
		}
		else if (detect_args("exit"))
		{
			printf("Good Bye \n");
			return 0;
		}
		else if (detect_args("pwd"))
		{
			char* ret = getcwd(pwdbuf, SIZE);
			if (ret == NULL)
			{
				printf("could not read the process \n");
			}
			else
			{
				printf("%s \n", ret);
			}
		}
		else {
			pid_t pid = fork();

			if (pid < 0) {
				printf("child is not created\n");
			} 
			else if (pid == 0) {
				execvp(argv[0], argv);
				printf("Command not found: %s\n", argv[0]);
				exit(1); 
			} 
			else {
				int status;
				wait(&status);
			}
		}

		for (int i = 0; i < argc; i++) {
			free(argv[i]);
		}
		free(argv);
		free(buf);
	}
	return 0;
}

char *get_var(char *name)
{
	for (int i = 0; i < var_count; i++) {
		if (strcmp(vars[i].name, name) == 0)
			return vars[i].val;
	}
	return NULL;
}

void set_var(char *name, char *val)
{
	for (int i = 0; i < var_count; i++) {
		if (strcmp(vars[i].name, name) == 0) {
			free(vars[i].val);
			vars[i].val = strdup(val);
			return;
		}
	}
	vars = realloc(vars, (var_count + 1) * sizeof(Var));
	vars[var_count].name = strdup(name);
	vars[var_count].val = strdup(val);
	var_count++;
}

char *expand(char *str)
{
	if (str == NULL || strchr(str, '$') == NULL)
		return strdup(str);

	char res[11 * SIZE] = "";
	int i = 0, j = 0;

	while (str[i] != '\0') {
		if (str[i] == '$') {
			i++;
			char varname[SIZE] = "";
			int k = 0;
			while (str[i] != '\0' && str[i] != '/' && str[i] != ' ' && str[i] != '$') {
				varname[k++] = str[i++];
			}
			varname[k] = '\0';

			char *v = get_var(varname);
			if (v == NULL) v = getenv(varname);
			if (v != NULL) {
				strcat(res, v);
				j += strlen(v);
			}
		} else {
			res[j++] = str[i++];
			res[j] = '\0';
		}
	}
	return strdup(res);
}

int parse(void)
{
	int buf_capacity = SIZE;
	int position = 0;

	buf = (char *) malloc(buf_capacity * sizeof(char));
	if (!buf) {
		exit(-1);
	}

	while (1) {
		char c;
		int _read = read(0, &c, 1); 
		if (_read <= 0) {
			if (position == 0) {
				free(buf);
				return -1; 
			}
			break; 
		}
		
		if (c == '\n') {
			break;
		}
		buf[position] = c;
		position++;
		
		if (position >= buf_capacity - 1) {
			buf_capacity *= 2;
			char *temp = (char *) realloc(buf, buf_capacity * sizeof(char));
			if (!temp) {
				free(buf);
				exit(-1);
			}
			buf = temp;
		}
	}
	buf[position] = '\0';

	int count = 0;
	int capacity = CAP;

	argv = (char **) malloc(capacity * sizeof(char *));
	if (!argv) {
		exit(-1);
	}

	char *token = strtok(buf, " \t");

	while (token != NULL) {

		if (count >= capacity - 1) {
			capacity *= 2;
			char **temp = (char **) realloc(argv, capacity * sizeof(char *));
			if (!temp) {
				exit(-1);
			}

			argv = temp;
		}

		argv[count++] = token;
		token = strtok(NULL, " \t");
	}
	argv[count] = NULL;

	return count;
}

int detect_args(char *arg)
{
	if (argv == NULL || argv[0] == NULL)
		return 0;
	if (strlen(arg) != strlen(argv[0]))
		return 0;

	int counter = 0;
	while (arg[counter] != '\0' && argv[0][counter] != '\0') {
		if (argv[0][counter] == arg[counter]){
			counter++;
			continue;
		}
		else {
			return 0;
		}
	}

	return 1;
}