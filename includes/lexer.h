/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 13:39:02 by gdelvign          #+#    #+#             */
/*   Updated: 2024/03/19 09:30:53 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

enum e_quotes_char
{
	DBL_Q = 34,
	SGL_Q = 39
};

enum e_redirect_op
{
	REDIR_IN = '<',
	REDIR_OUT = '>',
	PIPE = '|'
};

typedef enum e_tok_type
{
	WORD,
	OPERATOR
}	t_tok_type;

typedef struct s_tok_lst
{
	int					id;
	char				*token;
	t_tok_type			type;
	struct s_tok_lst	*next;
	struct s_tok_lst	*prev;
}	t_tok_lst;

/* Lexer utils to handle t_tok_lst : lexer_utils.c */
int		ft_add_tok_node(char *str, int id, t_tok_type type, t_tok_lst **lst);

/* Lexer utils to handle the storing of tokens : lexer_utils2.c */
bool	ft_is_quote(char c);
bool	ft_is_space(int c);
bool	ft_is_operator(char c);
int		ft_skip_whitespaces(char *str);

#endif
