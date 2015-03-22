#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <functional>
#include <cassert>

#include <unistd.h>
#include <linux/limits.h>

#include <json/json.h>

static char cwd[PATH_MAX];

struct CompilerFlag {
    const std::string str;
    const std::uint32_t n_args;
};

static const CompilerFlag flags[] = {
    /* include paths */
    { "-I", 1 },
    { "-include", 1 },
    { "-isystem", 1 },
    { "-nostdinc", 0 },
    /* errors */
    { "-W", 0 },
    /* defines */
    { "-D", 1 },
    { "-U", 1 },
    /* lang */
    { "-std=", 0},
    /* output */
    { "-o", 1 },
    /* link library */
    { "-l", 1 },
    { "-g", 0 },
    { "-c", 0 },
    { "-O", 0 },
    { "-m", 0 },
    { "-f", 0 },
    { "-S", 0 },
    { "-x", 1 },
    /* version */
    { "-v", 0 },
    { "--version", 0 },
    { "-V", 1 },
    { "-qversion", 0 },
    { "-pg", 0 },
    { "--param=", 0 },
    /* print */
    { "-print-file-name=", 0 },
    { "-print-search-dirs", 0 },
    { "-print-multi-os-directory", 0 },
    { "-nostdlib", 0 },
    /* library */
    { "-shared", 0 },
    { "-static", 0 },
    { "-pipe", 0 },
    { "-pthread", 0 },
    { "-MT", 1 },
    { "-MF", 1 },
    { "-M", 0 },
    { "-E", 0 },
    { "-d", 0 }
};

const std::string exclude_files[] = {
    "-",
    "/dev/null"
};

bool file_exists(const std::string& filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "usage: dcc <path> <compiler> <flags>" << std::endl;
        return -1;
    }

    auto ret = getcwd(cwd, PATH_MAX);
    if (ret == NULL) {
        std::cerr << "could not get current working dir!" << std::endl;
        return -1;
    }

    std::stringstream ss;
    std::vector<const char*> source_files;
    std::uint32_t n_args = 0;
    std::string compiler(argv[2]);

    ss << compiler << " ";
    for (int i = 3; i < argc; i++) {
        if (i + 1 < argc) {
            ss << argv[i] << " ";
        } else {
            ss << argv[i];
        }

        if (n_args == 0) {
            bool match = false;
            for (auto& flag : flags) {
                auto match_iter =
                    std::mismatch(flag.str.begin(), flag.str.end(), argv[i]);
                if (match_iter.first == flag.str.end()) {
                    n_args = flag.n_args;
                    if (n_args > 0 && *match_iter.second != '\0') {
                        n_args--;
                    }
                    match = true;
                    break;
                }
            }
            if (!match) {
                bool exclude = false;
                for (auto& filename : exclude_files) {
                    if (filename == argv[i]) {
                        exclude = true;
                        break;
                    }
                }
                if (!exclude) {
                    source_files.push_back(argv[i]);
                }
            }
        } else {
            n_args--;
        }
    }

    if (!source_files.empty()) {
        std::string cdb_path = std::string(argv[1]) + "/compile_commands.json";
        bool cdb_exists = file_exists(cdb_path);

        std::ios_base::openmode cdb_openmode = std::ios::out;
        if (cdb_exists) {
            cdb_openmode |= std::ios::in;
        }
        std::fstream cdb(cdb_path, cdb_openmode);
        if (cdb) {
            Json::Value root(Json::arrayValue);
            if (cdb_exists) {
                cdb >> root;
            }

            for (auto filename : source_files) {
                bool update = false;
                std::stringstream filename_absolut;
                if (filename[0] != '/') {
                    filename_absolut << cwd << "/";
                }
                filename_absolut << filename;

                for (auto& e : root) {
                    if (e["file"].asString() == filename_absolut.str()) {
                        assert(e["directory"].asString() == cwd);
                        e["command"] = ss.str();
                        update = true;
                        break;
                    }
                }

                if (!update) {
                    Json::Value e;
                    e["file"] = filename_absolut.str();
                    e["directory"] = cwd;
                    e["command"] = ss.str();
                    root.append(e);
                }
            }

            if (cdb_exists) {
                cdb.close();
                // We want to replace the file
                cdb.open(cdb_path, std::ios::out);
            }

            cdb << root;
            cdb.close();
        } else {
            std::cerr << "could not open compile_commands.json" << std::endl;
        }
    }

    // ok, now let's call the real thing
    std::uint32_t argc_null = argc - 1;
    char** argv_null = new char*[argc_null];
    std::copy(argv + 2, argv + argc, argv_null);
    argv_null[argc_null - 1] = nullptr;
    return execvp(compiler.c_str(), argv_null);
}
