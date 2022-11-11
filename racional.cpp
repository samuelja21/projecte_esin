#include "racional.hpp"

int mcd(int a, int b){
	int m;
	if (b == 0) m = a;
	else if (a == 0) m = b;
	
	else{
		if (a > b) m = mcd(a % b, b);
		else m = mcd(b % a, a);
	}
	
	return m;
}

int mcm(int a, int b){
	int m = a*b/mcd(a,b);
	return m;
}


  // Constructora. Construeix un racional en la seva versió simplificada.
  // Es produeix un error si el denominador és 0.
   racional::racional(int n, int d) throw(error){
  	if (d == 0) throw error(DenominadorZero);
  	int m = mcd(n, d);
  	numerador = n/m;
  	denominador = d/m;
  }

  // Constructora per còpia, assignació i destructora.
  racional::racional(const racional & r) throw(error){
  	numerador = r.numerador;
  	denominador = r.denominador;
  }
  
  racional & racional::operator=(const racional & r) throw(error){
    numerador = r.numerador;
  	denominador = r.denominador;
	return *this;	
  }
  
  racional::~racional() throw(){
  }

  // Consultores. La part_entera d'un racional pot ser
  // positiva o negativa. El residu SEMPRE és un racional positiu.
  int racional::num() const throw(){ 
  	return numerador;
  }
  
  int racional::denom() const throw(){
  	return denominador;
  }
  
  int racional::part_entera() const throw(){
  	int pentera = numerador/denominador;
  	return pentera;
  }
  
  racional racional::residu() const throw(){
	racional res;
	if (numerador > denominador) res.numerador = numerador - denominador;
	else res.numerador = numerador;
	res.denominador = denominador;
	return res;
  }

  /* Sobrecàrrega d'operadors aritmètics. Retorna un racional en la seva
     versió simplificada amb el resultat de l'operació. Es produeix un
     error al dividir dos racionals si el segon és 0.*/
  racional racional::operator+(const racional & r) const throw(error){
  	int m = mcm(denominador, r.denominador);
  	int a = (m/denominador)*numerador;
  	int b = (m/r.denominador)*r.numerador; 
  	racional sum(a+b, m);
  	return sum;
  }
  
  racional racional::operator-(const racional & r) const throw(error){
    int m = mcm(denominador, r.denominador);
  	int a = (m/denominador)*numerador;
  	int b = (m/r.denominador)*r.numerador;
  	racional resta(a-b, m);
  	return resta;
  }
  
  racional racional::operator*(const racional & r) const throw(error){
  	racional mul(numerador * r.numerador, denominador * r.denominador);
  	return mul;
  }
  
  racional racional::operator/(const racional & r) const throw(error){
  	racional div(numerador * r.denominador, denominador * r.numerador);
  	return div;
  }

  /* Sobrecàrrega de operadors de comparació. Retornen cert, si i només si
     el racional sobre el que s'aplica el mètode és igual (==), diferent
     (!=), menor (<), menor o igual (<=), major (>) o major o igual(>=)
     que el racional r.*/
     
  bool racional::operator==(const racional & r) const throw(){
	return numerador == r.numerador and denominador == r.denominador;
  }
  
  bool racional::operator!=(const racional & r) const throw(){
	return not (*this == r);
  }
  
  bool racional::operator<(const racional & r) const throw(){
 	int a = numerador*r.denominador;
  	int b = r.numerador*denominador;
  	return a < b;	
  }
  
  bool racional::operator<=(const racional & r) const throw(){
 	int a = numerador*r.denominador;
  	int b = r.numerador*denominador;
  	return a <= b;	
  }
  
  bool racional::operator>(const racional & r) const throw(){
 	int a = numerador*r.denominador;
  	int b = r.numerador*denominador;
  	return a > b;	
  }
  
  bool racional::operator>=(const racional & r) const throw(){
    	double a = numerador/denominador;
  	double b = r.numerador/r.denominador;
  	return a >= b;
  }

