#include <util/exec.h>

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace util
{

bool spawn_process_and_wait(std::string name, std::vector<std::string> args)
{
    std::cout << "- Running " << name << " with [";
    for(auto& str: args)
        std::cout << str << " ";
    std::cout << "]" << std::endl;
    
    pid_t pid = fork();
    if(pid == 0)
    {
        // Child
        std::cout << "-- I'm in child! Replacing image..." << std::endl;
        std::vector<char*> data;
        data.push_back((char*)name.c_str());
        for(auto& it: args)
        {
            data.push_back((char*)it.c_str());
        }
        data.push_back(nullptr);
        if(execv(name.c_str(), data.data()) < 0)
        {
            std::cout << "-- Failed to exec: " << strerror(errno) << std::endl;
            exit(1);
        }
    }
    else if(pid > 0)
    {
        // Parent
        std::cout << "-- New child PID: " << pid << std::endl;
        int status;
        pid_t w;
        do
        {
            w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
            if(w == -1) 
            {
                std::cout << "-- Failed to wait: " << strerror(errno) << std::endl;
                return false;
            }

            if(WIFEXITED(status))
            {
                std::cout << "--- Process " << w << " exited, status = " << WEXITSTATUS(status) << std::endl;
            }
            else if(WIFSIGNALED(status))
            {
                std::cout << "--- Killed by signal " << WTERMSIG(status) << std::endl;
            }
            else if(WIFSTOPPED(status))
            {
                std::cout << "--- Stopped by signal "  << WSTOPSIG(status) << std::endl;
            }
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    else
    {
        std::cout << "-- Failed to fork: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}
    
}
