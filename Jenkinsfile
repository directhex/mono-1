env.TIMEOUTCMD = "scripts/babysitter"
env.MONO_BABYSITTER_LOG_FILE = "babysitter_report.json_lines"

withEnv([STEP="configure"],[STARTTIME = `date +%s`]) {
echo "*** start: ${STEP}"
sh "${TIMEOUTCMD} --signal=ABRT --kill-after=60s 60m ./autogen.sh --with-monodroid --with-monotouch --with-monotouch_watch --with-monotouch_tv --with-xammac --with-mobile_static $EXTRA_CONF_FLAGS && echo -e "*** end($(echo $(date +%s) - ${STARTTIME} | bc)): ${STEP}: \e[42mPassed\e[0m" || (echo -e "*** end($(echo $(date +%s) - ${STARTTIME} | bc)): ${STEP}: \e[41mFailed\e[0m" && exit 1)"
}
