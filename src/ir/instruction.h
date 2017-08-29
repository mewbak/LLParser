//
// Created by GentlyGuitar on 6/7/2017.
//

#ifndef LLPARSER_INSTRUCTION_H
#define LLPARSER_INSTRUCTION_H

#include "value.h"

class BasicBlock;
class Function;

class DILocation;

class InstParser;

class Instruction: public Value {
public:
    enum InstType {
        UnknownInstType,
        CallInstType,
        InvokeInstType,
        LoadInstType,
        BitCastInstType,
    };

protected:
    bool _is_fully_parsed;
    InstParser* _parser;
    InstType _type;
    string _opstr;
    BasicBlock* _parent;
    bool _has_assignment;
    int _dbg_id;
    string _owner;  // mainly for debug

public:
    Instruction();

    string opstr()                            { return _opstr; }
    void set_opstr(string op)                 { _opstr = op; }

    string owner()                            { return _owner; }
    void set_owner(string owner)             { _owner = owner; }

    bool is_fully_parsed()                    { return _is_fully_parsed; }
    void set_is_fully_parsed(bool v=1)        { _is_fully_parsed = v; }

    InstParser* parser()                      { return _parser; }

    InstType type()                           { return _type; }
    void set_type(InstType t)                 { _type = t; }

    int dbg_id()                              { return _dbg_id; }
    void set_dbg_id(int i)                    { _dbg_id = i; }

    DILocation* debug_loc();

    BasicBlock* parent()                      { return _parent; }
    void set_parent(BasicBlock* bb)           { _parent = bb; }

    bool has_assignment()                     { return _has_assignment; }
    void set_has_assignment(bool v=true)      { _has_assignment = v; }

    Function* function();
    void copy_metadata_from(Instruction* i);

    /** @brief Return the index of this instruction in its parent's instruction list
     *
     * @return
     */
    int get_index_in_block();

    /** @brief Clone this instruction except the parent.
     *
     * Each specific instruction should override clone().
     * @return
     */
    virtual Instruction* clone();
//    Function* function()                      { return _func; }
//    void set_function(Function* f)            { _func = f; }
};


#endif //LLPARSER_INSTRUCTION_H
