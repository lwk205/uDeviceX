#!/usr/bin/awk -f

BEGIN {
    sc = ARGV[1]; shift()
}

function vertp() {
    return (NF == 3) && NR > 6
}

vertp() {
    x = $1; y = $2; z = $3
    x *= sc; y *= sc; z *= sc
    print x, y, z
}

{ print }

function shift(  i) { for (i = 2; i < ARGC; i++) ARGV[i-1] = ARGV[i]; ARGC-- }
