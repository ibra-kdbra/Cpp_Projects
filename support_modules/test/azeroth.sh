#---------------------------------------
# SH Test Framework v1.0

# To write a test you must call filtered, announce, pass, and fail from
# your test function.  The following psuedocode shows a minimal example:
#
# # 1:test-id 2:test-input
# my_test() {
#   if filtered $1 $(funcname); then return 0; fi
#   announce $1 $(funcname) <brief description>
#   <execute the test>
#   if <test failed>; then
#       fail <reason for failure>
#       return 1
#   fi
#   pass
# }
#
# my_test 20 test-file


declare -A FILTER   # Requires bash 4 or zsh.

TOTAL=0
PASSED=0
FAILURES=()
KEEP_OUTPUT=""
PRINT_CMD=""
VALGRIND_CMD=""

UP=""
RED=""
GRN=""
DFG=""
spaces="                                                        "

# Call to declare the beginning of a test.
# 1:test-id 2:test-type [3:description]
announce() {
    TOTAL=$((TOTAL+1))
    TEST_ID=$1
    AMSG="$@"
    if [ -n "$UP" ]; then
        echo $AMSG
    fi
    SECONDS=0
}

# Private function to set TMS to elapsed test time in <min>:<sec>.
az_elapsed() {
    TMS=$SECONDS
    TMS=`printf "%d:%02d" $(($TMS / 60)) $(($TMS % 60))`
}

# Call after announce to declare that test is successful.
pass() {
    PASSED=$((PASSED+1))
    az_elapsed
    echo -e "$UP$AMSG${spaces:${#AMSG}}$GRN $TMS OK$DFG"
}

# Call after announce to declare that test failed.
# 1:failure-message
fail() {
    FAILURES+=($TEST_ID)
    az_elapsed
    echo -e "$UP$AMSG${spaces:${#AMSG}}$RED $TMS FAIL: $1$DFG"
}

# Print summary of test results.
report() {
    if [ -z "$PRINT_CMD" ]; then
        echo -e "\n$PASSED of $TOTAL tests pass."
    fi
    if [ "$TOTAL" -ne "$PASSED" ]; then
        echo "FAILURES: ${FAILURES[@]}"
    fi
    [ "$TOTAL" -eq "$PASSED" ]
}

# 1:test-id 2:test-type
# Return non-zero if this test should not be run.
filtered() {
    if [[ ${#FILTER[@]} -eq 0 ]]; then return 1; fi
    if [[ ${FILTER[$2]} == "Y" ]]; then return 1; fi
    if [[ ${FILTER[$1]} == "Y" ]]; then return 1; fi
    return 0
}

if [[ -n $ZSH_VERSION ]]; then
    setopt KSH_ARRAYS BASH_REMATCH  # Emulate Bash regex.
    funcname() { echo ${funcstack[1]}; }
else
    funcname() { echo ${FUNCNAME[1]}; }
fi

for arg in "$@"
do
    case $arg in
        -h)
            echo "Usage: $0 [-c] [-h | -p | -v] <test-id> <test-id> ..."
            echo -e "\nTest-id may be a dash separated range or type name.\n"
            echo "Options:"
            echo "  -c  Enable terminal coloring."
            echo "  -h  Show this help and exit."
            echo "  -k  Keep output when tests pass."
            echo "  -p  Print test commands without running tests."
            echo "  -v  Run valgrind on commands."
            exit 0
            ;;
        -c)
            UP="\033[1A"
            RED="\033[31m"
            GRN="\033[32m"
            DFG="\033[39m"
            ;;
        -k)
            KEEP_OUTPUT=1
            ;;
        -p)
            PRINT_CMD=1
            ;;
        -v)
            VALGRIND_CMD=1
            ;;
        *)
            # NOTE: Must use "pat" variable to work around Bash 4.x bug with
            # regex literals.
            pat='([[:digit:]]*)-([[:digit:]]*)'
            if [[ $arg =~ $pat ]]; then
                for (( id=${BASH_REMATCH[1]}; id<=${BASH_REMATCH[2]}; id++ ))
                do
                    FILTER[$id]="Y"
                done
            else
                FILTER[$arg]="Y"
            fi
            ;;
    esac
done

#---------------------------------------
