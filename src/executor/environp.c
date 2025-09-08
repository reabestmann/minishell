#include "../minishell.h"

void free_env_struct(t_env *head)
{
	t_env *temp;

	while(head)
	{
		temp = head->next;
		free(head->key);
		free(head->value);
		free(head);
		head = temp;	
	}
}

void free_envp_array(char **envp)
{
	int i;

	i = 0;
	while(envp[i])
	{
		free(envp[i]);
		i++;	
	}
	free(envp);
}

t_env *add_nodes(t_env *current, char *equals, char *env_str)
{
	t_env *new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return(NULL);
	new_node->key = ft_substr(env_str, 0, equals - env_str);
	if(!new_node->key)
	{
		free(new_node);
		return(NULL);
	}
	new_node->value = ft_strdup(equals + 1);
	if(!new_node->value)
	{
		free(new_node->key);
		free(new_node);
		return(NULL);
	}
	new_node->next = NULL;
	if(current)
		current->next = new_node;
	return(new_node);
}

t_env *envp_to_struct(char **envp)
{
	char *equals;
	int i;
	t_env *head;
	t_env *current;

	head = NULL;
	current = NULL;
	i = 0;		
	while(envp[i])
	{
		equals = ft_strchr(envp[i], '=');
		if(equals)
		{
			current = add_nodes(current, equals, envp[i]);
			if (!head)
				head = current;
			if(!current)
			{
				free_env_struct(head);
				return(NULL);
			}
		}
		i++;
	}
	return(head);
}

int count_nodes(t_env *head)
{
	int count;

	count = 0;
	while(head)
	{
		count++;
		head = head->next;
	}
	return(count);
}

/*reminder that I will need to free struct later*/
int add_strings(t_env *head, char **envp)
{
	int i;
	int buff;
	t_env *temp;

	i = 0;
	temp = head;
	while(temp)
	{
		buff = (ft_strlen(temp->key) + ft_strlen(temp->value) + 2);
		envp[i] = malloc(buff);
		if(!envp[i])
		{
			while(--i >= 0)
				free(envp[i]);
			return (0);
		}
		ft_strlcpy(envp[i], temp->key, buff);
		ft_strlcat(envp[i], "=", buff);
		ft_strlcat(envp[i], temp->value, buff);
		i++;
		temp = temp->next;
	}
	return(1);
}

char **struct_to_envp(t_env *head)
{
	char **envp;
	int struct_size;

	struct_size = count_nodes(head);
	envp = malloc(sizeof(char *) * (struct_size + 1));
	if (!envp)
		return(NULL);
	envp[struct_size] = NULL;
	if (!add_strings(head, envp))
	{
		free_envp_array(envp);
		return(NULL);
	}
	return(envp);
}
/*
======================== ENV STRUCT / ENVP HELPERS ========================

1. free_env_struct(t_env *head)
   - Purpose: Frees a linked list of t_env nodes, including keys and values.
   - Variables:
     * head: pointer to the first node of the linked list; iterates through list.
     * temp: temporary pointer to store next node while freeing current node.
   - Called from: Anywhere you need to free a t_env linked list (e.g., on error or program exit).
   - Calls: free().

2. free_envp_array(char **envp)
   - Purpose: Frees a dynamically allocated array of environment strings.
   - Variables:
     * envp: array of strings.
     * i: index to iterate through envp.
   - Called from: Anywhere you need to free a char ** representing the environment.
   - Calls: free().

3. add_nodes(t_env *current, char *equals, char *env_str)
   - Purpose: Creates a new t_env node from a single environment string and links it to the current node.
   - Variables:
     * current: last node in the existing list; new node is attached here.
     * equals: pointer to '=' character in env_str.
     * env_str: full environment string ("KEY=VALUE").
     * new_node: newly allocated node being created.
   - Called from: envp_to_struct().
   - Calls: malloc(), ft_substr(), ft_strdup().

4. envp_to_struct(char **envp)
   - Purpose: Converts a char ** (environment array) into a linked list of t_env nodes.
   - Variables:
     * envp: array of strings to convert.
     * i: index for iterating envp.
     * head: pointer to first node of linked list.
     * current: last node in the list; used for appending.
     * equals: pointer to '=' character in current env string.
   - Called from: Anywhere you need a linked list representation of envp.
   - Calls: add_nodes(), free_env_struct(), ft_strchr().

5. count_nodes(t_env *head)
   - Purpose: Counts the number of nodes in a t_env linked list.
   - Variables:
     * head: iterates through the list.
     * count: counter for nodes.
   - Called from: struct_to_envp().
   - Calls: None.

6. add_strings(t_env *head, char **envp)
   - Purpose: Converts a t_env linked list into a char ** array of strings.
   - Variables:
     * head: start of linked list to convert.
     * temp: iterator through linked list.
     * i: index in envp array.
     * buff: size of string buffer to allocate (key + value + '=' + '\0').
   - Called from: struct_to_envp().
   - Calls: malloc(), ft_strlen(), ft_strlcpy(), ft_strlcat(), free_env_struct() on failure.

7. struct_to_envp(t_env *head)
   - Purpose: Converts a t_env linked list into a null-terminated array of strings (char **).
   - Variables:
     * head: linked list to convert.
     * envp: resulting array of strings.
     * struct_size: number of nodes in linked list.
   - Called from: Anywhere you need envp-style array from a linked list.
   - Calls: count_nodes(), add_strings(), free_envp_array() on failure.

============================================================================
*/
