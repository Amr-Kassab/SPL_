#include<stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZE 100
#define CAP 8

char *buf;
char **argv;
int argc;

int parse(void);
int detect_args(char *arg);

int main(void)
{
	while (1) {
		printf("Femto> ");
		fflush(stdout);

		argc = parse();
		if (argc == -1)
			exit(-2);

		if (argc == 0 || argv[0] == NULL) {
			free(argv);
			continue;
        }

		if (detect_args("echo")) {
			for (int i = 1; i < argc; i++) {
				printf("%s", argv[i]);
				printf(" ");
			}
			printf("\n");
		} 
		else if(detect_args("exit"))
		{
			printf("Good Bye \n");
			return 0;
		}
		else {
			printf("Invalid Command \n");
		}
		free(argv);
		free(buf);
	}
	return 0;

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
        
        if (c == '\n' || c == '\\') {
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
	if(strlen(arg) != strlen(argv[0]))
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
