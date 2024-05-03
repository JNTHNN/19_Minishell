/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgasparo <jgasparo@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 19:14:15 by jgasparo          #+#    #+#             */
/*   Updated: 2024/05/03 23:51:38 by jgasparo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/*
**	checker for cd ~ | return 0 if error
*/
int	ft_check_tilde(t_cd *cd)
{
	if (!cd->home)
	{
		ft_putstr_fd(ERR_HOME, 2);
		ft_errno(NULL, 1, cd->data, false);
		return (0);
	}
	if (chdir(ft_strjoin(cd->home + 5, cd->dir + 1)) == -1)
	{
		ft_errno("cd", 1, cd->data, false);
		return (0);
	}
	return (1);
}

/*
**	checker for cd | return 0 if error
*/
int	ft_check_home(t_cd *cd)
{
	if (!cd->home)
	{
		ft_putstr_fd(ERR_HOME, 2);
		ft_errno(NULL, 1, cd->data, false);
		return (0);
	}
	if (chdir(cd->home + 5) == -1)
	{
		ft_errno("cd", 1, cd->data, false);
		return (0);
	}
	return (1);
}

/*
**	checker for cd - | return 0 if error
*/
int	ft_check_minus(t_cd *cd)
{
	if (!cd->oldpwd)
	{
		ft_putstr_fd(ERR_OLDPWD, 2);
		ft_errno(NULL, 1, cd->data, false);
		return (0);
	}
	if (chdir(cd->oldpwd + 7) == -1)
	{
		ft_errno("cd", 1, cd->data, false);
		return (0);
	}
	return (1);
}
