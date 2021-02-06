docker run --rm -v $GITHUB_WORKSPACE:/workdir dockcross/$CROSS bash -c 'mkdir -p /usr/share/man/man1 && apt-get install -y default-jdk && cd /workdir && /workdir/cross/build.sh'
