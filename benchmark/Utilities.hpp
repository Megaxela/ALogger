#pragma once

#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <string>
#include <zconf.h>
#include <cstdlib>

bool createPath(const std::string& s)
{
    std::string command = "mkdir -p \"" + s + "\""; 
    return system(command.c_str()) == 0;
}

bool deleteFolder(const std::string& s)
{
    DIR* dir;
    struct dirent* entry;
    char path[PATH_MAX];

    dir = opendir(s.c_str());
    if (dir == NULL)
    {
        return false;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry
                       ->d_name, ".") != 0 &&
            strcmp(entry
                       ->d_name, "..") != 0)
        {
            snprintf(path, (size_t)
                PATH_MAX, "%s/%s", s.c_str(), entry->d_name);
            if (entry->d_type == DT_DIR)
            {
                deleteFolder(path);
            }
            else
            {
                unlink(path);
            }
        }

    }
    closedir(dir);

    rmdir(s.c_str());

    return true;
}