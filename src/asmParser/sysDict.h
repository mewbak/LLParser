//
// Created by tlaber on 6/9/17.
//

#ifndef LLPARSER_SYSDICT_H
#define LLPARSER_SYSDICT_H

#include <vector>
#include <ir/irEssential.h>


class LLParser;
class InstParser;

class SysDict {
public:
    static void init();
    static void destroy();
    static LLParser* get_thread_llparser();
    //static Module* get_module(string name);
    static void add_module(LLParser* );
    /* thread specific */
    static Module* module();
    static const string& filename();

    static std::map<pthread_t , LLParser*> thread_table;
    static std::vector<Module*> modules;
    static LLParser* llParser;
    static InstParser* instParser;
};

#endif //LLPARSER_SYSDICT_H
