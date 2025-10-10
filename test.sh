#!/bin/bash

# Helper: compare actual file content to expected
check_output() {
    local file="$1"
    local expected="$2"
    if [ -f "$file" ]; then
        actual=$(cat "$file")
    else
        actual=""
    fi
    if [ "$actual" = "$expected" ]; then
        echo "PASS: $file"
    else
        echo "FAIL: $file"
        echo "Expected: '$expected'"
        echo "Actual  : '$actual'"
    fi
}

rm -f out err both heredoc_file pipe_out

echo "=== Testing stdout redirections > and >> ==="
./minishell <<< "echo hi > out"
check_output out "hi"
./minishell <<< "echo again >> out"
check_output out "hi
again"

echo "=== Testing numeric stdout 1> and 1>> ==="
./minishell <<< "echo hello 1> out"
check_output out "hello"
./minishell <<< "echo world 1>> out"
check_output out "hello
world"

echo "=== Testing numeric stderr 2> and 2>> ==="
./minishell <<< "ls missing_file 2> err"
err_expected="ls: cannot access 'missing_file': No such file or directory"
check_output err "$err_expected"
./minishell <<< "ls another_missing 2>> err"
err_expected="$err_expected
ls: cannot access 'another_missing': No such file or directory"
check_output err "$err_expected"

echo "=== Testing combined stdout+stderr &> and &>> ==="
./minishell <<< "echo hi &> both"
check_output both "hi"
./minishell <<< "ls missing_file &>> both"
both_expected="hi
ls: cannot access 'missing_file': No such file or directory"
check_output both "$both_expected"
./minishell <<< "echo again &>> both"
both_expected="$both_expected
again"
check_output both "$both_expected"

echo "=== Testing heredoc redirection ==="
./minishell <<< "cat << EOF > heredoc_file
line1
line2
EOF"
check_output heredoc_file "line1
line2"

echo "=== Testing pipe + outfile ==="
./minishell <<< "echo hi | cat > pipe_out"
check_output pipe_out "hi"

echo "=== Testing numeric stderr + stdout + pipe ==="
./minishell <<< "echo hello | ls missing 2> err 1> out"
check_output out "hello"
err_expected="ls: cannot access 'missing': No such file or directory"
check_output err "$err_expected"

echo "=== Cleanup ==="
rm -f out err both heredoc_file pipe_out

