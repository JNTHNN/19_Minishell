/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgasparo <jgasparo@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 13:03:27 by jgasparo          #+#    #+#             */
/*   Updated: 2024/05/03 23:43:22 by jgasparo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/*
**	change pwd in env according to path type (without arg; absolute; relative)
*/
static void	ft_change_pwd(t_data *data, t_cmd *cmd)
{
	char	*pwd;
	// static char	cwd[PATH_MAX];

	pwd = ft_getenv(data, "PWD");
	// if (!pwd)
	// {
	// 	if (getcwd(cwd, sizeof(cwd)) != NULL)
	// 		pwd = cwd;
	// 	printf("start pwd = %s\n", cwd);
	// }
	if (!cmd->args[1])
		ft_cd_home(data, pwd);
	else if (cmd->args[1][0] == '/')
		ft_cd_absolute(data, cmd, pwd);
	else
		ft_cd_relative(data, cmd, pwd);
}

/*
**	check arg after cd ( - ; ~ ; path)
*/
static int	ft_check_dir(t_cd *cd)
{
	int		rv;

	rv = 0;
	if (cd->dir)
	{
		// if (!ft_strncmp(cd->dir, "-", 1))
		// 	return (rv = ft_check_minus(cd));
		if (!ft_strncmp(cd->dir, "~", 1))
			rv = ft_check_tilde(cd);
		// else if (chdir(cmd->args[1]) == 0)
		// 	rv = 1;
		// else
		// 	ft_errno(cmd->args[1], 1, data, false);
	}
	if (!cd->dir)
		rv = ft_check_home(cd);
	return (rv);
}

t_cd	*ft_init_cd(t_data *data, t_cmd *cmd)
{
	t_cd		*cd;
	static char	cwd[PATH_MAX];

	cd = (t_cd *)malloc(sizeof(t_cd));
	cd->data = data;
	cd->cmd = cmd;
	cd->dir = cmd->args[1];
	cd->pwd = getcwd(cwd, sizeof(cwd));
	if (!cd->pwd)
		perror("pwd"); // determiner la sortie d'erreur
	cd->oldpwd = ft_getenv(data, "OLDPWD=");
	cd->home = ft_getenv(data, "HOME=");
	return (cd);
}
// a determiner suivant ce que j'ai besoin
// void	ft_cd_error(t_cd *cd, char *s, int flag)

/*
**	if the direction works -> we change pwd + oldpwd
*/
void	ft_cd(t_data *data, t_cmd *cmd)
{
	t_cd	*cd;

	cd = ft_init_cd(data, cmd);
	if (ft_check_dir(cd))
		ft_change_pwd(data, cmd);
}
