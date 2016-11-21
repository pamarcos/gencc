#include "builder.h"
#include "helper.h"

#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

Builder::Builder(GenccOptions* options, Helper* helper)
    : Common(options, helper)
{
}

/* Helper function to simply set the PATH correctly so that this binary
 * is called instead of the default CXX one */
void Builder::doWork(const std::vector<std::string>& params)
{
    std::string cwd;
    std::stringstream ss;

    if (!m_helper->getCwd(cwd)) {
        throw std::runtime_error("Couldn't get current working dir");
    }

    std::cout << "Original CXX = " << m_options->cxx << '\n';
    std::cout << "Original CC = " << m_options->cc << '\n';
    std::cout << "CWD = " << cwd << '\n';

    m_options->dbFilename = cwd + "/" + m_options->dbFilename;
    std::remove(m_options->dbFilename.c_str());
    std::remove((m_options->dbFilename + COMPILATION_DB_LOCK_EXT).c_str());

    // Serialize the options through an environment variable
    json jsonObj;
    jsonObj["build"] = m_options->build;
    jsonObj["dbFilename"] = m_options->dbFilename;
    ss.str("");
    ss.clear();
    ss << jsonObj;
    m_helper->setEnvVar(GENCC_OPTIONS, ss.str());
    m_helper->setEnvVar(CXX, params.at(0) + " -gencc-compiler " + m_options->cxx);
    m_helper->setEnvVar(CC, params.at(0) + " -gencc-compiler " + m_options->cc);

    ss.str("");
    ss.clear();
    for (size_t i = 1; i < params.size(); ++i) {
        ss << params.at(i);
        if (i != params.size() - 1) {
            ss << " ";
        }
    }

    if (int ret = system(ss.str().c_str())) {
        std::cout << "The command " << ss.str() << " exited with error code "
                  << ret << '\n';
    }
}
