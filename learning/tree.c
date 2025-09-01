/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 09:48:06 by mdahani           #+#    #+#             */
/*   Updated: 2025/09/01 09:48:07 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

typedef struct s_tree
{
    int value;
    struct s_tree *left; 
    struct s_tree *right; 
}t_tree;

t_tree *new_node(int value)
{
    t_tree *new_node = malloc(sizeof(t_tree));
    new_node->value = value;
    new_node->left = NULL;
    new_node->right = NULL;
    return (new_node);
}

void free_tree(t_tree *node)
{
    if (!node)
        return ;
    if (node->left)
        free_tree(node->left);
    if (node->right)
        free_tree(node->right);
    free(node);
}


// let try to create a simple tree like this
/*
   0
  / \                
 1  2

*/

int main ()
{
    t_tree *node = new_node(0);
    node->left = new_node(1);
    node->right = new_node(2);

    printf("value of root: %d\n", node->value);
    printf("value of first left: %d\n", node->left->value);
    printf("value of first right: %d\n", node->right->value);

    free_tree(node);
}

