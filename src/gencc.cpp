#include "gencc.h"
#include "builder.h"
#include "common.h"
#include "compiler.h"
#include "helper.h"

#include "json.hpp"
#include <exception>
#include <iostream>

Gencc::Gencc(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

bool Gencc::parseArgs(std::vector<std::string>& params)
{
    auto it = params.begin();
    for (; it != params.end(); ++it) {
        if (it == params.begin() + 1 && it->find("-") != std::string::npos) {
            break;
        }
    }

    for (; it != params.end();) {
        std::string param = *it;
        if (param.find('-') == std::string::npos) {
            break;
        }

        if (param == "-gencc-compiler" && it + 1 != params.end()) {
            params.erase(it);
            m_options->compiler = *it;
            params.erase(it);

            // The compiler is the only parameter that needs to be parsed in compiler mode
            break;
        } else if (param == "-cxx" && it + 1 != params.end()) {
            params.erase(it);
            m_options->cxx = *it;
            params.erase(it);
        } else if (param == "-cc" && it + 1 != params.end()) {
            params.erase(it);
            m_options->cc = *it;
            params.erase(it);
        } else if (param == "-o" && it + 1 != params.end()) {
            params.erase(it);
            m_options->dbFilename = *it;
            params.erase(it);
        } else if (param == "-r" && it + 1 != params.end()) {
            params.erase(it);
            m_options->retries = std::stoi(*it);
            params.erase(it);
        } else if (param == "-f" && it + 1 != params.end()) {
            params.erase(it);
            m_options->fallback = std::stoi(*it);
            params.erase(it);
        } else if (param == "-build") {
            params.erase(it);
            m_options->build = true;
        } else {
            return false;
        }
    }

    return true;
}

void Gencc::help()
{
    std::cout << "Help:\n"
                 "\t-cxx    [value] - CXX compiler\n"
                 "\t-cc     [value] - CC compiler\n"
                 "\t-o      [value] - DB file\n"
                 "\t-r      [value] - Number of retries if DB locked\n"
                 "\t-f      [value] - Max fallback time in ms in case of DB locked\n"
                 "\t-build          - Call the actual compiler\n";
}

void Gencc::setWorker(std::unique_ptr<GenccWorker>& worker)
{
    m_worker = std::move(worker);
}

int Gencc::init(int argc, char* argv[])
{
    if (m_options == nullptr || m_helper == nullptr) {
        std::cout << "Options and helper cannot be null\n";
        return -1;
    }

    std::cout << NAME << " v" << VERSION << ": ";

    if (argc < 2) {
        help();
        return 0;
    }

    std::vector<std::string> params;
    std::string genccComand = argv[0];

    // Ensure the GenCC command uses the absolute path
    if (genccComand.find_first_of('/') != 0) {
        std::string cwd;
        if (!m_helper->getCwd(cwd)) {
            throw std::runtime_error("Couldn't get current working dir");
        }
        genccComand = cwd + "/" + genccComand;
    }
    params.emplace_back(genccComand);

    for (int i = 1; i < argc; ++i) {
        params.emplace_back(argv[i]);
    }

    std::string mode;
    if (!m_helper->getEnvVar(GENCC_OPTIONS, mode)) {
        m_options->mode = gencc_mode::BUILDER;
    } else {
        m_options->mode = gencc_mode::COMPILER;
    }

    if (!parseArgs(params)) {
        std::cout << "Error parsing arguments\n";
        help();
        return -1;
    }

    if (m_options->mode == gencc_mode::BUILDER) {
        std::cout << '\n';
        m_worker = std::unique_ptr<GenccWorker>(new Builder(m_options, m_helper));
    } else {
        m_worker = std::unique_ptr<GenccWorker>(new Compiler(m_options, m_helper));
    }

    m_worker->doWork(params);

    return 0;
}
