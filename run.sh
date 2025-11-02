#!/bin/bash

assert() {
    input=$1
    expected=$2

    actual=$(./asari-lox "$input")

    if [ "$actual" = "$expected" ]; then
        echo "$input => $expected"
    else
        echo "$input => $expected but, got $actual"
        exit 1
    fi
}

assert "" ""
assert "(){}" "(){}"
assert "!=" "!="
assert "!" "!"
assert "=" "="
assert "==" "=="
assert "<" "<"
assert "<=" "<="
assert ">" ">"
assert ">=" ">="
assert "/" "/"
assert "// comment" ""
assert $'// co\n/' "/"
assert $'\"this is a string\"' "this is a string"
assert "123.456 // number" "123.456"
assert "   and " "and"
assert "   fun " "fun"

echo "Ok"