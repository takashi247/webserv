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
		got = self.send_request("http://localhost:8080/index.html")
		self.assertEqual(got.status_code, 200)
		self.assertEqual(got.text, "Hello, world!\n")

	def test_index_1(self):
		got = self.send_request("http://localhost:8080/")
		self.assertEqual(got.status_code, 200)
		self.assertEqual(got.text, "Hello, world!\n")

	def test_index_2(self):
		got = self.send_request("http://localhost:8080")
		self.assertEqual(got.status_code, 200)
		self.assertEqual(got.text, "Hello, world!\n")

if __name__ == "__main__":
    unittest.main()
