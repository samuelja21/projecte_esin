#include "expressio.hpp"
#include <stack>
using namespace std;

   expressio::node* expressio::copia_nodes(node* m){
      node* n;
      if (m == nullptr) n = nullptr;
      else{
         n = new node;
         try{
            n->info = m->info;
            n->fe = copia_nodes(m->fe);
            n->fd = copia_nodes(m->fd);
         } catch(...){
            delete n;
            throw;
         }
      }
      return n;
   }

   void expressio::esborra_nodes(node* m){
      if (m != nullptr){
         esborra_nodes(m->fe);
         esborra_nodes(m->fd);
         delete m;
      }
   }

   bool expressio::compara_nodes(node* m, node* n){
      if (m != nullptr and n != nullptr){
         return (compara_nodes(m->fe, n->fe) and compara_nodes(m->fd, n->fd));
      }
      else if (m == nullptr and n == nullptr) return true;
      else return false;
   }
 
 /* Constructora d'una expressió formada per un sol token: un operand. Si
     s'utiliza el valor del token per defecte es construeix la que
     anomenem "expressió buida". Si el tipus del token no és el del token
     per defecte (NULLTOK), ni el d'una CT_ENTERA, CT_RACIONAL, CT_REAL,
     CT_E, VARIABLE o VAR_PERCENTAtGE es produeix un error sintàctic. */
    expressio::expressio(const token t) throw(error){
        if (t.tipus() > token::VAR_PERCENTATGE) throw error(ErrorSintactic);
        else{
         _arrel = new node;
         _arrel->info = t;
        }
  }

  /* Constructora a partir d'una seqüència de tokens. Es produeix un error si
     la seqüència és buida o si no es pot construir l'arbre d'expressió
     corresponent(és a dir, si és sintàcticament incorrecta). */
   expressio::expressio(const list<token> & l) throw(error){
      if (l.size() == 0) throw error(ErrorSintactic);
      else{
         list<token>::const_iterator it = l.begin();
         stack<token> op;
         stack<node*> ex;
         int par(0);
         while(it != l.end()){
            if ((*it).tipus() == token::TANCAR_PAR) ++par;
            else if ((*it).tipus() < token::VAR_PERCENTATGE){
               node *a = new node;
               a->info = *it;
               a->fd = nullptr;
               a->fe = nullptr;
               ex.push(a);
            }
            else if ((*it).tipus() > token::CANVI_DE_SIGNE) op.push(*it);
            else {
               if ((not op.empty() and op.top().tipus() < (*it).tipus()) or par > 0){
                  while (not op.empty()){
                     if (op.top().tipus() == token::OBRIR_PAR){
                        op.pop();
                        --par;
                        if (op.top().tipus() >= token::SQRT and op.top().tipus() <= token::EVALF){
                           node *a = new node;
                           a->info = op.top();
                           a->fd = ex.top();
                           ex.pop();
                           ex.push(a);
                           op.pop();
                        }
                     }
                     else{
                        node *a = new node;
                        a->info = op.top();
                        a->fd = ex.top();
                        ex.pop();
                        a->fe = ex.top();
                        ex.pop();
                        ex.push(a);
                        op.pop();
                     }
                  } 
               }
               op.push(*it);
            }
            ++it;
         }
         while (not op.empty()){
            if (op.top().tipus() == token::OBRIR_PAR){
               op.pop();
               --par;
               if (op.top().tipus() >= token::SQRT and op.top().tipus() <= token::EVALF){
                  node *a = new node;
                  a->info = op.top();
                  a->fd = ex.top();
                  ex.pop();
                  ex.push(a);
                  op.pop();
               }
            }
            else{
               node *a = new node;
               a->info = op.top();
               a->fd = ex.top();
               ex.pop();
               a->fe = ex.top();
               ex.pop();
               ex.push(a);
               op.pop();
            }
         } 
         _arrel = ex.top();  
      }
   }

  // Constructora per còpia, assignació i destructora.
  expressio::expressio(const expressio & e) throw(error){
   _arrel = copia_nodes(e._arrel);
  }

  expressio & expressio::operator=(const expressio & e) throw(error){
   if (this != &e){
      node *aux;
      aux = copia_nodes(e._arrel);
      esborra_nodes(_arrel);
      _arrel = aux;
   }
   return *this;
  }
  expressio::~expressio() throw(error){
   esborra_nodes(_arrel);
  }

  // Retorna cert si i només si s'aplica a l'expressió buida.
  expressio::operator bool() const throw(){
   return _arrel == nullptr; 
  }

  /* Operadors d'igualtat i desigualtat. Dues expressions es consideren
     iguals si i només si els seus arbres d'expressió són idèntics. */
  bool expressio::operator==(const expressio & e) const throw(){
      return compara_nodes(_arrel, e._arrel);
  }
  bool expressio::operator!=(const expressio & e) const throw(){
      return not compara_nodes(_arrel, e._arrel);
  }

  /* Retorna una llista sense repeticions, en qualsevol ordre, amb
     els noms de les variables de l'expressió. */
  void expressio::vars(list<string> & l) const throw(error){

  }

  /* Substitueix totes les aparicions de la variable de nom v per
     l'expressió e. Si no existeix la variable v dins de l'expressió a la
     que apliquem aquest mètode l'expressió no es modifica. */
  void expressio::apply_substitution(const string & v, const expressio & e) throw(error){

  }

  /* Aplica un pas de simplificació a l'expressió. La subexpressió a
     simplificar es busca seguint el recorregut "left to right" postordre
     explicat a l'apartat "Procés d'avaluació". Es pot produir qualsevol dels
     errors semàntics que apareixen més avall numerats des del 32 al 35. */
  void expressio::simplify_one_step() throw(error){

  }

  /* Aplica successius passos de simplificació com l'anterior fins que
     l'expressió es trobi completament simplificada. Llavors diem que es
     troba en "forma normal". Es pot produir qualsevol dels errors
     semàntics que apareixen més avall numerats des del 32 al 35. */
  void expressio::simplify() throw(error){

  }

  /* Converteix l'expressió en la seqüència de tokens lt corresponent: els
     operadors apareixen entre els operands si són infixos, abans si són
     prefixos i els símbols de funció van seguits de parèntesis que tanquen
     els seus arguments separats per comes. S'han d'introduir només aquells
     parèntesis que siguin estrictament necessaris per trencar les regles de
     precedència o associativitat en l'ordre d'aplicació dels operadors. */
  void expressio::list_of_tokens(list<token> & lt) throw(error){

  }

