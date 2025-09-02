#include "../minishell.h"

// Basic split by spaces
static char **split_by_spaces(char *input)
{
    int count = 0;
    char *tmp = strdup(input);
    char *token = strtok(tmp, " ");
    while (token)
    {
        count++;
        token = strtok(NULL, " ");
    }
    free(tmp);

    char **result = malloc((count + 1) * sizeof(char *));
    int i = 0;
    token = strtok(input, " ");
    while (token)
    {
        result[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    result[i] = NULL;
    return result;
}

// Temporary stub parser supporting one pipe and simple redirections
t_command parse_input(char *input)
{
    t_command cmd = {0};
    cmd.args = split_by_spaces(input);
    cmd.input_file = NULL;
    cmd.output_file = NULL;
    cmd.append_output = 0;
    cmd.next = NULL;

    // Look for simple redirection symbols
    for (int i = 0; cmd.args[i]; i++)
    {
        if (strcmp(cmd.args[i], "<") == 0 && cmd.args[i + 1])
        {
            cmd.input_file = strdup(cmd.args[i + 1]);
            cmd.args[i] = NULL; // truncate args at redirection
            break;
        }
        else if (strcmp(cmd.args[i], ">") == 0 && cmd.args[i + 1])
        {
            cmd.output_file = strdup(cmd.args[i + 1]);
            cmd.append_output = 0;
            cmd.args[i] = NULL;
            break;
        }
        else if (strcmp(cmd.args[i], ">>") == 0 && cmd.args[i + 1])
        {
            cmd.output_file = strdup(cmd.args[i + 1]);
            cmd.append_output = 1;
            cmd.args[i] = NULL;
            break;
        }
        else if (strcmp(cmd.args[i], "|") == 0 && cmd.args[i + 1])
        {
            cmd.args[i] = NULL; // split command here
            cmd.next = malloc(sizeof(t_command));
            cmd.next->args = split_by_spaces(cmd.args[i + 1]); // naive next command
            cmd.next->input_file = NULL;
            cmd.next->output_file = NULL;
            cmd.next->append_output = 0;
            cmd.next->next = NULL;
            break;
        }
    }

    return cmd;
}
