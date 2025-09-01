/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   no_tree.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/31 16:50:11 by mdahani           #+#    #+#             */
/*   Updated: 2025/08/31 19:21:26 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <ctype.h>

int	unexpected(char c)
{
	if (!c)
		printf("Unexpected end of input\n");
	else
		printf("Unexpected token '%c'\n", c);
	return (-1);
}

int	eval(const char *s, int *i)
{
	int		result;
	int		term;
	int		factor;

	result = 0;
	
	while (s[*i] && s[*i] != ')')
	{
		term = 1;
		
		// Parse term (handle multiplication)
		while (1)
		{
			// Parse factor
			if (s[*i] == '(')
			{
				(*i)++;
				factor = eval(s, i);
				if (factor == -1)
					return (-1);
				if (s[*i] != ')')
					return (unexpected(s[*i]));
				(*i)++;
			}
			else if (isdigit(s[*i]))
			{
				factor = s[*i] - '0';
				(*i)++;
			}
			else
				return (unexpected(s[*i]));
			
			term *= factor;
			
			// Check if next operator is multiplication
			if (s[*i] == '*')
			{
				(*i)++;
				if (!s[*i])
					return (unexpected(s[*i]));
			}
			else
				break;
		}
		
		// Apply the previous operator to result
		result += term;
		
		// Check for next operator
		if (s[*i] == '+')
		{
			(*i)++;
			if (!s[*i])
				return (unexpected(s[*i]));
		}
		else
			break;
	}
    
	return (result);
}

int	main(int ac, char **av)
{
	int	res;
	int	i;

	if (ac != 2)
		return (1);
	i = 0;
	res = eval(av[1], &i);
	if (res == -1)
		return (1);
	if (av[1][i])
	{
		unexpected(av[1][i]);
		return (1);
	}
	printf("%d\n", res);
	return (0);
}
