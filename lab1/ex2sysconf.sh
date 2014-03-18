#!/bin/bash

echo "#include <stdio.h>"
echo "#include <unistd.h>"
echo "#include <limits.h>"
echo "#include <errno.h>"

echo "int main(void) {"
echo "long r;"

for i in `cat sysconf.list.txt | grep _SC`
do
    echo '#ifdef '$i
    echo "r = sysconf($i);"
    echo "if (r == -1 && errno == EINVAL) printf(\"brak wartosci dla $i\n\"); else "
    echo "printf(\"The value of $i is %ld\\n\", r);"
    echo "#else"
    echo "printf(\"The $i is undefined.\\n\");"
    echo '#endif'
done

echo "return 0;"
echo "}"
