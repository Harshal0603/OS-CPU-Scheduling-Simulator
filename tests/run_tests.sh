#!/bin/sh
# Runs the simulator with piped input and compares its output with the saved expected output.
# Usage: make test   (or: sh tests/run_tests.sh from the project root)
#
# The expected files were checked by hand / against a separate reference calculation.
# The application itself contains NO hard-coded answers.

cd "$(dirname "$0")/.." || exit 1

if [ ! -x ./scheduler ]; then
    echo "Build first: make"
    exit 1
fi

pass=0
fail=0
actual=$(mktemp)

# check <test name> <input to send to the menu>
check() {
    name=$1
    printf "$2" | ./scheduler > "$actual"
    if diff -u "tests/expected/$name.out" "$actual" > /dev/null; then
        echo "PASS  $name"
        pass=$((pass + 1))
    else
        echo "FAIL  $name"
        diff -u "tests/expected/$name.out" "$actual" | head -20
        fail=$((fail + 1))
    fi
}

# Menu input: 2 = load file, 7 = run all, <quantum>, 8 = exit
check test1_normal              '2\ntests/inputs/test1_normal.txt\n7\n2\n8\n'
check test2_all_arrive_at_0     '2\ntests/inputs/test2_all_arrive_at_0.txt\n7\n3\n8\n'
check test3_idle_time           '2\ntests/inputs/test3_idle_time.txt\n7\n2\n8\n'
check test4_rr_small_quantum    '2\ntests/inputs/test4_rr_small_quantum.txt\n7\n1\n8\n'
check test5_single_task         '2\ntests/inputs/test5_single_task.txt\n7\n10\n8\n'
# Bad input must not crash: text instead of a number, out-of-range choice, run before loading tasks.
check test6_invalid_input       'abc\n99\n3\n2\nno_such_file.txt\n8\n'

rm -f "$actual"
echo "Passed: $pass  Failed: $fail"
[ "$fail" -eq 0 ]
