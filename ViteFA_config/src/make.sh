
dirs='server agent admin'
for subdir in $dirs; do
  echo "directory $subdir:"
  cd $subdir && make $1
  cd ..
  echo
done

if [ "$1" = 'install' ]; then
  if [ ! -f /etc/fcfg/fcfg_serverd.conf ]; then
     mkdir -p /etc/fcfg
     cp ../conf/fcfg_serverd.conf /etc/fcfg/
  fi

  if [ ! -f /etc/fcfg/fcfg_agentd.conf ]; then
     mkdir -p /etc/fcfg
     cp ../conf/fcfg_agentd.conf /etc/fcfg/
  fi
fi
