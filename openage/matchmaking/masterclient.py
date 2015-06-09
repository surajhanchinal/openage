# Copyright 2015-2015 the openage authors. See copying.md for legal info.

import json
import socket

# master server port = SFTOA
MASTERPORT = 58704


def test():
    s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

    s.connect(("localhost", MASTERPORT))

    serverversion = json.loads(s.recv(4096).decode())
    print("masterserver version: %s" % (serverversion))

    clientversion = {
        "peersoftware": "openage",
        "peertype": "client",
        "protocol": [0, 0],
    }

    s.send(b"".join((json.dumps(clientversion).encode(), b"\n")))

    version_handshake = json.loads(s.recv(4096).decode())
    if version_handshake["compatible"]:
        print("this client is compatible! congratulations!")
    else:
        print("incompatible client, bzzzzzzzzzzt!")

if __name__ == "__main__":
    test()
