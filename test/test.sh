make -C ../

mkdir -p www/autoindex

cd .. && ./webserv test/confs/test.conf > /dev/null &


if [ $? == 0 ]; then
	python3 request_test.py
	killall webserv
fi
