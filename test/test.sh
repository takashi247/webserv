make -C ../

cd .. && ./webserv confs/test.conf > /dev/null &

python3 request_test.py

killall webserv
