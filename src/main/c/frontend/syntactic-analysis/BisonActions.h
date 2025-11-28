#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "../../support/symbol-table/SymbolTable.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions.
 */

Program * ProgramSemanticAction(CircuitList * circuitList);
CircuitList * NewCircuitListSemanticAction(Circuit * circuit);
CircuitList * AppendCircuitSemanticAction(CircuitList * circuitList, Circuit * circuit);
void SetCurrentScopeSemanticAction(Identifier * identifier);
Circuit * CircuitSemanticAction(Identifier * identifier, ElementList * elementList);
ElementList * EmptyElementListSemanticAction();
ElementList * NewElementListSemanticAction(Element * element);
ElementList * AppendElementSemanticAction(ElementList * elementList, Element * element);
ElementList * TwoElementListSemanticAction(Element * firstElement, Element * secondElement);
ElementList * ThreeElementListSemanticAction(Element * firstElement, Element * secondElement, Element * thirdElement);
ElementList * FourElementListSemanticAction(Element * firstElement, Element * secondElement, Element * thirdElement, Element * fourthElement);
Element * ComponentElementSemanticAction(Component * component);
Element * ParallelElementSemanticAction(Parallel * parallel);
Parallel * ParallelSemanticAction(BranchList * branchList);
BranchList * NewBranchListSemanticAction(Branch * leftBranch, Branch * rightBranch);
BranchList * AppendBranchSemanticAction(BranchList * branchList, Branch * branch);
Branch * BranchSemanticAction(Identifier * identifier, ComponentList * componentList);
Component * ACSourceComponentSemanticAction(Identifier * identifier, ParameterList * parameterList);
Component * DCSourceComponentSemanticAction(Identifier * identifier, ParameterList * parameterList);
Component * ResistorComponentSemanticAction(Identifier * identifier, ParameterList * parameterList);
Component * VoltmeterComponentSemanticAction(Identifier * identifier);
Component * AmmeterComponentSemanticAction(Identifier * identifier);
Component * InductorComponentSemanticAction(Identifier * identifier, ParameterList * parameterList);
Component * CapacitorComponentSemanticAction(Identifier * identifier, ParameterList * parameterList);
Component * SwitchComponentSemanticAction(Identifier * identifier, ParameterList * parameterList);
ParameterList * EmptyParameterListSemanticAction();
ParameterList * NewParameterListSemanticAction(Parameter * parameter);
ParameterList * AppendParameterSemanticAction(ParameterList * parameterList, Parameter * parameter);
ParameterList * AppendParameterListSemanticAction(ParameterList * baseParameterList, ParameterList * newParameterList);
ParameterList * ResistorParamsSemanticAction(ResistorType resistorType);
ParameterList * ResistorParamsWithValueSemanticAction(ResistorType resistorType, ParameterList * valueParameters);
ParameterList * SwitchParamsSemanticAction(SwitchState switchState);
ParameterList * ValueParamsSemanticAction(char * value);
ParameterList * ValueParamsWithUnitSemanticAction(char * value, UnitMultiplier unitMultiplier);
ParameterList * ComplexValueParamsSemanticAction(char * value);
ParameterList * ComplexValueParamsWithUnitSemanticAction(char * value, UnitMultiplier unitMultiplier);
ParameterList * PolarityParamsSemanticAction(Polarity * polarity);
ParameterList * PolarityParamsWithValueSemanticAction(Polarity * polarity, ParameterList * valueParameters);
Parameter * ParameterValueSemanticAction(char * value);
Parameter * ParameterUnitMultiplierSemanticAction(UnitMultiplier unitMultiplier);
Parameter * ParameterResistorTypeSemanticAction(ResistorType resistorType);
Parameter * ParameterSwitchStateSemanticAction(SwitchState switchState);
Parameter * ParameterPolaritySemanticAction(Polarity * polarity);
Identifier * IdentifierSemanticAction(char * identifier);
ComponentList * NewComponentListSemanticAction(Component * component);
ComponentList * AppendComponentSemanticAction(ComponentList * componentList, Component * component);

#endif
