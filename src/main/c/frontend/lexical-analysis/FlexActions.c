#include "FlexActions.h"
#include <string.h>
#include <stdlib.h>

/* MODULE INTERNAL STATE */

static bool _logIgnoredLexemes = true;
static InputBuffer * _inputBuffer = NULL;
static LexicalAnalyzer * _lexicalAnalyzer = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownFlexActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: FlexActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	if (_inputBuffer != NULL) {
		destroyInputBuffer(_inputBuffer);
		_inputBuffer = NULL;
	}
	_lexicalAnalyzer = NULL;
}

ModuleDestructor initializeFlexActionsModule(LexicalAnalyzer * lexicalAnalyzer) {
	_inputBuffer = NULL;
	_lexicalAnalyzer = lexicalAnalyzer;
	_logger = createLogger("FlexActions");
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	return _shutdownFlexActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logTokenAction(const char * actionName, Token * token);

/**
 * Logs a lexical-analyzer action over a token in DEBUGGING level.
 */
static void _logTokenAction(const char * actionName, Token * token) {
	char * _lexeme = escape(token->lexeme);
	logDebugging(_logger, WARNING_COLOR "%s" DEFAULT_COLOR ": Token(context=%d, label=%d, length=%d, lexeme=%s\"%s\"%s, line=%d, semanticValue=%p)",
		actionName,
		token->context,
		token->label,
		token->length,
		INFORMATION_COLOR, _lexeme, DEFAULT_COLOR,
		token->line,
		token->semanticValue);
	free(_lexeme);
	_lexeme = NULL;
}

/* PUBLIC FUNCTIONS */

CompilationStatus KeywordLexemeAction(TokenLabel label) {
    Token * token = createToken(_lexicalAnalyzer, label);
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus IdentifierLexemeAction(TokenLabel label) {
    Token * token = createToken(_lexicalAnalyzer, label);
    _logTokenAction(__FUNCTION__, token);
	token->semanticValue->string = strdup(token->lexeme);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus RealValueLexemeAction() {
    Token * token = createToken(_lexicalAnalyzer, REAL_VALUE_TOKEN);
    _logTokenAction(__FUNCTION__, token);
	token->semanticValue->string = strdup(token->lexeme);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus ComplexValueLexemeAction() {
    Token * token = createToken(_lexicalAnalyzer, COMPLEX_VALUE_TOKEN);
    _logTokenAction(__FUNCTION__, token);
	token->semanticValue->string = strdup(token->lexeme);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus UnitMultiplierLexemeAction(UnitMultiplier unitMultiplier) {
    Token * token = createToken(_lexicalAnalyzer, UNIT_MULTIPLIER_TOKEN);
	token->semanticValue->unitMultiplierToken = unitMultiplier;
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus ResistorTypeLexemeAction(ResistorType resistorType) {
    Token * token = createToken(_lexicalAnalyzer, RESISTOR_TYPE_TOKEN);
	token->semanticValue->resistorTypeToken = resistorType;
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus SwitchStateLexemeAction(SwitchState switchState) {
    Token * token = createToken(_lexicalAnalyzer, SWITCH_STATE_TOKEN);
	token->semanticValue->switchStateToken = switchState;
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus PolarityLexemeAction(TokenLabel label) {
    Token * token = createToken(_lexicalAnalyzer, label);
	Polarity * polarity = calloc(1, sizeof(Polarity));
    *polarity = (label == POSITIVE_FIRST_TOKEN) ? POSITIVE_FIRST : NEGATIVE_FIRST;
    token->semanticValue->polarityToken = polarity;
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus CommaLexemeAction() {
    Token * token = createToken(_lexicalAnalyzer, COMMA);
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus BraceLexemeAction(TokenLabel label) {
    Token * token = createToken(_lexicalAnalyzer, label);
    _logTokenAction(__FUNCTION__, token);
    CompilationStatus status = pushToken(_lexicalAnalyzer, token);
    destroyToken(token);
    return status;
}

CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context) {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, OPEN_COMMENT);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	enterLexicalAnalyzerContext(_lexicalAnalyzer, context);
	return IN_PROGRESS;
}

CompilationStatus EOFLexemeAction() {
	CompilationStatus status = IN_PROGRESS;
	Token * token = createToken(_lexicalAnalyzer, 0);
	_logTokenAction(__FUNCTION__, token);
	if (!popInputBuffer(_lexicalAnalyzer)) {
		status = pushToken(_lexicalAnalyzer, token);
		FlexContext context = currentLexicalAnalyzerContext(_lexicalAnalyzer);
		if (0 < context) {
			logError(_logger, "The final context is not closed (context=%d).", context);
			status = FAILED;
		}
	}
	destroyToken(token);
	return status;
}

CompilationStatus IgnoredLexemeAction() {
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, IGNORED);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus LeaveMultilineCommentLexemeAction() {
	leaveLexicalAnalyzerContext(_lexicalAnalyzer);
	if (_logIgnoredLexemes) {
		Token * token = createToken(_lexicalAnalyzer, CLOSE_COMMENT);
		_logTokenAction(__FUNCTION__, token);
		destroyToken(token);
	}
	return IN_PROGRESS;
}

CompilationStatus ParenthesisLexemeAction(TokenLabel label) {
	Token * token = createToken(_lexicalAnalyzer, label);
	_logTokenAction(__FUNCTION__, token);
	CompilationStatus status = pushToken(_lexicalAnalyzer, token);
	destroyToken(token);
	return status;
}

CompilationStatus UnknownLexemeAction() {
	Token * token = createToken(_lexicalAnalyzer, UNKNOWN);
	_logTokenAction(__FUNCTION__, token);
	destroyToken(token);
	return FAILED;
}