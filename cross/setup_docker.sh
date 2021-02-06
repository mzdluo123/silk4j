docker run --rm -v $GITHUB_WORKSPACE:/workdir -v $JAVA_HOME:/java_home --env JAVA_HOME=/java_home dockcross/$CROSS bash -c 'cd /workdir && /workdir/cross/build.sh'
