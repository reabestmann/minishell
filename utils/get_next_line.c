/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 15:15:31 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 14:05:44 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* append_buffers:
**   Concatenates small_buf onto big_buf, freeing the old big_buf.
**   Returns the new combined string or NULL on allocation failure.
*/
static char	*append_buffers(char *small_buf, char *big_buf)
{
	char	*temp_buf;

	temp_buf = ft_strjoin(big_buf, small_buf);
	if (!temp_buf)
		return (free(big_buf), NULL);
	free(big_buf);
	return (temp_buf);
}

/* get_line:
**   Extracts the next line (up to and including '\n') from big_buf.
**   Returns the line string, or NULL if big_buf is empty.
*/
static char	*get_line(char *big_buf)
{
	size_t	len;
	char	*line;

	len = 0;
	if (!big_buf || *big_buf == '\0')
		return (NULL);
	while (big_buf[len] && big_buf[len] != '\n')
		len++;
	if (big_buf[len] == '\n')
		len++;
	line = ft_substr(big_buf, 0, len);
	if (!line)
		return (NULL);
	return (line);
}

/* read_into_buffer:
**   Reads from fd in chunks into big_buf until a newline is found or EOF.
**   Returns updated big_buf, or NULL on read/alloc failure.
*/
static char	*read_into_buffer(char	*big_buf, int fd)
{
	char	*small_buf;
	int		bytes_read;

	if (!big_buf)
		big_buf = ft_strdup("");
	small_buf = malloc(BUFFER_SIZE + 1);
	if (!small_buf)
		return (free(big_buf), NULL);
	bytes_read = read(fd, small_buf, BUFFER_SIZE);
	while (bytes_read > 0)
	{
		small_buf[bytes_read] = '\0';
		big_buf = append_buffers(small_buf, big_buf);
		if (ft_strchr(big_buf, '\n'))
			break ;
		bytes_read = read(fd, small_buf, BUFFER_SIZE);
	}
	free(small_buf);
	if (bytes_read == -1)
		return (free(big_buf), NULL);
	if (bytes_read == 0 && (!big_buf || *big_buf == '\0'))
		return (free(big_buf), NULL);
	return (big_buf);
}

/* get_next_line:
**   Returns the next line from fd each call.
**   Keeps leftover data in a static buffer for subsequent calls.
*/
char	*get_next_line(int fd)
{
	static char	*big_buf;
	char		*temp_buf;
	char		*line;
	size_t		len;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	big_buf = read_into_buffer(big_buf, fd);
	if (!big_buf)
		return (NULL);
	line = get_line(big_buf);
	len = ft_strlen(line);
	temp_buf = ft_substr(big_buf, len, ft_strlen(big_buf) - len);
	free(big_buf);
	big_buf = temp_buf;
	return (line);
}
