#include "token.hpp"

// Especificació funcions "private"


bool token::isAscii(const string &var_name) {
    // S'assegura que "var_name" només conté caràcters els codis ASCII dels quals estan entre 65 ('A') i 90('Z'), entre 97 ('a') i 122 ('z') o el 95 ('_'). Si no, retorna "false"
    bool isAscii = true;
    for (unsigned int i = 0; (i < var_name.size()) or isAscii; i++) {
        char c = var_name[i];
        int cAscii = (int)c;
        if (not (cAscii >= 65 or cAscii <= 90 or cAscii >= 97 or cAscii <= 122 or cAscii == 95))
            isAscii = false;
    }

    return isAscii;
}

bool token::isOperand(const string &var_name) {
    // S'assegura que "var_name" coincideixi amb un nom reservat: unassign, e, sqrt, log, exp i evalf. Si no, torna "false"
    return var_name == "unassign" or var_name == "e" or var_name == "sqrt" or var_name == "log" or var_name == "exp" or var_name == "evalf";
}

/*Constructores: Construeixen tokens pels operadors, les constants enteres,
les constants racionals, les constants reals i les variables(el seu
identificador), respectivament.*/
token::token(codi cod) throw(error) {
    if (cod == CT_ENTERA or cod == CT_RACIONAL or cod == CT_REAL or cod == VARIABLE)
        throw error(ConstructoraInadequada);
    else _token = cod;
}
token::token(int n) throw(error) {
    _token = CT_ENTERA;
    _value.i = n;
}
token::token(const racional & r) throw(error) {
    _token = CT_RACIONAL;
    _value.r = r;
}
token::token(double x) throw(error) {
    _token = CT_REAL;
    _value.d = x;
}
token::token(const string & var_name) throw(error) {
    if (not isAscii(var_name) or isOperand(var_name))
        throw error(IdentificadorIncorrecte);
    else {
        _token = VARIABLE;
        _value.v = var_name;
    }
}

// Constructora por còpia, assignació i destructora.
token::token(const token & t) throw(error) {
    _token = t._token;
    //_value = t._value;
    switch (t._token){
    	case CT_ENTERA:
    		_value.i = t._value.i;
    	case CT_RACIONAL:
    		_value.r = t._value.r;
    	case CT_REAL:
    		_value.d = t._value.d;
    	case VARIABLE:
    		_value.v = t._value.v;
    	default:
    		break;
    }
}

token & token::operator=(const token & t) throw(error) {
    _token = t._token;
    //_value = t._value;
    switch (t._token){
    	case CT_ENTERA:
    		_value.i = t._value.i;
    	case CT_RACIONAL:
    		_value.r = t._value.r;
    	case CT_REAL:
    		_value.d = t._value.d;
    	case VARIABLE:
    		_value.v = t._value.v;
    	default:
    		break;
    }
    return *this;
}

token::~token() throw() {
}

/*Consultores: Retornen respectivament el codi i el valor (en el cas de
constants enteres, racionals o reals) o l'identificador (en el cas de
variables). Es produeix un error si apliquem una consultora inadequada
sobre un token, p.e. si apliquem valor_enter sobre un token que no sigui
una CT_ENTERA.*/
token::codi token::tipus() const throw() {
    return _token;
}
int token::valor_enter() const throw(error) {
    if (_token != CT_ENTERA) throw error(ConsultoraInadequada);
    else return _value.i;
}
racional token::valor_racional() const throw(error) {
    if (_token != CT_RACIONAL) throw error(ConsultoraInadequada);
    else return _value.r;
}
double token::valor_real() const throw(error) {
    if (_token != CT_REAL) throw error(ConsultoraInadequada);
    else return _value.d;
}
string token::identificador_variable() const throw(error) {
    if (_token != VARIABLE) throw error(ConsultoraInadequada);
    else return _value.v;
}

/*Igualtat i desigualtat entre tokens. Dos tokens es consideren iguals si els
seus codis ho són i si 1) en cas de ser CT_ENTERA, CT_RACIONAL o CT_REAL,
els seus valors són iguals i 2) en cas de ser VARIABLE, tenen el mateix
nom. */
bool token::operator==(const token & t) const throw(){
	if (_token == t._token){
        switch (_token){
            case CT_ENTERA: return _value.i == t._value.i;
            case CT_RACIONAL: return _value.r == t._value.r;
            case CT_REAL: return _value.d == t._value.d;
            case VARIABLE: return _value.v == t._value.v;
            default: return true;
        }
	}
	else return false;
}

bool token::operator!=(const token & t) const throw(){
	return not (*this == t);
}

/*Precedència entre tokens. L'operador > retorna cert si i només si el token
és un operador amb major precedència que l'operador del token t. Si algun
dels tokens no és un operador es produeix un error.*/
bool token::operator>(const token & t) const throw(error){
	if ((_token >= CT_ENTERA and _token <= VARIABLE) or (t._token >= CT_ENTERA and t._token <= VARIABLE))
        	throw error(PrecedenciaEntreNoOperadors);
    
    switch(_token){
        case EXPONENCIACIO: return t._token != EXPONENCIACIO;
        case CANVI_DE_SIGNE: return t._token != EXPONENCIACIO and t._token != CANVI_DE_SIGNE and t._token != SIGNE_POSITIU;
        case SIGNE_POSITIU: return t._token != EXPONENCIACIO and t._token != CANVI_DE_SIGNE and t._token != SIGNE_POSITIU;
        case MULTIPLICACIO: return t._token == SUMA or t._token == RESTA;
        case DIVISIO: return t._token == SUMA or t._token == RESTA;
        default: return false;
    }    	
}

bool token::operator<(const token & t) const throw(error){
	return not(*this > t or *this == t);
}


