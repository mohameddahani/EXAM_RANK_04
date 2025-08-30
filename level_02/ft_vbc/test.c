/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <student.42.fr>                   +#+  +:+       +#+        */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum { ADD, MULTI, VAL } type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return NULL;
    *ret = n;
    return ret;
}

void destroy_tree(node *n)
{
    if (!n) return;
    if (n->type != VAL) {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

static node *parse_expr(char **s); // forward

/* ---------- parsing ---------- */

static node *parse_primary(char **s)
{
    if (**s == '(') {
        (*s)++;
        node *res = parse_expr(s);
        if (!res || **s != ')') {
            destroy_tree(res);
            unexpected(**s ? **s : 0);
            return NULL;
        }
        (*s)++;
        return res;
    }
    if (isdigit((unsigned char)**s)) {
        node tmp = { .type = VAL, .val = **s - '0', .l = NULL, .r = NULL };
        (*s)++;
        return new_node(tmp);
    }
    unexpected(**s ? **s : 0);
    return NULL;
}

static node *parse_multiplication(char **s)
{
    node *left = parse_primary(s);
    while (left && **s == '*') {
        (*s)++;
        node *right = parse_primary(s);
        if (!right) { destroy_tree(left); return NULL; }
        node tmp = { .type = MULTI, .val = 0, .l = left, .r = right };
        left = new_node(tmp);
        if (!left) { destroy_tree(right); return NULL; }
    }
    return left;
}

static node *parse_addition(char **s)
{
    node *left = parse_multiplication(s);
    while (left && **s == '+') {
        (*s)++;
        node *right = parse_multiplication(s);
        if (!right) { destroy_tree(left); return NULL; }
        node tmp = { .type = ADD, .val = 0, .l = left, .r = right };
        left = new_node(tmp);
        if (!left) { destroy_tree(right); return NULL; }
    }
    return left;
}

static node *parse_expr(char **s)
{
    node *ret = parse_addition(s);
    if (!ret) return NULL;
    if (**s) {
        destroy_tree(ret);
        unexpected(**s ? **s : 0);
        return NULL;
    }
    return ret;
}

/* ---------- evaluation ---------- */

int eval_tree(node *t)
{
    if (t->type == ADD)   return eval_tree(t->l) + eval_tree(t->r);
    if (t->type == MULTI) return eval_tree(t->l) * eval_tree(t->r);
    return t->val;
}

/* ---------- main ---------- */

int main(int argc, char **argv)
{
    if (argc != 2) return 1;
    char *s = argv[1];
    node *tree = parse_expr(&s);
    if (!tree) return 1;
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return 0;
}
