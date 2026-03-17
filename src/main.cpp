#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

std::atomic<bool> g_running(true);
std::atomic<unsigned long long> g_total_requests(0);
std::atomic<unsigned long long> g_prime_requests(0);
std::atomic<unsigned long long> g_prime_true_results(0);
std::atomic<unsigned long long> g_prime_false_results(0);

bool isPrime(unsigned long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (unsigned long long i = 3; i <= n / i; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

std::string urlDecode(const std::string& s) {
    std::string out;
    out.reserve(s.size());

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            std::string hex = s.substr(i + 1, 2);
            char ch = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            out.push_back(ch);
            i += 2;
        } else if (s[i] == '+') {
            out.push_back(' ');
        } else {
            out.push_back(s[i]);
        }
    }

    return out;
}

std::string getQueryParam(const std::string& path, const std::string& key) {
    size_t qpos = path.find('?');
    if (qpos == std::string::npos) return "";

    std::string query = path.substr(qpos + 1);
    std::stringstream ss(query);
    std::string item;

    while (std::getline(ss, item, '&')) {
        size_t eq = item.find('=');
        if (eq == std::string::npos) continue;

        std::string k = item.substr(0, eq);
        std::string v = item.substr(eq + 1);

        if (k == key) {
            return urlDecode(v);
        }
    }

    return "";
}

bool parseUnsignedLongLong(const std::string& input, unsigned long long& value) {
    if (input.empty()) return false;

    for (char c : input) {
        if (c < '0' || c > '9') return false;
    }

    try {
        size_t pos = 0;
        value = std::stoull(input, &pos);
        return pos == input.size();
    } catch (...) {
        return false;
    }
}

std::string metricsText() {
    std::ostringstream out;

    out << "# HELP prime_checker_requests_total Total HTTP requests\n";
    out << "# TYPE prime_checker_requests_total counter\n";
    out << "prime_checker_requests_total " << g_total_requests.load() << "\n";

    out << "# HELP prime_checker_prime_requests_total Total /prime requests\n";
    out << "# TYPE prime_checker_prime_requests_total counter\n";
    out << "prime_checker_prime_requests_total " << g_prime_requests.load() << "\n";

    out << "# HELP prime_checker_prime_true_total Count of prime results\n";
    out << "# TYPE prime_checker_prime_true_total counter\n";
    out << "prime_checker_prime_true_total " << g_prime_true_results.load() << "\n";

    out << "# HELP prime_checker_prime_false_total Count of non-prime results\n";
    out << "# TYPE prime_checker_prime_false_total counter\n";
    out << "prime_checker_prime_false_total " << g_prime_false_results.load() << "\n";

    return out.str();
}

std::string buildResponse(const std::string& status,
                          const std::string& contentType,
                          const std::string& body) {
    std::ostringstream out;
    out << "HTTP/1.1 " << status << "\r\n";
    out << "Content-Type: " << contentType << "\r\n";
    out << "Content-Length: " << body.size() << "\r\n";
    out << "Connection: close\r\n\r\n";
    out << body;
    return out.str();
}

void handleClient(int clientSocket) {
    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }

    g_total_requests++;

    std::string request(buffer);
    std::istringstream requestStream(request);

    std::string method, path, version;
    requestStream >> method >> path >> version;

    std::string response;

    if (method != "GET") {
        response = buildResponse("405 Method Not Allowed", "text/plain",
                                 "Only GET is supported\n");
    } else if (path == "/health") {
        response = buildResponse("200 OK", "text/plain", "OK\n");
    } else if (path.rfind("/prime", 0) == 0) {
        g_prime_requests++;

        std::string nStr = getQueryParam(path, "n");
        unsigned long long number = 0;

        if (!parseUnsignedLongLong(nStr, number)) {
            response = buildResponse("400 Bad Request", "text/plain",
                                     "Invalid or missing parameter n\n");
        } else {
            bool result = isPrime(number);

            if (result) g_prime_true_results++;
            else g_prime_false_results++;

            std::ostringstream body;
            body << "{ \"number\": " << number
                 << ", \"is_prime\": " << (result ? "true" : "false") << " }\n";

            response = buildResponse("200 OK", "application/json", body.str());
        }
    } else if (path == "/metrics") {
        response = buildResponse("200 OK", "text/plain; version=0.0.4",
                                 metricsText());
    } else {
        response = buildResponse("404 Not Found", "text/plain", "Not found\n");
    }

    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
}

void signalHandler(int) {
    g_running = false;
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    const int port = 8080;

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // ⬇️ ВАЖНО: таймаут, чтобы Ctrl+C работал
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr),
             sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Listen failed\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "Prime checker service started on port " << port << std::endl;

    while (g_running) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        int clientSocket = accept(serverSocket,
                                  reinterpret_cast<sockaddr*>(&clientAddr),
                                  &clientLen);

        if (clientSocket < 0) {
            continue; // просто пробуем снова (таймаут)
        }

        handleClient(clientSocket);
    }

    std::cout << "Shutting down..." << std::endl;

    close(serverSocket);
    return 0;
}
