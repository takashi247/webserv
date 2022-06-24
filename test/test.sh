make -C ../

cd .. && ./webserv test/confs/test.conf > /dev/null &

mkdir -p www/autoindex

if [ $? == 0 ]; then
	python3 request_test.py
	killall webserv
fi
