docker run --rm -v $GITHUB_WORKSPACE:/workdir dockcross/$CROSS bash -c 'apt-get install -y default-jdk && cd /workdir && /workdir/cross/build.sh'
