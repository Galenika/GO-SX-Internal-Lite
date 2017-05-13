//
//  Logger.hpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 21.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#ifndef Logger_hpp
#define Logger_hpp

#include "Engine/common.h"

class FileLogger {
public:
    FileLogger();
    static std::shared_ptr<FileLogger> Instance() {
        if(!instance) {
            instance = std::make_unique<FileLogger>();
        }

        return instance;
    }

    template <typename type>
    void Log(const char* file, const char* mask, type content) {
        std::ofstream logFile;

        logFile.open(file, std::ios::out | std::ios::app);

        if(!logFile.good()) {
            logFile.close();
            return;
        }

        char fileContent[1024] = "";
        sprintf(fileContent, mask, content);

        logFile << fileContent << std::endl;
        
        logFile.close();
    }
private:
    static std::shared_ptr<FileLogger> instance;
};

#endif /* Logger_hpp */
