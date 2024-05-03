/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgasparo <jgasparo@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 09:19:19 by jgasparo          #+#    #+#             */
/*   Updated: 2024/05/03 20:42:47 by jgasparo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	ft_check_var(char *str)
{
	int		i;
	char	*var;

	i = 0;
	var = ft_var(str);
	if (var[i] == '=')
	{
		ft_putstr_fd(ERR_VAR_EQ, 2);
		ft_errno(NULL, 1, NULL, false);
		return (0);
	}
	if (!var || ft_isdigit(var[i]))
	{
		ft_putstr_fd(ERR_VAR_ID, 2);
		ft_errno(NULL, 1, NULL, false);
		return (0);
	}
	while (var[i])
	{
		if (!ft_isalnum(var[i]))
		{
			ft_putstr_fd(ERR_HOME, 2);
			ft_errno(NULL, 1, NULL, false);
			return (0);
		}
		i++;
	}
	return (1);
}

int	ft_export(t_data *data, t_cmd *cmd)
{
	t_env	*head;
	int		i;

	head = ft_setup_env(data->env);
	i = 1;
	if (cmd->args)
	{
		if (!ft_strncmp(cmd->args[0], "export", 6))
		{
			if (cmd->args[i])
			{
				if (!ft_check_var(cmd->args[i]))
					return (E_VAR);
				while (cmd->args[i])
				{
					ft_modify_or_add_env(&head,
						cmd->args[i]);
					i++;
				}
			}
			else
				ft_print_env(head);
		}
	}
	ft_update_env(head, data);
	return (EXIT_SUCCESS);
}
