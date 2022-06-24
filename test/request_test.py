import string
import requests
import unittest


class TestRequest(unittest.TestCase):
    def send_request(self, uri: string) -> requests.Response:
        try:
            response = requests.get(uri, timeout=2.0)
        except Exception as e:
            print(e)
            self.fail()
        return (response)

    def test_normal(self):
        got = self.send_request("http://127.0.0.1:8081/2.html")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "2\n")

    def test_index_1(self):
        got = self.send_request("http://127.0.0.1:8081")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "1\n")

    def test_index_2(self):
        got = self.send_request("http://127.0.0.1:8081/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "1\n")

    def test_autoindex_1(self):
        got = self.send_request("http://localhost:8080/autoindex/")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "<html>\n" +
                         "<head><title>Index of /autoindex/</title></head>\n<body bgcolor=\"white\">\n" +
                         "<h1>Index of /autoindex/</h1><hr><pre><a href=\"../\">../</a>\n" +
                         "</pre><hr></body>\n</html>\n")

    def test_autoindex_2(self):
        got = self.send_request("http://localhost:8080/autoindex")
        self.assertEqual(got.status_code, 200)
        self.assertEqual(got.text, "<html>\n" +
                         "<head><title>Index of /autoindex/</title></head>\n<body bgcolor=\"white\">\n" +
                         "<h1>Index of /autoindex/</h1><hr><pre><a href=\"../\">../</a>\n" +
                         "</pre><hr></body>\n</html>\n")

    def test_nosuch_file(self):
        got = self.send_request("http://localhost:8080/nosuch.txt")
        self.assertEqual(got.status_code, 404)

    def test_nosuch_dir(self):
        got = self.send_request("http://localhost:8080/nosuchdir/")
        self.assertEqual(got.status_code, 404)


if __name__ == "__main__":
    unittest.main()
