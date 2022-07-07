#!/usr/bin/python3

import cgi, os
import cgitb; cgitb.enable()

try: # Windows needs stdio set for binary mode.
    import msvcrt
    msvcrt.setmode (0, os.O_BINARY) # stdin  = 0
    msvcrt.setmode (1, os.O_BINARY) # stdout = 1
except ImportError:
    pass

if os.environ.get('X_IS_UPLOADABLE') == 'true':

    form = cgi.FieldStorage()

    # A nested FieldStorage instance holds the file
    fileitem = form['file']

    # Get folder to upload from environment variable
    upload_dir = os.environ.get('X_UPLOAD_DIR')

    # Test if the file was uploaded
    if fileitem.filename:

        # strip leading path from file name
        # to avoid directory traversal attacks
        fn = os.path.basename(fileitem.filename)
        open(upload_dir + '/' + fn, 'wb').write(fileitem.file.read())
        message = 'The file "' + fn + '" was uploaded successfully'

    else:
        message = 'No file was uploaded'

    print("Content-Type: text/html\r\n\r\n", end = "")
    print("<html><body><p>%s</p></body></html>" % message)

else:
    print("Content-Type: text/html\r\n", end = "")
    print("Status: 405\r\n\r\n", end = "")

print(chr(26), end = '')