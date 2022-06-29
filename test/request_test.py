import requests
import unittest


class TestRequest(unittest.TestCase):
    def send_request(self, uri: str) -> requests.Response:
        try:
            response = requests.get(uri, timeout=2.0, allow_redirects=False)
        except Exception as e:
            print(e)
            self.fail()
        return (response)

    def get_autoindex_titile(self, text: str) -> str:
        return ''.join(text.splitlines()[:2])

    def test_normal(self):
        got = self.send_request("http://127.0.0.1:8080/2.html")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "2\n")

    def test_index_1(self):
        got = self.send_request("http://127.0.0.1:8080")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "1\n")

    def test_index_2(self):
        got = self.send_request("http://127.0.0.1:8080/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "1\n")

    def test_autoindex_1(self):
        got = self.send_request("http://localhost:8081/autoindex/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(self.get_autoindex_titile(got.text), "<html><head><title>Index of /autoindex/</title></head>")

    def test_autoindex_2(self):
        got = self.send_request("http://localhost:8081/autoindex")
        self.assertEqual(got.status_code, 301)
    # config

    def test_autoindex_3(self):
        got = self.send_request("http://localhost:8081/autoindex/inner/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(self.get_autoindex_titile(got.text), "<html><head><title>Index of /autoindex/inner/</title></head>")

    def test_autoindex_4(self):
        got = self.send_request("http://localhost:8081/autoindex/inner")
        self.assertEqual(got.status_code, 301)

    def test_autoindex_and_index(self):
        got = self.send_request("http://localhost:8081/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "1\n")

    def test_autoindex_and_index(self):
        got = self.send_request("http://localhost:8081")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "1\n")

    def test_invalid_server_name(self):
        got = self.send_request("http://127.0.0.1:8081/autoindex/inner/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(self.get_autoindex_titile(got.text), "<html><head><title>Index of /autoindex/inner/</title></head>")

    def test_nosuch_file(self):
        got = self.send_request("http://localhost:8081/nosuch.txt")
        self.assertEqual(got.status_code, 404)

    def test_nosuch_dir(self):
        got = self.send_request("http://localhost:8081/nosuchdir/")
        self.assertEqual(got.status_code, 404)

    def test_invalid_location_1(self):
        got = self.send_request("http://localhost:8082/")
        self.assertEqual(got.status_code, 403)

    def test_invalid_location_2(self):
        got = self.send_request("http://localhost:8082/1.html")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "index\n")
    # 要調査

    # def test_cgi_cgi(self):
    #     got = self.send_request("http://localhost:8082/cgi-bin/hello-perl.cgi")
    #     self.assertEqual(got.status_code, 200)
    #     self.assertEqual(got.text, "hello\n")
    # infinite loop...

    def test_invalid_location_2(self):
        got = self.send_request("http://localhost:8083/1.html")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "index\n")

if __name__ == "__main__":
    unittest.main()
