#!/usr/bin/env bash

# Verify release
CURRENT_DIRECTORY=`pwd`

${CURRENT_DIRECTORY}/scripts/verifyReleaseLinuxSingleCase.sh ${CURRENT_DIRECTORY}/cpp 64 STATIC &> verify_64_STATIC.txt &
STATIC_64_pid=$!

${CURRENT_DIRECTORY}/scripts/verifyReleaseLinuxSingleCase.sh ${CURRENT_DIRECTORY}/cpp 64 SHARED &> verify_64_SHARED.txt &
SHARED_64_pid=$!

tail -f verify*.txt &

FAIL=0
wait ${STATIC_64_pid} || let "FAIL+=1"
wait ${SHARED_64_pid} || let "FAIL+=1"

if [ $FAIL -ne 0 ]; then
    echo "$FAIL verifications FAILED !!!"
else
    echo "All verifications PASSED"
fi

exit $FAIL



