/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohameddahani <mohameddahani@student.42.fr> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 19:54:04 by mohameddahani     #+#    #+#             */
/*   Updated: 2025/09/01 19:54:04 by mohameddahani    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <ctype.h>

int unexpected(char c)
{
    if (c == '\0')
        printf("Unexpected end of input\n");
    else
        printf("Unexpected token '%c'\n", c);
    return -1;
}

int eval(const char *s, int *i)
{
    int result = 0;
    int term = 1;
    int factor;
    
    while (s[*i] && s[*i] != ')')
    {
        // Parse factor
        if (s[*i] == '(')
        {
            (*i)++;
            factor = eval(s, i);
            if (factor == -1)
                return -1;
            if (s[*i] != ')')
                return unexpected(s[*i]);
            (*i)++;
        }
        else if (isdigit(s[*i]))
        {
            factor = s[*i] - '0';
            (*i)++;
        }
        else
            return unexpected(s[*i]);
        
        // Apply multiplication
        term *= factor;
        
        // Check next character
        if (s[*i] == '*')
        {
            (*i)++;
            if (!s[*i] || s[*i] == ')' || s[*i] == '+' || s[*i] == '*')
                return unexpected(s[*i]);
        }
        else
        {
            // End of multiplication chain, add to result
            result += term;
            term = 1;
            
            if (s[*i] == '+')
            {
                (*i)++;
                if (!s[*i] || s[*i] == ')' || s[*i] == '+' || s[*i] == '*')
                    return unexpected(s[*i]);
            }
            else
                break;
        }
    }
    
    return result;
}

int main(int argc, char **argv)
{
    int i = 0;
    int result;

    if (argc != 2)
        return 1;

    if (!argv[1][0] || (!isdigit(argv[1][0]) && argv[1][0] != '('))
        return (unexpected(argv[1][0]), 1);

    result = eval(argv[1], &i);
    
    if (result == -1)
        return 1;
        
    if (argv[1][i])
        return (unexpected(argv[1][i]), 1);

    printf("%d\n", result);
    return 0;
}
