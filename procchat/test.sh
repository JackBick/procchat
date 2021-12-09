./test & gcc test_gevent.c -o test_gevent
if(./test_gevent | diff - testout/test_gevent.out) then
    echo "test_gevent passed"; else
    echo "test_gevent failed"; fi

./test & gcc test_connect.c -o test_connect
if(./test_connect | diff - testout/test_connect.out) then
    echo "test_open_domain passed"; else
    echo "test_open_domain failed"; fi

