#include "LatexGenerator.h"

static Logger * _logger = NULL;

static void _shutdownLatexGeneratorModule();

static void _output(const char *format, ...);
static void _generateProgram(Program * program);
static void _generateCircuit(Circuit * circuit);
static void _generateNewPage();
static void _generateLeftSide(Element * element);
static void _generateTopSide(Element * element);
static void _generateRightSide(Element * element);
static void _generateBottomSide(Element * element);
static void _generateParallelLeft(Parallel * parallel);
static void _generateParallelBottom(Parallel * parallel);
static void _generateParallelRight(Parallel * parallel);
static void _generateParallelTop(Parallel * parallel);
static void _generateComponent(Component * component);
static void _generateLabel(Identifier * id, ParameterList * params, char * unit);
static void _generateMultiplier(UnitMultiplier multiplier);
static void _generateResistor(Identifier * id, ParameterList * params);
static void _generateDcSource(Identifier * id, ParameterList * params);
static void _generateAcSource(Identifier * id, ParameterList * params);
static void _generateCapacitor(Identifier * id, ParameterList* params);
static void _generateInductor(Identifier * id, ParameterList * params);
static void _generateSwitch(Identifier * id, ParameterList * params);
static void _generateAmmeter(Identifier * id);
static void _generateVoltmeter(Identifier * id);
static void _generatePrologue();
static void _generateGeneralPrologue();
static void _generateEpilogue();
static void _generateGeneralEpilogue();


ModuleDestructor initializeLatexGeneratorModule() {
        _logger = createLogger("LatexGenerator");
        return _shutdownLatexGeneratorModule;
}

static void _shutdownLatexGeneratorModule() {
        if (_logger != NULL) {
                logDebugging(_logger, "Destroying module: LatexGenerator...");
                destroyLogger(_logger);
                _logger = NULL;
        }
}

void _generateGeneralEpilogue(){
	_output("\\end{document}\n");
}

void _generateGeneralPrologue(){
	_output("\\documentclass{article}\n");
	_output("\\usepackage{circuitikz}\n");
	_output("\\begin{document}\n");
}

void _generatePrologue(){
	_output("\\begin{circuitikz}\n");
	_output("\\draw\n");
 }

 void _generateEpilogue(){
	_output(";\n");
	_output("\\end{circuitikz}\n");
 }

void _generateParallelBottom(Parallel * parallel){
	BranchList * branches = parallel->branchList;
    int i = 0;

    if (branches->current->componentList->current != NULL){
        _generateComponent(branches->current->componentList->current);
        _output("(0,0)\n");
    }
    branches = branches->next;

    while (branches != NULL){
        _output("(1,%d) to (1,%d)\n", i, i-2);
        _output("(3,%d) to (3,%d)\n", i, i-2);

        _output("(1,%d) to", i-2);
        _generateComponent(branches->current->componentList->current);
        _output("(3,%d)\n", i-2);

        i = i - 2;
        branches = branches->next;
    }
}

void _generateParallelTop(Parallel * parallel){
	BranchList * branches = parallel->branchList;
    int i = 4; 

    if (branches->current->componentList->current != NULL){
        _generateComponent(branches->current->componentList->current);
        _output("(4,4)\n");
    }
    branches = branches->next;

    while (branches != NULL){
        _output("(1,%d) to (1,%d)\n", i, i+2);
        _output("(3,%d) to (3,%d)\n", i, i+2);

        _output("(1,%d) to", i+2);
        _generateComponent(branches->current->componentList->current);
        _output("(3,%d)\n", i+2);

        i = i + 2;
        branches = branches->next;
    }
}

void _generateParallelRight(Parallel * parallel){
	BranchList * branches = parallel->branchList;
    int i = 4;
    if (branches->current->componentList->current != NULL){
        _generateComponent(branches->current->componentList->current);
        _output("(4,0)\n");
    }
    branches = branches->next;

    while (branches != NULL){
        _output("(%d,1) to (%d,1)\n", i, i+2);
        _output("(%d,3) to (%d,3)\n", i, i+2);

        _output("(%d,1) to", i+2);
        _generateComponent(branches->current->componentList->current);
        _output("(%d,3)\n", i+2);

        i = i + 2;
        branches = branches->next;
    }
}

void _generateParallelLeft(Parallel * parallel){
	BranchList * branches = parallel->branchList;
	int i = 0;
	if (branches->current->componentList->current != NULL){
		_generateComponent(branches->current->componentList->current);
		_output("(0,4)\n");
	}	
	branches = branches->next;

	while (branches != NULL){
		_output("(%d, 1) to (%d, 1)\n", i, i-2);
		_output("(%d, 3) to (%d, 3)\n", i, i-2);
		_output("(%d, 1) to", i-2);
		_generateComponent(branches->current->componentList->current);
		_output("(%d, 3)\n", i-2);
		i = i-2;
		branches = branches->next;
	}
}

void _generateSwitch(Identifier * id, ParameterList * params){
	if (params->current->switchState == OPEN){
		_output("[nos, l=$%s$] ", id->id);
	}else{
		_output("[ncs, l=$%s$] ", id->id);
	}
}

void _generateVoltmeter(Identifier * id){
	_output("[voltmeter, l=$%s$] ", id->id);
}

void _generateAmmeter(Identifier * id){
	_output("[ammeter, l=$%s$] ", id->id);
}

void _generateInductor(Identifier * id, ParameterList * params){
	_output("[L, ");
	_generateLabel(id, params, "H");
	_output("] ");
}

void _generateAcSource(Identifier * id, ParameterList * params){
	_output("[sI, ");
	_generateLabel(id, params, "V");
	_output("] ");
}

void _generateDcSource(Identifier * id, ParameterList * params){
	_output("[battery1, ");
	if (params != NULL && params->current != NULL && params->current->type == POLARITY){
		if(params->current->polarity != NULL && *(params->current->polarity) == POSITIVE_FIRST){
			_output("invert, ");
		}
		params = params->next;
	}else{
		_output("invert, ");
	}
	_generateLabel(id, params, "V");
	_output("] ");
}

void _generateMultiplier(UnitMultiplier multiplier){
	switch (multiplier){
		case GIGA:
			_output("G");
			break;
		case MEGA:
			_output("M");
			break;
		case KILO:
			_output("k");
			break;
		case MILLI:
			_output("m");
			break;
		case MICRO:
			_output("\\mu ");
			break;
		case NANO:
			_output("n");
			break;
	}
}

void _generateLabel(Identifier * id, ParameterList * params, char * unit){
	if (params != NULL && params->current != NULL && params->current->type == VALUE){
		_output("l=$%s", params->current->value);
		params = params->next;
		if (params != NULL){
			_generateMultiplier(params->current->unitMultiplier);
		}
		_output("%s$", unit);
	}else {
		_output("l=$%s$", id->id);
	}
}

void _generateResistor(Identifier * id, ParameterList * params){
	switch (params->current->resistorType){
	case REGULAR:
		_output("[R, ");
		break;
	case PHOTORESISTOR:
		_output("[photoresistor, ");
		break;
	case THERMISTOR:
		_output("[thermistor, ");
		break;
	case POTENTIOMETER:
		_output("[potentiometer, ");
		break;
	}
	params = params->next;
	_generateLabel(id, params, "\\Omega");
	_output("] ");
}

void _generateCapacitor(Identifier * id, ParameterList * params) {
	if(params->current == NULL || params->current->type != POLARITY) {
		_output("[C, ");
	} else {
		_output("[eC, ");
		if(params->current->polarity != NULL && *(params->current->polarity) == POSITIVE_FIRST) {
			_output("invert, ");
		}
		params = params->next;
	}
	_generateLabel(id, params, "F");
	_output("] ");
}

void _generateComponent(Component * component){
	switch (component->type){
		case COMPONENT_RESISTOR:
			_generateResistor(component->id, component->parameterList);
			break;
		case COMPONENT_AC_SOURCE:
			_generateAcSource(component->id, component->parameterList);
			break;
		case COMPONENT_DC_SOURCE:
			_generateDcSource(component->id, component->parameterList);
			break;
		case COMPONENT_INDUCTOR:
			_generateInductor(component->id, component->parameterList);
			break;
		case COMPONENT_AMMETER:
			_generateAmmeter(component->id);
			break;
		case COMPONENT_VOLTMETER:
			_generateVoltmeter(component->id);
			break;
		case COMPONENT_SWITCH:
			_generateSwitch(component->id, component->parameterList);
			break;
		case COMPONENT_CAPACITOR:
			_generateCapacitor(component->id, component->parameterList);
			break;
	}
}

void _generateBottomSide(Element * element){
	_output("(4, 0) to");
	if (element == NULL) {
		_output("(0, 0)\n");
		return;
	}
	if (element->type == ELEMENT_COMPONENT){
		_generateComponent(element->component);
	}else if (element->type == ELEMENT_PARALLEL){
		_generateParallelBottom(element->parallel);
		return;
	}
	_output("(0, 0)\n");
}

void _generateRightSide(Element * element){
	_output("(4, 4) to");
	if (element == NULL) {
		_output("(4, 0)\n");
		return;
	}
	if (element->type == ELEMENT_COMPONENT){
		_generateComponent(element->component);
	}else if (element->type == ELEMENT_PARALLEL){
		_generateParallelRight(element->parallel);
		return;
	}
	_output("(4, 0)\n");
}

void _generateTopSide(Element * element){
	_output("(0, 4) to");
	if (element == NULL) {
		_output("(4, 4)\n");
		return;
	}
	if (element->type == ELEMENT_COMPONENT){
		_generateComponent(element->component);
	}else if (element->type == ELEMENT_PARALLEL){
		_generateParallelTop(element->parallel);
		return;
	}
	_output("(4, 4)\n");
}

void _generateLeftSide(Element * element){
	_output("(0, 0) to");
	if (element == NULL) {
		_output("(0, 4)\n");
		return;
	}
	if (element->type == ELEMENT_COMPONENT){
		_generateComponent(element->component);
	}else if (element->type == ELEMENT_PARALLEL){
		_generateParallelLeft(element->parallel);
		return;
	}
	_output("(0, 4)\n");
}

void _generateCircuit(Circuit * circuit){
	ElementList *elements = circuit->elementList;

    _generateLeftSide(elements ? elements->current : NULL);

    elements = elements ? elements->next : NULL;
    _generateTopSide(elements ? elements->current : NULL);

    elements = elements ? elements->next : NULL;
    _generateRightSide(elements ? elements->current : NULL);

	elements = elements ? elements->next : NULL;
    _generateBottomSide(elements ? elements->current : NULL);
}

void _generateNewPage(){
	_output("\\newpage\n");
}

void _generateProgram(Program * program){
	size_t i = 0;
	CircuitList * list = program->circuitList;
	while (list != NULL){
		if (i > 0) _generateNewPage();
		
		_generatePrologue();
		_generateCircuit(list->current);
		_generateEpilogue();
		list = list->next;
		i++;
	}
}

void _output(const char *format, ...){
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    fflush(stdout);
    va_end(args);
}

void executeGenerator(CompilerState * compilerState) {
 	if (_logger == NULL) {
			_logger = createLogger("LatexGenerator");
	}
	logDebugging(_logger, "Generating final output...");
	_generateGeneralPrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
 	_generateGeneralEpilogue();
 	logDebugging(_logger, "Generation is done.");
}
