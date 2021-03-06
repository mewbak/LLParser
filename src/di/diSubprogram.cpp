//
// Created by tlaber on 6/15/17.
//

#include <asmParser/sysDict.h>
#include "diEssential.h"

DISubprogram::DISubprogram() {
    _scope = NULL;
    _file = NULL;
    _line = -1;
}

void DISubprogram::resolve_non_refs() {
    DI_SET_STR_FIELD(name);
    DI_SET_STR_FIELD(linkageName);
    DI_SET_INT_FIELD(line);
}

void DISubprogram::resolve_refs() {
    // todo: scope field could also be of type DICompositeType in flang's IR
    /* C: a function's scope is a DIFile
     * C++: a class method's scope is a DICompositeType that represents the class
     */
    //DI_SET_REF_FIELD(scope, DIScope); // need to parse DICompositeType
    DI_SET_REF_FIELD(file, DIFile);
}

string DISubprogram::to_string() {
    return filename()+" "+linkageName();
}