/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 13:49:37 by jgasparo          #+#    #+#             */
/*   Updated: 2024/04/30 12:32:44 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_cmd_exec(t_data *data)
{
	pid_t	pid;
	int		status;

	pid = fork();
	status = 0;
	if (pid == -1)
	{
		perror("fork");
		return (EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status))
			printf("^\\Quit: %d\n", SIGQUIT);
	}
	else
	{
		ft_signal(SIG_DFL);
		execute_command(data, data->cmd);
	}
	return (EXIT_SUCCESS);
}

int	ft_open_redir_in(t_data *data, t_cmd *cmd, t_exec *exec)
{
	t_redir_lst	*current;

	if (!cmd->redirections)
		return (EXIT_SUCCESS);
	current = cmd->redirections;
	while (current)
	{
		if (current->r_type == IN)
		{
			exec->fdin = open(current->filename, O_RDONLY);
			if (exec->fdin == -1)
				return (data->err_info = current->filename, E_OPEN);
		}
		else if (current->r_type == HEREDOC)
		{
			exec->fdin = open(current->hd_path, O_RDONLY);
			if (exec->fdin == -1)
				return (data->err_info = current->filename, E_OPEN);
		}
		current = current->next;
	}
	if (exec->fdin != -1)
	{
		if (dup2(exec->fdin, STDIN_FILENO) == -1)
			return (E_DUP);
		close(exec->fdin);
	}
	return (EXIT_SUCCESS);
}

int	ft_open_redir_out(t_data *data, t_cmd *cmd, t_exec *exec)
{
	t_redir_lst	*current;

	if (!cmd->redirections)
		return (EXIT_SUCCESS);
	current = cmd->redirections;
	while (current)
	{
		if (current->r_type == OUT)
		{
			exec->fdout = open(current->filename,
					O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (exec->fdout == -1)
			{
				perror(current->filename);
				return (data->err_info = current->filename, E_OPEN);
			}
		}
		else if (current->r_type == OUT_T)
		{
			exec->fdout = open(current->filename,
					O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (exec->fdout == -1)
				return (data->err_info = current->filename, E_OPEN);
		}
		current = current->next;
	}
	if (exec->fdout != -1)
	{
		if (dup2(exec->fdout, STDOUT_FILENO) == -1)
			return (E_DUP);
		close(exec->fdout);
	}
	return (EXIT_SUCCESS);
}

t_redir_lst	*ft_check_redir_in(t_redir_lst **lst)
{
	t_redir_lst	*temp;
	t_redir_lst	*failed;

	if (!*lst)
		return (NULL);
	failed = NULL;
	temp = *lst;
	while (temp)
	{
		if (temp->r_type == IN && access(temp->filename, F_OK | R_OK))
			failed = temp;
		temp = temp->next;
	}
	return (failed);
}

int	ft_handle_heredoc(t_redir_lst *node)
{
	char	*line;
	int		fd;

	fd = open(node->hd_path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd == -1)
		return (E_OPEN);
	while (true)
	{
		line = readline("> ");
		if (ft_strncmp(line, node->filename, ft_strlen(line)) == 0)
			break ;
		ft_putendl_fd(line, fd);
		free(line);
	}
	free(line);
	close(fd);
	return (EXIT_SUCCESS);
}

int	ft_trigger_heredoc(t_data *data)
{
	int			i;
	int			ret;
	t_redir_lst	*current;

	i = -1;
	while (++i < data->nb_of_cmds)
	{
		if (data->redirections[i])
		{
			current = data->redirections[i];
			while (current != NULL)
			{
				if (current->r_type == HEREDOC)
				{
					ret = ft_handle_heredoc(current);
					if (ret)
						return (ret);
				}
				current = current->next;
			}
		}
	}
	return (EXIT_SUCCESS);
}

int	ft_init_pipes(t_data *data, t_exec *exec)
{
	int	nb_of_pipes;
	int	i;

	nb_of_pipes = data->nb_of_cmds - 1;
	exec->pipes = (int **)malloc(nb_of_pipes * sizeof(int *));
	if (!exec->pipes)
		return (E_MEM);
	i = -1;
	while (++i < nb_of_pipes)
	{
		exec->pipes[i] = (int *)malloc(2 * sizeof(int));
		if (!exec->pipes[i])
			return (E_MEM);
		if (pipe(exec->pipes[i]) == F_ERROR)
			return (E_MEM);
	}
	return (EXIT_SUCCESS);
}

int	ft_executor(t_data *data)
{
	t_exec		*exec;
	t_cmd		*current_cmd;
	int			i;

	i = 0;
	exec = ft_init_exec(data);
	if (!exec)
		return (E_MEM);
	g_exit_code = ft_init_pipes(data, exec);
	if (g_exit_code)
		return (g_exit_code);
	exec->tmpin = dup(STDIN_FILENO);
	exec->tmpout = dup(STDOUT_FILENO);
	if (exec->tmpin == -1 || exec->tmpout == -1)
		return (E_DUP);
	if (data->nb_of_cmds == 1)
	{
		ft_trigger_heredoc(data);
		g_exit_code = ft_open_redir_in(data, data->cmd, exec);
		if (g_exit_code)
			return (g_exit_code);
		g_exit_code = ft_open_redir_out(data, data->cmd, exec);
		if (g_exit_code)
			return (g_exit_code);
		if (!g_exit_code)
		{
			if (!data->cmd->is_builtin)
				ft_cmd_exec(data);
			else
				ft_builtin(data, data->cmd);
		}
		if (dup2(exec->tmpin, STDIN_FILENO) == -1)
			return (E_DUP);
		if (dup2(exec->tmpout, STDOUT_FILENO) == -1)
			return (E_DUP);
		close(exec->tmpin);
		close(exec->tmpout);
	}
	else
	{
		current_cmd = data->cmd;
		while (current_cmd)
		{
			ft_trigger_heredoc(data);
			g_exit_code = ft_open_redir_in(data, current_cmd, exec);
			if (g_exit_code)
				return (g_exit_code);
			g_exit_code = ft_open_redir_out(data, data->cmd, exec);
			if (g_exit_code)
				return (g_exit_code);
			exec->child_pid[i] = fork();
			exec->status = 0;
			if (exec->child_pid[i] == F_ERROR)
				perror("fork");
			if (exec->child_pid[i] == FORKED_CHILD)
			{
				if (current_cmd->left && exec->fdin == NOT_INIT)
				{
					if (dup2(exec->pipes[i - 1][0], STDIN_FILENO) == F_ERROR)
						return (E_DUP);
					close(exec->pipes[i - 1][1]);
				}
				if (current_cmd->right && exec->fdout == NOT_INIT)
				{
					if (dup2(exec->pipes[i][1], STDOUT_FILENO) == F_ERROR)
						return (E_DUP);
					close(exec->pipes[i][0]);
				}
				if (!current_cmd->is_builtin)
					execute_command(data, current_cmd);
				else
					ft_builtin(data, current_cmd);
				exit(EXIT_SUCCESS);
			}
			i++;
			current_cmd = current_cmd->right;
			exec->fdin = -1;
			exec->fdout = -1;
		}
		dup2(exec->tmpin, STDIN_FILENO);
		dup2(exec->tmpout, STDOUT_FILENO);
		close(exec->tmpin);
		close(exec->tmpout);
		
		
		// Close all parent pipes ends
		int j = -1;
		while (++j < data->nb_of_cmds - 1)
		{
			close(exec->pipes[j][0]);
			close(exec->pipes[j][1]);
		}
	}
	while (i--)
	{
		exec->child_pid[i] = waitpid(0, &exec->status, 0);
	}
	if (WIFSIGNALED(exec->status))
		printf("^\\Quit: %d\n", SIGQUIT);
	return (EXIT_SUCCESS);
}

// FREE PIPES HERE OR IN CLEAN, ETC.