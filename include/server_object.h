#include <string>
#include <vector>

class ServerObject {
    public:
        static int port;
        static std::string echo_dir;
        static std::vector <std::string> staticfile_dir;
        static std::vector <std::string> staticfile_url;

        static int findStaticDir(std::string key);
};