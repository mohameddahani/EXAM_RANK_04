/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 17:07:24 by lsilva-x          #+#    #+#             */
/*   Updated: 2025/08/04 17:22:17 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
---------------- FT_ARGO ----------------

int argo(json *dst, FILE *stream);

Arguments:
- dst: Pointer to a json struct where the Abstract Syntax Tree (AST) will be built.
- stream: Input source (FILE*) to parse.

Supported types:
- Numbers (integers)
- Strings (with escaping for backslashes and quotation marks)
- Maps (key-value pairs)

Return values:
-  1 : Success
- -1 : Failure

Error handling:
- On unexpected token: prints "Unexpected token '%c'\n"
- On unexpected end of input: prints "Unexpected end of input\n"

Union explanation:
A union is a special struct that allows multiple members to share the same memory location.
Only one member can be used at a time, and the size of the union equals its largest member.
Useful for storing different types of data in the same variable, but only one at a time.
*/

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct	json {
	enum {
		MAP,
		INTEGER,
		STRING
	} type;
	union {
		struct {
			struct pair	*data;
			size_t		size;
		} map;
		int	integer;
		char	*string;
	};
}	json;

typedef struct	pair {
	char	*key;
	json	value;
}	pair;

#define BUFFER_SIZE 99999

//Given functions
int		peek(FILE *stream);
void	unexpected(FILE *stream);
int		expect(FILE *stream, char c);
void	free_json(json j);
int		argo(json *dst, FILE *stream);
int		accept(FILE *stream, char c);

int	parse_string(json *dst, FILE *stream);
int	parse_int(json *dst, FILE *stream);
int	parse_map(json *dst, FILE *stream);
int parser(json *dst, FILE *stream);
static char	*ft_strdup(char *str);


//! The initial function to handle differnt types of input 
int parser(json *dst, FILE *stream)
{
	int	c;

	c = peek(stream); //*take a single char
	if (c == '"')
		return (parse_string(dst, stream)); //* handle string type
	else if (isdigit(c) || c == '-')
		return (parse_int(dst, stream)); //* handle int type
	else if (c == '{')
		return (parse_map(dst, stream)); //* handle map type recursively
	else
	{
		//!Handle error case, EOF or unexpected format type information
		unexpected(stream);
		return (-1);
	}
}

//! auxiliary function to duplicate the string in the heap memory
static char	*ft_strdup(char *str)
{
	size_t	str_s;
	int		i;
	char	*str_cpy;
	
	i = -1;
	str_s = -1;
	while (str[++str_s])
		continue ;
	str_cpy = (char *)malloc(sizeof(char) * (str_s + 1));
	while (str[++i])
		str_cpy[i] = str[i];
	str_cpy[i] = '\0';
	return (str_cpy);
}

/**
 * Parses a JSON map (object) from the given input stream and stores the result in the provided destination json structure.
 *
 * @param dst Pointer to a json structure where the parsed map will be stored.
 * @param stream Input file stream to read the JSON map from.
 * @return Returns 1 on successful parsing, -1 on error.
 *
 * The function expects the map to start with '{' and end with '}'. It parses key-value pairs,
 * where keys are strings and values are parsed recursively. On error, allocated memory is freed
 * and -1 is returned. On success, the dst json structure is populated with the map type, size,
 * and data.
 */
int	parse_map(json *dst, FILE *stream)
{
	pair	*items; // Dynamic array to hold parsed key-value pairs
	size_t	size;   // Number of key-value pairs parsed
	json	key;     // Temporary holder for each parsed key

	// Ensure the map starts with a '{'
	if (!expect(stream, '{'))
		return (-1);

	items = NULL;
	size = 0;

	// Loop until closing '}' is found
	while (!accept(stream, '}'))
	{
		// Reallocate memory for a new pair
		items = realloc(items, sizeof(pair) * (size + 1));

		// Parse the key as a JSON string
		if (parse_string(&key, stream) == -1)
		{
			unexpected(stream); // Report error
			return (-1);
		}

		// Expect a colon ':' between key and value
		if (!expect(stream, ':'))
		{
			free(key.string); // Free key if colon is missing
			free(items);      // Free all previously parsed items
			return (-1);
		}

		// Parse the value recursively
		if (parser(&items[size].value, stream) == -1)
		{
			free(key.string); // Free key on failure
			free(items);      // Free all items
			return (-1);
		}

		// Store the key in the current pair
		items[size].key = key.string;
		size++; // Increase the count of parsed items

		// Accept a comma ',' to continue, or end if next is '}'
		if (!accept(stream, ',') && peek(stream) != '}')
		{
			unexpected(stream); // Invalid token found
			free(items);        // Clean up
			return (-1);
		}

		// Set the resulting JSON structure as a map with parsed data
		dst->type = MAP;
		dst->map.size = size;
		dst->map.data = items;
		return (1); // Success
	}
}

//! Parse int type function
int	parse_int(json *dst, FILE *stream)
{
	int		n;

	if (fscanf(stream, "%d", &n) == 1)
	{
		dst->type = INTEGER;
		dst->integer = n;
		return (1);
	}
	//* unexpected int format
	unexpected(stream);
	return (-1);
}

//!Parse string informartion
int	parse_string(json *dst, FILE *stream)
{
	char	buffer[BUFFER_SIZE];
	char	c;
	int		i;

	if (!expect(stream, '"')) //!handle cases where the data is not surrounded by double quouetes
		return (-1);
	i = 0;
	while(1)
	{
		c = getc(stream); //* Take a single char
		if (c == EOF)
		{
			//*EOF, not expect end of string
			unexpected(stream);
			return (-1);
		}
		//!normal end of function
		if (c == '"')
			break ;
		//!handle escape characters
		if (c == '\\')
		{
			c = getc(stream);
			if (c == EOF)
			{
				//*EOF, not expect end of string
				unexpected(stream);
				return (-1);
			}
		}
		buffer[i++] = c;
	}
	buffer[i] = '\0';
	dst->type = STRING;
	dst->string = ft_strdup(buffer); // ! dup the string to the binary tree
	return (1);
}


//============================== Given code =================

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

void	free_json(json j)
{
	switch (j.type)
	{
		case MAP:
			for (size_t i = 0; i < j.map.size; i++)
			{
				free(j.map.data[i].key);
				free_json(j.map.data[i].value);
			}
			free(j.map.data);
			break ;
		case STRING:
			free(j.string);
			break ;
		default:
			break ;
	}
}

void	serialize(json j)
{
	switch (j.type)
	{
		case INTEGER:
			printf("%d", j.integer);
			break ;
		case STRING:
			putchar('"');
			for (int i = 0; j.string[i]; i++)
			{
				if (j.string[i] == '\\' || j.string[i] == '"')
					putchar('\\');
				putchar(j.string[i]);
			}
			putchar('"');
			break ;
		case MAP:
			putchar('{');
			for (size_t i = 0; i < j.map.size; i++)
			{
				if (i != 0)
					putchar(',');
				serialize((json){.type = STRING, .string = j.map.data[i].key});
				putchar(':');
				serialize(j.map.data[i].value);
			}
			putchar('}');
			break ;
	}
}

int	argo(json *dst, FILE *stream)
{
	return (parser(dst, stream));
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	char *filename = argv[1];
	FILE *stream = fopen(filename, "r");
	json	file;
	if (argo (&file, stream) != 1)
	{
		free_json(file);
		return 1;
	}
	serialize(file);
	printf("\n");
}