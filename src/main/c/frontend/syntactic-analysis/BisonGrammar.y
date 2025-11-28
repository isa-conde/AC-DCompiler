%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	TokenLabel token;
    UnitMultiplier unitMultiplierToken;
    char * string;
    Polarity * polarityToken;
    ResistorType resistorTypeToken;
    SwitchState switchStateToken;

	/** Non-terminals. */

	Program * program;
    Circuit * circuit;
    CircuitList * circuitList;
    Element * element;
    ElementList * elementList, elementListOpt;
    Component * component;
    ComponentList * componentList;
    ParameterList * componentParamsOpt, * parameterList, * parameterListOpt;
    Identifier * identifier;
    Branch * branch;
    BranchList * branchList;
    Parallel * parallel;
    Polarity * polarity;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */

%destructor { destroyCircuit($$); }        <circuit>
%destructor { destroyCircuitList($$); }    <circuitList>
%destructor { destroyElement($$); }        <element>
%destructor { destroyElementList($$); }    <elementList>
%destructor { destroyParallel($$); }       <parallel>
%destructor { destroyBranch($$); }         <branch>
%destructor { destroyBranchList($$); }     <branchList>
%destructor { destroyComponent($$); }      <component>
%destructor { destroyComponentList($$); }  <componentList>
%destructor { destroyParameterList($$); }  <parameterList>
%destructor { destroyIdentifier($$); }     <identifier>
%destructor { destroyPolarity($$); }       <polarity>

/* ---------- Terminals. ---------- */
%token <string> ID
%token <token> CIRCUIT
%token <token> PARALLEL
%token <token> BRANCH
%token <token> AC_SOURCE
%token <token> DC_SOURCE
%token <token> RESISTOR
%token <token> CAPACITOR
%token <token> INDUCTOR
%token <token> SWITCH
%token <token> VOLTMETER
%token <token> AMMETER

%token <polarityToken> POSITIVE_FIRST_TOKEN
%token <polarityToken> NEGATIVE_FIRST_TOKEN

%token <unitMultiplierToken> UNIT_MULTIPLIER_TOKEN
%token <string> REAL_VALUE_TOKEN
%token <string> COMPLEX_VALUE_TOKEN
%token <resistorTypeToken> RESISTOR_TYPE_TOKEN
%token <switchStateToken> SWITCH_STATE_TOKEN

%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT
%token <token> COMMA

%token <token> IGNORED
%token <token> UNKNOWN

/* ---------- Non-terminals. ---------- */
%type <program> program
%type <circuit> circuit
%type <circuitList> circuitList
%type <element> element
%type <elementList> elementList elementListOpt
%type <component> component
%type <componentList> componentList
%type <parameterList> valueParams complexValueParams polarityParams emptyParams
%type <parameterList> inductorParams resistorParams capacitorParams switchParams directSourceParams alternatingSourceParams
%type <identifier> identifier
%type <branch> branch
%type <branchList> branchList
%type <parallel> parallel
%type <polarity> polarity

%%

/* Program: one or more independent circuits */
program: circuitList                                 { $$ = ProgramSemanticAction($1); }
    ;

circuitList: 
      circuit                            	{ $$ = NewCircuitListSemanticAction($1); }
    | circuitList COMMA circuit             { $$ = AppendCircuitSemanticAction($1, $3); }
    ;

/* Circuit: "Circuit" ID { body } */
circuit: CIRCUIT identifier { SetCurrentScopeSemanticAction($2); } OPEN_BRACE elementListOpt CLOSE_BRACE		{ $$ = CircuitSemanticAction($2, $5); }
    ;

/* Element list inside a circuit or branch */
elementListOpt: 
      %empty                             	{ $$ = EmptyElementListSemanticAction(); }
	| elementList                           { $$ = $1; }
    ;

elementList:
	    element                                             { $$ = NewElementListSemanticAction($1); }
    | element COMMA element                                 { $$ = TwoElementListSemanticAction($1, $3); }
    | element COMMA element COMMA element                   { $$ = ThreeElementListSemanticAction($1, $3, $5); }
    | element COMMA element COMMA element COMMA element     { $$ = FourElementListSemanticAction($1, $3, $5, $7); }
    ;

/* Elementos posibles */
element:
      component                                { $$ = ComponentElementSemanticAction($1); }
    | parallel                                 { $$ = ParallelElementSemanticAction($1); }
    ;

/* Parallel: "Parallel" { elementos } */
parallel:
      PARALLEL OPEN_BRACE branchList CLOSE_BRACE	{ $$ = ParallelSemanticAction($3); }
    ;

branchList: 
      branch COMMA branch                           { $$ = NewBranchListSemanticAction($1, $3); }
	| branchList COMMA branch                       { $$ = AppendBranchSemanticAction($1, $3); }
    ;

componentList:
      component                                        { $$ = NewComponentListSemanticAction($1); }
    ;

/* Branch: "Branch" ID { componentes } */
branch:
      BRANCH identifier OPEN_BRACE componentList CLOSE_BRACE
                                                    { $$ = BranchSemanticAction($2, $4); }
    ;

/* Components: can have parenthesis, parameters (posibles parámetros) o no */
component:
      AC_SOURCE identifier alternatingSourceParams      { $$ = ACSourceComponentSemanticAction($2, $3); }
    | DC_SOURCE identifier directSourceParams           { $$ = DCSourceComponentSemanticAction($2, $3); }
    | RESISTOR identifier resistorParams                { $$ = ResistorComponentSemanticAction($2, $3); }
    | VOLTMETER identifier                              { $$ = VoltmeterComponentSemanticAction($2); }
    | AMMETER identifier                                { $$ = AmmeterComponentSemanticAction($2); }
    | INDUCTOR identifier inductorParams                { $$ = InductorComponentSemanticAction($2, $3); }
    | CAPACITOR identifier capacitorParams              { $$ = CapacitorComponentSemanticAction($2, $3); }
    | SWITCH identifier switchParams                    { $$ = SwitchComponentSemanticAction($2, $3); }
    ;

directSourceParams:
      emptyParams                                                                                        { $$ = $1; }
    | OPEN_PARENTHESIS polarityParams CLOSE_PARENTHESIS                                                  { $$ = $2; }
    | OPEN_PARENTHESIS valueParams CLOSE_PARENTHESIS                                                     { $$ = $2; }

alternatingSourceParams:
      emptyParams                                                                                        { $$ = $1; }
    | OPEN_PARENTHESIS complexValueParams CLOSE_PARENTHESIS                                              { $$ = $2; }
    | OPEN_PARENTHESIS valueParams CLOSE_PARENTHESIS                                                     { $$ = $2; }

inductorParams: 
      emptyParams                                                                                        { $$ = $1; }
    | OPEN_PARENTHESIS valueParams CLOSE_PARENTHESIS                                                     { $$ = $2; }

capacitorParams: 
      emptyParams                                                                                        { $$ = $1; }
    | OPEN_PARENTHESIS polarityParams CLOSE_PARENTHESIS                                                  { $$ = $2; }
    | OPEN_PARENTHESIS valueParams CLOSE_PARENTHESIS                                                     { $$ = $2; }

resistorParams:
      OPEN_PARENTHESIS RESISTOR_TYPE_TOKEN CLOSE_PARENTHESIS                                        { $$ = ResistorParamsSemanticAction($2); }
    | OPEN_PARENTHESIS RESISTOR_TYPE_TOKEN COMMA valueParams CLOSE_PARENTHESIS                      { $$ = ResistorParamsWithValueSemanticAction($2, $4); }

switchParams:
      OPEN_PARENTHESIS SWITCH_STATE_TOKEN CLOSE_PARENTHESIS                                         { $$ = SwitchParamsSemanticAction($2); }

valueParams: 
      REAL_VALUE_TOKEN                                                                              { $$ = ValueParamsSemanticAction($1); }
    | REAL_VALUE_TOKEN COMMA UNIT_MULTIPLIER_TOKEN                                                  { $$ = ValueParamsWithUnitSemanticAction($1, $3); }

complexValueParams: 
      COMPLEX_VALUE_TOKEN                                                                           { $$ = ComplexValueParamsSemanticAction($1); }
    | COMPLEX_VALUE_TOKEN COMMA UNIT_MULTIPLIER_TOKEN                                               { $$ = ComplexValueParamsWithUnitSemanticAction($1, $3); }

polarityParams:
      polarity                                                                                      { $$ = PolarityParamsSemanticAction($1); }
    | polarity COMMA valueParams                                                                    { $$ = PolarityParamsWithValueSemanticAction($1, $3); }

emptyParams: %empty                                                                                 { $$ = EmptyParameterListSemanticAction(); }
    | OPEN_PARENTHESIS CLOSE_PARENTHESIS                                                            { $$ = EmptyParameterListSemanticAction(); }

identifier: ID                                            { $$ = IdentifierSemanticAction($1); }
    ;

polarity: 
      POSITIVE_FIRST_TOKEN                                      { $$ = $1; }
    | NEGATIVE_FIRST_TOKEN                                      { $$ = $1; }

%%
