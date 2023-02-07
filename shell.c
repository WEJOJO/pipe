#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int	g_fd;

static int	print(char *string)
{
	int	length = 0;

	while (string[length])
		length++;
	write(2, string, length);
	return (1);
}

static int	executor(char **argv, int i, char **env)
{
	int	status;
	int	fd[2];
	int	pid;
	int	next = 0;
	printf("in exec %d \n",i);

	if (argv[i] && strcmp(argv[i], "|") == 0)
		next = 1;
	if (argv[i] == *argv) ///맨 마지막에 널문자???? 모르겠음
	{
		printf("???\n");
		return (0);
	}
	if (pipe(fd) == -1)
		return (print("error: fatal\n"));
	pid = fork();
	if (pid == -1)
		return (print("error: fatal\n"));
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(g_fd, 0);
		argv[i] = 0;
		if (next)
			dup2(fd[1], 1);
		if (g_fd != 0)
			close(g_fd);
		close(fd[1]);
		if (execve(*argv, argv, env) == -1)
		{
			print("error: cannot execute ");
			print(*argv);
			print("\n");
			exit(0);
		}
	}
	else
	{
		close(fd[1]);
		waitpid(pid, &status, 0);
		if (g_fd != 0)
			close(g_fd);
		if (next)
			g_fd = dup(fd[0]);
		close(fd[0]);
	}
	return (0);
}

static int	builtin_cd(char **argv)
{
	if (argv[2] && strcmp(argv[2], "|") != 0 && strcmp(argv[2], ";") != 0)
	{
		printf("%s ::: \n", argv[2]);
		return (print("error: cd: bad arguments\n"));
	}
	if (chdir(argv[1]) == -1)
		return (print("error: cannot execute cd\n"));
	return (0);
}

int	main(int argc, char **argv, char **env)
{
	int	i = 1;
	printf("g_fd %d \n",g_fd);
	if (argc == 1)
		return (0);
	argv[argc] = 0;
	while (argv[i - 1] && argv[i])
	{
		argv = argv + i;
		i = 0; //i의 변경이 변하는 것 이해되지 않는 상태.
		while (argv[i] && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
			i++; //
		printf("in main i %d \n",i);
		if (!strcmp(*argv, "cd"))
		{
			builtin_cd(argv);
		}
		else
			executor(argv, i, env);
		i++;
	}
}