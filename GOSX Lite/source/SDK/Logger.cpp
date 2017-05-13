//
//  Logger.cpp
//  GOSX Pro
//
//  Created by Andre Kalisch on 21.02.17.
//  Copyright © 2017 Andre Kalisch. All rights reserved.
//

#include "Logger.h"

std::shared_ptr<FileLogger> FileLogger::instance = nullptr;

FileLogger::FileLogger() {}

