import socket

def send_request_once(local_port, host='localhost', port=8020, path='/sylar/xx'):
    # 创建 TCP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # 绑定源端口
        sock.bind(('0.0.0.0', local_port))
        # 建立连接
        sock.connect((host, port))
        # 发送 HTTP/1.1 GET 请求，带 Connection: close
        request = (
            f"GET {path} HTTP/1.1\r\n"
            f"Host: {host}:{port}\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        sock.sendall(request.encode('ascii'))

        # 立刻开始接收响应
        response_chunks = []
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            response_chunks.append(chunk)

        # 打印完整响应
        response = b''.join(response_chunks)
        print(f"=== Response on local port {local_port} ===")
        print(response.decode('utf-8', errors='ignore'))
    finally:
        # 接收完毕后立即关闭连接
        try:
            sock.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass  # 可能已经半关
        sock.close()

if __name__ == '__main__':
    start_port = 9000
    count = 5
    for i in range(count):
        port_to_use = start_port + i
        try:
            send_request_once(port_to_use)
        except Exception as e:
            print(f"Failed on port {port_to_use}: {e}")

