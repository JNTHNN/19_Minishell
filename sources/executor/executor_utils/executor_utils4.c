/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgasparo <jgasparo@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 15:05:20 by gdelvign          #+#    #+#             */
/*   Updated: 2024/05/21 16:39:36 by jgasparo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_create_new_line(char *old, char *new, t_data *data, size_t buffsize)
{
	char			*cursor;
	size_t			space_left;

	cursor = new;
	while (*old)
	{
		if (*old == DOLLAR)
		{
			old++;
			if (!(*old) || ft_is_space(*old) || !ft_is_valid_var_char(*old))
				*cursor++ = '$';
			else
			{
				space_left = ft_space_left(buffsize, cursor, new);
				ft_create_var_val(data, &old, &cursor, space_left);
			}
		}
		else
			*cursor++ = *old++;
	}
	*cursor = '\0';
}

static int	ft_calc_line_new_len(char *str, t_data *data)
{
	int		i;
	int		len;
	char	*start;

	start = str;
	len = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == DOLLAR)
		{
			if (!(str[i + 1]) || ft_is_space(str[i + 1])
				|| !ft_is_valid_var_char(str[i + 1]))
				ft_increment(&i, &len);
			ft_get_var_val_length(data, &str, &i, &len);
		}
		else
			ft_increment(&i, &len);
	}
	return (len);
}

static int	ft_handle_hd_expansion(char **line, t_data *data)
{
	int		new_length;
	char	*cursor;
	char	*str;

	str = ft_strdup(*line);
	if (!str)
		return (E_MEM);
	if (ft_count_dollars(str))
	{
		new_length = ft_calc_line_new_len(*line, data);
		free(*line);
		*line = (char *)malloc(new_length + 1);
		if (!*line)
			return (E_MEM);
		cursor = *line;
		ft_create_new_line(str, cursor, data, (new_length + 2));
	}
	free(str);
	return (EXIT_SUCCESS);
}

static int	ft_heredoc_process(t_redir_lst *node, t_data *data)
{
	char	*line;
	int		fd;

	ft_restore_signals(true);
	fd = open(node->hd_path, O_CREAT | O_RDWR | O_TRUNC, 0644);
	if (fd == F_ERROR)
		exit(E_OPEN);
	while (true)
	{
		line = readline("> ");
		if (!line)
			exit(20);
		if (!node->quoted)
			ft_handle_hd_expansion(&line, data);
		if ((line && line[0])
			&& ft_strncmp(line, node->filename, ft_strlen(line)) == 0)
			break ;
		ft_putendl_fd(line, fd);
		free(line);
	}
	free(line);
	close(fd);
	exit(EXIT_SUCCESS);
}

static int	ft_handle_heredoc(t_redir_lst *node, t_data *data)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == F_ERROR)
		return (ft_errno("fork", EXEC_FAIL, data), EXIT_FAILURE);
	else if (pid == FORKED_CHILD)
		ft_heredoc_process(node, data);
	else
	{
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			return (WEXITSTATUS(status));
		if (WIFSIGNALED(status))
		{
			ft_print_signals(data->exec->status);
			return (EXIT_HD);
		}
	}
	return (EXIT_SUCCESS);
}

void	ft_restore_signals(bool heredoc)
{
	struct termios	term;

	ft_memset(&term, 0, sizeof(term));
	ft_signal(SIG_DFL, heredoc);
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void	ft_print_signals(int status)
{
	if (status == SIGQUIT)
	{
		ft_putendl_fd(STR_QUIT, STDERR_FILENO);
		g_exit_code = 128 + SIGQUIT;
	}
	else
	{
		printf(CLEAR_LINE);
		g_exit_code = 128 + SIGINT;
	}
}

int	ft_trigger_heredoc(t_data *data)
{
	int			i;
	t_redir_lst	*current;
	int			ret;

	ret = 0;
	i = -1;
	while (++i < data->nb_of_cmds)
	{
		current = data->redirections[i];
		while (current != NULL)
		{
			if (current->r_type == HEREDOC)
				ret = ft_handle_heredoc(current, data);
			if (ret == EXIT_HD || ret == CTRL_D)
				break ;
			if (ret == E_OPEN)
				return (E_OPEN);
			current = current->next;
		}
		if (ret)
			break ;
	}
	data->exec->trigger_hd = true;
	return (EXIT_SUCCESS);
}
