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
private:
    static std::vector<Instruction*> _inst_stack;
    static std::map<pthread_t , Module*> _thread_module_table;
    static std::map<string, Module*> _module_table;
    //static std::vector<Module*> _modules;
public:

    static void init();
    static void destroy();

    static void worker_push_inst(Instruction*);
    static Instruction* worker_fetch_instruction();
    static bool inst_stack_is_empty();

    static Module* get_module(string name);
    /* thread specific */
    static void add_module(Module*);
    /* all these functions assume thread_module_table has been constructed */
    static Module* module();
    static const string& filename();
    static const string filedir();
    static const string get_pass_out_name(string passname);
    static void pass_print_to_file(string passname, Module*);

    /* for UseSplitModule */
    static void merge_modules();

    static std::map<pthread_t , Module*>& thread_module_table()                { return _thread_module_table; }
    static std::map<string, Module*>& module_table()                           { return _module_table; }

    static LLParser* parser;

    //static InstParser* instParser;
};

#endif //LLPARSER_SYSDICT_H
