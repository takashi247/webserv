from http import HTTPStatus
import os
import requests
import unittest
import sys


server = ""

class TestRequest(unittest.TestCase):
    def setUp(self) -> None:
        self.server = server
        os.chmod('www/no_perm.html', 0o333)

    def tearDown(self) -> None:
        os.chmod('www/no_perm.html', 0o755)

    # [TODO] CUSTAMIZE HEADER!!!
    def send_get_request(self, uri: str, header = None) -> requests.Response:
        try:
            response = requests.get(uri, timeout=2.0, allow_redirects=False, headers=header)
        except Exception as e:
            # print(e)
            self.fail()
        return (response)

    def send_post_request(self, uri: str, header = None, data = None) -> requests.Response:
        try:
            response = requests.post(uri, timeout=2.0, allow_redirects=False, headers=header, data=data)
        except Exception as e:
            # print(e)
            self.fail()
        return (response)

    def send_delete_request(self, uri: str, header = None) -> requests.Response:
        try:
            response = requests.delete(uri, timeout=2.0, allow_redirects=False, headers=header)
        except Exception as e:
            # print(e)
            self.fail()
        return (response)

    def skip_test_if_not_suppoeted(self, not_supported: str):
        if (self.server == not_supported):
            self.skipTest("not supported in " + not_supported)

    def get_autoindex_titile(self, text: str) -> str:
        return ''.join(text.splitlines()[:2])

    def test_normal(self):
        got = self.send_get_request("http://127.0.0.1:8080/2.html")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "2\n")

    def test_index_1(self):
        got = self.send_get_request("http://127.0.0.1:8080")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "1\n")

    def test_index_2(self):
        got = self.send_get_request("http://127.0.0.1:8080/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "1\n")

    def test_autoindex_1(self):
        got = self.send_get_request("http://localhost:8081/autoindex/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(self.get_autoindex_titile(got.text), "<html><head><title>Index of /autoindex/</title></head>")

    def test_autoindex_2(self):
        got = self.send_get_request("http://localhost:8081/autoindex")
        self.assertEqual(got.status_code, HTTPStatus.MOVED_PERMANENTLY)
    # config

    def test_autoindex_3(self):
        got = self.send_get_request("http://localhost:8081/autoindex/inner/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(self.get_autoindex_titile(got.text), "<html><head><title>Index of /autoindex/inner/</title></head>")

    def test_autoindex_4(self):
        got = self.send_get_request("http://localhost:8081/autoindex/inner")
        self.assertEqual(got.status_code, HTTPStatus.MOVED_PERMANENTLY)

    def test_autoindex_and_index(self):
        got = self.send_get_request("http://localhost:8081/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "1\n")

    def test_autoindex_and_index(self):
        got = self.send_get_request("http://localhost:8081")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "1\n")

    def test_invalid_server_name(self):
        got = self.send_get_request("http://127.0.0.1:8081/autoindex/inner/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(self.get_autoindex_titile(got.text), "<html><head><title>Index of /autoindex/inner/</title></head>")

    def test_nosuch_file(self):
        got = self.send_get_request("http://localhost:8081/nosuch.txt")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)

    def test_nosuch_dir(self):
        got = self.send_get_request("http://localhost:8081/nosuchdir/")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)

    # apache only
    def test_cgi(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/cgi-bin/hello.cgi")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "hello")

    def test_cgi_post(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_post_request("http://localhost:8082/cgi-bin/upload.cgi", data={'param': 'hello'})
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "param = hello")

    def test_cgi_post_by_get(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/cgi-bin/upload.cgi")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    # execve in cgi fails
    # def test_cgi_no_header(self):
    #     got = self.send_request("http://localhost:8082/cgi-bin/no_header_perl.cgi")
    #     self.assertEqual(got.status_code, 500)
    #     self.assertEqual(got.text, "hello")

    def test_delete(self):
        got = self.send_delete_request("http://localhost:8080/delete")
        self.assertEqual(got.status_code, HTTPStatus.NO_CONTENT)

    def test_delete_no_perm(self):
        got = self.send_delete_request("http://localhost:8080/no_perm.html")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)

    def test_delete_no_file(self):
        got = self.send_delete_request("http://localhost:8080/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)

    def test_delete_dir(self):
        got = self.send_delete_request("http://localhost:8082/")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)
    ## index ?? 

    def test_cgi_index(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/cgi-bin/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "hello")

    def test_rewrite_1(self):
        got = self.send_get_request("http://localhost:8083/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8080//nosuchfile")

    def test_rewrite_2(self):
        got = self.send_get_request("http://localhost:8083")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8080//")

    def test_rewrite_3(self):
        got = self.send_get_request("http://localhost:8083/rewrite1/")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8081//rewrite1/")

    def test_rewrite_4(self):
        got = self.send_get_request("http://localhost:8083/rewrite1/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8081//rewrite1/nosuchfile")

    def test_rewrite_5(self):
        got = self.send_get_request("http://localhost:8083/rewrite2/")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8082//rewrite2/")

    def test_rewrite_6(self):
        got = self.send_get_request("http://localhost:8083/rewrite2/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8082//rewrite2/nosuchfile")

    def test_error_page(self):
        got = self.send_get_request("http://localhost:8080/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)
        self.assertEqual(got.text, "2\n")

    def test_invalid_location_1(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)

    def test_invalid_location_2(self):
        got = self.send_get_request("http://localhost:8082/1.html")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)

    # expect to ~~ not allowd ~~
    def test_post_request_to_normal_file(self):
        got = self.send_post_request("http://localhost:8080/1.html")
        self.assertEqual(got.status_code, HTTPStatus.METHOD_NOT_ALLOWED)

    def test_post_request_to_cgi_file(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_post_request("http://localhost:8082/cgi-bin/hello.cgi")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    def test_no_perm(self):
        got = self.send_get_request("http://localhost:8080/no_perm.html")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)

    # recv failed and no response
    def test_over_client_max_body_size(self):
        got = self.send_get_request("http://127.0.0.1:8083/1.html", {'Content-Length': '120'})
        self.assertEqual(got.status_code, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)

    def test_not_permitted_method(self):
        got = self.send_post_request("http://127.0.0.1:8083/1.html")
        self.assertEqual(got.status_code, HTTPStatus.METHOD_NOT_ALLOWED)

    # recv failed and no response
    def test_over_client_max_body_size_and_no_such_file(self):
        got = self.send_get_request("http://127.0.0.1:8083/nosuchfile", {'Content-Length': '120'})
        self.assertEqual(got.status_code, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)

    def test_not_permitted_method_and_over_client_max_body(self):
        got = self.send_post_request("http://127.0.0.1:8083/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.METHOD_NOT_ALLOWED)

    # recv failed and no response
    def test_over_client_max_body_and_no_perm_with_post(self):
        got = self.send_post_request("http://127.0.0.1:8083/no_perm.html", {'Content-Length': '120'})
        self.assertEqual(got.status_code, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        server = sys.argv[1]
        del sys.argv[1:]
    open('www/delete', 'w')
    unittest.main()
    f = open('www/1.html', 'w')
    f.write("\n")
    f.close()
