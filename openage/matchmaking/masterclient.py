# Copyright 2015-2015 the openage authors. See copying.md for legal info.


import socket

# master server port = SFTOA
MASTERPORT = 58704


def test():
    s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

    s.connect(("localhost", MASTERPORT))

    msg = s.recv(4096)
    print(msg)

if __name__ == "__main__":
    test()
