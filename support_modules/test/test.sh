# Support Code Tests

if [ ! -d good ]; then
    echo "test.sh must be invoked inside the test directory."
    exit 2
fi

source ./azeroth.sh

# 1:test-id 2:name 3:command
stdout() {
    if filtered $1 $(funcname); then return 0; fi
    if [ -n "$PRINT_CMD" ]; then
        echo "Test $1: ./$3 >/tmp/$2"
        return 0
    fi
    announce $1 $(funcname) "$3"

    if [ -n "$VALGRIND_CMD" ]; then
        valgrind --leak-check=full ./$3 >/tmp/$2.vg 2>&1
        ES=$?
        if [[ $ES -ne 0 ]]; then
            fail valgrind
            return 1
        fi
        if [ -z "$KEEP_OUTPUT" ]; then
            rm /tmp/$2.vg
        fi
        pass
        return 0
    fi

    ./$3 >/tmp/$2
    ES=$?
    if [[ $ES -ne 0 ]]; then
        fail "command $ES"
        return 1
    fi

    if ! diff /tmp/$2 good/$2; then
        fail diff
        return 1
    fi

    if [ -z "$KEEP_OUTPUT" ]; then
        rm /tmp/$2
    fi
    pass
}

stdout  1 t01-rqueue "rqueueTest"
stdout  2 t02-btree2 "btree2Test"

report
