#include "racional.hpp"

int racional::mcd(int a, int b){
	int m;
	if (b == 0) m = a;
	else if (a == 0) m = b;
	else{
		if (a > b) m = mcd(a % b, b);
		else m = mcd(b % a, a);
	}
	return m;
}

int racional::mcm(int a, int b){
	return a*b/mcd(a,b);;
}


  // Constructora. Construeix un racional en la seva versió simplificada.
  // Es produeix un error si el denominador és 0.
  racional::racional(int n, int d) throw(error){
  	if (d == 0) throw error(DenominadorZero);
  	bool negatiu(false);
  	if (n < 0) {
  		n = n*-1;
      		negatiu = !negatiu;
    	}
    	if (d < 0){
      		d = d*-1;
      		negatiu = !negatiu;
    	} 
  	int m = mcd(n, d);
  	if (negatiu) _numerador = n*-1/m;
    	else _numerador = n/m;
  	_denominador = d/m;
  }

  // Constructora per còpia, assignació i destructora.
  racional::racional(const racional & r) throw(error){
  	_numerador = r._numerador;
  	_denominador = r._denominador;
  }
  
  racional & racional::operator=(const racional & r) throw(error){
    	_numerador = r._numerador;
  	_denominador = r._denominador;
	return *this;	
  }
  
  racional::~racional() throw(){
  }

  // Consultores. La part_entera d'un racional pot ser
  // positiva o negativa. El residu SEMPRE és un racional positiu.
  int racional::num() const throw(){ 
  	return _numerador;
  }
  
  int racional::denom() const throw(){
  	return _denominador;
  }
  
  int racional::part_entera() const throw(){
  	if (num() > 0) return _numerador/_denominador;
    	else return _numerador/_denominador - (_numerador % _denominador != 0);
  }
  
  racional racional::residu() const throw(){
	int num;
	if (_numerador > _denominador) num = _numerador - _denominador;
	else {
    		if (_numerador > 0) num = _numerador;
    		else num = (*this - racional(part_entera())).num();
  	}
  	return racional(num, _denominador);
  }

  /* Sobrecàrrega d'operadors aritmètics. Retorna un racional en la seva
     versió simplificada amb el resultat de l'operació. Es produeix un
     error al dividir dos racionals si el segon és 0.*/
  racional racional::operator+(const racional & r) const throw(error){
  	int m = mcm(_denominador, r._denominador);
  	int a = (m/_denominador)*_numerador;
  	int b = (m/r._denominador)*r._numerador; 
  	return racional(a+b, m);
  }
  
  racional racional::operator-(const racional & r) const throw(error){
    	int m = mcm(_denominador, r._denominador);
  	int a = (m/_denominador)*_numerador;
  	int b = (m/r._denominador)*r._numerador;
  	return racional(a-b, m);
  }
  
  racional racional::operator*(const racional & r) const throw(error){
  	return racional(_numerador * r._numerador, _denominador * r._denominador);
  }
  
  racional racional::operator/(const racional & r) const throw(error){
  	return racional(_numerador * r._denominador, _denominador * r._numerador);
  }

  /* Sobrecàrrega de operadors de comparació. Retornen cert, si i només si
     el racional sobre el que s'aplica el mètode és igual (==), diferent
     (!=), menor (<), menor o igual (<=), major (>) o major o igual(>=)
     que el racional r.*/
     
  bool racional::operator==(const racional & r) const throw(){
	return _numerador == r._numerador and _denominador == r._denominador;
  }
  
  bool racional::operator!=(const racional & r) const throw(){
	return not (*this == r);
  }
  
  bool racional::operator<(const racional & r) const throw(){
 	int a = _numerador*r._denominador;
  	int b = r._numerador*_denominador;
  	return a < b;	
  }
  
  bool racional::operator<=(const racional & r) const throw(){
 	int a = _numerador*r._denominador;
  	int b = r._numerador*_denominador;
  	return a <= b;	
  }
  
  bool racional::operator>(const racional & r) const throw(){
 	int a = _numerador*r._denominador;
  	int b = r._numerador*_denominador;
  	return a > b;	
  }
  
  bool racional::operator>=(const racional & r) const throw(){
 	int a = _numerador*r._denominador;
  	int b = r._numerador*_denominador;
  	return a >= b;
  }
