#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Program * ProgramSemanticAction(CircuitList * circuitList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->circuitList = circuitList;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

CircuitList * NewCircuitListSemanticAction(Circuit * circuit) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CircuitList * circuitList = calloc(1, sizeof(CircuitList));
	circuitList->current = circuit;
	circuitList->next = NULL;
	return circuitList;
}

CircuitList * AppendCircuitSemanticAction(CircuitList * circuitList, Circuit * circuit) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CircuitList * newNode = calloc(1, sizeof(CircuitList));
	newNode->current = circuit;
	newNode->next = NULL;
	CircuitList * aux = circuitList;
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = newNode;
	return circuitList;
}

void SetCurrentScopeSemanticAction(Identifier * identifier) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        _compilerState->currentScopeId = identifier->id;
}

Circuit * CircuitSemanticAction(Identifier * identifier, ElementList * elementList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Circuit * circuit = calloc(1, sizeof(Circuit));
	circuit->id = identifier;
	circuit->elementList = elementList;
    addDeclaration(_compilerState->symbolTable, circuit->id->id, TYPE_CIRCUIT, "");
    return circuit;
}

ElementList * EmptyElementListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ElementList * elementList = calloc(1, sizeof(ElementList));
	elementList->current = NULL;
	elementList->next = NULL;
	return elementList;
}

ElementList * NewElementListSemanticAction(Element * element) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ElementList * elementList = calloc(1, sizeof(ElementList));
	elementList->current = element;
	elementList->next = NULL;
	return elementList;
}

ElementList * AppendElementSemanticAction(ElementList * elementList, Element * element) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ElementList * newNode = calloc(1, sizeof(ElementList));
	newNode->current = element;
	newNode->next = NULL;

	ElementList * aux = elementList;
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = newNode;
	return elementList;
}

ElementList * TwoElementListSemanticAction(Element * firstElement, Element * secondElement) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ElementList * elementList = NewElementListSemanticAction(firstElement);
        return AppendElementSemanticAction(elementList, secondElement);
}

ElementList * ThreeElementListSemanticAction(Element * firstElement, Element * secondElement, Element * thirdElement) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ElementList * elementList = TwoElementListSemanticAction(firstElement, secondElement);
        return AppendElementSemanticAction(elementList, thirdElement);
}

ElementList * FourElementListSemanticAction(Element * firstElement, Element * secondElement, Element * thirdElement, Element * fourthElement) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ElementList * elementList = ThreeElementListSemanticAction(firstElement, secondElement, thirdElement);
        return AppendElementSemanticAction(elementList, fourthElement);
}

Element * ComponentElementSemanticAction(Component * component) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Element * element = calloc(1, sizeof(Element));
	element->component = component;
	element->type = ELEMENT_COMPONENT;
	return element;
}

Element * ParallelElementSemanticAction(Parallel * parallel) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Element * element = calloc(1, sizeof(Element));
	element->parallel = parallel;
	element->type = ELEMENT_PARALLEL;
	return element;
}

Parallel * ParallelSemanticAction(BranchList * branchList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parallel * parallel = calloc(1, sizeof(Parallel));
	parallel->branchList = branchList;
	return parallel;
}

BranchList * NewBranchListSemanticAction(Branch * leftBranch, Branch * rightBranch) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	BranchList * branchList = calloc(1, sizeof(BranchList));
	BranchList * secondNode = calloc(1, sizeof(BranchList));
	branchList->current = leftBranch;
	branchList->next = secondNode;
	secondNode->current = rightBranch;
	secondNode->next = NULL;
	return branchList;
}

BranchList * AppendBranchSemanticAction(BranchList * branchList, Branch * branch) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	BranchList * newNode = calloc(1, sizeof(BranchList));
	newNode->current = branch;
	newNode->next = NULL;

	BranchList * aux = branchList;
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = newNode;
	return branchList;
}

Branch * BranchSemanticAction(Identifier * identifier, ComponentList * componentList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Branch * branch = calloc(1, sizeof(Branch));
	branch->id = identifier;
	branch->componentList = componentList;
	addDeclaration(_compilerState->symbolTable, branch->id->id, TYPE_BRANCH, _compilerState->currentScopeId);
	return branch;
}

Component * ACSourceComponentSemanticAction(Identifier * identifier, ParameterList * parameterList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_AC_SOURCE;
	component->id = identifier;
	component->parameterList = parameterList;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_AC_SOURCE, _compilerState->currentScopeId);
	return component;
}

Component * DCSourceComponentSemanticAction(Identifier * identifier, ParameterList * parameterList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_DC_SOURCE;
	component->id = identifier;
	component->parameterList = parameterList;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_DC_SOURCE, _compilerState->currentScopeId);
	return component;
}

Component * ResistorComponentSemanticAction(Identifier * identifier, ParameterList * parameterList){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_RESISTOR;
	component->id = identifier;
	component->parameterList = parameterList;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_RESISTOR, _compilerState->currentScopeId);
	return component;
}

Component * VoltmeterComponentSemanticAction(Identifier * identifier){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_VOLTMETER;
	component->id = identifier;
	component->parameterList = NULL;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_VOLTMETER, _compilerState->currentScopeId);
	return component;
}

Component * AmmeterComponentSemanticAction(Identifier * identifier){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_AMMETER;
	component->id = identifier;
	component->parameterList = NULL;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_AMMETER, _compilerState->currentScopeId);
	return component;
}

Component * InductorComponentSemanticAction(Identifier * identifier, ParameterList * parameterList){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_INDUCTOR;
	component->id = identifier;
	component->parameterList = parameterList;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_INDUCTOR, _compilerState->currentScopeId);
	return component;
}

Component * CapacitorComponentSemanticAction(Identifier * identifier, ParameterList * parameterList){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_CAPACITOR;
	component->id = identifier;
	component->parameterList = parameterList;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_CAPACITOR, _compilerState->currentScopeId);
	return component;
}

Component * SwitchComponentSemanticAction(Identifier * identifier, ParameterList * parameterList){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Component * component = calloc(1, sizeof(Component));
	component->type = COMPONENT_SWITCH;
	component->id = identifier;
	component->parameterList = parameterList;
	addDeclaration(_compilerState->symbolTable, component->id->id, TYPE_SWITCH, _compilerState->currentScopeId);
	return component;
}

ParameterList * EmptyParameterListSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParameterList * parameterList = calloc(1, sizeof(ParameterList));
	parameterList->current = NULL;
	parameterList->next = NULL;
	return parameterList;
}

ParameterList * NewParameterListSemanticAction(Parameter * parameter) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParameterList * parameterList = calloc(1, sizeof(ParameterList));
	parameterList->current = parameter;
	parameterList->next = NULL;
	return parameterList;
}

ParameterList * AppendParameterSemanticAction(ParameterList * parameterList, Parameter * parameter) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParameterList * newNode = calloc(1, sizeof(ParameterList));
	newNode->current = parameter;
	newNode->next = NULL;

	ParameterList * aux = parameterList;
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = newNode;
	return parameterList;
}

ParameterList * AppendParameterListSemanticAction(ParameterList * baseParameterList, ParameterList * newParameterList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ParameterList * aux = baseParameterList;
	while(aux->next != NULL) {
		aux = aux->next;
	}
	aux->next = newParameterList;
	return baseParameterList;
}

ParameterList * ResistorParamsSemanticAction(ResistorType resistorType) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        Parameter * resistorTypeParameter = ParameterResistorTypeSemanticAction(resistorType);
        return NewParameterListSemanticAction(resistorTypeParameter);
}

ParameterList * ResistorParamsWithValueSemanticAction(ResistorType resistorType, ParameterList * valueParameters) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ParameterList * parameterList = ResistorParamsSemanticAction(resistorType);
        return AppendParameterListSemanticAction(parameterList, valueParameters);
}

ParameterList * SwitchParamsSemanticAction(SwitchState switchState) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        Parameter * switchStateParameter = ParameterSwitchStateSemanticAction(switchState);
        return NewParameterListSemanticAction(switchStateParameter);
}

ParameterList * ValueParamsSemanticAction(char * value) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        Parameter * valueParameter = ParameterValueSemanticAction(value);
        return NewParameterListSemanticAction(valueParameter);
}

ParameterList * ValueParamsWithUnitSemanticAction(char * value, UnitMultiplier unitMultiplier) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ParameterList * parameterList = ValueParamsSemanticAction(value);
        Parameter * unitMultiplierParameter = ParameterUnitMultiplierSemanticAction(unitMultiplier);
        return AppendParameterSemanticAction(parameterList, unitMultiplierParameter);
}

ParameterList * ComplexValueParamsSemanticAction(char * value) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        Parameter * valueParameter = ParameterValueSemanticAction(value);
        return NewParameterListSemanticAction(valueParameter);
}

ParameterList * ComplexValueParamsWithUnitSemanticAction(char * value, UnitMultiplier unitMultiplier) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ParameterList * parameterList = ComplexValueParamsSemanticAction(value);
        Parameter * unitMultiplierParameter = ParameterUnitMultiplierSemanticAction(unitMultiplier);
        return AppendParameterSemanticAction(parameterList, unitMultiplierParameter);
}

ParameterList * PolarityParamsSemanticAction(Polarity * polarity) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        Parameter * polarityParameter = ParameterPolaritySemanticAction(polarity);
        return NewParameterListSemanticAction(polarityParameter);
}

ParameterList * PolarityParamsWithValueSemanticAction(Polarity * polarity, ParameterList * valueParameters) {
        _logSyntacticAnalyzerAction(__FUNCTION__);
        ParameterList * parameterList = PolarityParamsSemanticAction(polarity);
        return AppendParameterListSemanticAction(parameterList, valueParameters);
}

Parameter * ParameterValueSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * parameter = calloc(1, sizeof(Parameter));
	parameter->value = value;
	parameter->type = VALUE;
	return parameter;
}

Parameter * ParameterUnitMultiplierSemanticAction(UnitMultiplier unitMultiplier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * parameter = calloc(1, sizeof(Parameter));
	parameter->unitMultiplier = unitMultiplier;
	parameter->type = UNIT_MULTIPLIER;
	return parameter;
}

Parameter * ParameterResistorTypeSemanticAction(ResistorType resistorType) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * parameter = calloc(1, sizeof(Parameter));
	parameter->resistorType = resistorType;
	parameter->type = RESISTOR_TYPE;
	return parameter;
}

Parameter * ParameterSwitchStateSemanticAction(SwitchState switchState) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * parameter = calloc(1, sizeof(Parameter));
	parameter->switchState = switchState;
	parameter->type = SWITCH_STATE;
	return parameter;
}

Parameter * ParameterPolaritySemanticAction(Polarity * polarity) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Parameter * parameter = calloc(1, sizeof(Parameter));
	parameter->polarity = polarity;
	parameter->type = POLARITY;
	return parameter;
}

Identifier * IdentifierSemanticAction(char * id) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Identifier * identifier = calloc(1, sizeof(Identifier));
	identifier->id = id;
	return identifier;
}

ComponentList * NewComponentListSemanticAction(Component * component) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ComponentList * componentList = calloc(1, sizeof(ComponentList));
	componentList->current = component;
	componentList->next = NULL;
	return componentList;
}

ComponentList * AppendComponentSemanticAction(ComponentList * componentList, Component * component) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ComponentList * newNode = calloc(1, sizeof(ComponentList));
	newNode->current = component;
	newNode->next = NULL;

	ComponentList * aux = componentList;
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = newNode;
	return componentList;
}