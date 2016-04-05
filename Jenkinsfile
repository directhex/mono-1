withEnv([TIMEOUTCMD="scripts/babysitter",MONO_BABYSITTER_LOG_FILE="babysitter_report.json_lines"]) {
STARTTIME = Math.round(​System.currentTimeMillis() / 1000L​)​
withEnv([STEP="configure"]) {
echo "*** start: ${env.STEP}"
sh "${env.TIMEOUTCMD} --signal=ABRT --kill-after=60s 60m ./autogen.sh --with-monodroid --with-monotouch --with-monotouch_watch --with-monotouch_tv --with-xammac --with-mobile_static $EXTRA_CONF_FLAGS && echo -e "*** end($(echo $(date +%s) - ${STARTTIME} | bc)): ${env.STEP}: \e[42mPassed\e[0m" || (echo -e "*** end($(echo $(date +%s) - ${STARTTIME} | bc)): ${env.STEP}: \e[41mFailed\e[0m" && exit 1)"
}
}
