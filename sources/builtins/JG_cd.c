/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JG_cd_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgasparo <jgasparo@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 13:03:27 by jgasparo          #+#    #+#             */
/*   Updated: 2024/01/24 12:23:21 by jgasparo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <dirent.h>

/*
**	devoir changer la var PWD avec la nouvelle dest
*/

// void	cd_builtins(char *str, char **env)
// {
// 	char	*pos;
// 	char	*here = NULL;
// 	char	*final = NULL;
// 	(void)env;
	
// 	pos = getenv("PWD");
// 	if (!chdir(str))
// 	{
// 		opendir(str);
// 	}
// 	printf("on est ici -> %s\n", getcwd(here, 0));
// 	final = ft_strjoin(pos, str);
// 	printf("final pos -> %s\n", final);
	
// }

/* besoin de */
void	cd_builtin(char **path)
{
	if (chdir(path[1]) == -1)
		perror("chdir()");
}