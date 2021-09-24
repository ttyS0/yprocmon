#include <httplib.h>
#include <random>

int main()
{
    httplib::Server s;
    std::thread t;
    s.Get("/", [&](const httplib::Request &, httplib::Response &res) {
        res.set_content("{\"hello\": true}", "application/json");
        // s.
        s.stop();
    });
    s.listen("0.0.0.0", 11451);
}
