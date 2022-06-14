import socket

HOSTS = [
        'www.inhatc.ac.kr',
        'www.naver.com',
        'www.google.co.kr',
        'testname',
        ]

for host in HOSTS:
    try:
        print('{} : {}'.format(host,socket.gethostbyname(host)))
    except socket.error as msg:
        print('{} : {}'.format(host,msg))
