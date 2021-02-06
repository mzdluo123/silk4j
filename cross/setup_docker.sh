docker run --rm -v $GITHUB_WORKSPACE:/workdir -v $JAVA_HOME:/java_home dockcross/$CROSS bash -c 'export JAVA_HOME=/java_home && cd /workdir && /workdir/cross/build.sh'
