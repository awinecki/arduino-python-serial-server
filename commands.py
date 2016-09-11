from multiprocessing.connection import Client


class Commander(object):

    def __init__(self, host, port, authkey=None):
        self.adress = (host, port)
        self.authkey = authkey

    def order(self, command):
        conn = Client(self.adress, authkey=self.authkey)
        conn.send_bytes(command.encode())
        conn.close()
