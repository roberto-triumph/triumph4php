/**
 * @copyright  2009-2011 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
 * This software is released under the terms of the MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "language_php/CallStackClass.h"
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <wx/ffile.h>
#include <algorithm>
#include <string>
#include <vector>
#include "globals/Assets.h"
#include "globals/Sqlite.h"
#include "globals/String.h"

t4p::VariableSymbolClass::VariableSymbolClass()
    : Type(t4p::VariableSymbolClass::SCALAR)
    , DestinationVariable()
    , ScalarValue()
    , ArrayKey()
    , SourceVariable()
    , ObjectName()
    , PropertyName()
    , FunctionName()
    , ClassName()
    , FunctionArguments() {
}

void t4p::VariableSymbolClass::ToScalar(const UnicodeString& variableName, const UnicodeString& scalar) {
    Type = t4p::VariableSymbolClass::SCALAR;
    DestinationVariable = variableName;
    ScalarValue = scalar;
}

void t4p::VariableSymbolClass::ToArray(const UnicodeString& variableName) {
    Type = t4p::VariableSymbolClass::ARRAY;
    DestinationVariable = variableName;
}

void t4p::VariableSymbolClass::ToArrayKey(const UnicodeString& variableName, const UnicodeString& keyName) {
    Type = t4p::VariableSymbolClass::ARRAY_KEY;
    DestinationVariable = variableName;
    ArrayKey = keyName;
}

void t4p::VariableSymbolClass::ToNewObject(const UnicodeString& variableName, const UnicodeString& className) {
    Type = t4p::VariableSymbolClass::NEW_OBJECT;
    DestinationVariable = variableName;
    ClassName = className;
}

void t4p::VariableSymbolClass::ToAssignment(const UnicodeString& variableName, const UnicodeString& sourceVariableName) {
    Type = t4p::VariableSymbolClass::ASSIGN;
    DestinationVariable = variableName;
    SourceVariable = sourceVariableName;
}

void t4p::VariableSymbolClass::ToProperty(const UnicodeString& variableName, const UnicodeString& objectName,
        const UnicodeString& propertyName) {
    Type = t4p::VariableSymbolClass::PROPERTY;
    DestinationVariable = variableName;
    ObjectName = objectName;
    PropertyName = propertyName;
}

void t4p::VariableSymbolClass::ToMethodCall(const UnicodeString& variableName, const UnicodeString& objectName,
        const UnicodeString& methodName,
        const std::vector<UnicodeString> arguments) {
    Type = t4p::VariableSymbolClass::METHOD_CALL;
    DestinationVariable = variableName;
    ObjectName = objectName;
    MethodName = methodName;
    FunctionArguments = arguments;
}

void t4p::VariableSymbolClass::ToFunctionCall(const UnicodeString& variableName,
        const UnicodeString& functionName, const std::vector<UnicodeString> arguments) {
    Type = t4p::VariableSymbolClass::FUNCTION_CALL;
    DestinationVariable = variableName;
    FunctionName = functionName;
    FunctionArguments = arguments;
}

void t4p::VariableSymbolClass::ToBeginMethod(const UnicodeString& className,
        const UnicodeString& methodName) {
    Type = t4p::VariableSymbolClass::BEGIN_METHOD;
    ClassName = className;
    MethodName = methodName;
}

void t4p::VariableSymbolClass::ToBeginFunction(const UnicodeString& functionName) {
    Type = t4p::VariableSymbolClass::FUNCTION_CALL;
    FunctionName = functionName;
}

std::string t4p::VariableSymbolClass::ToString() const {
    UnicodeString line;
    switch (Type) {
    case ARRAY:
        line = DestinationVariable;
        break;
    case ARRAY_KEY:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += ArrayKey;
        break;
    case ASSIGN:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += SourceVariable;
        break;
    case SCALAR:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += ScalarValue;
        break;
    case NEW_OBJECT:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += ClassName;
        break;
    case PROPERTY:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += ObjectName;
        line += UNICODE_STRING_SIMPLE(",");
        line += PropertyName;
        break;
    case METHOD_CALL:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += ObjectName;
        line += UNICODE_STRING_SIMPLE(",");
        line += MethodName;
        for (std::vector<UnicodeString>::const_iterator it = FunctionArguments.begin(); it != FunctionArguments.end(); ++it) {
            line += UNICODE_STRING_SIMPLE(",");
            line += *it;
        }
        break;
    case FUNCTION_CALL:
        line = DestinationVariable;
        line += UNICODE_STRING_SIMPLE(",");
        line += ObjectName;
        line += UNICODE_STRING_SIMPLE(",");
        line += MethodName;
        for (std::vector<UnicodeString>::const_iterator it = FunctionArguments.begin(); it != FunctionArguments.end(); ++it) {
            line += UNICODE_STRING_SIMPLE(",");
            line += *it;
        }
        break;
    case BEGIN_METHOD:
        line += ClassName;
        line += UNICODE_STRING_SIMPLE(",");
        line += MethodName;
        break;
    case BEGIN_FUNCTION:
        line += FunctionName;
        break;
    }
    std::string stdLine = t4p::IcuToChar(line);
    return stdLine;
}

std::string t4p::VariableSymbolClass::TypeString() const {
    std::string line;
    switch (Type) {
    case ARRAY:
        line = "ARRAY";
        break;
    case ARRAY_KEY:
        line = "ARRAY_KEY";
        break;
    case ASSIGN:
        line = "ASSIGN";
        break;
    case SCALAR:
        line = "SCALAR";
        break;
    case NEW_OBJECT:
        line = "NEW_OBJECT";
        break;
    case PROPERTY:
        line = "PROPERTY";
        break;
    case METHOD_CALL:
        line = "METHOD_CALL";
        break;
    case FUNCTION_CALL:
        line = "FUNCTION_CALL";
        break;
    case BEGIN_METHOD:
        line = "BEGIN_METHOD";
        break;
    case BEGIN_FUNCTION:
        line = "BEGIN_FUNCTION";
        break;
    }
    return line;
}

t4p::CallStackClass::CallStackClass(t4p::TagCacheClass& tagCache)
    : Variables()
    , LintResults()
    , MatchError()
    , Parser()
    , CurrentClass()
    , CurrentMethod()
    , CurrentFunction()
    , ResourcesRemaining()
    , TagCache(tagCache)
    , ParsedMethods()
    , TempVarIndex(1)
    , FoundScope(false) {
    Parser.SetClassObserver(this);
    Parser.SetClassMemberObserver(this);
    Parser.SetFunctionObserver(this);
    Parser.SetExpressionObserver(this);
}

void t4p::CallStackClass::Clear() {
    Variables.clear();
    LintResults.Clear();
    MatchError.Clear();
    CurrentClass.remove();
    CurrentMethod.remove();
    CurrentFunction.remove();
    ParsedMethods.clear();
    while (!ResourcesRemaining.empty()) {
        ResourcesRemaining.pop();
    }
    TempVarIndex = 1;
    FoundScope = false;
}

bool t4p::CallStackClass::Build(const wxFileName& fileName, const UnicodeString& className, const UnicodeString& methodName,
                                pelet::Versions version, t4p::CallStackClass::Errors& error) {
    Clear();
    t4p::PhpTagClass nextResource;
    nextResource.Type = t4p::PhpTagClass::METHOD;
    nextResource.ClassName = className;
    nextResource.Identifier = methodName;

    ResourceWithFile item;
    item.FileName = fileName;
    item.Resource = nextResource;
    ResourcesRemaining.push(item);
    return Recurse(version, error);
}

bool t4p::CallStackClass::Recurse(pelet::Versions version, t4p::CallStackClass::Errors& error) {
    // base case: no more functions to parse
    if (ResourcesRemaining.empty()) {
        return true;
    }

    // at a certain point, just stop the recursion.
    if (Variables.size() >= (size_t)3000) {
        error = STACK_LIMIT;
        return false;
    }
    if (TagCache.IsResourceCacheEmpty()) {
        error = EMPTY_CACHE;
        return false;
    }

    ResourceWithFile item = ResourcesRemaining.front();

    // don't pop() yet; the parser callbacks need to know the tag that we want to examine
    // this is because we only want to look at expressions in one function
    wxFileName fileName = item.FileName;

    // ScanFile will call the callbacks MethodFound, ExpressionFound; any function calls for this file will be collected
    FoundScope = false;

    // need to create the symbols for the file if the cache does not have them yet; symbols allow us to know the variable
    // types
    t4p::WorkingCacheClass* workingCache = new t4p::WorkingCacheClass;

    // here file identifier == file name because file name exists and is unique
    t4p::SymbolTableClass emptyTable;
    workingCache->Init(fileName.GetFullPath(), fileName.GetFullPath(), false, version, true, emptyTable);
    bool newlyRegistered = TagCache.RegisterWorking(fileName.GetFullPath(), workingCache);

    wxFFile file(fileName.GetFullPath(), wxT("rb"));
    bool ret = Parser.ScanFile(file.fp(), t4p::WxToIcu(fileName.GetFullPath()), LintResults);
    file.Close();
    UnicodeString key = item.Resource.ClassName + UNICODE_STRING_SIMPLE("::")  + item.Resource.Identifier;
    ParsedMethods[key] = true;
    if (ret && FoundScope) {
        // check to see if we have any new functions to parse
        ResourcesRemaining.pop();
        if (!ResourcesRemaining.empty()) {
            // need to get the file that the next function is in
            // make sure we don't go over the same function again in case there is a
            // recursive function call along the way
            bool hasNext = false;
            while (!hasNext && !ResourcesRemaining.empty()) {
                ResourceWithFile nextItem = ResourcesRemaining.front();
                wxFileName nextFile = nextItem.FileName;
                bool alreadyParsed = false;
                UnicodeString key = nextItem.Resource.ClassName + UNICODE_STRING_SIMPLE("::")  + nextItem.Resource.Identifier;
                alreadyParsed =  ParsedMethods.find(key) == ParsedMethods.end();
                if (alreadyParsed) {
                    // already been parsed; write the function arguments for this call and nothing else
                    // this is because we want to write a function call if the same function is called
                    // twice but we don't want to parse it twice
                    TempVarIndex = 1;
                    ResourcesRemaining.pop();
                }
            }
            if (hasNext) {
                TempVarIndex = 1;
                return Recurse(version, error);
            }
        }
    } else if (!ret) {
        error = PARSE_ERR0R;
    } else if (!FoundScope) {
        ret = false;
        error = RESOURCE_NOT_FOUND;
    }
    if (newlyRegistered) {
        // clean up, but only if this method created the symbols
        // this call will delete the WorkingCache pointer for us
        TagCache.RemoveWorking(fileName.GetFullPath());
    } else {
        // tag cache did not use the cache, delete it ourselves
        delete workingCache;
    }
    return ret;
}

bool t4p::CallStackClass::Persist(soci::session& session) {
    wxString error;
    bool good = false;
    try {
        int stepNumber = 0;
        std::string stepType;
        std::string expression;
        soci::transaction transaction(session);

        // TODO(roberto): prograte?
        int sourceId = 0;

        // delete any old rows; we only store one call stack for the active URL
        session.once << "DELETE FROM call_stacks";
        soci::statement stmt = (session.prepare <<
                                "INSERT INTO call_stacks(step_number, step_type, expression, source_id) VALUES (?, ?, ?, ?)",
                                soci::use(stepNumber), soci::use(stepType), soci::use(expression), soci::use(sourceId));
        for (std::vector<t4p::VariableSymbolClass>::const_iterator it = Variables.begin(); it != Variables.end(); ++it) {
            stepType = it->TypeString();
            expression = it->ToString();
            stmt.execute(true);
            stepNumber++;
        }
        transaction.commit();
        good = true;
    } catch (std::exception& e) {
        wxUnusedVar(e);
        error = t4p::CharToWx(e.what());
        good = false;
        wxASSERT_MSG(good, error);
    }
    return good;
}

bool t4p::CallStackClass::InDesiredScope() const {
    if (ResourcesRemaining.empty()) {
        return false;
    }
    ResourceWithFile item = ResourcesRemaining.front();
    if (item.Resource.Identifier == CurrentFunction) {
        return true;
    }
    return item.Resource.ClassName == CurrentClass && item.Resource.Identifier == CurrentMethod;
}

void t4p::CallStackClass::ExpressionVariableFound(pelet::VariableClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(expression, Variables);
}

void t4p::CallStackClass::ExpressionAssignmentFound(pelet::AssignmentExpressionClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolsFromVariable(expression->Destination, expression->Expression);

    // SymbolFromExpression(expression->Expression, Variables);
}

void t4p::CallStackClass::ExpressionAssignmentCompoundFound(pelet::AssignmentCompoundExpressionClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolsFromVariable(expression->Variable, expression->RightOperand);
}

void t4p::CallStackClass::ExpressionBinaryOperationFound(pelet::BinaryOperationClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(expression->LeftOperand, Variables);
    SymbolFromExpression(expression->RightOperand, Variables);
}

void t4p::CallStackClass::ExpressionUnaryOperationFound(pelet::UnaryOperationClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(expression->Operand, Variables);
}

void t4p::CallStackClass::ExpressionUnaryVariableOperationFound(pelet::UnaryVariableOperationClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(&(expression->Variable), Variables);
}

void t4p::CallStackClass::ExpressionTernaryOperationFound(pelet::TernaryOperationClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(expression->Expression1, Variables);
    SymbolFromExpression(expression->Expression2, Variables);
    if (expression->Expression3) {
        SymbolFromExpression(expression->Expression3, Variables);
    }
}

void t4p::CallStackClass::ExpressionScalarFound(pelet::ScalarExpressionClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(expression, Variables);
}

void t4p::CallStackClass::ExpressionNewInstanceFound(pelet::NewInstanceExpressionClass* expression) {
    // only collect expressions that are in the scope we want
    if (!InDesiredScope()) {
        return;
    }
    FoundScope = true;
    SymbolFromExpression(expression, Variables);
}

void t4p::CallStackClass::SymbolsFromVariable(const pelet::VariableClass& variable, pelet::ExpressionClass* expression) {
    t4p::VariableSymbolClass expressionResultSymbol;

    // follow associativity, do the right hand side first
    size_t oldSize = Variables.size();
    SymbolFromExpression(expression, Variables);
    if (Variables.size() > oldSize) {
        expressionResultSymbol = Variables.back();
    }

    if (variable.ChainList.size() == 2 && variable.ChainList[1].IsArrayAccess) {
        // check to see if the array access key is a scalar
        pelet::ExpressionClass* arrayAccessExpr = variable.ChainList[1].ArrayAccess;
        UnicodeString arrayKey;
        if (arrayAccessExpr && pelet::ExpressionClass::SCALAR == arrayAccessExpr->ExpressionType) {
            arrayKey = ((pelet::ScalarExpressionClass*)arrayAccessExpr)->Value;
        }

        // dont need to insert the same array key multiple times
        // add the variable to the list only if we have not added it yet
        // an array key may be assigned if the variable is not yet
        // seen; need to look for this case also
        // ie.  $arr[] = 'name';
        // as the array initialization for $arr
        std::vector<t4p::VariableSymbolClass>::iterator var;
        bool foundIndex = false;
        bool foundVariable = false;
        for (var = Variables.begin(); var != Variables.end(); ++var) {
            if (var->DestinationVariable == variable.ChainList[0].Name) {
                foundVariable = true;
            }
            if (var->DestinationVariable == variable.ChainList[0].Name && var->ArrayKey == arrayKey) {
                foundIndex = true;
            }
        }
        if (!foundVariable) {
            t4p::VariableSymbolClass arrayVariableSymbol;
            arrayVariableSymbol.ToArray(variable.ChainList[0].Name);
            Variables.push_back(arrayVariableSymbol);
        }
        if (!foundIndex) {
            t4p::VariableSymbolClass arrayVariableKeySymbol;
            arrayVariableKeySymbol.ToArrayKey(variable.ChainList[0].Name, arrayKey);
            Variables.push_back(arrayVariableKeySymbol);
        }
    } else if (!variable.ChainList.empty()) {
        UnicodeString destinationVariable = variable.ChainList[0].Name;
        if (variable.ChainList.size() > 1) {
            std::vector<pelet::VariablePropertyClass>::const_iterator prop = variable.ChainList.begin();
            prop++;
            size_t oldSize = Variables.size();
            UnicodeString nextObjectName = variable.ChainList[0].Name;
            for (; prop != variable.ChainList.end(); ++prop) {
                SymbolFromVariableProperty(nextObjectName, *prop, Variables);
                nextObjectName = Variables.back().DestinationVariable;
            }
            if (Variables.size() > oldSize) {
                destinationVariable = Variables.back().DestinationVariable;
            }
        }

        // now assign the right side of the expression to the left side of the
        // expression
        t4p::VariableSymbolClass assignSymbol;
        assignSymbol.ToAssignment(destinationVariable, expressionResultSymbol.DestinationVariable);
        Variables.push_back(assignSymbol);
    }
}

void t4p::CallStackClass::SymbolFromVariableProperty(const UnicodeString& objectName,
        const pelet::VariablePropertyClass& property,
        std::vector<t4p::VariableSymbolClass>& symbols) {
    // recurse down the arguments first
    std::vector<UnicodeString> argumentVariables;
    if (property.IsFunction && !property.CallArguments.empty()) {
        std::vector<pelet::ExpressionClass*>::const_iterator expr;
        for (expr = property.CallArguments.begin(); expr != property.CallArguments.end(); ++expr) {
            size_t oldSize = symbols.size();
            SymbolFromExpression(*expr, symbols);
            if (symbols.size() > oldSize) {
                argumentVariables.push_back(symbols.back().DestinationVariable);
            } else if (pelet::ExpressionClass::VARIABLE == (*expr)->ExpressionType) {
                pelet::VariableClass* varExpr = (pelet::VariableClass*)(*expr);
                if (!varExpr->ChainList.empty()) {
                    // a new variable symbol was not created because the argument already exists in the symbols list
                    argumentVariables.push_back(varExpr->ChainList[0].Name);
                }
            }
        }
    }

    // now the symbol for this property
    UnicodeString tempVarName = NewTempVariable();
    t4p::VariableSymbolClass symbol;
    if (property.IsFunction) {
        symbol.ToMethodCall(tempVarName, objectName, property.Name, argumentVariables);
        symbols.push_back(symbol);
    } else if (!property.IsArrayAccess) {
        symbol.ToProperty(tempVarName, objectName, property.Name);
        symbols.push_back(symbol);
    }
    /*
    else if (property.IsArrayAccess && property.ArrayAccess) {
    	// check to see if the array access key is a scalar
    	pelet::ExpressionClass* arrayAccessExpr = property.ArrayAccess;
    	UnicodeString arrayKey;
    	if (arrayAccessExpr && pelet::ExpressionClass::SCALAR == arrayAccessExpr->ExpressionType) {
    		arrayKey = ((pelet::ScalarExpressionClass*)arrayAccessExpr)->Value;
    		symbol.ToArrayKey(objectName, arrayKey);
    		symbols.push_back(symbol);
    	}

    	// dont need to insert the same array key multiple times
    	// add the variable to the list only if we have not added it yet
    	// an array key may be assigned if the variable is not yet
    	// seen; need to look for this case also
    	// ie.  $arr[] = 'name';
    	// as the array initialization for $arr
    }
    */
}

void t4p::CallStackClass::SymbolFromExpression(pelet::ExpressionClass* expression, std::vector<t4p::VariableSymbolClass>& symbols) {
    if (pelet::ExpressionClass::SCALAR == expression->ExpressionType) {
        UnicodeString tempVarName = NewTempVariable();
        t4p::VariableSymbolClass scalarSymbol;
        scalarSymbol.ToScalar(tempVarName, ((pelet::ScalarExpressionClass*)expression)->Value);
        symbols.push_back(scalarSymbol);
    } else if (pelet::ExpressionClass::ARRAY == expression->ExpressionType) {
        UnicodeString tempVarName = NewTempVariable();
        t4p::VariableSymbolClass arraySymbol;
        arraySymbol.ToArray(tempVarName);
        symbols.push_back(arraySymbol);

        // array keys are expressions, only want to capture scalar keys
        std::vector<UnicodeString> arrayKeys;
        std::vector<pelet::ArrayPairExpressionClass*> pairs = ((pelet::ArrayExpressionClass*)expression)->ArrayPairs;
        std::vector<pelet::ArrayPairExpressionClass*>::const_iterator pair;
        for (pair = pairs.begin(); pair != pairs.end(); ++pair) {
            if ((*pair)->Key && pelet::ExpressionClass::SCALAR == (*pair)->Key->ExpressionType) {
                arrayKeys.push_back(((pelet::ScalarExpressionClass*)(*pair)->Key)->Value);
            }
        }

        for (std::vector<UnicodeString>::const_iterator key = arrayKeys.begin(); key != arrayKeys.end(); ++key) {
            t4p::VariableSymbolClass keySymbol;
            keySymbol.ToArrayKey(tempVarName, *key);
            symbols.push_back(keySymbol);
        }
    } else if (pelet::ExpressionClass::NEW_CALL == expression->ExpressionType) {
        UnicodeString tempVarName = NewTempVariable();
        t4p::VariableSymbolClass newSymbol;
        newSymbol.ToNewObject(tempVarName, ((pelet::NewInstanceExpressionClass*)expression)->ClassName);
        symbols.push_back(newSymbol);
    } else if (pelet::ExpressionClass::VARIABLE == expression->ExpressionType) {
        pelet::VariableClass* varExpression = (pelet::VariableClass*) expression;

        // do the function calls first
        // a function will never have more than 1 item in the chain list because the following code
        // is not possible
        // func1() funct2();
        if (!varExpression->ChainList.empty() && varExpression->ChainList[0].IsFunction) {
            std::vector<pelet::ExpressionClass*>::const_iterator arg;
            std::vector<UnicodeString> argumentVariables;
            for (arg = varExpression->ChainList[0].CallArguments.begin(); arg != varExpression->ChainList[0].CallArguments.end(); ++arg) {
                SymbolFromExpression(*arg, symbols);
                if (!symbols.empty()) {
                    argumentVariables.push_back(symbols.back().DestinationVariable);
                }
            }

            // variable for the function result
            UnicodeString tempVarName = NewTempVariable();
            t4p::VariableSymbolClass functionSymbol;
            functionSymbol.ToFunctionCall(tempVarName, varExpression->ChainList[0].Name, argumentVariables);
        } else if (!varExpression->ChainList.empty()) {
            // add the variable to the list only if we have not added it yet
            std::vector<t4p::VariableSymbolClass>::iterator var;
            bool found = false;
            for (var = symbols.begin(); var != symbols.end(); ++var) {
                if (var->DestinationVariable == varExpression->ChainList[0].Name) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                t4p::VariableSymbolClass varSymbol;
                varSymbol.ToAssignment(varExpression->ChainList[0].Name, UNICODE_STRING_SIMPLE(""));
                symbols.push_back(varSymbol);
            }

            if (varExpression->ChainList.size() > 1) {
                // now add any property / method accesses
                std::vector<pelet::VariablePropertyClass>::const_iterator prop = varExpression->ChainList.begin();
                prop++;
                UnicodeString nextObjectName = varExpression->ChainList[0].Name;
                for (; prop != varExpression->ChainList.end(); ++prop) {
                    size_t oldSize = symbols.size();
                    SymbolFromVariableProperty(nextObjectName, *prop, symbols);
                    if (symbols.size() > oldSize) {
                        nextObjectName = symbols.back().DestinationVariable;
                    }
                }
            }
        }
    }
}

UnicodeString t4p::CallStackClass::NewTempVariable() {
    UnicodeString newName;

    // 11 == length of "$@tmp" + a 5 digit number + NUL should be big enough
    // using $@tmp so that a temp variable will never collide with a variable found in
    // the source
    int32_t len = u_sprintf(newName.getBuffer(11), "$@tmp%d", TempVarIndex);
    newName.releaseBuffer(len);
    TempVarIndex++;
    return newName;
}

void t4p::CallStackClass::MethodFound(const UnicodeString& namespaceName, const UnicodeString& className,
                                      const UnicodeString& methodName, const UnicodeString& signature,
                                      const UnicodeString& returnType, const UnicodeString& comment,
                                      pelet::TokenClass::TokenIds visibility,
                                      bool isStatic, const int lineNumber, bool hasVariableArguments) {
    CurrentClass = className;
    CurrentMethod = methodName;
    CurrentFunction.remove();

    if (ResourcesRemaining.empty()) {
        return;
    }

    ResourceWithFile item = ResourcesRemaining.front();

    // if a method was found set the flag
    // we need to do this because the method iself may be empty or not contain variable
    // and we dont want to flag this as an error
    if (item.Resource.ClassName == CurrentClass && item.Resource.Identifier == CurrentMethod) {
        FoundScope = true;

        t4p::VariableSymbolClass beginScope;
        beginScope.ToBeginMethod(className, methodName);
        Variables.push_back(beginScope);
    }
}

void t4p::CallStackClass::FunctionFound(const UnicodeString& namespaceName, const UnicodeString& functionName,
                                        const UnicodeString& signature, const UnicodeString& returnType,
                                        const UnicodeString& comment, const int lineNumber, bool hasVariableArguments) {
    CurrentClass.remove();
    CurrentMethod.remove();
    CurrentFunction = functionName;

    if (ResourcesRemaining.empty()) {
        return;
    }

    ResourceWithFile item = ResourcesRemaining.front();

    // if a method was found set the flag
    // we need to do this because the method iself may be empty or not contain variable
    // and we dont want to flag this as an error
    if (item.Resource.Identifier == CurrentFunction) {
        FoundScope = true;
        t4p::VariableSymbolClass beginScope;
        beginScope.ToBeginFunction(functionName);
        Variables.push_back(beginScope);
    }
}
