#ifndef AUXTYPES_H
#define AUXTYPES_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

enum ExpType{
	INT_EXP,
	BOOL_EXP,
	BYTE_EXP,

	STRING_EXP,
	VOID_EXP
};

std::string ExpTypeString(ExpType type, bool capital_letters = false);
std::vector<std::string> ExpTypeStringVector(std::vector<ExpType> types, bool capital_letters = false);

struct Parameter{
	Parameter(const std::string& id, ExpType type, int line_of_origin, bool is_const)
		:id(id), type(type), line_of_origin(line_of_origin), is_const(is_const){}
	std::string id;
	ExpType type;
	int line_of_origin;
	bool is_const;
};

struct DecInfo{
	bool is_const;
	ExpType raw_type;
	std::string* id;
};

struct FunctionType{
	/**
	 * @brief this c'tor uses the 'parameters' param by simply pointing at it as the set of parameters of the function.
	 * 		as a part of the destruction of the function type, the d'tor will delete this pointer.
	 * @param parameters_backwards - this is the list of parameters that the function expects.
	 * 		at the front of this vector there should be the LAST parameter, 
	 * 		and at the back there should be the FIRST parameter.
	 */
	FunctionType(ExpType return_type, std::vector<Parameter>* parameters_backwards)
		:return_type(return_type), parameters(parameters_backwards){}
	FunctionType()
		:return_type(VOID_EXP), parameters(nullptr){}

	/**
	 * @return std::vector<ExpType> - the list of parameters that should be given to a function of this type.
	 * 		the parameters vector will have the first parameter at the front, and the last parameter at the back. 
	 */
	std::vector<ExpType> getParameterTypes() const{
		std::vector<ExpType> result;
		for(auto it = parameters->rbegin(); it != parameters->rend(); ++it)
			result.push_back((*it).type);
		return result;
	}

	ExpType return_type;
	std::shared_ptr<std::vector<Parameter>> parameters;
};
#endif