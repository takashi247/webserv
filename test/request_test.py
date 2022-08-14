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

    def send_get_request(self, uri: str, header=None) -> requests.Response:
        try:
            response = requests.get(
                uri, timeout=2.0, allow_redirects=False, headers=header)
        except Exception as e:
            # print(e)
            self.fail()
        return (response)

    def send_post_request(self, uri: str, header=None, data=None) -> requests.Response:
        try:
            response = requests.post(
                uri, timeout=2.0, allow_redirects=False, headers=header, data=data)
        except Exception as e:
            # print(e)
            self.fail()
        return (response)

    def send_delete_request(self, uri: str, header=None) -> requests.Response:
        try:
            response = requests.delete(
                uri, timeout=2.0, allow_redirects=False, headers=header)
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
        self.assertEqual(self.get_autoindex_titile(
            got.text), "<html><head><title>Index of /autoindex/</title></head>")

    def test_autoindex_2(self):
        got = self.send_get_request("http://localhost:8081/autoindex")
        self.assertEqual(got.status_code, HTTPStatus.MOVED_PERMANENTLY)
        self.assertEqual(got.headers["Location"],
                         "http://localhost:8081/autoindex/")

    def test_autoindex_3(self):
        got = self.send_get_request("http://localhost:8081/autoindex/inner/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(self.get_autoindex_titile(
            got.text), "<html><head><title>Index of /autoindex/inner/</title></head>")

    def test_autoindex_4(self):
        got = self.send_get_request("http://localhost:8081/autoindex/inner")
        self.assertEqual(got.status_code, HTTPStatus.MOVED_PERMANENTLY)
        self.assertEqual(got.headers["Location"],
                         "http://localhost:8081/autoindex/inner/")

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
        self.assertEqual(self.get_autoindex_titile(
            got.text), "<html><head><title>Index of /autoindex/inner/</title></head>")

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

    def test_cgi_chucked_request(self):
        got = self.send_post_request("http://localhost:8082/cgi-bin/post.cgi", data='param=hello', header={'Transfer-encoding': 'chunked'})
        self.assertEqual(got.status_code, HTTPStatus.OK)
        print(got.history)

    def test_cgi_post(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_post_request(
            "http://localhost:8082/cgi-bin/post.cgi", data={'param': 'hello'})
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "param = hello")

    def test_cgi_compile_error(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_post_request(
            "http://localhost:8082/cgi-bin/compile-error.cgi")
        self.assertEqual(got.status_code, HTTPStatus.INTERNAL_SERVER_ERROR)

    def test_cgi_no_header(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_post_request(
            "http://localhost:8082/cgi-bin/no_header.cgi")
        self.assertEqual(got.status_code, HTTPStatus.INTERNAL_SERVER_ERROR)

    # plese test by hand
    # def test_cgi_timeout(self):
    #     self.skip_test_if_not_suppoeted("nginx")
    #     got = self.send_post_request("http://localhost:8082/cgi-bin/timeout.cgi")
    #     self.assertEqual(got.status_code, HTTPStatus.GATEWAY_TIMEOUT)

    def test_cgi_post_by_get(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/cgi-bin/post.cgi")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    def test_delete_readable(self):
        f = open('www/delete_r', 'w')
        os.chmod('www/delete_r', 0o444)
        got = self.send_delete_request("http://localhost:8080/delete_r")
        self.assertEqual(got.status_code, HTTPStatus.NO_CONTENT)
        f.close()

    def test_delete_writable(self):
        f = open('www/delete_w', 'w')
        os.chmod('www/delete_w', 0o222)
        got = self.send_delete_request("http://localhost:8080/delete_w")
        f.close()
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)

    def test_delete_executanle(self):
        f = open('www/delete_x', 'w')
        os.chmod('www/delete_x', 0o111)
        got = self.send_delete_request("http://localhost:8080/delete_x")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)
        f.close()

    def test_delete_no_perm(self):
        got = self.send_delete_request("http://localhost:8080/no_perm.html")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)

    def test_delete_no_file(self):
        got = self.send_delete_request("http://localhost:8080/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)

    def test_cgi_index(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/cgi-bin/")
        self.assertEqual(got.status_code, HTTPStatus.OK)
        self.assertEqual(got.text, "hello")

    def test_rewrite_1(self):
        got = self.send_get_request("http://localhost:8083/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"],
                         "http://localhost:8080//nosuchfile")

    def test_rewrite_2(self):
        got = self.send_get_request("http://localhost:8083")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8080//")

    def test_rewrite_3(self):
        got = self.send_get_request("http://localhost:8083/rewrite1/")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8081/")

    def test_rewrite_4(self):
        got = self.send_get_request(
            "http://localhost:8083/rewrite1/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"],
                         "http://localhost:8081/nosuchfile")

    def test_rewrite_5(self):
        got = self.send_get_request("http://localhost:8083/rewrite2/")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"], "http://localhost:8082//")

    def test_rewrite_6(self):
        got = self.send_get_request(
            "http://localhost:8083/rewrite2/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"],
                         "http://localhost:8082//nosuchfile")

    def test_rewrite_6(self):
        got = self.send_get_request(
            "http://localhost:8083/rewrite3/")
        self.assertEqual(got.status_code, HTTPStatus.FOUND)
        self.assertEqual(got.headers["Location"],
                         "http://localhost:8083/rewrite4/")

    def test_error_page(self):
        got = self.send_get_request("http://localhost:8080/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.NOT_FOUND)
        self.assertEqual(got.text, "2\n")

    def test_invalid_location_1(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_get_request("http://localhost:8082/")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    def test_invalid_location_2(self):
        got = self.send_get_request("http://localhost:8082/1.html")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    def test_post_request_to_normal_file(self):
        got = self.send_post_request("http://localhost:8080/1.html")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    def test_post_request_to_cgi_file(self):
        self.skip_test_if_not_suppoeted("nginx")
        got = self.send_post_request("http://localhost:8082/cgi-bin/hello.cgi")
        self.assertEqual(got.status_code, HTTPStatus.OK)

    def test_no_perm(self):
        got = self.send_get_request("http://localhost:8080/no_perm.html")
        self.assertEqual(got.status_code, HTTPStatus.FORBIDDEN)

    # recv failed and no response
    def test_over_client_max_body_size(self):
        got = self.send_get_request(
            "http://127.0.0.1:8083/1.html", {'Content-Length': '120'})
        self.assertEqual(got.status_code, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)

    def test_not_permitted_method(self):
        got = self.send_post_request("http://127.0.0.1:8083/1.html")
        self.assertEqual(got.status_code, HTTPStatus.METHOD_NOT_ALLOWED)

    # Content-Length is invalid, so ignore
    # def test_over_client_max_body_size_and_no_such_file(self):
    #     got = self.send_get_request("http://127.0.0.1:8083/nosuchfile", {'Content-Length': '120'})
    #     self.assertEqual(got.status_code, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)

    def test_not_permitted_method_and_over_client_max_body(self):
        got = self.send_post_request("http://127.0.0.1:8083/nosuchfile")
        self.assertEqual(got.status_code, HTTPStatus.METHOD_NOT_ALLOWED)

    # Content-Length is invalid, so ignore
    # def test_over_client_max_body_and_no_perm_with_post(self):
    #     got = self.send_post_request("http://127.0.0.1:8083/no_perm.html", {'Content-Length': '120'})
    #     self.assertEqual(got.status_code, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        server = sys.argv[1]
        del sys.argv[1:]
    unittest.main()
