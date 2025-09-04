#include "../minishell.h"
/*Perfect! Let’s think about it step by step — conceptually first, then you can write the code yourself.

1️⃣ Decide what your struct looks like

For a linked list holding environment variables, each node should store:

typedef struct s_env
{
    char *key;          // The name, e.g., "PATH"
    char *value;        // The value, e.g., "/usr/bin:/bin"
    struct s_env *next; // Pointer to the next node
} t_env;


key → everything before = in the envp string.

value → everything after =.

next → points to the next environment variable node.

2️⃣ Loop through envp

envp is a char ** where each element is a string like "PATH=/usr/bin:/bin".

For each string in envp, you want to:

Find the = character.

Copy the part before = into key.

Copy the part after = into value.

Create a new t_env node with these values.

Append it to your linked list.

3️⃣ Creating and appending nodes

Use malloc to allocate memory for each node.

Either maintain a head pointer and a tail pointer to append easily, or always append by walking to the end.

Example conceptual steps (pseudo-code):

head = NULL
for each string in envp:
    split string into key/value at '='
    node = malloc t_env
    node->key = strdup(key)
    node->value = strdup(value)
    node->next = NULL
    append node to linked list

4️⃣ Things to keep in mind

Use ft_strdup (from your libft) to copy strings.

Don’t forget to set node->next = NULL.

Free the list properly when your shell exits.

If an environment variable has no value (like "SOMEVAR="), store an empty string for value.

If you want, I can also sketch exactly how the loop and node creation would look in code, but very cleanly, so you can implement it without trying to manipulate envp[i][j] manually.

Do you want me to do that next?*/