/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct _to_pointer.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:13 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 15:01:14 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



/*5. count_nodes(t_env *head)
   - Purpose: Counts the number of nodes in a t_env linked list.
   - Variables:
     * head: iterates through the list.
     * count: counter for nodes.
   - Called from: struct_to_envp().
   - Calls: None.*/

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

/*6. add_strings(t_env *head, char **envp)
   - Purpose: Converts a t_env linked list into a char ** array of strings.
   - Variables:
     * head: start of linked list to convert.
     * temp: iterator through linked list.
     * i: index in envp array.
     * buff: size of string buffer to allocate (key + value + '=' + '\0').
   - Called from: struct_to_envp().
   - Calls: malloc(), ft_strlen(), ft_strlcpy(), ft_strlcat(), free_env_struct() on failure.*/
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
			return (1);
		}
		ft_strlcpy(envp[i], temp->key, buff);
		ft_strlcat(envp[i], "=", buff);
		ft_strlcat(envp[i], temp->value, buff);
		i++;
		temp = temp->next;
	}
	return(0);
}

/*7. struct_to_envp(t_env *head)
   - Purpose: Converts a t_env linked list into a null-terminated array of strings (char **).
   - Variables:
     * head: linked list to convert.
     * envp: resulting array of strings.
     * struct_size: number of nodes in linked list.
   - Called from: Anywhere you need envp-style array from a linked list.
   - Calls: count_nodes(), add_strings(), free_envp_array() on failure.*/

char **struct_to_envp(t_env *head)
{
	char **envp;
	int struct_size;

	struct_size = count_nodes(head);
	envp = malloc(sizeof(char *) * (struct_size + 1));
	if (!envp)
		return(NULL);
	envp[struct_size] = NULL;
	if (add_strings(head, envp) == 1)
	{
		free_envp_array(envp);
		return(NULL);
	}
	return(envp);
}