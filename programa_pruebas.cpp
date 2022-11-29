#include <iostream>
#include "racional.hpp"
#include"token.hpp"
#include"expressio.hpp"
#include<list>
#include <string>

using namespace std;

int main(){
    token suma(token::SUMA);
    token resta(token::RESTA);
    token mul(token::MULTIPLICACIO);
    token obrir(token::OBRIR_PAR);
    token tancar(token::TANCAR_PAR);
    token arrel(token::SQRT);
    token log(token::LOG);
    list<token> l = {token(2), suma, obrir, token(2), mul, obrir, token(1), suma, token(2), tancar, tancar, suma, log, obrir, token(2), suma, token(2), tancar};
    //2+(2*(1+2))+log(2+2)
    expressio e(l);
}   
  
//Añadir al expressio.rep
static void print(node* n);
//Añadir al expressio.cpp
void expressio::print(node* n){
    if (n != nullptr){
        print(n->fe);
        if ((n->info).tipus() == token::MULTIPLICACIO) cout<<"* ";
        else if ((n->info).tipus() == token::SUMA) cout<<"+ ";
        else if ((n->info).tipus() == token::RESTA) cout<<"- ";
        else if ((n->info).tipus() == token::CT_ENTERA) cout<<(n->info).valor_enter()<<" ";
        else if ((n->info).tipus() == token::VARIABLE) cout<<(n->info).identificador_variable()<<" ";
        else if ((n->info).tipus() == token::SQRT) cout<<"raiz de ";
        else if ((n->info).tipus() == token::LOG) cout<<"logaritmo de ";
        print(n->fd);
    }
}